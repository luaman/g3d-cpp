/**
 @file AtomicInt32.h

 @maintainer Morgan McGuire, matrix@graphics3d.com

 @created 2005-09-01
 @edited  2005-09-26
 */
#ifndef G3D_ATOMICINT32_H
#define G3D_ATOMICINT32_H

#include "G3D/platform.h"
#include "G3D/g3dmath.h"

#if (!defined G3D_WIN32) || (_MSC_VER > 1200)

namespace G3D {

/**
 An integer that may safely be used on different threads without
 external locking.

 On Win32 and Linux this is implemented without locks.  

 <B>BETA API</B>  This is unsupported and may change
 */
class AtomicInt32 {
private:
#   if defined(G3D_WIN32) && (_MSC_VER > 1200)
    volatile long           _value;
#   else
    volatile int32          _value;
#   endif

public:

    /** Atomic set */
    explicit inline AtomicInt32(const int32 x) {
        // APR does this assignment using InterlockedExchange
        // Morgan believes that volatile should be sufficient, however.
        _value = x;
    }

    /** Atomic set */
    inline AtomicInt32(const AtomicInt32& x) {
        _value = x._value;
    }

    /** Atomic set */
    inline const AtomicInt32& operator=(const int32 x) {
        _value = x;
        return *this;
    }

    /** Atomic set */
    inline const void operator=(const AtomicInt32& x) {
        _value = x._value;
    }

    /** Returns the current value */
    inline const int32 value() const {
        return _value;
    }

    /** Returns the old value, before the add. */
    inline int32 add(const int32 x) {
#       if defined(G3D_WIN32)

            return InterlockedExchangeAdd(&_value, x);

#       elif defined(G3D_LINUX)

            int32 old;
            asm volatile ("lock; xaddl %0,%1"
                  : "=r"(old), "=m"(_value) /* outputs */
                  : "0"(x), "m"(_value)   /* inputs */
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
                  : "b" (_value), "r" (x)
                  : "memory", "cc");

             return old;
#       endif
    }

    /** Returns old value. */
    inline int32 sub(const int32 x) {
        return add(-x);
    }

    inline void increment() {
#       if defined(G3D_WIN32)
            // Note: returns the newly incremented value
            InterlockedIncrement(&_value);
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
            return InterlockedDecrement(&_value) != 0;
#       elif defined(G3D_LINUX)
            unsigned char nz;

            asm volatile ("lock; decl %1;\n\t"
                          "setnz %%al"
                          : "=a" (nz)
                          : "m" (_value)
                          : "memory", "cc");
            return nz;
#       elif defined(G3D_OSX)
            // TODO: PPC
            return (--_value) != 0;
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
            return InterlockedCompareExchange(&_value, exchange, comperand);
#       elif defined(G3D_LINUX)
            int32 ret;
            asm volatile ("lock; cmpxchgl %1, %2"
                          : "=a" (ret)
                          : "r" (exchange), "m" (_value), "0"(comperand)
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
                          : "b" (&_value), "r" (exchange), "r"(comperand)    /* inputs      */
                          : "memory", "cc");                   /* clobbered   */
            return old;
#       endif
    }

};

} // namespace
#endif // !defined G3D_WIN32 || _MSC_VER > 1200
#endif
