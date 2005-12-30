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
  @edited  2005-12-29
 */

#include "G3D/platform.h"
#include "G3D/System.h"
#include "G3D/debug.h"
#include "G3D/g3derror.h"
#include "G3D/fileutils.h"
#include "G3D/TextOutput.h"

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
    #include <pthread.h>

#elif defined(G3D_OSX)

    #include <stdlib.h>
    #include <stdio.h>
    #include <errno.h>
    #include <sys/types.h>
    #include <sys/select.h>
    #include <termios.h>
    #include <unistd.h>
    #include <sys/time.h>
    #include <pthread.h>

    #include <sstream>
    #include <CoreServices/CoreServices.h>
#endif

#if defined(SSE)
    #include <xmmintrin.h>
#endif

namespace G3D {

uint32 crc32(const void* byte, size_t numBytes) {
    static const uint32 crc32Table[256] = {
	    0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA,
	    0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
	    0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
	    0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
	    0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE,
	    0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
	    0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC,
	    0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
	    0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
	    0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
	    0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940,
	    0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
	    0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116,
	    0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
	    0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
	    0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,

	    0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A,
	    0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
	    0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818,
	    0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
	    0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
	    0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
	    0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C,
	    0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
	    0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2,
	    0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
	    0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
	    0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
	    0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086,
	    0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
	    0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4,
	    0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,

	    0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
	    0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
	    0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8,
	    0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
	    0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE,
	    0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
	    0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
	    0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
	    0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252,
	    0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
	    0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60,
	    0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
	    0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
	    0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
	    0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04,
	    0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,

	    0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A,
	    0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
	    0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
	    0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
	    0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E,
	    0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
	    0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C,
	    0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
	    0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
	    0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
	    0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0,
	    0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
	    0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6,
	    0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
	    0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
	    0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D,
    };

    // By definition, initialize to all binary 1's
    uint32 value = 0xFFFFFFFF;

    for (size_t i = 0; i < numBytes; ++i) {
        value = (value >> 8 ) ^ crc32Table[static_cast<const uint8*>(byte)[i] ^ (value & 0xFF)];
    }

    return value;
}


std::string demoFindData(bool errorIfNotFound) {

    // Directories that might contain the data
    Array<std::string> potential;

    // Look back up the directory tree
    std::string x = "../";
    std::string f = "";
    for (int i = 0; i < 6; ++i) {
        potential.append(f);
        f = f + x;
    }
    
    // Hard-code in likely install directories
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
        potential.append(std::string("e:/") + lpath);
        potential.append(std::string("f:/") + lpath);
        potential.append(std::string("g:/") + lpath);
        potential.append(std::string("x:/") + lpath);
        potential.append(std::string("c:/users/morgan/data/"));
    #else
        potential.append(std::string("/course/cs224/") + lpath);
        potential.append(std::string("/map/gfx0/common/games/") + lpath);
    #endif

    // Scan all potentials for the font directory
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


static bool                                     _rdtsc              = false;
static bool                                     _mmx                = false;
static bool                                     _sse                = false;
static bool                                     _sse2               = false;
static bool                                     _3dnow              = false;
static char                                     _cpuVendorCstr[1024] = {'U', 'n', 'k', 'n', 'o', 'w', 'n', '\0'};
static bool                                     _cpuID              = false;
static G3DEndian                                _machineEndian      = G3D_LITTLE_ENDIAN;
static char                                     _cpuArchCstr[1024];
static char                                     _operatingSystemCstr[1024];

#ifdef G3D_WIN32
/** Used by getTick() for timing */
static LARGE_INTEGER                            _start;
static LARGE_INTEGER                            _counterFrequency;
#else
static struct timeval                           _start;
#endif

static char                                     versionCstr[1024];
System::OutOfMemoryCallback                     System::outOfMemoryCallback = NULL;

#ifdef G3D_OSX
    long System::m_OSXCPUSpeed;
    double System::m_secondsPerNS;
#endif

/** The Real-World time of System::getTick() time 0.  Set by initTime */
static RealTime             realWorldGetTickTime0;


static int               maxSupportedCPUIDLevel = 0;
static int            maxSupportedExtendedLevel = 0;

#define checkBit(var, bit)   ((var & (1 << bit)) ? true : false)

/** Checks if the CPUID command is available on the processor (called from init) */
static void checkForCPUID();

/** ReadRead the standard processor extensions.  Called from init(). */
static void getStandardProcessorExtensions();

/** Called from init */
static void initTime();


bool System::hasCPUID() {
    init();
    return _cpuID;
}

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
    static const std::string _cpuVendor = _cpuVendorCstr;
    return _cpuVendor;
}


G3DEndian System::machineEndian() {
    init();
    return _machineEndian;
}

const std::string& System::operatingSystem() {
    init();
    static const std::string _operatingSystem =_operatingSystemCstr;
    return _operatingSystem;
}
        

const std::string& System::cpuArchitecture() {
    init();
    static const std::string _cpuArch = _cpuArchCstr;
    return _cpuArch;
}

const std::string& System::version() {
    init();

    static const std::string _version = versionCstr;
    return _version;
}


void System::init() {
    // Cannot use most G3D data structures or utility functions in here because
    // they are not initialized.

    static bool initialized = false;

    if (initialized) {
        return;
    }

    initialized = true;

    if ((G3D_VER % 100) != 0) {
        sprintf(versionCstr, "G3D %d.%02d beta %d",
            G3D_VER / 10000,
            (G3D_VER / 100) % 100,
            G3D_VER % 100);
    } else {
        sprintf(versionCstr, "G3D %d.%02d",
            G3D_VER / 10000,
            (G3D_VER / 100) % 100);
    }

    unsigned long eaxreg, ebxreg, ecxreg, edxreg;
    eaxreg = ebxreg = ecxreg = edxreg = 0;
 
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
                push eax
                push ebx
                push ecx
                push edx
                mov eax, 0
                cpuid
                mov eaxreg, eax
                mov ebxreg, ebx
                mov edxreg, edx
                mov ecxreg, ecx
                pop edx
                pop ecx
                pop ebx
                pop eax
            }
        #elif defined(__GNUC__) && defined(i386)
            asm (
                "movl $0, %%eax \n"
                "cpuid          \n"
                "movl %%eax, %0 \n"
                "movl %%ebx, %1 \n"
                "movl %%ecx, %2 \n"
                "movl %%edx, %3 \n"
                : 
                "=m" (eaxreg), 
                "=m" (ebxreg), 
                "=m" (ecxreg), 
                "=m" (edxreg) 
                :
                // No inputs
                : 
                "%eax","%ebx","%ecx","%edx" );
        #else
            ebxreg = 0;
            edxreg = 0;
            ecxreg = 0;
        #endif

        // Then we connect the single register values to the vendor string
        *((unsigned long *) _cpuVendorCstr)       = ebxreg;
        *((unsigned long *) (_cpuVendorCstr + 4)) = edxreg;
        *((unsigned long *) (_cpuVendorCstr + 8)) = ecxreg;
        _cpuVendorCstr[12] = '\0';

        // We can also read the max. supported standard CPUID level
        maxSupportedCPUIDLevel = eaxreg & 0xFFFF;

        // Then we read the ext. CPUID level 0x80000000
        #ifdef _MSC_VER
            __asm {
                push eax
                push ebx
                push ecx
                push edx
                mov eax, 0x80000000
                cpuid
                mov eaxreg, eax
                pop edx
                pop ecx
                pop ebx
                pop eax
            }
        #elif defined(__GNUC__) && defined(i386)
            asm (
                "movl $0x80000000, %%eax \n"
                "cpuid                   \n"
                "movl %%eax, %0          \n"
                : 
                "=m" (eaxreg)
                :
                // No inputs
                : 
                "%eax", "%ebx", "%ecx", "%edx" );
        #else
            eaxreg = 0;
        #endif

        // ...to check the max. supported extended CPUID level
        maxSupportedExtendedLevel = eaxreg;

        // Then we switch to the specific processor vendors.
        // Fill out _cpuArch based on this information.  It will
        // be overwritten by the next block of code on Windows,
        // but on Linux will stand.
        switch (ebxreg) {
        case 0x756E6547:        // GenuineIntel
            strcpy(_cpuArchCstr, "Intel Processor");
            break;
            
        case 0x68747541:        // AuthenticAMD
            strcpy(_cpuArchCstr, "AMD Processor");
            break;

        case 0x69727943:        // CyrixInstead
            strcpy(_cpuArchCstr, "Cyrix Processor");
            break;

        default:
            strcpy(_cpuArchCstr, "Unknown Processor Vendor");
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

        uint32 maxAddr = (uint32)systemInfo.lpMaximumApplicationAddress;
        sprintf(_cpuArchCstr, "%d x %d-bit %s processor",
                    systemInfo.dwNumberOfProcessors,
                    (int)(::log((double)maxAddr) / ::log(2.0) + 2.0),
                    arch);

        OSVERSIONINFO osVersionInfo;
        osVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
        bool success = GetVersionEx(&osVersionInfo) != 0;

        if (success) {
            sprintf(_operatingSystemCstr, "Windows %d.%d build %d Platform %d %s",
                osVersionInfo.dwMajorVersion, 
                osVersionInfo.dwMinorVersion,
                osVersionInfo.dwBuildNumber,
                osVersionInfo.dwPlatformId,
                osVersionInfo.szCSDVersion);
        } else {
            strcpy(_operatingSystemCstr, "Windows");
        }
    
    #elif defined(G3D_LINUX)

        {
            // Shell out to the 'uname' command

            FILE* f = popen("uname -a", "r");

            int len = 100;
            char* r = (char*)::malloc(len * sizeof(char));
            fgets(r, len, f);
            // Remove trailing newline
            if (r[strlen(r) - 1] == '\n') {
                r[strlen(r) - 1] = '\0';
            }
            fclose(f);

            strcpy(_operatingSystemCstr, r);
            ::free(r);
        }

    #elif defined(G3D_OSX)

        //Operating System:
        SInt32 macVersion;
        Gestalt(gestaltSystemVersion, &macVersion);

        int major = (macVersion >> 8) & 0xFF;
        int minor = (macVersion >> 4) & 0xF;
        int revision = macVersion & 0xF;

        sprintf(_operatingSystemCstr, "OS X %x.%x.%x", major, minor, revision); 
                 
        //Clock Cycle Timing Information:
        Gestalt('pclk', &System::m_OSXCPUSpeed);
        m_secondsPerNS = 1.0 / 1.0e9;
        
        //System Architecture:
        SInt32 CPUtype;
        Gestalt('cpuf', &CPUtype);
        switch (CPUtype){
        case 0x0108:
            strcpy(_cpuArchCstr, "PPC G3");
            strcpy(_cpuVendorCstr, "Motorola");
            break;
        case 0x010C:
            strcpy(_cpuArchCstr, "PPC G4");
            strcpy(_cpuVendorCstr, "Motorola");
            break;
        case 0x0139:
            strcpy(_cpuArchCstr, "PPC G5");
            strcpy(_cpuVendorCstr, "IBM");
            break;
        }
            
    #endif

    initTime();

    getStandardProcessorExtensions();
}


void checkForCPUID() {
    unsigned long bitChanged = 0;

    // We've to check if we can toggle the flag register bit 21.
    // If we can't the processor does not support the CPUID command.
    
#   ifdef _MSC_VER
        __asm {
                push eax
                push ebx
                pushfd
                pushfd
                pop   eax
                mov   ebx, eax
                xor   eax, 0x00200000 
                push  eax
                popfd
                pushfd
                pop   eax
                popfd
                xor   eax, ebx 
                mov   bitChanged, eax
                pop ebx
                pop eax
        }

#    elif defined(__GNUC__) && defined(i386)
        // Linux
        __asm__ (
"        pushfl                      # Get original EFLAGS             \n"
"        pushfl                                                        \n"
"        popl    %%eax                                                 \n"
"        movl    %%eax, %%ecx                                          \n"
"        xorl    $0x200000, %%eax    # Flip ID bit in EFLAGS           \n"
"        pushl   %%eax               # Save new EFLAGS value on stack  \n"
"        popfl                       # Replace current EFLAGS value    \n"
"        pushfl                      # Get new EFLAGS                  \n"
"        popl    %%eax               # Store new EFLAGS in EAX         \n"
"        popfl                                                         \n"
"        xorl    %%ecx, %%eax        # Can not toggle ID bit,          \n"
"        movl    %%eax, %0           # We have CPUID support           \n"
        : "=m" (bitChanged)
        : // No inputs
        : "%eax", "%ecx"
        );

#    else               
       // Unknown architecture
        _cpuID = false;
#    endif

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
                "movl    $1, %%eax                                                 \n"
                "cpuid                       # Get family/model/stepping/features  \n"
                "movl    %%edx, %0                                                 \n"
                : 
                "=m" (features)
                : 
                // No inputs
                : 
                "%eax", "%ebx", "%ecx", "%edx" );

    #else
        // Other
        features = 0;
    #endif
    
        // FPU_FloatingPointUnit                                                        = checkBit(features, 0);
        // VME_Virtual8086ModeEnhancements                                      = checkBit(features, 1);
        // DE_DebuggingExtensions                                                       = checkBit(features, 2);
        // PSE_PageSizeExtensions                                                       = checkBit(features, 3);
        // TSC_TimeStampCounter                                                         = checkBit(features, 4);
        // MSR_ModelSpecificRegisters                                           = checkBit(features, 5);
        // PAE_PhysicalAddressExtension                                         = checkBit(features, 6);
        // MCE_MachineCheckException                                            = checkBit(features, 7);
        // CX8_COMPXCHG8B_Instruction                                           = checkBit(features, 8);
        // APIC_AdvancedProgrammableInterruptController         = checkBit(features, 9);
        // APIC_ID                                                                                      = (ebxreg >> 24) & 0xFF;
        // SEP_FastSystemCall                                                           = checkBit(features, 11);
        // MTRR_MemoryTypeRangeRegisters                                        = checkBit(features, 12);
        // PGE_PTE_GlobalFlag                                                           = checkBit(features, 13);
        // MCA_MachineCheckArchitecture                                         = checkBit(features, 14);
        // CMOV_ConditionalMoveAndCompareInstructions           = checkBit(features, 15);

    // (According to SDL)
        _rdtsc                                                                  = checkBit(features, 16);

        // PSE36_36bitPageSizeExtension                                         = checkBit(features, 17);
        // PN_ProcessorSerialNumber                                                     = checkBit(features, 18);
        // CLFSH_CFLUSH_Instruction                                                     = checkBit(features, 19);
        // CLFLUSH_InstructionCacheLineSize                                     = (ebxreg >> 8) & 0xFF;
        // DS_DebugStore                                                                        = checkBit(features, 21);
        // ACPI_ThermalMonitorAndClockControl                           = checkBit(features, 22);
        _mmx                                                                                            = checkBit(features, 23);
        // FXSR_FastStreamingSIMD_ExtensionsSaveRestore         = checkBit(features, 24);
        _sse                                                                                            = checkBit(features, 25);
        _sse2                                                                                           = checkBit(features, 26);
        // SS_SelfSnoop                                                                         = checkBit(features, 27);
        // HT_HyperThreading                                                            = checkBit(features, 28);
        // HT_HyterThreadingSiblings = (ebxreg >> 16) & 0xFF;
        // TM_ThermalMonitor                                                            = checkBit(features, 29);
        // IA64_Intel64BitArchitecture                                          = checkBit(features, 30);
        _3dnow                                              = checkBit(features, 31);
}


#undef checkBit



#if defined(SSE)

// Copy in 128 bytes chunks, where each chunk contains 8*float32x4 = 8 * 4 * 4 bytes = 128 bytes
//
//
void memcpySSE2(void* dst, const void* src, int nbytes) {
    int remainingBytes = nbytes;

    if (nbytes > 128) {

        // Number of chunks
        int N = nbytes / 128;

        float* restrict d = (float*)dst;
        const float* restrict s = (const float*)src;
    
        // Finish when the destination pointer has moved 8N elements 
        float* stop = d + (N * 8 * 4);

        while (d < stop) {
            // Inner loop unrolled 8 times
            const __m128 r0 = _mm_loadu_ps(s);
            const __m128 r1 = _mm_loadu_ps(s + 4);
            const __m128 r2 = _mm_loadu_ps(s + 8);
            const __m128 r3 = _mm_loadu_ps(s + 12);
            const __m128 r4 = _mm_loadu_ps(s + 16);
            const __m128 r5 = _mm_loadu_ps(s + 20);
            const __m128 r6 = _mm_loadu_ps(s + 24);
            const __m128 r7 = _mm_loadu_ps(s + 28);

            _mm_storeu_ps(d, r0);
            _mm_storeu_ps(d + 4, r1);
            _mm_storeu_ps(d + 8, r2);
            _mm_storeu_ps(d + 12, r3);
            _mm_storeu_ps(d + 16, r4);
            _mm_storeu_ps(d + 20, r5);
            _mm_storeu_ps(d + 24, r6);
            _mm_storeu_ps(d + 28, r7);

            s += 32;
            d += 32;
        }

        remainingBytes -= N * 8 * 4 * 4; 
    }

    if (remainingBytes > 0) {
        // Memcpy the rest
        memcpy((uint8*)dst + (nbytes - remainingBytes), (const uint8*)src + (nbytes - remainingBytes), remainingBytes); 
    }
}
#else

    // Fall back to memcpy
    void memcpySSE2(void *dst, const void *src, int nbytes) {
        memcpy(dst, src, nbytes);
    }

#endif

#if defined(G3D_WIN32) && defined(SSE)
/** Michael Herf's fast memcpy */
void memcpyMMX(void* dst, const void* src, int nbytes) {
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

#else
    // Fall back to memcpy
    void memcpyMMX(void *dst, const void *src, int nbytes) {
        memcpy(dst, src, nbytes);
    }

#endif


void System::memcpy(void* dst, const void* src, size_t numBytes) {
    if (System::hasSSE2() && System::hasMMX()) {
        G3D::memcpyMMX(dst, src, numBytes);
    } else if (System::hasSSE() && System::hasMMX()) {
        G3D::memcpyMMX(dst, src, numBytes);
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
        ::memset((uint8*)dst + (originalSize - bytesRemaining), n32, bytesRemaining); 
    }
}

#else

// For non x86 processors, we fall back to the standard memset
void memfill(void *dst, int n32, unsigned long i) {
    ::memset(dst, n32, i);
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
            
        debugAssert((int)sizeof(filename) > ret);
            
            // Ensure proper NULL termination
            filename[ret] = 0;      
    }
    #endif

    return filename;
}


void System::sleep(RealTime t) {

    // Overhead of calling this function.
    static const RealTime OVERHEAD = .000006;

    RealTime now = getTick();
    RealTime wakeupTime = now + t - OVERHEAD;

    RealTime remainingTime = wakeupTime - now;
    RealTime sleepTime = 0;

    while (remainingTime > 0) {
        

        if (remainingTime > 0.001) {
            // Safe to use Sleep with a time... sleep for half the remaining time
            sleepTime = max(remainingTime * .5, 0.0005);
        } else if (remainingTime > 0.0001) {
            // Safe to use Sleep with a zero time;
            // causes the program to yield only
            // the current time slice, and then return.
            sleepTime = 0;
        } else {
            // Not safe to use Sleep; busy wait
            sleepTime = -1;
        }

        if (sleepTime >= 0) {
            #ifdef G3D_WIN32
                // Translate to milliseconds
                Sleep((int)(sleepTime * 1e3));
            #else
                // Translate to microseconds
                usleep((int)(sleepTime * 1e6));
            #endif
        }

        now = getTick();
        remainingTime = wakeupTime - now;
    }
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


////////////////////////////////////////////////////////////////
class BufferPool {
private:

    /** Only store buffers up to these sizes (in bytes) in each pool->
        Different pools have different management strategies.

        A large block is preallocated for tiny buffers; they are used with
        tremendous frequency.  Other buffers are allocated as demanded.
      */
    enum {tinyBufferSize = 64, smallBufferSize = 1000, medBufferSize = 5000};

    /** Most buffers we're allowed to store. */
    enum {maxTinyBuffers = 5000, maxSmallBuffers = 100, maxMedBuffers = 30};

    class MemBlock {
    public:
        void*           ptr;
        size_t          bytes;

        inline MemBlock() : ptr(NULL), bytes(0) {}
        inline MemBlock(void* p, size_t b) : ptr(p), bytes(b) {}
    };

    MemBlock smallPool[maxSmallBuffers];
    int smallPoolSize;

    MemBlock medPool[maxMedBuffers];
    int medPoolSize;

    /** The tiny pool is a single block of storage into which all tiny
        objects are allocated.  This provides better locality for
        small objects and avoids the search time, since all tiny
        blocks are exactly the same size. */
    void* tinyPool[maxTinyBuffers];
    int tinyPoolSize;

    /** Pointer to the data in the tiny pool */
    void* tinyHeap;

#   ifdef G3D_WIN32
    CRITICAL_SECTION    mutex;
#   else
    pthread_mutex_t     mutex;
#   endif

    /** Provide synchronization between threads */
    void lock() {
#       ifdef G3D_WIN32
            EnterCriticalSection(&mutex);
#       else
            pthread_mutex_lock(&mutex);
#       endif
    }

    void unlock() {
#       ifdef G3D_WIN32
            LeaveCriticalSection(&mutex);
#       else
            pthread_mutex_unlock(&mutex);
#       endif
    }

    /** 
     Malloc out of the tiny heap.
     */
    inline void* tinyMalloc(size_t bytes) {
        // Note that we ignore the actual byte size
        // and create a constant size block.
        (void)bytes;
        debugAssert(tinyBufferSize >= bytes);

        void* ptr = NULL;

        if (tinyPoolSize > 0) {
            --tinyPoolSize;
            // Return the last one
            ptr = tinyPool[tinyPoolSize];
        }

        return ptr;
    }

    /** Returns true if this is a pointer into the tiny heap. */
    bool inTinyHeap(void* ptr) {
        return (ptr >= tinyHeap) && 
               (ptr < (uint8*)tinyHeap + maxTinyBuffers * tinyBufferSize);
    }

    void tinyFree(void* ptr) {
        debugAssert(tinyPoolSize < maxTinyBuffers);

        // Put the pointer back into the free list
        tinyPool[tinyPoolSize] = ptr;
        ++tinyPoolSize;

    }

    void flushPool(MemBlock* pool, int& poolSize) {
        for (int i = 0; i < poolSize; ++i) {
            ::free(pool->ptr);
            pool->ptr = NULL;
            pool->bytes = 0;
        }
        poolSize = 0;
    }


    /**  Allocate out of a specific pool->  Return NULL if no suitable 
         memory was found. 
    
         */
    void* malloc(MemBlock* pool, int& poolSize, size_t bytes) {

        // TODO: find the smallest block that satisfies the request.

        // See if there's something we can use in the buffer pool->
        // Search backwards since usually we'll re-use the last one.
        for (int i = (int)poolSize - 1; i >= 0; --i) {
            if (pool[i].bytes >= bytes) {
                // We found a suitable entry in the pool->

                // No need to offset the pointer; it is already offset
                void* ptr = pool[i].ptr;

                // Remove this element from the pool
                --poolSize;
                pool[i] = pool[poolSize];

                return ptr;
            }
        }

        return NULL;
    }

public:

    /** Count of memory allocations that have occurred. */
    int totalMallocs;
    int mallocsFromTinyPool;
    int mallocsFromSmallPool;
    int mallocsFromMedPool;

    /** Amount of memory currently allocated (according to the application). 
        This does not count the memory still remaining in the buffer pool,
        but does count extra memory required for rounding off to the size
        of a buffer.
        Primarily useful for detecting leaks.*/
    // TODO: make me an atomic int!
    int bytesAllocated;

    BufferPool() {
        totalMallocs         = 0;

        mallocsFromTinyPool  = 0;
        mallocsFromSmallPool = 0;
        mallocsFromMedPool   = 0;

        bytesAllocated       = true;

        tinyPoolSize         = 0;
        tinyHeap             = NULL;

        smallPoolSize        = 0;

        medPoolSize          = 0;


        // Initialize the tiny heap as a bunch of pointers into one
        // pre-allocated buffer.
        tinyHeap = ::malloc(maxTinyBuffers * tinyBufferSize);
        for (int i = 0; i < maxTinyBuffers; ++i) {
            tinyPool[i] = (uint8*)tinyHeap + (tinyBufferSize * i);
        }
        tinyPoolSize = maxTinyBuffers;

#       ifdef G3D_WIN32
            InitializeCriticalSection(&mutex);
#       else
            pthread_mutex_init(&mutex, NULL);
#       endif
    }


    ~BufferPool() {
        ::free(tinyHeap);
#       ifdef G3D_WIN32
            DeleteCriticalSection(&mutex);
#       else
            // No destruction on pthreads
#       endif
    }

    
    void* realloc(void* ptr, size_t bytes) {
        if (ptr == NULL) {
            return malloc(bytes);
        }

        if (inTinyHeap(ptr)) {
            if (bytes <= tinyBufferSize) {
                // The old pointer actually had enough space.
                return ptr;
            } else {
                // Free the old pointer and malloc
                
                void* newPtr = malloc(bytes);
                System::memcpy(newPtr, ptr, tinyBufferSize);
                tinyFree(ptr);
                return newPtr;

            }
        } else {
            // In one of our heaps.

            // See how big the block really was
            size_t realSize = ((uint32*)ptr)[-1];
            if (bytes <= realSize) {
                // The old block was big enough.
                return ptr;
            }

            // Need to reallocate
            void* newPtr = malloc(bytes);
            System::memcpy(newPtr, ptr, realSize);
            free(ptr);
            return newPtr;
        }
    }


    void* malloc(size_t bytes) {
        lock();
        ++totalMallocs;

        if (bytes <= tinyBufferSize) {

            void* ptr = tinyMalloc(bytes);

            if (ptr) {
                ++mallocsFromTinyPool;
                unlock();
                return ptr;
            }

        } 
        
        // Failure to allocate a tiny buffer is allowed to flow
        // through to a small buffer
        if (bytes <= smallBufferSize) {
            
            void* ptr = malloc(smallPool, smallPoolSize, bytes);

            if (ptr) {
                ++mallocsFromSmallPool;
                unlock();
                return ptr;
            }

        } else  if (bytes <= medBufferSize) {
            // Note that a small allocation failure does *not* fall
            // through into a medium allocation because that would
            // waste the medium buffer's resources.

            void* ptr = malloc(medPool, medPoolSize, bytes);

            if (ptr) {
                ++mallocsFromMedPool;
                unlock();
                return ptr;
            }
        }

        bytesAllocated += 4 + bytes;
        unlock();

        // Heap allocate

        // Allocate 4 extra bytes for our size header (unfortunate,
        // since malloc already added its own header).
        void* ptr = ::malloc(bytes + 4);

        if (ptr == NULL) {
            // Flush memory pools to try and recover space
            flushPool(smallPool, smallPoolSize);
            flushPool(medPool, medPoolSize);
            ptr = ::malloc(bytes + 4);
        }


        if (ptr == NULL) {
            if ((System::outOfMemoryCallback != NULL) &&
                (System::outOfMemoryCallback(bytes + 4, true) == true)) {
                // Re-attempt the malloc
                ptr = ::malloc(bytes + 4);
            }
        }

        if (ptr == NULL) {
            if (System::outOfMemoryCallback != NULL) {
                // Notify the application
                System::outOfMemoryCallback(bytes + 4, false);
            }
            return NULL;
        }

        *(uint32*)ptr = bytes;

        return (uint8*)ptr + 4;
    }


    void free(void* ptr) {
        if (ptr == NULL) {
            // Free does nothing on null pointers
            return;
        }

        debugAssert(isValidPointer(ptr));

        if (inTinyHeap(ptr)) {
            lock();
            tinyFree(ptr);
            unlock();
            return;
        }

        uint32 bytes = ((uint32*)ptr)[-1];

        lock();
        if (bytes <= smallBufferSize) {
            if (smallPoolSize < maxSmallBuffers) {
                smallPool[smallPoolSize] = MemBlock(ptr, bytes);
                ++smallPoolSize;
                unlock();
                return;
            }
        } else if (bytes <= medBufferSize) {
            if (medPoolSize < maxMedBuffers) {
                medPool[medPoolSize] = MemBlock(ptr, bytes);
                ++medPoolSize;
                unlock();
                return;
            }
        }
        bytesAllocated -= bytes + 4;
        unlock();

        // Free; the buffer pools are full or this is too big to store.
        ::free((uint8*)ptr - 4);
    }
};

// Dynamically allocated because we need to ensure that
// the buffer pool is still around when the last global variable 
// is deallocated.
static BufferPool* bufferpool = NULL;

std::string System::mallocPerformance() {    
    if (bufferpool->totalMallocs > 0) {

        int pooled = bufferpool->mallocsFromTinyPool +
                     bufferpool->mallocsFromSmallPool + 
                     bufferpool->mallocsFromMedPool;

        int total = bufferpool->totalMallocs;

        return format("malloc perf:  %5.1f%% tiny   %5.1f%% small   "
                      "%5.1f%% med   %5.1f%% heap", 
                      100.0 * bufferpool->mallocsFromTinyPool  / total,
                      100.0 * bufferpool->mallocsFromSmallPool / total,
                      100.0 * bufferpool->mallocsFromMedPool   / total,
                      100.0 * (1.0 - (double)pooled / total));
    } else {
        return "No System::malloc calls made yet.";
    }
}


void System::resetMallocPerformanceCounters() {
    bufferpool->totalMallocs         = 0;
    bufferpool->mallocsFromMedPool   = 0;
    bufferpool->mallocsFromSmallPool = 0;
    bufferpool->mallocsFromTinyPool  = 0;
}


void initMem() {
    // Putting the test here ensures that the system is always
    // initialized, even when globals are being allocated.
    static bool initialized = false;
    if (! initialized) {
        bufferpool = new BufferPool();
        initialized = true;
    }
}


void* System::malloc(size_t bytes) {
    initMem();
    return bufferpool->malloc(bytes);
}


void* System::realloc(void* block, size_t bytes) {
    initMem();
    return bufferpool->realloc(block, bytes);
}


void System::free(void* p) {
    bufferpool->free(p);
}


void* System::alignedMalloc(size_t bytes, size_t alignment) {
    alwaysAssertM(isPow2(alignment), "alignment must be a power of 2");

    // We must align to at least a word boundary.
    alignment = iMax(alignment, sizeof(void *));

    // Pad the allocation size with the alignment size and the
    // size of the redirect pointer.
    size_t totalBytes = bytes + alignment + sizeof(void*);

    size_t truePtr = (size_t)System::malloc(totalBytes);

    if (truePtr == 0) {
        // malloc returned NULL
        return NULL;
    }

    debugAssert(isValidHeapPointer((void*)truePtr));
    #ifdef G3D_WIN32
    // The blocks we return will not be valid Win32 debug heap
    // pointers because they are offset 
    //  debugAssert(_CrtIsValidPointer((void*)truePtr, totalBytes, TRUE) );
    #endif

    // The return pointer will be the next aligned location (we must at least
    // leave space for the redirect pointer, however).
    size_t  alignedPtr = truePtr + sizeof(void*);

    // 2^n - 1 has the form 1111... in binary.
    uint32 bitMask = (alignment - 1);

    // Advance forward until we reach an aligned location.
    while ((alignedPtr & bitMask) != 0) {
        alignedPtr += sizeof(void*);
    }

    debugAssert(alignedPtr - truePtr + bytes <= totalBytes);

    // Immediately before the aligned location, write the true array location
    // so that we can free it correctly.
    size_t* redirectPtr = (size_t *)(alignedPtr - sizeof(void *));
    redirectPtr[0] = truePtr;

    debugAssert(isValidHeapPointer((void*)truePtr));

    #ifdef G3D_WIN32
        debugAssert( _CrtIsValidPointer((void*)alignedPtr, bytes, TRUE) );
    #endif
    return (void *)alignedPtr;
}


void System::alignedFree(void* _ptr) {
    if (_ptr == NULL) {
        return;
    }

    size_t alignedPtr = (size_t)_ptr;

    // Back up one word from the pointer the user passed in.
    // We now have a pointer to a pointer to the true start
    // of the memory block.
    size_t* redirectPtr = (size_t*)(alignedPtr - sizeof(void *));

    // Dereference that pointer so that ptr = true start
    void* truePtr = (void*)redirectPtr[0];

    debugAssert(isValidHeapPointer((void*)truePtr));
    System::free(truePtr);
}


void System::setEnv(const std::string& name, const std::string& value) {
    #ifdef G3D_WIN32
        std::string cmd = name + "=" + value;
        putenv(name.c_str());
    #else
        setenv(name.c_str(), value.c_str(), 1);
    #endif
}



static void var(TextOutput& t, const std::string& name, const std::string& val) {
    t.writeSymbols(name,"=");
    t.writeString(val);
    t.writeNewline();
}


static void var(TextOutput& t, const std::string& name, const bool val) {
    t.writeSymbols(name, "=", val ? "Yes" : "No");
    t.writeNewline();
}


static void var(TextOutput& t, const std::string& name, const int val) {
    t.writeSymbols(name,"=");
    t.writeNumber(val);
    t.writeNewline();
}

void System::describeSystem(
    std::string&        s) {

    TextOutput t;
    describeSystem(t);
    t.commitString(s);
}

void System::describeSystem(
    TextOutput& t) {

    t.writeSymbols("OS", "{");
    t.writeNewline();
    t.pushIndent();
        var(t, "Name", System::operatingSystem());
    t.popIndent();
    t.writeSymbols("}");
    t.writeNewline();
    t.writeNewline();

    t.writeSymbols("CPU", "{");
    t.writeNewline();
    t.pushIndent();
        var(t, "Vendor", System::cpuVendor());
        var(t, "Architecture", System::cpuArchitecture());
        var(t, "hasCPUID", System::hasCPUID());
        var(t, "hasMMX", System::hasMMX());
        var(t, "hasSSE", System::hasSSE());
        var(t, "hasSSE2", System::hasSSE2());
        var(t, "has3DNow", System::has3DNow());
        var(t, "hasRDTSC", System::hasRDTSC());
    t.popIndent();
    t.writeSymbols("}");
    t.writeNewline();
    t.writeNewline();
       
    t.writeSymbols("G3D", "{");
    t.writeNewline();
    t.pushIndent();
        var(t, "Link version", G3D_VER);
        var(t, "Compile version", System::version());
    t.popIndent();
    t.writeSymbols("}");
    t.writeNewline();
    t.writeNewline();
}


}  // namespace
