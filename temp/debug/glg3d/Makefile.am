INCLUDES=-I../../../source/include -Wall $(SDL_CFLAGS) -DDEBUG

lib_LTLIBRARIES= libGLG3D_debug.la

libGLG3D_debug_la_SOURCES= ../../../source/GLG3Dcpp/Camera.cpp \
                           ../../../source/GLG3Dcpp/CFont.cpp \
                           ../../../source/GLG3Dcpp/Draw.cpp \
                           ../../../source/GLG3Dcpp/G3DGameUnits.cpp \
                           ../../../source/GLG3Dcpp/GApp.cpp \
                           ../../../source/GLG3Dcpp/GLight.cpp \
                           ../../../source/GLG3Dcpp/GPUProgram.cpp \
                           ../../../source/GLG3Dcpp/LightingParameters.cpp \
                           ../../../source/GLG3Dcpp/MD2Model.cpp \
                           ../../../source/GLG3Dcpp/MD2Model_load.cpp \
                           ../../../source/GLG3Dcpp/ManualCameraController.cpp \
                           ../../../source/GLG3Dcpp/Milestone.cpp \
                           ../../../source/GLG3Dcpp/PixelProgram.cpp \
                           ../../../source/GLG3Dcpp/RenderDevice.cpp \
                           ../../../source/GLG3Dcpp/Sky.cpp \
                           ../../../source/GLG3Dcpp/Texture.cpp \
                           ../../../source/GLG3Dcpp/TextureFormat.cpp \
                           ../../../source/GLG3Dcpp/UserInput.cpp \
                           ../../../source/GLG3Dcpp/VAR.cpp \
                           ../../../source/GLG3Dcpp/VARSystem.cpp \
                           ../../../source/GLG3Dcpp/VertexProgram.cpp \
                           ../../../source/GLG3Dcpp/getOpenGLState.cpp \
                           ../../../source/GLG3Dcpp/glcalls.cpp \
                           ../../../source/GLG3Dcpp/glenumtostring.cpp

libGLG3D_debug_la_LIBADD= $(SDL_LIBS) -lGLU -L../g3d -lG3D_debug
