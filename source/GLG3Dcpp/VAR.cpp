/**
 @file VAR.cpp
 
 @maintainer Morgan McGuire, morgan@graphics3d.com
 
 @created 2003-04-08
 @edited  2003-12-07
 */

#include "GLG3D/VAR.h"
#include "GLG3D/RenderDevice.h"
#include "GLG3D/VARArea.h"
#include "G3D/Log.h"
#include "GLG3D/getOpenGLState.h"

namespace G3D {

VAR::VAR() : _pointer(NULL), elementSize(0), numElements(0), generation(0), underlyingRepresentation(GL_FLOAT) {
}


void VAR::init(
    const void*         sourcePtr,
    int                 _numElements,
    VARAreaRef          _area,
    GLenum              glformat,
    size_t              eltSize) {

	alwaysAssertM(! _area.isNull(), "Bad VARArea");

	numElements              = _numElements;
	area		             = _area;
    underlyingRepresentation = glformat;
	elementSize              = eltSize;

    size_t size              = elementSize * numElements;
    maxSize                  = size;

    debugAssertM(
        (elementSize % sizeOfGLFormat(underlyingRepresentation)) == 0,
        "Sanity check failed on OpenGL data format; you may"
        " be using an unsupported type in a vertex array.");

	_pointer = (uint8*)area->basePointer + area->allocated;

	// Ensure that the next memory address is 8-byte aligned
	_pointer = ((uint8*)_pointer +      
   			   ((8 - (size_t)_pointer % 8) % 8));

	generation = area->generation;
	
    size_t newAreaSize = size + ((size_t)_pointer - (size_t)area->basePointer);

	alwaysAssertM(newAreaSize <= area->size,
        "VARArea too small to hold new VAR");

	area->allocated = newAreaSize;

    area->peakAllocated = iMax(area->peakAllocated, area->allocated);

	// Upload the data
    uploadToCard(sourcePtr, size);
}


void VAR::update(
    const void*         sourcePtr,
    int                 _numElements,
    GLenum              glformat,
    size_t              eltSize) {

	size_t size = eltSize * _numElements;

    alwaysAssertM(size <= maxSize,
        "A VAR can only be updated with an array that is smaller "
        "or equal size (in bytes) to the original array.");

    alwaysAssertM(generation == area->generation,
        "The VARArea has been reset since this VAR was created.");

	numElements              = _numElements;
    underlyingRepresentation = glformat;
	elementSize              = eltSize;

    debugAssertM(
        (elementSize % sizeOfGLFormat(underlyingRepresentation)) == 0,
        "Sanity check failed on OpenGL data format; you may"
        " be using an unsupported type in a vertex array.");
	
	// Upload the data
    uploadToCard(sourcePtr, size);
}


void VAR::uploadToCard(const void* sourcePtr, size_t size) {
    switch (area->mode) {
    case VARArea::VBO_MEMORY:
        // Don't destroy any existing bindings; this call can
        // be made at any time and the program might also
        // use VBO on its own.
        glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);
            glBindBufferARB(GL_ARRAY_BUFFER_ARB, area->glbuffer);

            glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, (GLintptrARB)_pointer, size, sourcePtr);
        glPopClientAttrib();
        break;

    case VARArea::MAIN_MEMORY:
        System::memcpy(_pointer, sourcePtr, size);
        break;

    default:
        alwaysAssertM(false, "Fell through switch");
    }
}


bool VAR::ok() const {
    return (generation == area->generation) &&
        // If we're in VBO_MEMORY mode, the pointer can be null
        (VARArea::mode == VARArea::VBO_MEMORY || _pointer);
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
