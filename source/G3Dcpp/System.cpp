/** 
  @file System.cpp
 
  @maintainer Morgan McGuire, matrix@graphics3d.com

  Note: every routine must call init() first.

  There are two kinds of detection used in this file.  At compile
  time, the _MSC_VER #define is used to determine whether x86 assembly
  can be used at all.  At runtime, processor detection is used to
  determine if we can safely call the routines that use that assembly.

  @cite Rob Wyatt http://www.gamasutra.com/features/wyatts_world/19990709/processor_detection_01.htm
  @cite Benjamin Jurke http://www.flipcode.com/cgi-bin/msg.cgi?showThread=COTD-ProcessorDetectionClass&forum=cotd&id=-1
  @cite Michael Herf http://www.stereopsis.com/memcpy.html

  @created 2003-01-25
  @edited  2004-01-27
 */

#include "G3D/platform.h"
#include "G3D/System.h"
#include "G3D/debug.h"
#include "G3D/g3derror.h"
#include "G3D/fileutils.h"

#ifdef G3D_WIN32

    #include <conio.h>
    #include <sys/timeb.h>

#elif defined(G3D_LINUX) 

    #include <stdlib.h>
    #include <stdio.h>
    #include <errno.h>
    #include <sys/types.h>
    #include <sys/select.h>
    #include <termios.h>
    #include <stropts.h>
    #include <unistd.h>
    #include <sys/ioctl.h>
    #include <sys/time.h>

#elif defined(G3D_OSX)

    #include <stdlib.h>
    #include <stdio.h>
    #include <errno.h>
    #include <sys/types.h>
    #include <sys/select.h>
    #include <termios.h>
    #include <unistd.h>
    #include <sys/time.h>

#endif

namespace G3D {

std::string demoFindData(bool errorIfNotFound) {

    Array<std::string> potential;
    
	potential.append("");
	potential.append("../");
	potential.append("../../");
	potential.append("../../../");
	potential.append("../../../../");
	potential.append("../../../../../");
	
	int ver = G3D_VER;
	std::string lname = format("g3d-%d_%02d", ver / 10000, (ver / 100) % 100);

	if (G3D_VER % 10 != 0) {
		lname = lname + format("-b%02d/", ver % 100);
	} else {
		lname = lname + "/";
	}
    std::string lpath = "libraries/" + lname;
    #ifdef G3D_WIN32
        potential.append(std::string("c:/") + lpath);
        potential.append(std::string("d:/") + lpath);
        potential.append(std::string("g:/") + lpath);
        potential.append(std::string("c:/users/morgan/data/"));
    #else
        potential.append(std::string("/map/gfx0/common/games/") + lpath);
    #endif

    for (int p = 0; p < potential.size();  ++p) {
        std::string path = potential[p];
        if (fileExists(path + "data") && fileExists(path + "data/font")) {
            return path + "data/";
        }
    }


    if (errorIfNotFound) {
        const char* choice[] = {"Exit"};

        prompt("Demo Error", "The demo could not locate the data directory.  "
            "The data is required to run this demo.  If you have not downloaded "
            "the data zipfile, get it from http://g3d-cpp.sf.net.  If you have "
            "downloaded it, it needs to be no more than 4 directories above the "
            "demo directory.", choice, 1, true);
    }

    return "";
}


static bool					_rdtsc              = false;
static bool					_mmx                = false;
static bool					_sse                = false;
static bool					_sse2		        = false;
static bool					_3dnow              = false;
static std::string			_cpuVendor          = "Unknown";
static bool					initialized         = false;
static bool					_cpuID              = false;
static G3DEndian            _machineEndian      = G3D_LITTLE_ENDIAN;
static std::string          _cpuArch            = "Unknown";
static std::string          _operatingSystem    = "Unknown";

#ifdef G3D_WIN32
/** Used by getTick() for timing */
static LARGE_INTEGER        _start;
static LARGE_INTEGER        _counterFrequency;
#else
static struct timeval       _start;
#endif

/** The Real-World time of System::getTick() time 0.  Set by initTime */
static RealTime             realWorldGetTickTime0;


static int	 	 maxSupportedCPUIDLevel = 0;
static int    maxSupportedExtendedLevel = 0;

#define checkBit(var, bit)   ((var & (1 << bit)) ? true : false)

/** Checks if the CPUID command is available on the processor (called from init) */
static void checkForCPUID();

/** ReadRead the standard processor extensions.  Called from init(). */
static void getStandardProcessorExtensions();

/** Perform processor identification and initialize the library (if not
    already initialized). */
static void init();

/** Called from init */
static void initTime();


bool System::hasRDTSC() {
	init();
	return _rdtsc;
}


bool System::hasSSE() {
	init();
	return _sse;
}


bool System::hasSSE2() {
	init();
	return _sse2;
}


bool System::hasMMX() {
	init();
	return _mmx;
}


bool System::has3DNow() {
	init();
	return _3dnow;
}


const std::string& System::cpuVendor() {
	init();
	return _cpuVendor;
}


G3DEndian System::machineEndian() {
    init();
    return _machineEndian;
}

const std::string& System::operatingSystem() {
    init();
    return _operatingSystem;
}
        

const std::string& System::cpuArchitecture() {
    init();
    return _cpuArch;
}


void init() {

	if (initialized) {
		return;
	}

	initialized = true;

    unsigned long eaxreg, ebxreg, ecxreg, edxreg;

	char cpuVendorTmp[13];
    (void)cpuVendorTmp;
 
	// First of all we check if the CPUID command is available
	checkForCPUID();

    // Figure out if this machine is little or big endian.
    {
        int32 a = 1;
        if (*(uint8*)&a == 1) {
            _machineEndian = G3D_LITTLE_ENDIAN;
        } else {
            _machineEndian = G3D_BIG_ENDIAN;
        }
    }

	if (_cpuID) {
        // Process the CPUID information

	    // We read the standard CPUID level 0x00000000 which should
	    // be available on every x86 processor.  This fills out
        // a string with the processor vendor tag.
	    #ifdef _MSC_VER
		    __asm {
			    mov eax, 0
			    cpuid
			    mov eaxreg, eax
			    mov ebxreg, ebx
			    mov edxreg, edx
			    mov ecxreg, ecx
		    }
        #elif defined(__GNUC__) && defined(i386)
            // TODO: linux
            ebxreg = 0;
            edxreg = 0;
            ecxreg = 0;
        #else
            ebxreg = 0;
            edxreg = 0;
            ecxreg = 0;
        #endif

		// Then we connect the single register values to the vendor string
		*((unsigned long *) cpuVendorTmp)       = ebxreg;
		*((unsigned long *) (cpuVendorTmp + 4)) = edxreg;
		*((unsigned long *) (cpuVendorTmp + 8)) = ecxreg;
		cpuVendorTmp[12] = '\0';
		_cpuVendor = cpuVendorTmp;

		// We can also read the max. supported standard CPUID level
		maxSupportedCPUIDLevel = eaxreg & 0xFFFF;

		// Then we read the ext. CPUID level 0x80000000
	    #ifdef _MSC_VER
		    __asm {
			    mov eax, 0x80000000
			    cpuid
			    mov eaxreg, eax
		    }
        #elif defined(__GNUC__) && defined(i386)
            // TODO: Linux
            eaxreg = 0;
        #else
            eaxreg = 0;
        #endif

		// ...to check the max. supported extended CPUID level
		maxSupportedExtendedLevel = eaxreg;

		// Then we switch to the specific processor vendors.
        // Fill out _cpuArch based on this information.  It will
        // be overwritten by the next block of code on Windows,
        // but on Linux will stand.
		switch (ebxreg)	{
		case 0x756E6547:	// GenuineIntel
            _cpuArch = "Intel Processor";
			break;
		
		case 0x68747541:	// AuthenticAMD
            _cpuArch = "AMD Processor";
			break;

		case 0x69727943:	// CyrixInstead
            _cpuArch = "Cyrix Processor";
            break;

		default:
            _cpuArch = "Unknown Processor Vendor";
			break;
		}
    }

    #ifdef G3D_WIN32
        SYSTEM_INFO systemInfo;
        GetSystemInfo(&systemInfo);
        char* arch;
        switch (systemInfo.wProcessorArchitecture) {
        case PROCESSOR_ARCHITECTURE_INTEL:
            arch = "Intel";
            break;
    
        case PROCESSOR_ARCHITECTURE_MIPS:
            arch = "MIPS";
            break;

        case PROCESSOR_ARCHITECTURE_ALPHA:
            arch = "Alpha";
            break;

        case PROCESSOR_ARCHITECTURE_PPC:
            arch = "Power PC";
            break;

        default:
            arch = "Unknown";
        }

        _cpuArch = format(
                    "%d x %d-bit %s processor",
                    systemInfo.dwNumberOfProcessors,
                    (int)(::log((uint32)systemInfo.lpMaximumApplicationAddress) / ::log(2) + 2),
                    arch);

        OSVERSIONINFO osVersionInfo;
        osVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
        bool success = GetVersionEx(&osVersionInfo) != 0;

        if (success) {
            _operatingSystem = format("Windows %d.%d build %d Platform %d %s",
                osVersionInfo.dwMajorVersion, 
                osVersionInfo.dwMinorVersion,
                osVersionInfo.dwBuildNumber,
                osVersionInfo.dwPlatformId,
                osVersionInfo.szCSDVersion);
        } else {
            _operatingSystem = "Windows";
        }
    
    #elif defined(G3D_LINUX)

        _operatingSystem = "Linux";

    #elif defined(G3D_OSX)

        _operatingSystem = "OS X";
        
	#endif

    initTime();

	getStandardProcessorExtensions();
}


void checkForCPUID() {
	unsigned long bitChanged;

	// We've to check if we can toggle the flag register bit 21.
	// If we can't the processor does not support the CPUID command.

	#ifdef _MSC_VER
		__asm {
			pushfd
			pop   eax
			mov   ebx, eax
			xor   eax, 0x00200000 
			push  eax
			popfd
			pushfd
			pop   eax
			xor   eax, ebx 
			mov   bitChanged, eax
		}

	#elif defined(__GNUC__) && defined(i386)
        // Linux
        int has_CPUID = 0;
	__asm__ (
"push %%ecx\n"
"        pushfl                      # Get original EFLAGS             \n"
"        popl    %%eax                                                 \n"
"        movl    %%eax,%%ecx                                           \n"
"        xorl    $0x200000,%%eax     # Flip ID bit in EFLAGS           \n"
"        pushl   %%eax               # Save new EFLAGS value on stack  \n"
"        popfl                       # Replace current EFLAGS value    \n"
"        pushfl                      # Get new EFLAGS                  \n"
"        popl    %%eax               # Store new EFLAGS in EAX         \n"
"        xorl    %%ecx,%%eax         # Can not toggle ID bit,          \n"
"        jz      1f                  # Processor=80486                 \n"
"        movl    $1,%0               # We have CPUID support           \n"
"1:                                                                    \n"
"pop %%ecx\n"
	: "=r" (has_CPUID)
	:
	: "%eax", "%ecx"
	);
        _cpuID = (has_CPUID != 0);

    #else		
		// Unknown architecture
		_cpuID = false;
	
	#endif

	_cpuID = ((bitChanged) ? true : false);
}


void getStandardProcessorExtensions() {
    if (! _cpuID) {
        return;
    }

	unsigned long features;

    // Invoking CPUID with '1' in EAX fills out edx with a bit string.
    // The bits of this value indicate the presence or absence of 
    // useful processor features.
    #ifdef _MSC_VER
        // Windows

	    __asm {
            push eax
            push ebx
            push ecx
            push edx
		    mov eax, 1
		    cpuid
		    mov features, edx
            pop edx
            pop ecx
            pop ebx
            pop eax
	    }

    #elif defined(__GNUC__) && defined(i386)
        // Linux
	__asm__ (
"push %%eax\n"
"push %%ebx\n"
"push %%ecx\n"
"push %%edx\n"
"        xorl    %%eax,%%eax                                           \n"
"        incl    %%eax                                                 \n"
"        cpuid                       # Get family/model/stepping/features\n"
"        movl    %%edx,%0                                              \n"
"pop %%edx\n"
"pop %%ecx\n"
"pop %%ebx\n"
"pop %%eax\n"
	: "=r" (features)
	:
	: "%eax", "%ebx", "%ecx", "%edx"
	);

    #else
        // Other
        features = 0;
    #endif
    
	// FPU_FloatingPointUnit							= checkBit(features, 0);
	// VME_Virtual8086ModeEnhancements					= checkBit(features, 1);
	// DE_DebuggingExtensions							= checkBit(features, 2);
	// PSE_PageSizeExtensions							= checkBit(features, 3);
	// TSC_TimeStampCounter								= checkBit(features, 4);
	// MSR_ModelSpecificRegisters						= checkBit(features, 5);
	// PAE_PhysicalAddressExtension						= checkBit(features, 6);
	// MCE_MachineCheckException						= checkBit(features, 7);
	// CX8_COMPXCHG8B_Instruction						= checkBit(features, 8);
	// APIC_AdvancedProgrammableInterruptController		= checkBit(features, 9);
	// APIC_ID											= (ebxreg >> 24) & 0xFF;
	// SEP_FastSystemCall								= checkBit(features, 11);
	// MTRR_MemoryTypeRangeRegisters					= checkBit(features, 12);
	// PGE_PTE_GlobalFlag								= checkBit(features, 13);
	// MCA_MachineCheckArchitecture						= checkBit(features, 14);
	// CMOV_ConditionalMoveAndCompareInstructions		= checkBit(features, 15);

    // (According to SDL)
	_rdtsc						                    	= checkBit(features, 16);

	// PSE36_36bitPageSizeExtension						= checkBit(features, 17);
	// PN_ProcessorSerialNumber							= checkBit(features, 18);
	// CLFSH_CFLUSH_Instruction							= checkBit(features, 19);
	// CLFLUSH_InstructionCacheLineSize					= (ebxreg >> 8) & 0xFF;
	// DS_DebugStore									= checkBit(features, 21);
	// ACPI_ThermalMonitorAndClockControl				= checkBit(features, 22);
	_mmx												= checkBit(features, 23);
	// FXSR_FastStreamingSIMD_ExtensionsSaveRestore		= checkBit(features, 24);
	_sse												= checkBit(features, 25);
	_sse2												= checkBit(features, 26);
	// SS_SelfSnoop										= checkBit(features, 27);
	// HT_HyperThreading								= checkBit(features, 28);
	// HT_HyterThreadingSiblings = (ebxreg >> 16) & 0xFF;
	// TM_ThermalMonitor								= checkBit(features, 29);
	// IA64_Intel64BitArchitecture						= checkBit(features, 30);
	_3dnow                                              = checkBit(features, 31);
}


#undef checkBit



/** Michael Herf's fast memcpy */
#if defined(G3D_WIN32) && defined(SSE)

// On x86 processors, use MMX
void memcpy2(void *dst, const void *src, int nbytes) {
	int remainingBytes = nbytes;

	if (nbytes > 64) {
		_asm { 
			mov esi, src 
			mov edi, dst 
			mov ecx, nbytes 
			shr ecx, 6 // 64 bytes per iteration 

	loop1: 
			movq mm1,  0[ESI] // Read in source data 
			movq mm2,  8[ESI]
			movq mm3, 16[ESI]
			movq mm4, 24[ESI] 
			movq mm5, 32[ESI]
			movq mm6, 40[ESI]
			movq mm7, 48[ESI]
			movq mm0, 56[ESI]

			movntq  0[EDI], mm1 // Non-temporal stores 
			movntq  8[EDI], mm2 
			movntq 16[EDI], mm3 
			movntq 24[EDI], mm4 
			movntq 32[EDI], mm5 
			movntq 40[EDI], mm6 
			movntq 48[EDI], mm7 
			movntq 56[EDI], mm0 

			add esi, 64 
			add edi, 64 
			dec ecx 
			jnz loop1 

			emms
		}
		remainingBytes -= ((nbytes >> 6) << 6); 
	}

	if (remainingBytes > 0) {
		// Memcpy the rest
		memcpy((uint8*)dst + (nbytes - remainingBytes), (const uint8*)src + (nbytes - remainingBytes), remainingBytes); 
	}
}


// On x86 processors, use MMX
static void memcpy4(void *dst, const void *src, int nbytes) {
	static char _tbuf[2048];
	static char *tbuf = _tbuf;

	int remainingBytes = nbytes;

	if (nbytes > 2048) {

		__asm {
			mov esi, src 
			mov ecx, nbytes 
			mov ebx, ecx 
			shr ebx, 11 // 2048 bytes at a time 
			mov edi, dst 

	loop2k: // Copy 2k into temporary buffer 
			push edi 
			mov edi, tbuf 
			mov ecx, 2048 
			shr ecx, 6 

	loopMemToL1: 
			prefetchnta 64[ESI] // Prefetch next loop, non-temporal 
			prefetchnta 96[ESI] 

			movq mm1,  0[ESI] // Read in source data 
			movq mm2,  8[ESI] 
			movq mm3, 16[ESI] 
			movq mm4, 24[ESI] 
			movq mm5, 32[ESI] 
			movq mm6, 40[ESI] 
			movq mm7, 48[ESI] 
			movq mm0, 56[ESI] 

			movq  0[EDI], mm1 // Store into L1 
			movq  8[EDI], mm2 
			movq 16[EDI], mm3 
			movq 24[EDI], mm4 
			movq 32[EDI], mm5 
			movq 40[EDI], mm6 
			movq 48[EDI], mm7 
			movq 56[EDI], mm0 
			add esi, 64 
			add edi, 64 
			dec ecx 
			jnz loopMemToL1 

			pop edi // Now copy from L1 to system memory 
			push esi 
			mov esi, tbuf 
			mov ecx, 2048 
			shr ecx, 6 

	loopL1ToMem: 
			movq mm1, 0[ESI] // Read in source data from L1 
			movq mm2, 8[ESI] 
			movq mm3, 16[ESI] 
			movq mm4, 24[ESI] 
			movq mm5, 32[ESI] 
			movq mm6, 40[ESI] 
			movq mm7, 48[ESI] 
			movq mm0, 56[ESI] 

			movntq 0[EDI], mm1 // Non-temporal stores 
			movntq 8[EDI], mm2 
			movntq 16[EDI], mm3 
			movntq 24[EDI], mm4 
			movntq 32[EDI], mm5 
			movntq 40[EDI], mm6 
			movntq 48[EDI], mm7 
			movntq 56[EDI], mm0 

			add esi, 64 
			add edi, 64 
			dec ecx 
			jnz loopL1ToMem 

			pop esi // Do next 2k block 
			dec ebx 
			jnz loop2k 

			emms
		}

		remainingBytes -= (nbytes / 2048) * 2048; 
	}

	if (remainingBytes > 0) {
		// Memcpy the rest
		memcpy((uint8*)dst + (nbytes - remainingBytes), (const uint8*)src + (nbytes - remainingBytes), remainingBytes); 
	}
}

#else

    // Fall back to memcpy
    void memcpy2(void *dst, const void *src, int nbytes) {
	    memcpy(dst, src, nbytes);
    }

    static void memcpy4(void *dst, const void *src, int nbytes) {
	    memcpy(dst, src, nbytes);
    }

#endif


void System::memcpy(void* dst, const void* src, size_t numBytes) {
	if (System::hasSSE() && System::hasMMX()) {
		G3D::memcpy2(dst, src, numBytes);
	} else {
		::memcpy(dst, src, numBytes);
	}
}


/** Michael Herf's fastest memset. n32 must be filled with the same
    character repeated. */
#if defined(G3D_WIN32) && defined(SSE)

// On x86 processors, use MMX
void memfill(void *dst, int n32, unsigned long i) {

    int originalSize = i;
	int bytesRemaining = i;

	if (i > 16) {
        
        bytesRemaining = i % 16;
        i -= bytesRemaining;
		__asm {
			movq mm0, n32
			punpckldq mm0, mm0
			mov edi, dst

		loopwrite:

			movntq 0[edi], mm0
			movntq 8[edi], mm0

			add edi, 16
			sub i, 16
			jg loopwrite

			emms
		}
	}

	if (bytesRemaining > 0) {
		memset((uint8*)dst + (originalSize - bytesRemaining), n32, bytesRemaining); 
	}
}

#else

// For non x86 processors, we fall back to the standard memset
void memfill(void *dst, int n32, unsigned long i) {
	memset(dst, n32, i);
}

#endif


void System::memset(void* dst, uint8 value, size_t numBytes) {
	if (System::hasSSE() && System::hasMMX()) {
		uint32 v = value;
		v = v + (v << 8) + (v << 16) + (v << 24); 
		G3D::memfill(dst, v, numBytes);
	} else {
		::memset(dst, value, numBytes);
	}
}


std::string System::currentProgramFilename() {
    char filename[2048];

    #ifdef G3D_WIN32
    {
        GetModuleFileName(NULL, filename, sizeof(filename));
    } 
    #else
    {
	    int ret = readlink("/proc/self/exe", filename, sizeof(filename));
	    
	    // In case of an error, leave the handling up to the caller
        if (ret == -1) {
		    return "";
        }
	    
        debugAssert(sizeof(filename) > ret);
	    
	    // Ensure proper NULL termination
	    filename[ret] = 0;	    
    }
    #endif

    return filename;
}


void System::sleep(RealTime t) {
    #ifdef G3D_WIN32
        Sleep((int)(t * 1e3));
    #else
        usleep((int)(t * 1e6));
    #endif
}


void System::consoleClearScreen() {
    #ifdef G3D_WIN32
        system("cls");
    #else
        system("clear");
    #endif
}


bool System::consoleKeyPressed() {
    #ifdef G3D_WIN32
    
        return _kbhit() != 0;

    #else
    
        static const int STDIN = 0;
        static bool initialized = false;

        if (! initialized) {
            // Use termios to turn off line buffering
            termios term;
            tcgetattr(STDIN, &term);
            term.c_lflag &= ~ICANON;
            tcsetattr(STDIN, TCSANOW, &term);
            setbuf(stdin, NULL);
            initialized = true;
        }

        #ifdef G3D_LINUX

            int bytesWaiting;
            ioctl(STDIN, FIONREAD, &bytesWaiting);
            return bytesWaiting;

        #else

            timeval timeout;
            fd_set rdset;

            FD_ZERO(&rdset);
            FD_SET(STDIN, &rdset);
            timeout.tv_sec  = 0;
            timeout.tv_usec = 0;

            return select(STDIN + 1, &rdset, NULL, NULL, &timeout);
        #endif
    #endif
}


int System::consoleReadKey() {
    #ifdef G3D_WIN32
        return _getch();
    #else
        char c;
        read(0, &c, 1);
        return c;
    #endif
}


void initTime() {
    #ifdef G3D_WIN32
        if (QueryPerformanceFrequency(&_counterFrequency)) {
            QueryPerformanceCounter(&_start);
        }

        struct _timeb t;
        _ftime(&t);

        realWorldGetTickTime0 = (RealTime)t.time - t.timezone * MINUTE + (t.dstflag ? HOUR : 0);

    #else
        gettimeofday(&_start, NULL);
        // "sse" = "seconds since epoch".  The time
        // function returns the seconds since the epoch
        // GMT (perhaps more correctly called UTC). 
        time_t gmt = time(NULL);
        
        // No call to free or delete is needed, but subsequent
        // calls to asctime, ctime, mktime, etc. might overwrite
        // local_time_vals. 
        tm* localTimeVals = localtime(&gmt);
    
        time_t local = gmt;
        
        if (localTimeVals) {
            // tm_gmtoff is already corrected for daylight savings.
            local = local + localTimeVals->tm_gmtoff;
        }
        
        realWorldGetTickTime0 = local;
    #endif
}


RealTime System::getTick() { 
    init();
    #ifdef G3D_WIN32
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);

        return (RealTime)(now.QuadPart - _start.QuadPart) /
                _counterFrequency.QuadPart;
    #else
        // Linux resolution defaults to 100Hz.
        // There is no need to do a separate RDTSC call as gettimeofday
        // actually uses RDTSC when on systems that support it, otherwise
        // it uses the system clock.
        struct timeval now;
        gettimeofday(&now, NULL);

        return (now.tv_sec  - _start.tv_sec) +
               (now.tv_usec - _start.tv_usec) / 1e6;
    #endif
}


RealTime System::getLocalTime() {
    return getTick() + realWorldGetTickTime0;
}


void* System::alignedMalloc(size_t bytes, size_t alignment) {
    alwaysAssertM(isPow2(alignment), "alignment must be a power of 2");

    // We must align to at least a word boundary.
    alignment = iMax(alignment, sizeof(void *));

    // Pad the allocation size with the alignment size.
    // This also guarantees room to store the redirect pointer
    size_t truePtr = (size_t)malloc(bytes + alignment + sizeof(void*));

    if (truePtr == 0) {
        // malloc returned NULL
        return NULL;
    }

    debugAssert(isValidHeapPointer((void*)truePtr));

    // The return pointer will be the next aligned location (we must at least
    // leave space for the redirect pointer, however).
    size_t  alignedPtr = truePtr + sizeof(void*);
        
    while ((alignedPtr & (alignment - 1)) != 0) {
        alignedPtr += sizeof(void*);
    }

    // Immediately before the aligned location, write the true array location
    size_t* redirectPtr = (size_t *)(alignedPtr - sizeof(void *));
    redirectPtr[0] = truePtr;

    return (void *)alignedPtr;
}


void System::alignedFree(void* _ptr) {

    if (_ptr == NULL) {
        return;
    }

    size_t alignedPtr = (size_t)_ptr;

    // Back up one word from the pointer the user passed in.
    // This is a pointer to the true start of the memory block.
    size_t* redirectPtr = (size_t*)(alignedPtr - sizeof(void *));

    // Dereference that pointer so that ptr = true start
    void* truePtr = (void*)redirectPtr[0];

    debugAssert(isValidHeapPointer((void*)truePtr));
    free(truePtr);
}


}  // namespace
