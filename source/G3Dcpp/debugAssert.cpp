/**
 @file debugAssert.cpp

 Windows implementation of assertion routines.

 @author Morgan McGuire, graphics3d.com
 
 @created 2001-08-26
 @edited  2003-02-15
 */

#include "G3D/debugAssert.h"
#if _WIN32
    #include "windows.h"
    #include <tchar.h>
#endif
#include "G3D/format.h"
#include "G3D/prompt.h"
#include <string>
#include "G3D/debugPrintf.h"

#ifdef _MSC_VER
    // disable: "C++ exception handler used"
    #pragma warning (disable : 4530)
#endif // _MSC_VER

using namespace std;

namespace G3D { namespace _internal {
using namespace std;

#if _WIN32
static void postToClipboard(const char *text) {
    if (OpenClipboard(NULL)) {
        HGLOBAL hMem = GlobalAlloc(GHND | GMEM_DDESHARE, strlen(text) + 1);
        if (hMem) {
            char *pMem = (char*)GlobalLock(hMem);
            strcpy(pMem, text);
            GlobalUnlock(hMem);

            EmptyClipboard();
            SetClipboardData(CF_TEXT, hMem);
        }

        CloseClipboard();
        GlobalFree(hMem);
    }
}
#endif

bool _handleDebugAssert_(const char* expression,
                         const std::string& message,
                         const char* filename,
                         int   lineNumber,
                         bool &ignoreAlways,
                         bool  useGuiPrompt) {

    std::string le = "";
    std::string title = "Assertion failure";
    char* newline = "\n";

    #if _WIN32
        newline = "\r\n";

        // The last error value.  (Which is preserved across the call).
        DWORD lastErr = GetLastError();
    
        // The decoded message from FormatMessage
        LPTSTR formatMsg = NULL;

        if (NULL == formatMsg) {
            FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                          FORMAT_MESSAGE_IGNORE_INSERTS |
                          FORMAT_MESSAGE_FROM_SYSTEM,
                            NULL,
                            lastErr,
                            0,
                            (LPTSTR)&formatMsg,
                            0,
                            NULL);
        }

        // Make sure the message got translated into something.
        LPTSTR realLastErr;
        if (NULL != formatMsg) {
            realLastErr = formatMsg;
        } else {
            realLastErr = _T("Last error code does not exist.");
        }

		if (lastErr != 0) {
	        le = G3D::format("Last Error (0x%08X): %s\r\n\r\n", lastErr, (LPCSTR)realLastErr);
		}

        // Get rid of the allocated memory from FormatMessage.
        if (NULL != formatMsg) {
            LocalFree((LPVOID)formatMsg);
        }

        char modulePath[MAX_PATH];
        GetModuleFileName(NULL, modulePath, MAX_PATH);

        const char* moduleName = strrchr(modulePath, '\\');
        title = title + string(" - ") + string(moduleName ? (moduleName + 1) : modulePath);

    #endif

    // Build the message.
        std::string outputMessageText;

    outputMessageText = 
        G3D::format("%s%s%sExpression: %s%s%s:%d%s%s%s", 
                 message.c_str(), newline, newline, expression, newline, 
                 filename, lineNumber, newline, newline, le.c_str());

    #if _WIN32
        postToClipboard(outputMessageText.c_str());
    #endif

    const int cBreak = 0;
    const int cIgnore = 1;
    const int cIgnoreAlways = 2;
    const int cAbort = 3;

    static char* choices[] = {"Debug", "Ignore", "Ignore Always", "Abort"};

    int result = G3D::prompt(title.c_str(), outputMessageText.c_str(), (const char**)choices, 4, useGuiPrompt);

    #if _WIN32
        // Put the incoming last error back.
        SetLastError(lastErr);
    #endif

    switch (result) {
    // -1 shouldn't actually occur because it means 
    // that we're in release mode.
    case -1:
    case cBreak:
        return true;
        break;

    case cIgnore:
        return false;
        break;
   
    case cIgnoreAlways:
        ignoreAlways = true;
        return false;
        break;

    case cAbort:
        exit(-1);
        return false;
        break;
    default:
        // Shouldn't get here
        return false;
        break;
    }

}

}; }; // namespace


