/** @page guidemacosx Setting up a Project in Mac OS X

\htmlonly
<TABLE BORDER=0 WIDTH=80%><TR><TD><I><FONT FACE="Arial">
<A HREF="guidewelcome.html"><IMG SRC="backarrow.gif" BORDER=0 ALIGN=MIDDLE>
Welcome</A></I></FONT></TD><TD ALIGN=RIGHT><FONT FACE="Arial"><I>
<A HREF="guidetutorial.html">
Tutorial <IMG SRC="forwardarrow.gif" BORDER=0 ALIGN=MIDDLE></A></I></FONT></TD></TR></TABLE>
\endhtmlonly


@section introduction Introduction
Setting up %G3D for use with 
<a href="http://www.apple.com/macosx/features/xcode/">Apple Xcode</a>
is rather complicated, mostly due to the weirdnesses of Xcode
and Cocoa development. %G3D applications on OS X must be Cocoa
applications due to the SDL implementation. This document will
walk you through setting up a project. 

@section Prerequisites
These instructions assume that you have some basic familiarity with Xcode. 
These instructions have been designed for OS X 10.3 (Panther) and Xcode 1.5. 
Previous versions should of Xcode should work acceptably. Project Builder
might work, but hasn't been tested. 

@section startingxcode Starting a new project in Xcode

<ol>
<li><b>Start Xcode.</b>
</li>

<li>
<b>Create a new project.</b>Select File/New Project. In the dialog box that follows, select
Cocoa Application. Pick a location for the project on the file system.
</li>

<li><b>Delete main.m.</b> Xcode will create several files for you, including main.m.
Delete main.m from the project and the file system. Create a C++ file,
main.cpp, containing main(). 
<i>Note: The ".m" extension is for Objective-C files. Using Objective-C with
SDL has not been tested.</i> 
</li>

<li>
<b>Add <tt>OpenGL.framework</tt> to the Xcode project.</b> <tt>OpenGL.framework</tt> is
always located in <tt>/System/Library/Frameworks/</tt>. The dialog for
adding the framework to the project has a checkbox labeled "Copy items
into destination group's folder (if needed)". Make sure that this
checkbox is unchecked.
</li>

<li>
<b>Add <tt>SDL.framework</tt> to the Xcode project.</b>
The location of <tt>SDL.framework</tt> is a little more complicated,
unfortunately. The <tt>SDL.framework</tt> runtime is generally located in
<tt>/Library/Frameworks/</tt> -- note that this is different from
<tt>/System/Library/Frameworks</tt>.
However, the files needed for SDL development are usually located in
<tt>$(HOME)/Library/Frameworks</tt>. Add the runtime version,
<tt>/Library/Frameworks/SDL.framework</tt> to the Xcode project and then add
<tt>$(HOME)/Library/Frameworks</tt> to the "Frameworks Search Paths" setting
for the Xcode project.

To modify the Framework Search Paths settings:
<ol>
<li>
Using the mouse, select the project icon in the left pane of the
Xcode window. This icon usually has a blue Xcode icon.  Then press
Command-I (or the File/Get Info menu)
</li>

<li>
In the "All Settings" pane or in the "Search Paths" pane, add
<tt>$(HOME)/Library/Frameworks</tt> to the "Framework Search Paths" entry.
</li>
</ol>

<li>
<b>Add the path to the %G3D header files to the include search path.</b>
This setting is in the same location as Framework Search Paths. 
Select the project icon, press Command-I, and then modify the "Header
Search Path" entry to include the search path to the %G3D headers.
</li>

<li>
<b>Turn off ZeroLink.</b>
ZeroLink is an Xcode feature that allows for faster link times for
debug  builds of your application. Unfortunately, it doesn't work
with SDL for OS X and will cause link failures for SDL applications. 
Again, select the project icon in the left pane
and press Command-I (or the File/Get Info menu). Select the
"Development" build style from the drop down menu at the top of the
Project Info dialog, and then make sure that the ZeroLink  checkbox is
turned off. Also verify that ZeroLink is turned off for the
"Deployment" build style.
</li>

<li>
<b>Add the %G3D libraries to the project.</b>
Using the Project/Add To Project menu, add <tt>libG3D.a</tt> and 
<tt>libGLG3D.a</tt> to the project. Recent
versions of %G3D for OS X built the Independent JPEG Group (IJG) code
into a separate library (<tt>libjpeg.a</tt>). The IJG code has only recently
been folded into <tt>libG3D.a</tt>. If you're using %G3D 6.03 or earlier, 
you might have to add <tt>libjpeg.a</tt> to your project.
</li>

<li><b>Add "-lz" to "Other Linker Flags".</b>
This tells the linker to use the system's version of zlib, which G3D
requires.
</li>

<li>
<b>Very important: Set the link order of libraries.</b>
This is non-obvious but very important. Failure to set the link order
of libraries will give inexplicable link failures. In the left pane
of the Xcode project, select the "Target" item. This will have at
least one item under it corresponding to the name of the application.
Select the application target (it will have a triangle next to it)
and expand the triangle by clicking on it. Under the "Frameworks and Libraries" group
make sure that <tt>libG3D.a</tt> and <tt>libGLG3D.a</tt> are listed before
<tt>OpenGL.framework</tt>. Failure to do this will result in over 80 link
errors related to OpenGL extensions. 
</li>

<li>
<b>Add <tt>SDLMain.m</tt> and <tt>SDLMain.h.</tt></b>
SDL for OS X requires that we build <tt>SDLMain.m</tt> into the project.
Failure to do this will result in the symbol _main being undefined
when you try to link your %G3D application. <tt>SDLMain.m</tt> and <tt>SDLMain.h</tt>
are included in %G3D for OS X for convenience. Note that they are not
part of %G3D per se, and they are part of the SDL distribution.
</li>

<li>
<b>Write your code!</b>
You can now write code for %G3D just like you would for Linux or
Windows.
</li>

<li><b>Optional: nullify Windows cludges.</b> Define <tt>__stdcall</tt> and <tt>__cdecl</tt> 
to evaluate to nothing in the preprocessor:
<tt>__stdcall</tt> and <tt>__cdecl</tt> are
needed for Windows code, but they don't do anything on OS X.  You can
add <tt>__stdcall=</tt> and <tt>__cdecl=</tt> to the "Preprocessor Macros" section of
the Project Info.
</li>

<li><b>Optional: Set relative paths.</b>
In general when working in Xcode, make sure all source files have 
Path Type "Relative to Enclosing
Group" or "Relative to Project" selected in file info (rather than
"Absolute Path"). This is so that if the Xcode project is
moved or a directory name is changed, the project file links
will not break. If you highlight all source and header files and
press Cmd-I, you can set Path Type easily.</li>
</ol>

**/