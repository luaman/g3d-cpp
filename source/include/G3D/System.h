/** 
  @file System.h
 
  @maintainer Morgan McGuire, matrix@graphics3d.com
 
  @cite Rob Wyatt http://www.gamasutra.com/features/wyatts_world/19990709/processor_detection_01.htm
  @cite Benjamin Jurke http://www.flipcode.com/cgi-bin/msg.cgi?showThread=COTD-ProcessorDetectionClass&forum=cotd&id=-1
  @cite Michael Herf http://www.stereopsis.com/memcpy.html

  @created 2003-01-25
  @edited  2004-04-29
 */

#ifndef G3D_SYSTEM_H
#define G3D_SYSTEM_H

#include "G3D/platform.h"
#include "G3D/g3dmath.h"
#include "G3D/G3DGameUnits.h"
#include <string>

#ifdef G3D_OSX
#include <CoreServices/CoreServices.h>
#endif

namespace G3D {

/**
 Routine used by the demos to find the data.  Searches in
 ../data, ../../data, etc. up to 5 levels back.  Checks
 common locations like c:\libraries\g3d-<version>\data
 and some hard-coded paths on the Brown University file
 system.
 */
std::string demoFindData(bool errorIfNotFound = true);

/** G3D, SDL, and IJG libraries require license documentation
    to be distributed with your program.  This generates the
    string that must appear in your documentation. 
    <B>Your program can be commercial, closed-source</B> under
    any license you want.*/
std::string license();

/**
 The order in which the bytes of an integer are stored on a machine.
 Intel/AMD chips tend to be G3D_LITTLE_ENDIAN, Mac PPC's and Suns are
 G3D_BIG_ENDIAN.  However, this is primarily used to specify the byte
 order of file formats, which are fixed.
 */
enum G3DEndian {G3D_BIG_ENDIAN, G3D_LITTLE_ENDIAN};

/**
 OS and processor abstraction.  The first time any method is called the processor
 will be analyzed.  Future calls are then fast.

 Timing function overview:
    System::getCycleCount
      - actual cycle count

    System::getTick
      - High-resolution time in seconds since program started

    System::getLocalTime
      - High-resolution time in seconds since Jan 1, 1970
        (because it is stored in a double, this may be less
         accurate than getTick)

 */
class System {
public:

	/** */
	static void init();
	static bool initialized;

	/** */
	static bool hasMMX();
	
	/** */
	static bool hasSSE();
	
	/** */
	static bool hasSSE2();
	
	/** */
	static bool has3DNow();

	
	/** */
    static bool hasRDTSC();

	static const std::string& cpuVendor();
	
	/** e.g. "Windows", "GNU/Linux" */
    static const std::string& operatingSystem();

	/** */
    static const std::string& cpuArchitecture();

    /**
     Returns the endianness of this machine.
     */
    static G3DEndian machineEndian();

    /**
     Guarantees that the start of the array is aligned to the 
     specified number of bytes.
     */
    static void* alignedMalloc(size_t bytes, size_t alignment);

    /**
     Frees memory allocated with alignedMalloc.
     */
    static void alignedFree(void* ptr);

	/** An implementation of memcpy that may be up to 2x as fast as the C library
	    one on some processors.  Guaranteed to have the same behavior as memcpy
		in all cases. */
	static void memcpy(void* dst, const void* src, size_t numBytes);

	/** An implementation of memset that may be up to 2x as fast as the C library
	    one on some processors.  Guaranteed to have the same behavior as memset
		in all cases. */
	static void memset(void* dst, uint8 value, size_t numBytes);

    /**
     Returns the fully qualified filename for the currently running executable.
     This is more reliable than arg[0], which may be intentionally set to an incorrect
     value by a calling program, relative to a now non-current directory, or obfuscated
     by sym-links.

     @cite Linux version written by Nicolai Haehnle <prefect_@gmx.net>, http://www.flipcode.com/cgi-bin/msg.cgi?showThread=COTD-getexename&forum=cotd&id=-1
     */
    static std::string currentProgramFilename();

    /**
     Causes the current thread to yield for the specified duration
     and consume almost no CPU.
     The sleep will be extremely precise; it uses System::time() 
     to calibrate the exact yeild time.
     */
    static void sleep(RealTime t);

    /**
     Clears the console.
     Console programs only.
     */
    static void consoleClearScreen();

    /**
     Returns true if a key is waiting.
     Console programs only.
     */
    static bool consoleKeyPressed();
    
    /**
     Blocks until a key is read (use consoleKeyPressed to determine if
     a key is waiting to be read) then returns the character code for
     that key.
     */
    static int consoleReadKey();

    /**
     Returns a highly accurate time in milliseconds that
     is relative to an arbitrary per-platform baseline
     (e.g. the time the program started)
     
     Use differences in two tick times to measure
     events to a high degree of precision (e.g. for profiling,
     frame rate counting).

     This is as accurate as System::getCycleCount, but returns a time
     in seconds instead of cycles.
     @deprecated Call time();
     */
    static RealTime getTick();

    /**
     @deprecated Call time();
     */
    static RealTime getLocalTime();

    /**
     The actual time (measured in seconds since
     Jan 1 1970 midnight).
     
     Adjusted for local timezone and daylight savings
     time.   This is as accurate and fast as getCycleCount().
    */
    static RealTime time() {
        return getLocalTime();
    }

    /**
     To count the number of cycles a given operation takes:

     <PRE>
     unsigned long count;
     System::beginCycleCount(count);
     ...
     System::endCycleCount(count);
     // count now contains the cycle count for the intervening operation.

     */
    static void beginCycleCount(uint64& cycleCount);
    static void endCycleCount(uint64& cycleCount);

    static uint64 getCycleCount();

    /** Set an environment variable for the current process */
    static void setEnv(const std::string& name, const std::string& value);
	
	//#ifdef G3D_OSX
		static long m_OSXCPUSpeed; //In Cycles/Second
		static double m_secondsPerNS;
	//#endif
};


#ifdef G3D_WIN32
    inline uint64 System::getCycleCount() {
       uint32 timehi, timelo;

       // Use the assembly instruction rdtsc, which gets the current
       // cycle count (since the process started) and puts it in edx:eax.
       __asm
       {
          rdtsc
          mov timehi, edx;
          mov timelo, eax;
       }

       return ((uint64)timehi << 32) + (uint64)timelo;
    }

#elif defined(G3D_LINUX)

    inline uint64 System::getCycleCount() {
       uint32 timehi, timelo;

       __asm__ __volatile__ (
          "rdtsc            "
          : "=a" (timelo),
            "=d" (timehi)
          : );
       return ((uint64)timehi << 32) + (uint64)timelo;
    }

#elif defined(G3D_OSX)

    inline uint64 System::getCycleCount() {
		//Note:  To put off extra processing until the end, this does not 
		//return the actual clock cycle count.  It is a bus cycle count.
		//When endCycleCount() is called, it converts the two into a difference
		//of clock cycles
		
        return (uint64) UnsignedWideToUInt64(UpTime());
		//return (uint64) mach_absolute_time();
    }

#endif

inline void System::beginCycleCount(uint64& cycleCount) {
    cycleCount = getCycleCount();
}


inline void System::endCycleCount(uint64& cycleCount) {
	#ifndef G3D_OSX
		cycleCount = getCycleCount() - cycleCount;
	#else
		AbsoluteTime end = UpTime();
		init();
		Nanoseconds diffNS = AbsoluteDeltaToNanoseconds(end, UInt64ToUnsignedWide(cycleCount));
		cycleCount = (uint64) ((double) (System::m_OSXCPUSpeed) * (double) UnsignedWideToUInt64(diffNS) * m_secondsPerNS);
	#endif
}


} // namespace


#endif
