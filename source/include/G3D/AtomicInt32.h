
#include "platform.h"


/**
 An integer that may safely be used on different threads without
 external locking.

 On Win32 and Linux this is implemented without locks.  On PPC,
 the current implementation uses locks and is comparatively slow.
 */
//TODO: PPC
class AtomicInt32 {
private:

    volatile int32           value;

public:

    // TODO: why does APR use interlocked exchange for this?

    /** Atomic set */
    explicit inline AtomicInt32(const int32 x) {
#       if defined(G3D_WIN32)
            InterlockedExchange(&value, x);
#       elif defined(G3D_LINUX)
            value = x;
#       endif
    }

    /** Atomic set */
    inline AtomicInt32(const Atomic& x) {
#       if defined(G3D_WIN32)
            InterlockedExchange(&value, x.value);
#       elif defined(G3D_LINUX)
            value = x.value;
#       endif
    }

    /** Atomic set */
    inline const AtomicInt32& operator=(const int32 x) {
#       if defined(G3D_WIN32)
            InterlockedExchange(&value, x);
#       elif defined(G3D_LINUX)
            value = x;
#       endif
        return *this;
    }

    /** Atomic set */
    inline const int32 operator=(const Atomic& x) {
#       if defined(G3D_WIN32)
            InterlockedExchange(&value, x.value);
#       elif defined(G3D_LINUX)
            value = x.value;
#       endif
        return *this;
    }

    /** Returns the old value, before the add. */
    inline int32 exchangeAdd(const int32 x) {
#       if defined(G3D_WIN32)
            return InterlockedExchangeAdd(&value, x);
#       elif defined(G3D_LINUX)
            asm volatile ("lock; xaddl %0, %1"
                          : "=r"(x), "=m"(value) // outputs
                          : "0"(x), "m"(value)   // inputs
                          : "memory", "cc");
            return x;
#       endif
    }

    /** Returns old value. */
    inline int32 exchangeSub(const int32 x) {
        return exchangeAdd(-x);
    }


    inline void increment() {
#       if defined(G3D_WIN32)
            InterlockedIncrement(&value);
#       elif defined(G3D_LINUX)
            // TODO: cgd: why doesn't APR use the incl instruction?
            exchangeAdd(1);
#       endif
    }

    inline void decrement() {
#       if defined(G3D_WIN32)
            InterlockedDecrement(&value);
#       elif defined(G3D_LINUX)
            // TODO: cgd: why is this unsigned char?
            unsigned char prev;

            asm volatile ("lock; decl %1;\n\t"
                          "setnz %%al"
                          : "=a" (prev)
                          : "m" (value)
                          : "memory", "cc");
            return prev;
#       endif
    }


    /** Atomic test-and-set:  if this == comperand then this := exchange
    
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
#       endif
    }

    // TODO: cgd: do we need compareAndSetPointer?  Why?
}
