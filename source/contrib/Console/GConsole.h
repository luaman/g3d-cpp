/**
 @file GConsole/GConsole.h

 Copyright 2006, Morgan McGuire, morgan3d@users.sf.net
 All rights reserved.

 Available under the BSD License.
 */

#ifndef G3D_GCONSOLE_H
#define G3D_GCONSOLE_H

#include "G3D/platform.h"

#if G3D_VER < 60900
    #error Requires G3D 6.09 or later
#endif

#include "G3D/Array.h"
#include "G3D/Queue.h"
#include "GLG3D/GFont.h"
#include "GLG3D/GWindow.h"

namespace G3D {

class RenderDevice;

/**
 Command-line console.

 When enter is pressed, onCommand is invoked.  The default implementation calls
 the callback function; this allows you to add a command processor either
 by subclassing GConsole or by passing a function to the constructor.

 ~: Open console (or write your own code that calls setActive)
 Esc: close console
 Enter: issue command
 Up arrow: scroll through history
 Down arrow: scroll through history
 Left arrow: cursor left
 Right arrow: cursor right

 Tab: reserved for substitute current completion

 <B>Beta API</B>
 Future versions may support access to the constants for blink rate and key repeat,
 provide colored fonts and line wrapping.

 */
class GConsole {
public:
    /** To allow later change to std::wstring */
    typedef std::string string;

    typedef void(*Callback)(const string&, void*);

    void setCallback(Callback c, void* data);

protected:

    /** Cursor flashes per second. */
    float               m_blinkRate;

    /** Keypresses per second. */
    float               m_keyRepeatRate;

    /** Pixel height between lines when displayed. */
    float               m_lineHeight;

    /** Number of lines visible at any time. */
    int                 m_numVisibleLines;

    /** Invoked when the user presses enter.  
        Default implementation calls m_callback. */
    virtual void onCommand(const string& command);

private:

    Callback            m_callback;
    void*               m_callbackData;

    /** Key that is currently auto-repeating. */
    SDL_keysym          m_repeatKeysym;

    /** If true, typed commands are automatically added to the buffer. */
    bool                m_commandEcho;

    GFontRef            m_font;

    /** Previously executed commands. */
    Array<string>       m_history;

    /** Previously displayed text. */
    Queue<string>       m_buffer;

    int                 m_maxBufferLength;

    /** Number of lines before the end of the buffer that are visible (affected
        by page up/page down).*/
    int                 m_bufferShift;

    bool                m_active;

    /** Currently entered command. */
    string              m_currentLine;

    /** Position of the cursor within m_currentLine (0 is the first slot) */
    int                 m_cursorPos;

    /** Time at which setRepeatKeysym was called. */
    RealTime            m_keyDownTime;

    /** Time at which the key will repeat (if down). */
    RealTime            m_keyRepeatTime;

    /** Delay before the first key repeat. */
    RealTime            m_keyRepeatDelay;

    /** Called from onEvent when a key is pressed. */
    void setRepeatKeysym(SDL_keysym key);

    /** Called from onEvent when the repeat key is released. */
    void unsetRepeatKeysym();

    /** Called from onGraphics and onEvent to enact the action triggered by the repeat key. */
    void processRepeatKeysym();

    /** Invoked when the user presses enter. */
    void issueCommand();

    void print(const string& s);

public:

    GConsole(const GFontRef& f, Callback c = NULL, void* callbackData = NULL);

    virtual ~GConsole();

    void setActive(bool a);

    inline bool active() const {
        return m_active;
    }

    /** Clear displayed text. */
    void clearBuffer();

    /** Clear command history. */
    void clearHistory();

    /** Print to the buffer. */
    void __cdecl printf(const char* fmt, ...) G3D_CHECK_PRINTF_METHOD_ARGS;

    /** Print to the buffer. */
    void __cdecl vprintf(const char*, va_list argPtr) G3D_CHECK_VPRINTF_METHOD_ARGS;

    /** Call to render the console */
    void onGraphics(RenderDevice* rd);

    /** Pass all events to the console. It returns true if it processed (consumed) the event.*/
    bool onEvent(const GEvent& event);
};

} //G3D

#endif
