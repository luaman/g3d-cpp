/**
 @file GThread.cpp

 GThread class.

 @created 2005-09-24
 @edited  2005-09-24
 */

#include <windows.h>

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

    static DWORD WINAPI GThreadProc(LPVOID param) {
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
    DWORD threadId;

    handle = ::CreateThread(
        NULL,
        0,
        &_internal::GThreadPrivate::GThreadProc,
        this,
        0,
        &threadId);

    return (handle != NULL);
}

void GThread::terminate() {
    if (handle) {
        ::TerminateThread(handle, 0);
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
    ::WaitForSingleObject(pthread->event, INFINITE);
}

} // namespace G3D
