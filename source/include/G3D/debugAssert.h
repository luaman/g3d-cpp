/**
  @file debugAssert.h
 
  debugAssert(expression);
  debugAssertM(expression, message);
 
  @cite
     John Robbins, Microsoft Systems Journal Bugslayer Column, Feb 1999.
     <A HREF="http://msdn.microsoft.com/library/periodic/period99/feb99_BUGSLAYE_BUGSLAYE.htm">
     http://msdn.microsoft.com/library/periodic/period99/feb99_BUGSLAYE_BUGSLAYE.htm</A>
 
  @cite 
     Douglas Cox, An assert() Replacement, Code of The Day, flipcode, Sept 19, 2000
     <A HREF="http://www.flipcode.com/cgi-bin/msg.cgi?showThread=COTD-AssertReplace&forum=cotd&id=-1">
     http://www.flipcode.com/cgi-bin/msg.cgi?showThread=COTD-AssertReplace&forum=cotd&id=-1</A>
 
  @maintainer Morgan McGuire, matrix@graphics3d.com
 
  @created 2001-08-26
  @edited  2004-01-02

 Copyright 2000-2004, Morgan McGuire.
 All rights reserved.
 */

#ifndef G3D_DEBUGASSERT_H
#define G3D_DEBUGASSERT_H

#include <string>
#include "G3D/platform.h"

#ifdef G3D_LINUX
    // Needed so we can define a global display
    // pointer for debugAssert.
    #include <X11/Xlib.h>
    #include <X11/Xutil.h>
    #include <X11/Xatom.h>
#endif

 /**
 @def debugBreak()
 
 Break at the current location (i.e. don't push a procedure stack frame
 before breaking).
 */

/**
  @def debugAssert(exp)
  Breaks if the expression is false. If G3D_DEBUG_NOGUI is defined, prompts at
  the console, otherwise pops up a dialog.  The user may then break (debug), 
  ignore, ignore always, or halt the program.
 
  The assertion is also posted to the clipboard under Win32.
 */

/**
  @def debugAssertM(exp, msg)
  Breaks if the expression is false and displays a message. If G3D_DEBUG_NOGUI 
  is defined, prompts at the console, otherwise pops up a dialog.  The user may
  then break (debug), ignore, ignore always, or halt the program.
 
  The assertion is also posted to the clipboard under Win32.
 */

/**
 @def __debugPromptShowDialog__
 @internal
 */

#ifdef _DEBUG

    #ifndef G3D_OSX
        #ifdef _MSC_VER
            #define rawBreak()  _asm { int 3 }
        #else
            #define rawBreak() __asm__ __volatile__ ( "int $3" ); 
        #endif
    #else
        #define rawBreak() assert(false); /* No breakpoints on OS X yet */
    #endif


    #define debugBreak() G3D::_internal::_releaseInputGrab_(); rawBreak(); G3D::_internal::_restoreInputGrab_();
    #define debugAssert(exp) debugAssertM(exp, "Debug assertion failure")

    #ifdef G3D_DEBUG_NOGUI
        #define __debugPromptShowDialog__ false
    #else
        #define __debugPromptShowDialog__ true
    #endif

    #define debugAssertM(exp, message) { \
        static bool __debugAssertIgnoreAlways__ = false; \
        if (!__debugAssertIgnoreAlways__ && !(exp)) { \
            G3D::_internal::_releaseInputGrab_(); \
            if (G3D::_internal::_handleDebugAssert_(#exp, message, __FILE__, __LINE__, __debugAssertIgnoreAlways__, __debugPromptShowDialog__)) { \
                 rawBreak(); \
            } \
            G3D::_internal::_restoreInputGrab_(); \
        } \
    }

    #define alwaysAssertM debugAssertM

#else  // Release
    #ifdef G3D_DEBUG_NOGUI
        #define __debugPromptShowDialog__ false
    #else
        #define __debugPromptShowDialog__ true
    #endif

    // In the release build, just define away assertions.
    #define rawBreak()
    #define debugAssert(exp)
    #define debugAssertM(exp, message)
    #define debugBreak()

    // But keep the always assertions
    #define alwaysAssertM(exp, message) { \
        if (!(exp)) { \
            G3D::_internal::_releaseInputGrab_(); \
            G3D::_internal::_handleErrorCheck_(#exp, message, __FILE__, __LINE__, __debugPromptShowDialog__); \
            exit(-1); \
        } \
    }

#endif  // if debug



namespace G3D {  namespace _internal {

#ifdef G3D_LINUX
    /**
     A pointer to the X11 display.  Initially NULL.  If set to a
     non-null value (e.g. by SDLWindow), debugAssert attempts to use
     this display to release the mouse/input grab when an assertion
     fails.
     */
    extern Display*      x11Display;

    /**
     A pointer to the X11 window.  Initially NULL.  If set to a
     non-null value (e.g. by SDLWindow), debugAssert attempts to use
     this window to release the mouse/input grab when an assertion
     fails.
     */
    extern Window        x11Window;
#endif

/**
 Pops up an assertion dialog or prints an assertion

 ignoreAlways      - return result of pressing the ignore button.
 useGuiPrompt      - if true, shows a dialog
 */
bool _handleDebugAssert_(
    const char* expression,
    const std::string& message,
    const char* filename,
    int         lineNumber,
    bool&       ignoreAlways,
    bool        useGuiPrompt);

void _handleErrorCheck_(
    const char* expression,
    const std::string& message,
    const char* filename,
    int         lineNumber,
    bool        useGuiPrompt);

/** Attempts to give the user back their mouse and keyboard if they 
    were locked to the current window.  
    @internal*/
void _releaseInputGrab_();

/** Attempts to restore the state before _releaseInputGrab_.  
    @internal*/
void _restoreInputGrab_();

}; }; // namespace

#endif

