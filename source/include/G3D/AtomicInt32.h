/**
 @file AtomicInt32.h

 @maintainer Morgan McGuire, matrix@graphics3d.com

 @created 2005-09-01
 @edited  2005-09-26
 */
#ifndef G3D_ATOMICINT32_H
#define G3D_ATOMICINT32_H

#include "platform.h"

namespace G3D {

/**
 An integer that may safely be used on different threads without
 external locking.

 On Win32 and Linux this is implemented without locks.  

 On PPC, AtomicInt32 is currently not atomic.  A future implementation may use locks 
 and be comparatively slow.

 <B>BETA API</B>  This is unsupported and may change
 */
//TODO: PPC
class AtomicInt32 {
private:

    volatile int32           value;

public:

    /** Atomic set */
    explicit inline AtomicInt32(const int32 x) {
#       if defined(G3D_WIN32)
            // Asignment is done this way because APR does it this way.
            // Morgan believes that volatile should be sufficient, however.
            InterlockedExchange(&value, x);
#       elif defined(G3D_LINUX)
            value = x;
#       elif defined(G3D_OSX)
            value = x;
#       endif
    }

    /** Atomic set */
    inline AtomicInt32(const Atomic& x) {
#       if defined(G3D_WIN32)
            InterlockedExchange(&value, x.value);
#       elif defined(G3D_LINUX)
            value = x.value;
#       elif defined(G3D_OSX)
            value = x;
#       endif
    }

    /** Atomic set */
    inline const AtomicInt32& operator=(const int32 x) {
#       if defined(G3D_WIN32)
            InterlockedExchange(&value, x);
#       elif defined(G3D_LINUX)
            value = x;
#       elif defined(G3D_OSX)
            value = x;
#       endif
        return *this;
    }

    /** Atomic set */
    inline const void operator=(const Atomic& x) {
#       if defined(G3D_WIN32)
            InterlockedExchange(&value, x.value);
#       elif defined(G3D_LINUX)
            value = x.value;
#       elif defined(G3D_OSX)
            value = x.value;
#       endif
    }

    /** Returns the current value */
    inline const int32 value() const {
        return value;
    }

    /** Returns the old value, before the add. */
    inline int32 add(const int32 x) {
#       if defined(G3D_WIN32)

            return InterlockedExchangeAdd(&value, x);

#       elif defined(G3D_LINUX)

            int32 old;
            asm volatile ("lock; xaddl %0,%1"
                  : "=r"(old), "=m"(value) /* outputs */
                  : "0"(x), "m"(value)   /* inputs */
                  : "memory", "cc");
            return old;
            
#       elif defined(G3D_OSX)

            int32 old, temp;
            asm volatile ("0:\n\t"
                  "lwarx  %0,0,%2\n\t"
                  "add    %1,%0,%3\n\t"
                  "stwcx. %1,0,%2\n\t"
                  "bne-   0b"

                  : "=&r" (old), "=&r" (temp)
                  : "b" (value), "r" (x)
                  : "memory", "cc");

             return old;
#       endif
    }

    /** Returns old value. */
    inline int32 sub(const int32 x) {
        return exchangeAdd(-x);
    }

    inline void increment() {
#       if defined(G3D_WIN32)
            // Note: returns the newly incremented value
            InterlockedIncrement(&value);
#       elif defined(G3D_LINUX)
            add(1);
#       elif defined(G3D_OSX)
            add(1);
#       endif
    }

    /** Returns zero if the result is zero after decrement, non-zero otherwise.*/
    inline uint32 decrement() {
#       if defined(G3D_WIN32)
            // Note: returns the newly decremented value
            return InterlockedDecrement(&value) != 0;
#       elif defined(G3D_LINUX)
            unsigned char nz;

            asm volatile ("lock; decl %1;\n\t"
                          "setnz %%al"
                          : "=a" (nz)
                          : "m" (value)
                          : "memory", "cc");
            return nz;
#       elif defined(G3D_OSX)
            // TODO: PPC
            return (--value) != 0;
#       endif
    }


    /** Atomic test-and-set:  if <code>*this == comperand</code> then <code>*this := exchange</code> else do nothing.
        In both cases, returns the old value of <code>*this</code>.
    
        Performs an atomic comparison of this with the Comperand value. 
        If this is equal to the Comperand value, the Exchange value is stored in this.
        Otherwise, no operation is performed.

     */ 
    inline int32 compareAndSet(const int32 comperand, const int32 exchange) {
#       if defined(G3D_WIN32)
            return InterlockedCompareExchange(&value, exchange, comperand);
#       elif defined(G3D_LINUX)
            int32 ret;
            asm volatile ("lock; cmpxchgl %1, %2"
                          : "=a" (ret)
                          : "r" (exchange), "m" (value), "0"(comperand)
                          : "memory", "cc");
            return ret;
#       elif defined(G3D_OSX)
            apr_uint32_t old;
                                                                                
            asm volatile ("0:\n\t"                   /* retry local label     */
                          "lwarx  %0,0,%1\n\t"       /* load prev and reserve */
                          "cmpw   %0,%3\n\t"         /* does it match cmp?    */
                          "bne-   1f\n\t"            /* ...no, bail out       */
                          "stwcx. %2,0,%1\n\t"       /* ...yes, conditionally
                                                        store swap            */
                          "bne-   0b\n\t"            /* start over if we lost
                                                        the reservation       */
                          "1:"                       /* exit local label      */
                          
                          : "=&r"(old)                        /* output      */
                          : "b" (&value), "r" (exchange), "r"(comperand)    /* inputs      */
                          : "memory", "cc");                   /* clobbered   */
            return old;
#       endif
    }

};

} // namespace

#endif
