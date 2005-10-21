/**
 @file GThread.cpp

 GThread class.

 @created 2005-09-24
 @edited  2005-09-24
 */

#include "G3D/GThread.h"
#include "G3D/debugAssert.h"


namespace G3D {

namespace _internal {

class GThreadPrivate {
public:
    bool running;
    bool completed;
    HANDLE event;

    GThreadPrivate():
        running(false),
        completed(false) {
        running = false;
        completed = false;
    }

    static DWORD WINAPI GThreadProc_windows(LPVOID param) {
        GThread* current = (GThread*)param;
        current->pthread->event = ::CreateEvent(NULL, TRUE, FALSE, NULL);
        current->pthread->running = true;
        current->pthread->completed = false;
        current->main();
        current->pthread->running = false;
        current->pthread->completed = true;
        ::SetEvent(current->pthread->event);
        return 0;
    }

    static void* GThreadProc_pthread(LPVOID param) {
        GThread* current = (GThread*)param;
        current->pthread->event = ::CreateEvent(NULL, TRUE, FALSE, NULL);
        current->pthread->running = true;
        current->pthread->completed = false;
        current->main();
        current->pthread->running = false;
        current->pthread->completed = true;
        ::SetEvent(current->pthread->event);
        return (void*)NULL;
    }
};

class BasicThread: public GThread {
private:
    void (*wrapperProc)();
public:
    BasicThread(const std::string& name, void (*proc)()):
        GThread(name), wrapperProc(proc) { }
protected:
    virtual void main() {
        wrapperProc();
    }
};

} // namespace _internal


GThread::GThread(const std::string& name):
    _name(name),
    handle(NULL),
    signalInt32(0) {

    pthread = new _internal::GThreadPrivate;
}

GThread::~GThread() {
    alwaysAssertM(pthread->running, "Deleting thread while running.");
    delete pthread;
}

GThread* GThread::create(const std::string& name, void (*proc)()) {
    return new _internal::BasicThread(name, proc);
}

bool GThread::start() {

    debugAssertM(!pthread->completed, "Thread has already executed.");

    if (pthread->completed) {
        return false;
    }

#   ifdef G3D_WIN32
    DWORD threadId;

    handle = ::CreateThread(
        NULL,
        0,
        &_internal::GThreadPrivate::GThreadProc_windows,
        this,
        0,
        &threadId);

    return (handle != NULL);
#   else
    if (!pthread_create(((pthread_t)handle,
                        &_internal::GThreadPrivate::GThreadProc_windows, 
                        this)) {
        return true;
    } else {
        handle = NULL;
        return false
    }
#   endif
}

void GThread::terminate() {
    if (handle) {
#       ifdef G3D_WIN32
        ::TerminateThread(handle, 0);
#       else
        pthread_kill((pthread_t)handle, SIGTHR);
#       endif
        handle = NULL;
    }
}

bool GThread::running() {
    return pthread->running;
}

bool GThread::completed() {
    return pthread->completed;
}

void GThread::signalStopSafely() {
    signalInt32 = 1;
}

void GThread::waitForCompletion() {
#   ifdef G3D_WIN32
    ::WaitForSingleObject(pthread->event, INFINITE);
#   else
    pthread_join((pthread_t)handle, NULL);
#   endif
}


GMutex::GMutex() {
#   ifdef G3D_WIN32
    ::InitializeCriticalSection(&handle);
#   else
    pthread_mutex_init(&handle, NULL);
#   endif
}

GMutex::~GMutex() {
    //TODO: Debug check for locked
#   ifdef G3D_WIN32
    ::DeleteCriticalSection(&handle);
#   else
    pthread_mutex_destroy(&handle);
#   endif
}

//bool GMutex::tryLock() {
//#   ifdef G3D_WIN32
//    return ::TryEnterCriticalSection(&handle);
//#   else
//    return pthread_mutex_trylock(&handle);
//#   endif
//}

void GMutex::lock() {
#   ifdef G3D_WIN32
    ::EnterCriticalSection(&handle);
#   else
    pthread_mutex_lock(&handle);
#   endif
}

void GMutex::unlock() {
#   ifdef G3D_WIN32
    ::LeaveCriticalSection(&handle);
#   else
    pthread_mutex_unlock();
#   endif
}

} // namespace G3D
