/**
 @file VAR.cpp
 
 @maintainer Morgan McGuire, morgan@graphics3d.com
 
 @created 2003-04-08
 @edited  2003-04-09
 */

#include "GLG3D/VAR.h"
#include "GLG3D/RenderDevice.h"
#include "GLG3D/VARArea.h"
#include "G3D/Log.h"
#include "GLG3D/getOpenGLState.h"

namespace G3D {

VAR::VAR() : _pointer(NULL), numElements(0), generation(0), elementSize(0), underlyingRepresentation(GL_FLOAT) {
}


bool VAR::ok() const {
	return _pointer && (generation == area->generation);
}


// The following are called by the VARSystem.
void VAR::vertexPointer() const {
	debugAssert(ok());
	glEnableClientState(GL_VERTEX_ARRAY);
    debugAssertM(underlyingRepresentation != GL_UNSIGNED_INT, 
              "OpenGL does not support GL_UNSIGNED_INT as a vertex format.");
    debugAssertM(underlyingRepresentation != GL_UNSIGNED_SHORT, 
              "OpenGL does not support GL_UNSIGNED_SHORT as a vertex format.");
    debugAssertM(underlyingRepresentation != GL_UNSIGNED_BYTE, 
              "OpenGL does not support GL_UNSIGNED_BYTE as a vertex format.");
	glVertexPointer(elementSize / sizeOfGLFormat(underlyingRepresentation), 
                    underlyingRepresentation, elementSize, _pointer);
}


void VAR::normalPointer() const {
	debugAssert(ok());
	debugAssert((double)elementSize / sizeOfGLFormat(underlyingRepresentation) == 3.0);
    debugAssertM(underlyingRepresentation != GL_UNSIGNED_INT, 
              "OpenGL does not support GL_UNSIGNED_INT as a normal format.");
    debugAssertM(underlyingRepresentation != GL_UNSIGNED_SHORT, 
              "OpenGL does not support GL_UNSIGNED_SHORT as a normal format.");
    debugAssertM(underlyingRepresentation != GL_UNSIGNED_BYTE,
              "OpenGL does not support GL_UNSIGNED_BYTE as a normal format.");
	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(underlyingRepresentation, elementSize, _pointer); 
}


void VAR::colorPointer() const {
	debugAssert(ok());
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(elementSize / sizeOfGLFormat(underlyingRepresentation),
                   underlyingRepresentation, elementSize, _pointer); 
}


void VAR::texCoordPointer(uint unit) const {
	debugAssert(ok());
	glClientActiveTextureARB(GL_TEXTURE0_ARB + unit);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(elementSize / sizeOfGLFormat(underlyingRepresentation),
                      underlyingRepresentation, elementSize, _pointer);
	glClientActiveTextureARB(GL_TEXTURE0_ARB);
}


void VAR::vertexAttribPointer(uint attribNum, bool normalize) const {
	debugAssert(ok());
	glEnableVertexAttribArrayARB(attribNum);
	glVertexAttribPointerARB(attribNum, elementSize / sizeOfGLFormat(underlyingRepresentation),
                      underlyingRepresentation, normalize, elementSize, _pointer);
}

}
