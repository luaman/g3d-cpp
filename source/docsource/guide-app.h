/** @page guideapp Application Framework

  \htmlonly
<TABLE BORDER=0 WIDTH=80%><TR><TD><I><FONT FACE="Arial">
<A HREF="guidedebug.html"><IMG SRC="backarrow.gif" BORDER=0 ALIGN=MIDDLE>
Debugging</A></I></FONT></TD><TD ALIGN=RIGHT><FONT FACE="Arial"><I>
<A HREF="guidevar.html">
Vertex Arrays <IMG SRC="forwardarrow.gif" BORDER=0 ALIGN=MIDDLE></A></I></FONT></TD></TR></TABLE>
\endhtmlonly

    This section isn't complete yet.  Follow the links below for more information.

 @section applets Applets
 Optional!  Easiest way to get a program up and running.  Expert users may want
 to forgo the framework and create a G3D::RenderDevice and other components manually.

 G3D::GApp, G3D::GApplet,
 G3D::GAppSettings, G3D::GWindowSettings

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