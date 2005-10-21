/** 
  @file GThread.h
 
  @created 2005-09-22
  @edited  2005-09-24

 */

#ifndef G3D_GTHREAD_H
#define G3D_GTHREAD_H

#if _MSC_VER > 1200

#include <string>

#include "G3D/AtomicInt32.h"

#ifdef G3D_WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <signal.h>
#endif

namespace G3D {

namespace _internal {
    class GThreadPrivate;
}
        
/**
    GThread's documentation
*/
class GThread {
private:

    _internal::GThreadPrivate*          pthread;

    // Thread handle to hold HANDLE and pthread_t
    void*                               handle;

    // Used for thread closure signaling
    AtomicInt32                         signalInt32;

    std::string                         _name;

    // Not implemented on purpose, don't use
    GThread& operator=(const GThread&);
    bool operator==(const GThread&);

public:
    GThread(const std::string& name);
    virtual ~GThread();

    template<class C>
    static GThread* create(const std::string& name, void (C::*proc)()) {
        return new _internal::TemplateThread(name, proc);
    }

    static GThread* create(const std::string& name, void (*proc)());

    /** Starts the thread and executes main() */
    bool start();

    /**
        Terminates the thread without notifying or
        waiting for a cancelation point. 
        
        Use of stopSafely() is preferred. */
    void terminate();

    /**
        Returns true is the thread and main() are
        currently executing, otherwise returns false. */
    bool running();

    bool completed();

    /**
        Signals an internal condition to exit the thread
        safely. Use with wait() to safely stop thread.
        
        TODO: Specify macro to use in thread that checks
        for the signal or loop an inlined call of main that
        is wrapped by a signal check and tell the user
        not to do an infinite loop in main(). */
    void signalStopSafely();

    /** 
        Waits for the thread to finish executing. 
        
        TODO: Does this need a timeout? */
    void waitForCompletion();

    const std::string& name() {
        return _name;
    }

protected:
    friend class _internal::GThreadPrivate;

    #define CHECK_STOP_SIGNAL \
        {if (signalInt32.value()) return;}

    virtual void main() = 0;
};

namespace _internal {
template<class C>
class TemplateThread: public GThread {
private:
    typename void (C::*wrapperProc)();
public:
    TemplateThread(const std::string& name, void (C::*proc)()):
        GThread(name), wrapperProc(proc) { }
protected:
    virtual void main() {
        wrapperProc();
    }
};
}


/**
    GMutex's documentation
*/
class GMutex {
private:
#   ifdef G3D_WIN32
    CRITICAL_SECTION                    handle;
#   else
    pthread_mutex_t                     handle;
#   endif

public:
    GMutex();
    ~GMutex();

//    /** Only available on NT and later. */
//    bool tryLock();

    void lock();

    void unlock();
};


/**
    GMutexLock's documentation.
    Automatically locks while in scope.
*/
class GMutexLock {
private:
    GMutex* m;

    GMutexLock(const GMutexLock &mlock);
    GMutexLock &operator=(const GMutexLock &);
    bool operator==(const GMutexLock&);

public:
    GMutexLock(GMutex* mutex) {
        m = mutex;
        m->lock();
    }

    ~GMutexLock() {
        m->unlock();
    }
};

} // namespace G3D

#endif
#endif //G3D_GTHREAD_H
