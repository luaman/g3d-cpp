/**
  @page guidedebug Debugging

    G3D provides many services to help you find bugs in your own code.

 	(show debug dialog)
	Debug and release builds are different!
	
    Use the debug binaries with your debug build (and build them yourself if you want to step into the G3D source code.)
    The debug version of the library is loaded with assertions to check bounds, argument validity, and trigger errors
    instead of exceptions in certain cases.  You should always begin debugging

	G3D::debugAssert, G3D::debugAssertM, G3D::alwaysAssertM, G3D::debugAssertGLOk
	G3D::getOpenGLState
	G3D::Log
	Debugging shaders

  G3D::Draw for quickly rendering bounding volumes and axes
  Debug mode on G3D::GApp

    */
