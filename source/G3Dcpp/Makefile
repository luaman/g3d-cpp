# g3dcpp makefile
#
# Targets:
#  make debug  (default)	
#  make fast
#  make clean
#  make tidy
#
# @maintainer Jordan Parker, jcparker@cs.brown.edu
#
# @created 2002-03-01
# @edited  2002-03-18

# enter source files w/o extension:
OBJECTS = BinaryInput BinaryOutput TextInput System Box CImage CollisionDetection \
	  Color3 Color4 Cone ConvexPolyhedron CoordinateFrame debugAssert fileutils \
	  format g3dmath Line Log Matrix3 PhysicsFrame prompt Quat Ray Sphere \
	  stringutils Vector2 Vector3 Vector4 g3derror Capsule LineSegment \
	  NetworkDevice Plane

SDLPATH = /map/gfx0/common/games/libraries/SDL-1.2.5
# -------------------------------------------------------------------------------------

CCC	= g++-3.0

FASTFLAGS   = -O3 -fpic -D__cdecl= -DNDEBUG
DEBUGFLAGS  = -g -fpic -D__cdecl= -D_DEBUG

CFLAGS     = $(DEBUGFLAGS)
IFLAGS     = -I$(SDLPATH)/include
LFLAGS	   =
SHAREDFLAG = -shared

FASTBT	   = release
DEBUGBT    = debug
BUILDTYPE  = $(DEBUGBT)

LIBNAME    = libG3D

ifeq ($(BUILDTYPE), $(DEBUGBT)) 
	TARGET	= $(LIBNAME)-debug.so
else
	TARGET	= $(LIBNAME).so
endif

TEMPPATH   = ../../temp
ODUMPPATH  = $(TEMPPATH)/$(BUILDTYPE)/g3d/
OFILES	   = $(OBJECTS:%=$(ODUMPPATH)%.o)
LIBPATH    = ../../build/lib

.SUFFIXES:
.SUFFIXES: .cpp .h .inl

.PHONY: debug fast tidy clean

debug:	$(OFILES)
	$(CCC) $(SHAREDFLAG) -o $(LIBPATH)/$(TARGET) $(OFILES) $(LFLAGS)
	chmod 775 $(LIBPATH)/$(TARGET)

fast:	
	$(MAKE) $(MFLAGS) CFLAGS="$(FASTFLAGS)" BUILDTYPE="$(FASTBT)"

$(ODUMPPATH)%.o:	%.cpp
	$(CCC) $(CFLAGS) $(IFLAGS) -c $< -o $@ 

tidy:
	$(RM) $(TEMPPATH)/$(FASTBT)/g3d/*.o
	$(RM) $(TEMPPATH)/$(DEBUGBT)/g3d/*.o
	$(RM) *.out *.mon core *~ 

clean:	tidy
	$(RM) $(LIBPATH)/$(LIBNAME)*.so
