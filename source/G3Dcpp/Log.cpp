/**
  @file Log.cpp

  @maintainer Morgan McGuire, morgan@graphics3d.com
  @cite       Backtrace by Aaron Orenstein
  @created 2001-08-04
  @edited  2003-02-15
 */

#include "../include/G3D/Log.h"
#include "../include/G3D/format.h"
#include "../include/G3D/Array.h"
#include "../include/G3D/fileutils.h"
#include <time.h>

#ifdef _MSC_VER
    #include <Windows.h>
    #include <imagehlp.h>
#endif

#ifndef _WIN32
    #include <stdarg.h>
#endif

namespace G3D {

Log::Log(const std::string& filename, int stripFromStackBottom) : 
    stripFromStackBottom(stripFromStackBottom) {

    logFile = fopen(filename.c_str(), "w");

    if (logFile == NULL) {
        std::string drive, base, ext;
        Array<std::string> path;
        parseFilename(filename, drive, path, base, ext);
        std::string logName = base + ((ext != "") ? ("." + ext) : ""); 

        // Write time is greater than 1ms.  This may be a network drive.... try another file.
        #ifdef _WIN32
            if (fileExists("c:/tmp")) {
                logName = std::string("c:/tmp/") + logName;
            } else if (fileExists("c:/temp")) { 
                logName = std::string("c:/temp/") + logName;
            } else {
                logName = std::string("c:/") + logName;
            }
        #else
            logName = std::string("/tmp/") + logName;
        #endif

        logFile = fopen(logName.c_str(), "w");
    }

    // Turn off buffering.
    setvbuf(logFile, NULL, _IONBF, 0);

    fprintf(logFile, "Application Log\n");
    time_t t;
    time(&t);
    fprintf(logFile, "Start: %s\n", ctime(&t));
    fflush(logFile);
}


Log::~Log() {
    section("Shutdown");
    println("Closing log file");
    fclose(logFile);
}


FILE* Log::getFile() const {
    return logFile;
}


void Log::section(const std::string& s) {
    fprintf(logFile, "_____________________________________________________\n");
    fprintf(logFile, "\n    ###    %s    ###\n\n", s.c_str());
}


void __cdecl Log::printf(const char* fmt, ...) {
    printHeader();

	va_list arg_list;
	va_start(arg_list, fmt);
    print(vformat(fmt, arg_list));
    va_end(arg_list);
}


void Log::print(const std::string& s) {
    printHeader();
    fprintf(logFile, "%s", s.c_str());
}


void Log::println(const std::string& s) {
    printHeader();
    fprintf(logFile, "%s\n", s.c_str());
}


/**
 Attempts to produce the stack frame list as a string.

  @param stripFromTop Number of stack frames on the top of the stack to hide.
 */
static std::string getBacktrace(
    int maxFrames,
    int stripFromTop = 0,
    int stripFromBottom = 0) {

    #ifdef _MSC_VER

        Array<std::string> trace;

	    HANDLE process = GetCurrentProcess();
	    BOOL success = SymInitialize(process, NULL, true);

	    int _ebp;
	    __asm { mov _ebp,ebp } 

	    int frame = 0;
	    while (frame < maxFrames) {
		    int pc = ((int*)_ebp)[1];
		    _ebp = ((int*)_ebp)[0];
            
            if (pc == 0) {
                break;
            }

		    char csymbol[sizeof(IMAGEHLP_SYMBOL) + 256];
		    memset(csymbol, 0, sizeof(csymbol));

		    IMAGEHLP_SYMBOL* symbol = (IMAGEHLP_SYMBOL*)csymbol;
		    symbol->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);
		    symbol->MaxNameLength = 256;

		    success = SymGetSymFromAddr(process, pc, 0, symbol);
            if (! success) {
                break;
            }

            trace.append(symbol->Name);

		    ++frame;
	    }


        std::string result;
        for (int i = trace.size() - stripFromBottom - 1; i >= stripFromTop; --i) {
            result += trace[i];
            if (i != stripFromTop) {
                result += " > ";
            }
        }

        return result;

    #else

        // On non-MSVC, just return the empty string
        return "";

    #endif
}


void Log::printHeader() {
    time_t t;
    if (time(&t) != ((time_t)-1)) {
        /*
        char buf[32];
        strftime(buf, 32, "[%H:%M:%S]", localtime(&t));
    
         Removed because this doesn't work on SDL threads.

        #ifdef _DEBUG
            std::string bt = getBacktrace(15, 2, stripFromStackBottom);
            fprintf(logFile, "\n %s %s\n\n", buf, bt.c_str());
        #endif

        fprintf(logFile, "\n %s \n", buf);
        */

    } else {
        println("[Error getting time]");
    }
}

}
