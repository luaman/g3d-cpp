#Note: -D__cdecl= is temporary until I find a better place to define it
INCLUDES=-I../include -D__cdecl= -Wall @SDL_INCDIR@

lib_LTLIBRARIES=libGLG3D.la

libGLG3D_la_SOURCES= Camera.cpp \
					 Font.cpp \
					 G3DGameUnits.cpp \
					 LightingParameters.cpp \
					 ManualCameraController.cpp \
					 RenderDevice.cpp \
					 Sky.cpp \
					 Texture.cpp \
					 UserInput.cpp \
					 VARSystem.cpp \
					 getOpenGLState.cpp \
					 glcalls.cpp \
					 glenumtostring.cpp 

libGLG3D_la_LDFLAGS= @SDL_LDFLAGS@
