/** 
  @file System.h
 
  @maintainer Morgan McGuire, matrix@graphics3d.com
 
  @cite Rob Wyatt http://www.gamasutra.com/features/wyatts_world/19990709/processor_detection_01.htm
  @cite Benjamin Jurke http://www.flipcode.com/cgi-bin/msg.cgi?showThread=COTD-ProcessorDetectionClass&forum=cotd&id=-1
  @cite Michael Herf http://www.stereopsis.com/memcpy.html

  @created 2003-01-25
  @edited  2003-02-13
 */

#ifndef G3D_SYSTEM_H
#define G3D_SYSTEM_H

#include "../G3D/g3dmath.h"
#include <string>

namespace G3D {

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
