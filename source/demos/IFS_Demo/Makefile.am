INCLUDES=-I../../include/ $(SDL_CFLAGS)

noinst_PROGRAMS = ifs_demo

ifs_demo_SOURCES = main.cpp IFSModel.cpp IFSReader.cpp initGL.cpp
#ifs_demo_LDFLAGS= @SDL_LDFLAGS@
ifs_demo_LDADD=../../../temp/release/glg3d/libGLG3D.la \
               ../../../temp/release/g3d/libG3D.la

