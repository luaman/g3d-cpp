/** 
  @file System.cpp
 
  @maintainer Morgan McGuire, matrix@graphics3d.com

  Note: every routine must call init() first.

  There are two kinds of detection used in this file.  At compile time,
  the _MSC_VER #define is used to determine whether x86 assembly can be used at all.
  At runtime, processor detection is used to determine if we can safely
  call the routines that use that assembly.

  @cite Rob Wyatt http://www.gamasutra.com/features/wyatts_world/19990709/processor_detection_01.htm
  @cite Benjamin Jurke http://www.flipcode.com/cgi-bin/msg.cgi?showThread=COTD-ProcessorDetectionClass&forum=cotd&id=-1
  @cite Michael Herf http://www.stereopsis.com/memcpy.html

  @created 2003-01-25
  @edited  2003-12-04
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

    // Search for the data
    for (int count = 0; count < 5; ++count) {
        potential.append("../");
    }

    std::string lpath = "libraries/g3d-6_00-b9/";
    #ifdef G3D_WIN32
        potential.append(std::string("c:/") + lpath);
        potential.append(std::string("d:/") + lpath);
        potential.append(std::string("g:/") + lpath);
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
static LARGE_INTEGER        _start;
static LARGE_INTEGER        _counterFrequency;
#else
static struct timeval       _start;
#endif

static int	 	 maxSupportedCPUIDLevel = 0;
static int    maxSupportedExtendedLevel = 0;

#define CheckBit(var, bit)   ((var & (1 << bit)) ? true : false)

/** Checks if the CPUID command is available on the processor (called from init) */
static void checkForCPUID();

/** ReadRead the standard processor extensions */
static void getStandardProcessorExtensions();

/** Perform processor identification and initialize the library (if not
    already initialized). */
static void init();

/** Called from init */
static void initIntel();
static void initAMD();
static void initUnknown();
static void initTime();



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

	#ifdef G3D_WIN32
        unsigned long eaxreg, ebxreg, ecxreg, edxreg;
    #endif

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

	if (! _cpuID) {
		return;
	}

	#if defined(G3D_WIN32)
		// We read the standard CPUID level 0x00000000 which should
		// be available on every x86 processor
		__asm {
			mov eax, 0
			cpuid
			mov eaxreg, eax
			mov ebxreg, ebx
			mov edxreg, edx
			mov ecxreg, ecx
		}

		// Then we connect the single register values to the vendor string
		*((unsigned long *) cpuVendorTmp)       = ebxreg;
		*((unsigned long *) (cpuVendorTmp + 4)) = edxreg;
		*((unsigned long *) (cpuVendorTmp + 8)) = ecxreg;
		cpuVendorTmp[12] = '\0';
		_cpuVendor = cpuVendorTmp;

		// We can also read the max. supported standard CPUID level
		maxSupportedCPUIDLevel = eaxreg & 0xFFFF;

		// Then we read the ext. CPUID level 0x80000000
		__asm {
			mov eax, 0x80000000
			cpuid
			mov eaxreg, eax
		}

		// ...to check the max. supported extended CPUID level
		maxSupportedExtendedLevel = eaxreg;

		// Then we switch to the specific processor vendors
		switch (ebxreg)	{
		case 0x756E6547:	// GenuineIntel
			initIntel();
			break;
		
		case 0x68747541:	// AuthenticAMD
			initAMD();
			break;

		case 0x69727943:	// CyrixInstead
		default:
			initUnknown();
			break;
		}

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

        _operatingSystem = "Linux";
        
	#endif

    initTime();

	getStandardProcessorExtensions();
}


void initIntel() {
}


void initAMD() {
	// Check if there is extended CPUID level support
	if ((unsigned long)maxSupportedExtendedLevel >= 0x80000001) {
		unsigned long edxreg = 0;

		// If we can access the extended CPUID level 0x80000001 we get the
		// edx register


#ifdef _MSC_VER
		__asm
		{
			mov eax, 0x80000001
			cpuid
			mov edxreg, edx
		}
#else
        /*
          TODO: james
		asm(
		"mov 0x8000001, %%eax                             "
		"cpuid                                            "
		"mov %%edx, %0                                    "
		: "r"(edxreg)
		);
        */
#endif
		
		// Now we can mask some AMD specific cpu extensions
		// EMMX_MultimediaExtensions                 = CheckBit(edxreg, 22);
		// AA64_AMD64BitArchitecture                 = CheckBit(edxreg, 29);
		//CPUInfo._Ext._E3DNOW_InstructionExtensions = CheckBit(edxreg, 30);
		_3dnow                                       = CheckBit(edxreg, 31);
	}

}


void initUnknown() {
}


void checkForCPUID() {
	unsigned long bitChanged;

	#ifdef _MSC_VER
		// We've to check if we can toggle the flag register bit 21
		// If we can't the processor does not support the CPUID command
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
	#else
		
		// Unknown architecture
		_cpuID = false;
	
	#endif

	_cpuID = ((bitChanged) ? true : false);
}

#ifdef _MSC_VER 
void getStandardProcessorExtensions() {
	unsigned long ebxreg, edxreg;

	// We just get the standard CPUID level 0x00000001 which should be
	// available on every x86 processor
	__asm {
		mov eax, 1
		cpuid
		mov ebxreg, ebx
		mov edxreg, edx
	}
    
	// Then we mask some bits
	// FPU_FloatingPointUnit							= CheckBit(edxreg, 0);
	// VME_Virtual8086ModeEnhancements					= CheckBit(edxreg, 1);
	// DE_DebuggingExtensions							= CheckBit(edxreg, 2);
	// PSE_PageSizeExtensions							= CheckBit(edxreg, 3);
	// TSC_TimeStampCounter								= CheckBit(edxreg, 4);
	// MSR_ModelSpecificRegisters						= CheckBit(edxreg, 5);
	// PAE_PhysicalAddressExtension						= CheckBit(edxreg, 6);
	// MCE_MachineCheckException						= CheckBit(edxreg, 7);
	// CX8_COMPXCHG8B_Instruction						= CheckBit(edxreg, 8);
	// APIC_AdvancedProgrammableInterruptController		= CheckBit(edxreg, 9);
	// APIC_ID											= (ebxreg >> 24) & 0xFF;
	// SEP_FastSystemCall								= CheckBit(edxreg, 11);
	// MTRR_MemoryTypeRangeRegisters					= CheckBit(edxreg, 12);
	// PGE_PTE_GlobalFlag								= CheckBit(edxreg, 13);
	// MCA_MachineCheckArchitecture						= CheckBit(edxreg, 14);
	// CMOV_ConditionalMoveAndCompareInstructions		= CheckBit(edxreg, 15);
	// FGPAT_PageAttributeTable							= CheckBit(edxreg, 16);
	// PSE36_36bitPageSizeExtension						= CheckBit(edxreg, 17);
	// PN_ProcessorSerialNumber							= CheckBit(edxreg, 18);
	// CLFSH_CFLUSH_Instruction							= CheckBit(edxreg, 19);
	// CLFLUSH_InstructionCacheLineSize					= (ebxreg >> 8) & 0xFF;
	// DS_DebugStore									= CheckBit(edxreg, 21);
	// ACPI_ThermalMonitorAndClockControl				= CheckBit(edxreg, 22);
	_mmx												= CheckBit(edxreg, 23);
	// FXSR_FastStreamingSIMD_ExtensionsSaveRestore		= CheckBit(edxreg, 24);
	_sse												= CheckBit(edxreg, 25);
	_sse2												= CheckBit(edxreg, 26);
	// SS_SelfSnoop										= CheckBit(edxreg, 27);
	// HT_HyperThreading								= CheckBit(edxreg, 28);
	// HT_HyterThreadingSiblings = (ebxreg >> 16) & 0xFF;
	// TM_ThermalMonitor								= CheckBit(edxreg, 29);
	// IA64_Intel64BitArchitecture						= CheckBit(edxreg, 30);
}

#else

void getStandardProcessorExtensions() {
}

#endif


#undef CheckBit



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
    #else
        gettimeofday(&_start, NULL);
    #endif
}


RealTime System::getTick() { 
    #ifdef G3D_WIN32
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);

        return RealTime(((now.QuadPart-_start.QuadPart)*1000)/_counterFrequency.QuadPart);
    #else
        struct timeval now;
        gettimeofday(&now, NULL);

        return ((now.tv_sec-_start.tv_sec)*1000+(now.tv_usec-_start.tv_usec)/1000);
    #endif
}

RealTime System::getLocalTime() {
  
    #ifdef G3D_WIN32
        struct _timeb t;
        _ftime(&t);

        return t.time - t.timezone * MINUTE + (t.dstflag ? HOUR : 0);

    #else

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
        
        return RealTime(local);

    #endif
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
