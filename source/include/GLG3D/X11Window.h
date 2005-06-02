/**
 @file Win32Window.h
  
 A GWindow that uses the Win32 API.

 @created 	  2005-06-04
 @edited  	  2005-06-04
    
 Copyright 2000-2005, Morgan McGuire.
 All rights reserved.
*/

#ifndef G3D_X11WINDOW_H
#define G3D_X11WWINDOW_H

#include <G3D/platform.h>

#if defined(G3D_LINUX)

// Current implementation: X11Window *is* SDLWindow
#include "G3D/SDLWindow.h"

#define X11Window SDLWindow

#endif // if Linux

#endif