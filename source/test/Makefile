OBJECTS = main
TARGET = test

CCC = g++-3.0
LFLAGS = -lSDL -lz -lglut -lGL \
	-L/map/gfx0/common/games/users/hkhalsa/g3d-5.00-b2-linux/source/G3D/IJG \
	-L/map/gfx0/common/games/users/hkhalsa/g3d-5.00-b2-linux/build/lib \
	-ljpeg -lG3Dd

IFLAGS = -I. \
	-I/usr/include/g++-v3 \
	-I/usr/include/g++-v3/i386-linux \
	-I/map/gfx0/common/games/users/hkhalsa/g3d-5.00-b2-linux/source/include \
	-I/map/gfx0/common/games/users/hkhalsa/g3d-5.00-b2-linux/source/G3D/IJG
	

OFILES = $(OBJECTS:%=%.o)

COMPILEFLAGS = -DUNIX -g -D__cdecl= -D_DEBUG

default:	$(OFILES) linkit

clean:
	rm -f *.o

%.o:	%.cpp
	$(CCC) $(COMPILEFLAGS) $(IFLAGS)  $(CCFLAGS) -c $<

linkit:
	$(CCC) $(COMPILEFLAGS) $(LFLAGS) *.o -o $(TARGET)
