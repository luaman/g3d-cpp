/** 
  @file GConsole/GConsole.cpp

*/

#include "GConsole.h"
#include <GLG3D/RenderDevice.h>
#include <GLG3D/Draw.h>

namespace G3D {

GConsole::GConsole(const GFontRef& f, const Settings& s, Callback callback, void* data) :
    m_font(f),
    m_settings(s),
    m_cursorPos(0),
    m_callback(callback),
    m_callbackData(data),
    m_bufferShift(0) {

    debugAssert(m_font.notNull());

    unsetRepeatKeysym();
    m_keyDownTime = System::time();
    setActive(true);

    /*
    // For debugging:
    for (int i = 0; i < 100; ++i) {
        m_buffer.pushBack(format("%d %d %d %d %d", i, i, i, i, i));
    }
    m_buffer.pushBack("set basecolor = (1, 0, 1)");
    m_buffer.pushBack("clear scene");
    m_buffer.pushBack("cd ..");
    m_currentLine = "load \"horse.ifs\"";
    m_cursorPos = m_currentLine.size() + 1;
    */
}


GConsole::~GConsole() {
    // Intentionally empty
}


void GConsole::setActive(bool a) {
    if (m_active != a) {
        unsetRepeatKeysym();
        m_active = a;
    }
}


void GConsole::issueCommand() {

    string oldCommandLine = m_currentLine;
    
    m_currentLine = string();
    m_cursorPos = 0;

    // Jump back to the end
    m_bufferShift = 0;

    if (m_settings.commandEcho) {
        print(oldCommandLine, m_settings.defaultCommandColor);
    }

    m_history.push(oldCommandLine);

    onCommand(oldCommandLine);
}


void GConsole::onCommand(const string& cmd) {
    if (m_callback) {
        m_callback(cmd, m_callbackData);
    }
}


void GConsole::clearBuffer() {
    m_buffer.clear();
    m_bufferShift = 0;
}


void GConsole::clearHistory() {
    m_history.clear();
}


void __cdecl GConsole::printf(const char* fmt, ...) {
	va_list arg_list;
	va_start(arg_list, fmt);
    vprintf(fmt, arg_list);
    va_end(arg_list);
}


void __cdecl GConsole::vprintf(const char* fmt, va_list argPtr) {
    print(vformat(fmt, argPtr), m_settings.defaultPrintColor);
}


void GConsole::print(const string& s, const Color4& c) {
    // If the buffer is too long, pop one from the front
    if (m_buffer.size() >= m_settings.maxBufferLength) {
        m_buffer.popFront();
    }

    m_buffer.pushBack(Text(s, c));
}


void GConsole::beginCompletion() {
    // TODO: If we are in the middle of the string, 
    // only try to complete a filename
    m_completionBase = m_currentLine;
}


void GConsole::endCompletion() {
    // Cancel the current completion
    m_completionBase = string();
}


void GConsole::processRepeatKeysym() {
    if (m_repeatKeysym.sym != SDLK_TAB) {
        endCompletion();
    }

    switch (m_repeatKeysym.sym) {
    case SDLK_UNKNOWN:
        // No key
        break;

    case SDLK_RIGHT:
        if (m_cursorPos < m_currentLine.size()) {
            ++m_cursorPos;
        }
        break;

    case SDLK_LEFT:
        if (m_cursorPos > 0) {
            --m_cursorPos;
        }
        break;

    case SDLK_DELETE:
        if (m_cursorPos < m_currentLine.size()) {
            m_currentLine = 
                m_currentLine.substr(0, m_cursorPos) + 
                m_currentLine.substr(m_cursorPos + 1, string::npos);
        }
        break;

    case SDLK_BACKSPACE:
        if (m_cursorPos > 0) {
            m_currentLine = 
                m_currentLine.substr(0, m_cursorPos - 1) + 
                ((m_cursorPos < m_currentLine.size()) ? 
                  m_currentLine.substr(m_cursorPos, string::npos) :
                  string());
           --m_cursorPos;
        }
        break;

    case SDLK_UP:
        // TODO: history
        break;

    case SDLK_DOWN:
        // TODO: history
        break;

    case SDLK_TAB:
        // Command completion
        if ((m_repeatKeysym.mod & KMOD_SHIFT) != 0) {
            completeCommand(-1);
        } else {
            completeCommand(1);
        }
        break;

    case SDLK_PAGEUP:
        if (m_bufferShift < m_buffer.length() - m_settings.numVisibleLines + 1) {
            ++m_bufferShift;
        }
        break;

    case SDLK_PAGEDOWN:
        if (m_bufferShift > 0) {
            --m_bufferShift;
        }
        break;

    case SDLK_RETURN:
        issueCommand();
        break;

    default:
        if ((m_repeatKeysym.sym >= SDLK_SPACE) &&
            (m_repeatKeysym.sym <= SDLK_z)) {

            // Insert character
            char c = m_repeatKeysym.unicode & 0xFF;
            m_currentLine = 
                m_currentLine.substr(0, m_cursorPos) + 
                c +
                ((m_cursorPos < m_currentLine.size()) ? 
                  m_currentLine.substr(m_cursorPos, string::npos) :
                  string());
            ++m_cursorPos;

        } else {
            // This key wasn't processed by the console
            debugAssertM(false, "Unexpected repeat key");
        }
    }
}


void GConsole::completeCommand(int direction) {
    // TODO
}

void GConsole::setRepeatKeysym(SDL_keysym key) {
    m_keyDownTime = System::time();
    m_keyRepeatTime = m_keyDownTime + m_settings.keyRepeatDelay;
    m_repeatKeysym = key;
}


void GConsole::unsetRepeatKeysym() {
    m_repeatKeysym.sym = SDLK_UNKNOWN;
}


bool GConsole::onEvent(const GEvent& event) {

    if (! m_active) {
        if ((event.type == SDL_KEYDOWN) &&
            ((event.key.keysym.unicode & 0xFF) == '~')) {

            // '~': Open console
            setActive(true);
            return true;

        } else {

            // Console is closed, ignore key
            return false;
        }
    }

    switch (event.type) {
    case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
        case SDLK_ESCAPE:
            // Close the console
            setActive(false);
            return true;

        case SDLK_RIGHT:
        case SDLK_LEFT:
        case SDLK_DELETE:
        case SDLK_BACKSPACE:
        case SDLK_UP:
        case SDLK_DOWN:
        case SDLK_TAB:
        case SDLK_PAGEUP:
        case SDLK_PAGEDOWN:
        case SDLK_RETURN:
            setRepeatKeysym(event.key.keysym);
            processRepeatKeysym();
            return true;
            break;

        default:
            if ((event.key.keysym.sym >= SDLK_SPACE) &&
                (event.key.keysym.sym <= SDLK_z)) {

                setRepeatKeysym(event.key.keysym);
                processRepeatKeysym();
                return true;

            } else {
                // This key wasn't processed by the console
                return false;
            }
        }
        break;

    case SDL_KEYUP:
        if (event.key.keysym.sym == m_repeatKeysym.sym) {
            unsetRepeatKeysym();
            return true;
        }
        break;
    }

    return false;
}


void GConsole::onGraphics(RenderDevice* rd) {
    
    if (! m_active) {
        return;
    }

    static const Color4 backColor(0.0f, 0.0f, 0.0f, 0.3f);
    static const float  pad = 2;
    const float         fontSize = m_settings.lineHeight - 3;
   
    Rect2D rect;

    RealTime now = System::time();

    bool hasKeyDown = (m_repeatKeysym.sym != SDLK_UNKNOWN);

    // If a key is being pressed, process it on a steady repeat schedule.
    if (hasKeyDown && (now > m_keyRepeatTime)) {
        processRepeatKeysym();
        m_keyRepeatTime = now + 1.0 / m_settings.keyRepeatRate;
    }


    // Only blink the cursor when keys are not being pressed or
    // have not recently been pressed.
    bool solidCursor = hasKeyDown || (now - m_keyRepeatTime < 1.0 / m_settings.blinkRate);
    if (! solidCursor) {
        static const RealTime zero = System::time();
        solidCursor = isOdd((int)((now - zero) * m_settings.blinkRate));
    }

    {
        float w = rd->getWidth();
        float h = rd->getHeight();
        float myHeight = m_settings.lineHeight * m_settings.numVisibleLines + pad * 2;

        rect = Rect2D::xywh(pad, h - myHeight - pad, w - pad * 2, myHeight);
    }

    rd->push2D();

        rd->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);
        Draw::fastRect2D(rect, rd, backColor);

        rect = Rect2D::xyxy(rect.x0y0() + Vector2(2,1), rect.x1y1() - Vector2(2,1));
        // Print history
        for (int count = 0; count < m_settings.numVisibleLines - 1; ++count) {
            int q = m_buffer.size() - count - 1 - m_bufferShift;
            if (q >= 0) {
                m_font->draw2D(rd, m_buffer[q].value, rect.x0y1() - Vector2(0, m_settings.lineHeight * (count + 2)), fontSize, m_buffer[q].color);
            }
        }

        if (m_bufferShift > 0) {
            // Draw a line indicating that we aren't looking at the bottom of the buffer
            rd->setColor(Color3::white());
            rd->setLineWidth(1.0f);
            rd->beginPrimitive(RenderDevice::LINES);
                Vector2 v(rect.x0() - 0.3, rect.y1() - m_settings.lineHeight + 1 - 0.3);
                rd->sendVertex(v);
                rd->sendVertex(v + Vector2(rect.width(), 0));
            rd->endPrimitive();
        }

        m_font->draw2D(rd, m_currentLine, rect.x0y1() - Vector2(0, m_settings.lineHeight), fontSize, m_settings.defaultCommandColor);

        // Draw cursor
        if (solidCursor) {
            // Put cursor under a specific character.  We need to check bounds to do this because we might not
            // have a fixed width font.
            Vector2 bounds;
            if (m_cursorPos > 0) {
                bounds = m_font->get2DStringBounds(m_currentLine.substr(0, m_cursorPos), fontSize);
            }

            m_font->draw2D(rd, "_", rect.x0y1() + Vector2(bounds.x, -m_settings.lineHeight), fontSize, m_settings.defaultCommandColor);
        }

    rd->pop2D();
}

} // G3D
