/**
 * Window.h
 *
 * Support routines for tests.
 *
 * @author	   Morgan McGuire and Max McGuire 
 * @created 	9/09/00
 * @edited  	9/22/00
 *
 * Copyright 2000, Morgan McGuire.
 * All rights reserved.
 */

#ifndef WINDOW_H
#define WINDOW_H

#include <windows.h>
#include <string>
#include <time.h>

const UINT BLIT_BUFFER = 0xC001;

class Window
{

public:
   Window();
   virtual ~Window();

   virtual bool create(const std::string& name, int width, int height);

   void close();
   void requestRedraw();
   void run();

   HDC hDC;

   int width, height;
   unsigned int* buffer;

   /**
    * Buffer bitmap info used for converting it to a DIB.
    */
   BITMAPINFO bitmap_info;

   HWND  window;
private:
   
   static LRESULT WINAPI window_proc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);


};

#endif