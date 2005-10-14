/** 
  @file GThread.h
 
  @created 2005-09-22
  @edited  2005-09-24

 */

#include <string>

#include "G3D/AtomicInt32.h"

namespace G3D {

namespace _internal {
    class GThreadPrivate;
}
        
/**
    GThread's documentation
*/
class GThread {
private:

    _internal::GThreadPrivate* pthread;

    // Thread handle to hold HANDLE and pthread_t
    void* handle;

    // Used for thread closure signaling
    AtomicInt32 signalInt32;

    std::string _name;

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


} // namespace G3D