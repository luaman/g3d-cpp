INCLUDES=-I../../include/ $(SDL_CFLAGS)

noinst_PROGRAMS = collision

collision_SOURCES = main.cpp Model.cpp Object.cpp Scene.cpp buildScene.cpp
collision_LDFLAGS= -lglut
collision_LDADD=../../../temp/release/glg3d/libGLG3D.la \
                ../../../temp/release/g3d/libG3D.la
