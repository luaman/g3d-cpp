# generic g3d demo makefile
#
# @maintainer Jordan Parker, jcparker@cs.brown.edu
#
# @created 2002-03-01
# @edited  2002-03-20 (morgan@cs.brown.edu)


# enter source files w/o extension:
OBJECTS = main

TARGET	= glg3d_demo

G3DLIBPATH = /map/gfx0/common/games/users/jcparker/g3d-5.00-b2-linux/build/lib
SDLLIBPATH = /map/gfx0/common/games/users/ekern/SDL-1.2.5/lib

G3DINCPATH = /map/gfx0/common/games/users/jcparker/g3d-5.00-b2-linux/source/include
SDLINCPATH = /map/gfx0/common/games/users/ekern/SDL-1.2.5/include/SDL
# -----------------------------------------------------

CCC	= g++-3.0

FASTFLAGS   = -O3 -D__cdecl= -DNDEBUG -DUNIX -D_REENTRANT
DEBUGFLAGS  = -g -D__cdecl= -D_DEBUG -DUNIX -D_REENTRANT

CFLAGS	    = $(DEBUGFLAGS)

FASTBT     = Release
DEBUGBT    = Debug
BUILDTYPE  = $(DEBUGBT)

IFLAGS	   = -I$(SDLINCPATH) -I$(G3DINCPATH)

LFLAGS	   = -L$(SDLLIBPATH) -L$(G3DLIBPATH)
RFLAGS	   = -Xlinker -R$(SDLLIBPATH) -Xlinker -R$(G3DLIBPATH)

LIBS	   = -lSDL -lz -lglut -lGL -ljpeg -lpthread

DUMPDIR	   = $(BUILDTYPE)

ifeq ($(BUILDTYPE), $(DEBUGBT)) 
	LIBS += -lG3D-debug -lGLG3D-debug
else
	LIBS += -lG3D -lGLG3D
endif

OFILES	   = $(OBJECTS:%=$(DUMPDIR)/%.o)

.SUFFIXES:
.SUFFIXES: .cpp .h .inl

.PHONY: debug fast tidy clean

debug:	create_dir $(OFILES)
	$(CCC) -o $(DUMPDIR)/$(TARGET) $(OFILES) $(LFLAGS) $(RFLAGS) $(LIBS)
	chmod 755 $(DUMPDIR)/$(TARGET)

fast:	
	$(MAKE) $(MFLAGS) CFLAGS="$(FASTFLAGS)" BUILDTYPE="$(FASTBT)"

$(DUMPDIR)/%.o:	%.cpp
	$(CCC) $(CFLAGS) $(IFLAGS) -c $< -o $@

tidy:
	$(RM) $(FASTBT)/*.o
	$(RM) $(DEBUGBT)/*.o
	$(RM) *.out *.mon core *~

create_dir:	
	-mkdir $(BUILDTYPE)

clean:	tidy
	$(RM) $(FASTBT)/$(TARGET)
	$(RM) $(DEBUGBT)/$(TARGET)
