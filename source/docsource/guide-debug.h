/**
  @page guidedebug Debugging
\htmlonly
<TABLE BORDER=0 WIDTH=80%><TR><TD><I><FONT FACE="Arial">
<A HREF="guidetutorial.html"><IMG SRC="backarrow.gif" BORDER=0 ALIGN=MIDDLE>
Tutorial</A></I></FONT></TD><TD ALIGN=RIGHT><FONT FACE="Arial"><I>
<A HREF="guideapp.html">
Application Framework <IMG SRC="forwardarrow.gif" BORDER=0 ALIGN=MIDDLE></A></I></FONT></TD></TR></TABLE>
\endhtmlonly

    This section isn't complete yet.  Follow the links below for more information.

    G3D provides many services to help you find bugs in your own code.


 	(show debug dialog)
	Debug and release builds are different!
	
    Use the debug binaries with your debug build (and build them yourself if you want to step into the G3D source code.)
    The debug version of the library is loaded with assertions to check bounds, argument validity, and trigger errors
    instead of exceptions in certain cases.  You should always begin debugging by switching to
    the debug target of your project-- many errors in your code will be caught in debug mode by 
    %G3D.

	G3D::debugAssert, G3D::debugAssertM, G3D::alwaysAssertM, G3D::debugAssertGLOk
	G3D::getOpenGLState
	G3D::Log
	Debugging shaders

  G3D::Draw for quickly rendering bounding volumes and axes
  Debug mode on G3D::GApp

    */
