/** @page guideapp Application Framework

  \htmlonly
<TABLE BORDER=0 WIDTH=80%><TR><TD><I><FONT FACE="Arial">
<A HREF="guidedebug.html"><IMG SRC="backarrow.gif" BORDER=0 ALIGN=MIDDLE>
Debugging</A></I></FONT></TD><TD ALIGN=RIGHT><FONT FACE="Arial"><I>
<A HREF="guidevar.html">
Vertex Arrays <IMG SRC="forwardarrow.gif" BORDER=0 ALIGN=MIDDLE></A></I></FONT></TD></TR></TABLE>
\endhtmlonly

 @section Introduction

G3D::GApp is an application-starter class. To easily create a %G3D application, make a subclass of
G3D::GApp, which handles any state shared between all pieces of your program. 

A G3D::GApplet is a usable <em>but not standalone</em> component of your application. Each GApplet corresponds to a 
different major UI state. For example, the main menu of a game is one GApplet, and the actual 
game screen is another. If you have a big, modal in-game menu, that can be another GApplet.
(A GApplet is <em>not</em> like a Java applet. A Java applet is a little application, but complete, and standalone,
while a GApplet is neither complete nor standalone.)

You may create one or more GApplet subclasses and instantiate them at the beginning of the program. Typically, 
they will be stored in your GApp subclass. You do not need to use GApplet's, however; you can just use GApp,
if your application isn't too complex.

You write code that calls G3D::GApplet::run on the current GApplet. That applet releases control by setting 
GApplet::endApplet = true. Your master loop (implemented inside the G3D::GApp::main) then chooses the next 
GApplet to run and invokes run() on it. If anything sets G3D::GApp::endProgram to true, the entire 
program should quit. 

@section gapps GApp and GAppSettings

GApp contains a lot of functionality that might make your life easier or better! In particular,
look at G3D::GWindowSettings for lots of rendering controls. G3D::GApp's construct takes an optional 
G3D::GAppSettings argument, whose window attribute is an instance of G3D::GWindowSettings. To turn on 
full-screen anti-aliasing, use G3D::GWindowSettings.fsaaSamples. You can even turn on stereo 
with G3D::GWindowSettings.stereo, set where the window should initially appear with G3D::GWindowSettings.stereo.x, 
or set the window to start up as full-screen with G3D::GWindowSettings.fullscreen. 

GApp also gives you a camera with keyboard controls (a, s, d, w) and mouse controls 
(left-mouse-button to look around), which you can use before you've written your own 
camera/navigation controls. SeeG3D::GApp::debugCamera. You also get an easy way to exit -- just hit escape. 

@section applets Applets

 G3D::GApp, G3D::GApplet,
 G3D::GAppSettings, G3D::GWindowSettings

@section ornotgapp Or Not!

The G3D::GApp classes are provisional: the interface is likely to change. You don't have to use them; 
it is fine to instantiate G3D::RenderDevice and the other classes yourself. See 
<A HREF="../demos/main-no-Gapp.cpp">demos/main-no-GApp.cpp</A> for an example
of how to <em>not</em> use GApp.

 @section models Models
  
  @section selection Selection
   
     When <CODE>G3D::UserInput::keyPressed(SDLK_LEFT_MOUSE)</CODE> is true, the user
     has clicked the mouse.  You can convert the G3d::UserInput::mouseXY position to
     a 3D G3D::Ray with G3D::GCamera::worldRay and use G3D::Ray::intersects or 
     G3D::CollisionDetection methods to see if that ray hits the G3D::PosedModel::getBounds 
     region or a primitive like G3D::Capsule.
  
  @section controls User Interface

   Consider using a GUI library (GLUT, FLTK, wxWindows, qtWindows, .NET) to  
   make your UI.

   FPS controls via G3D::ManualCameraController.  The GApp creates
   one for you and if you call <code>G3D::GApp::setDebugMode(true)</CODE>
   you can press TAB to use it.
  
   key strokes, mouse, and joystick through G3D::UserInput

  G3D::RenderDevice::push2D to switch to 2D rendering mode (don't forget 
  to call G3D::RenderDevice::pop2D at the end).  This mode lets you draw
  text with G3D::GFont and render UI elements.

  G3D::Rect2D::contains detects whether the mouse is within a 2D region.

  */