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
 Shift+Tab: Reserved for command completion

 <B>Beta API</B>
 Future versions may support access to the constants for blink rate and key repeat,
 provide colored fonts and line wrapping.

 */
class GConsole {
public:
    /** To allow later change to std::wstring */
    typedef std::string string;

    class Settings {
    public:
        /** Cursor flashes per second. */
        float               blinkRate;

        /** Keypresses per second. */
        float               keyRepeatRate;

        /** Pixel height between lines when displayed. (font is slightly smaller than this) */
        float               lineHeight;

        /** Number of lines visible at any time. */
        int                 numVisibleLines;

        /** Maximum number of lines of scrollback */
        int                 maxBufferLength;

        /** Delay before the first key repeat in seconds. */
        RealTime            keyRepeatDelay;

        /** If true, commands are shown in the buffer. */
        bool                commandEcho;

        /** If true, tab completion includes filenames from the local disk. */
        bool                performFilenameCompletion;

        /** If true, tab completion includes issued commands and commands in the completionSeed array. */
        bool                performCommandCompletion;

        Color4              defaultCommandColor;

        Color4              defaultPrintColor;

        /** 
         Commands that can be completed by TAB, in addition to those in the history.
         Include common keywords here, for example, to seed the command completion buffer.
         */
        Array<string>       commandCompletionSeed;

        Settings() : 
            lineHeight(13),
            numVisibleLines(10),
            blinkRate(3),
            keyRepeatRate(16),
            keyRepeatDelay(0.25),
            commandEcho(true),
            maxBufferLength(2000),
            allowFilenameCompletion(true),
            defaultCommandColor(Color3::white()),
            defaultPrintColor(0.8, 1.0, 0.8) {
            }
    };

    typedef void(*Callback)(const string&, void*);

    void setCallback(Callback c, void* data);

protected:

    /** Invoked when the user presses enter.  
        Default implementation calls m_callback. */
    virtual void onCommand(const string& command);

private:

    Settings            m_settings;

    Callback            m_callback;
    void*               m_callbackData;

    /** Key that is currently auto-repeating. */
    SDL_keysym          m_repeatKeysym;

    GFontRef            m_font;

    /** Current history line being retrieved when using UP/DOWN.
        When a history command is used unmodified,
        the history index sticks.  Otherwise it resets to the end
        of the list on Enter.*/
    int                 m_historyIndex;

    /** When true, the history item has been modified since the last UP/DOWN. */
    bool                m_resetHistoryIndexOnEnter;

    /** Previously executed commands. */
    Array<string>       m_history;

    class Text {
    public:
        string          value;
        Color4          color;

        inline Text() {}
        inline Text(const string& s, const Color4& c) : value(s), color(c) {}
    };

    /** Previously displayed text. */
    Queue<Text>         m_buffer;

    /** Number of lines before the end of the buffer that are visible (affected
        by page up/page down).*/
    int                 m_bufferShift;

    /** True when the console is open and processing events.*/
    bool                m_active;

    /** Currently entered command. */
    string              m_currentLine;

    /** When command completion has begun, this is the base string off 
        which completions are being matched.  Set to the empty string 
        as soon as a non-tab key is pressed.*/
    string              m_completionBase;

    /** Position of the cursor within m_currentLine (0 is the first slot) */
    int                 m_cursorPos;

    /** Time at which setRepeatKeysym was called. */
    RealTime            m_keyDownTime;

    /** Time at which the key will repeat (if down). */
    RealTime            m_keyRepeatTime;

    /** Invoked from processRepeatKeysym when a non-completion key is pressed. 
        */
    void endCompletion();

    /** Called from processCompletion the first time TAB is pressed. */
    void beginCompletion();

    /** Invoked from processRepeatKeysym to handle command completion keys. */
    void completeCommand(int direction);

    /** Called from onEvent when a key is pressed. */
    void setRepeatKeysym(SDL_keysym key);

    /** Called from onEvent when the repeat key is released. */
    void unsetRepeatKeysym();

    /** Called from onGraphics and onEvent to enact the action triggered by the repeat key. */
    void processRepeatKeysym();

    /** Invoked when the user presses enter. */
    void issueCommand();

    /** Called from repeatKeysym on UP/DOWN. */
    void historySelect(int direction);

    /** Issues text to the buffer. */
    void print(const string& s, const Color4& c);

public:

    GConsole(const GFontRef& f, const Settings& s = Settings(), Callback c = NULL, void* callbackData = NULL);

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
