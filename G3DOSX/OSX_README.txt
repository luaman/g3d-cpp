How to Build a G3D application on OS X 

Caveats
-------

The G3D libraries on OS X are still in a state of early development,
and there are not yet many users.  Consequently, you may find the OS X
version of OS X less reliable than the Win32 or Linux versions.  The
use of G3D on OS X for CS 224 is even not supported and is fraught
with peril.  You've been warned. 

However, if you're really bold and daring, you really like Xcode, and
you really like your Macintosh, you're welcome to try it out.  

OS X version 10.3 (Panther) is the only version where G3D is known to
work to any degree.  It is possible that G3D will work on previous
versions of OS X, but I don't know if anyone has ever done it. 

Prerequisites
-------------

You will need:

1.) OS X version 10.3 (Panther)
2.) The Developer Tools from OS X 10.3, including Xcode

3.) SDL Development Libraries, version 1.2.6.  Available
    at http://www.libsdl.org/ 

    You will need the two files SDLMain.m and SDLMain.h
    from the SDL distribution. As a convenience, these files
    are also in the G3D distribution. You can use these
    files from either place since they're identical except
    for comments. 

4.) The G3D libraries, obviously.  You will need the files
    libG3D.a, libGLG3D.a and libjpeg.a

How to Build a G3D application on OS X version 10.3 Using Xcode
---------------------------------------------------------------

1.) Start Xcode 

2.) Start a new project by selecting the menu item
    File/New Project

3.) Select Cocoa Project 

4.) Selecting a new Cocoa Project automatically generates
    main.m and a precompiled header (whose name depends 
    on your project name, but it always ends in .pch).
    If you want to use a precompiled header, you can 
    place any includes in this .pch file, but you
    should probably remove Cocoa.h.

5.) Add SDLMain.m and SDLMain.h to your project.
    you absolutely need these. Use the Project/Add Files menu
    for this. 

6.) Add the OpenGL and SDL frameworks to your project. 
    Use the Project/Add Frameworks menu for this. 

7.) Add the following libraries to your project 
    libGLG3D.a, libG3D.a, libjpeg.a to your project.  I often
    have to use the Finder to drag and drop these files into
    my project.  I also typically put them in the "Frameworks"
    section, before OpenGL.framework or SDL.framework.
  
8.) Disable the ZeroLink flag in the Xcode settings
    for this project.  ZeroLink is an Xcode feature 
    that allows for faster startup when debugging 
    or running a program from the Xcode IDE, but 
    this feature is not compatible with SDL.  

7.)  Add the Zlib library

    Add -lz to "Other Linker Flags" in the project settings. 

    To get to this setting, select the toplevel icon 
    in the left pane of your Xcode project.  For example,	
    if your project is called "MyApp", there will be a blue
    Xcode with the label "MyApp" in the "Groups and Files" 
    pane of your Xcode project.  Select this icon, and then 
    select the menu item "Project/Get Info" (or use the
    key equivalent Command-I). Select the "Styles" tab
    for the project info.  Under the "Common Settings" 
    pane, there is a setting called "Other Linker Flags".
    Add -lz to "Other Linker Flags" on all build Styles.
    Usually there is a build style for Development
    and one for Deployment.

8.) You'll have to add the appropriate header paths 
    to your Xcode project so the compiler can find the
    G3D header files.


How to Build a G3D Application on OS/X Using Project Builder
------------------------------------------------------------

<This section is still under development> 

Setting up your project for use with SDL is the most
painful part of using Xcode and G3D.  On the other hand,
using SDL with Project Builder is fairly straightforward.

1.) Open Project Builder
2.) Start a new project.
3.) Select "SDL Application" in the "New Project" dialog. 
4.) Add the G3D libraries: libjpeg.a, libG3D.a and libGLG3D.a
5.) Write your main function and then compile. 



If you have problems
--------------------

If you have a problem with G3D on OS X, feel free to post to the
G3D user's forum on Sourceforge. 









