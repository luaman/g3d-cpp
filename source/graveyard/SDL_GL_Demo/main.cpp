/**
 @file SDL_GL_Demo/main.cpp

 A sample application using OpenGL, SDL, and G3D.  This example 
 is nominally platform independent but has only been tested under Win32.
 See glheaders.h for more information on configuring your project
 for SDL and OpenGL.

 <P>
 This sample is a good starting point for writing platform independent
 realtime 3D applications.  Because this application uses SDL, you must
 download SDL from <A HREF="http://www.libsdl.org">http://www.libsdl.org</A>
 and install it.  Note that you will be bound by the LGPL when you
 do this.

 <P>
 To run this demo, you must install SDL and copy SDL.DLL to the same directory
 as main.cpp.
 
 <P>
 Requires:
   OpenGL
   SDL
   G3D
   GLG3D

 @maintainer Morgan McGuire, matrix@graphics3d.com
 @created 2002-01-01
 @edited  2002-09-09
 */

#include "glheaders.h"
    
int main(int argc, char** argv) {

    initGL();

    std::string x = getOpenGLState(false);
    debugPrintf("%s\n", x.c_str());

    bool _endProgram = false;

    // Main loop
    do {

        // Event handling
	    SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch(event.type) {
            case SDL_QUIT:
	            _endProgram = true;
	            break;

		    case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    _endProgram = true;
                    break;
                // TODO: Add other key handlers
                }
                break;

            
                // TODO: Add other event handlers
		    }
        }

        glClear(GL_COLOR_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glFrustum(-1, 1, -1, 1, -2, 2);

        // TODO: Your rendering code goes here; replace the simple triangle below.
        // Use getWindowHDC() if you want to make some kind of Win32 call
        static double angle = 0;
        angle += .01;
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glRotatef(angle, 0, 0, 1);
        glBegin(GL_TRIANGLES);
            glColor (Color3::BLUE);
            glVertex(Vector3(-0.5, -0.5, -1.0));
            glVertex(Vector3( 0.5, -0.5, -1.0));
            glColor (Color3::YELLOW);
            glVertex(Vector3( 0.0,  0.5, -1.0));
        glEnd();

        SDL_GL_SwapBuffers();

	} while (! _endProgram);

    SDL_Quit();

    return 0;
}
