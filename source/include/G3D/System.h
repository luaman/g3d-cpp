/** 
  @file System.h
 
  @maintainer Morgan McGuire, matrix@graphics3d.com
 
  @cite Rob Wyatt http://www.gamasutra.com/features/wyatts_world/19990709/processor_detection_01.htm
  @cite Benjamin Jurke http://www.flipcode.com/cgi-bin/msg.cgi?showThread=COTD-ProcessorDetectionClass&forum=cotd&id=-1
  @cite Michael Herf http://www.stereopsis.com/memcpy.html

  @created 2003-01-25
  @edited  2003-05-15
 */

#ifndef G3D_SYSTEM_H
#define G3D_SYSTEM_H

#include "G3D/g3dmath.h"
#include <string>

namespace G3D {

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
 */
class System {
public:

	static bool hasMMX();
	static bool hasSSE();
	static bool hasSSE2();
	static bool has3DNow();
	static std::string cpuVendor();

    static uint64 getCycleCount();

    /**
     Returns the endianness of this machine.
     */
    static G3DEndian machineEndian();

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
};


#ifdef _MSC_VER
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

#else

    // Not the Microsoft compiler; unknown assembly syntax
    inline uint64 System::getCycleCount() {
       uint32 timehi, timelo;

       __asm__ __volatile__ (
          "rdtsc            "
          : "=a" (timelo),
            "=d" (timehi)
          : );
       return ((uint64)timehi << 32) + (uint64)timelo;
       /*
          "mov %%edx, %0 \n\t"
          "mov %%eax, %1 \n\t"
          : "=r" (timehi, timelo)
            "=r" (timehi, timelo)
          : 
          : "%eax");
        return 0;
        */
    }

#endif

inline void System::beginCycleCount(uint64& cycleCount) {
    cycleCount = getCycleCount();
}


inline void System::endCycleCount(uint64& cycleCount) {
    cycleCount = getCycleCount() - cycleCount;
}


} // namespace


#endif
