INCLUDES=-I../include -Wall $(SDL_CFLAGS)

lib_LTLIBRARIES=libGLG3D.la libGLG3D_debug.la

libGLG3D_la_SOURCES= Camera.cpp \
					 Font.cpp \
					 G3DGameUnits.cpp \
					 LightingParameters.cpp \
					 ManualCameraController.cpp \
					 RenderDevice.cpp \
					 Sky.cpp \
					 TextureFormat.cpp \
					 Texture.cpp \
					 UserInput.cpp \
					 VARSystem.cpp \
					 VAR.cpp \
					 VertexProgram.cpp \
					 PixelProgram.cpp \
					 Milestone.cpp \
					 GPUProgram.cpp \
					 getOpenGLState.cpp \
					 glcalls.cpp \
					 glenumtostring.cpp 

libGLG3D_la_LIBADD= $(SDL_LIBS)

libGLG3D_debug_la_SOURCES= Camera.cpp \
					 Font.cpp \
					 G3DGameUnits.cpp \
					 LightingParameters.cpp \
					 ManualCameraController.cpp \
					 RenderDevice.cpp \
					 Sky.cpp \
  					 TextureFormat.cpp \
    				         Texture.cpp \
					 UserInput.cpp \
					 VARSystem.cpp \
					 VAR.cpp \
					 VertexProgram.cpp \
					 PixelProgram.cpp \
					 GPUProgram.cpp \
					 Milestone.cpp \
				         getOpenGLState.cpp \
					 glcalls.cpp \
					 glenumtostring.cpp 

libGLG3D_debug_la_CPPFLAGS= -D_DEBUG
libGLG3D_debug_la_LIBADD= $(SDL_LIBS)
