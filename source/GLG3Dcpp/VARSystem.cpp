/**
 @file VARSystem.cpp
 
 Implementation of the vertex array system used by RenderDevice.

 @maintainer Morgan McGuire, morgan@graphics3d.com
 
 @created 2003-01-08
 @edited  2003-02-15
 */

#include "GLG3D/RenderDevice.h"
#include "G3D/Log.h"
#include "GLG3D/getOpenGLState.h"

namespace G3D {


RenderDevice::VARSystem::VARSystem(
	RenderDevice*		rd,
	size_t				_size, 
	Log*				_debugLog) {

	// Where the memory was allocated
	char* memLoc = "ERROR";
	debugLog     = _debugLog;
	method		 = VAR_NONE;
	basePointer  = NULL;
	size         = _size;
	allocated    = 0;

	if (debugLog) {debugLog->section("VAR System");}

	if (size > 0) {
    		// See if we can switch to the NVIDIA method
		if (wglAllocateMemoryNV && wglFreeMemoryNV && glVertexArrayRangeNV && rd->supportsOpenGLExtension("GL_NV_vertex_array_range2")) {
			basePointer = wglAllocateMemoryNV(size, 0.0f, 0.0f, 1.0f);
			if (basePointer) {
				glVertexArrayRangeNV(size, basePointer);
				glEnableClientState(GL_VERTEX_ARRAY_RANGE_WITHOUT_FLUSH_NV);
				method = VAR_NVIDIA;
				memLoc = "wglAllocateMemoryNV";
			}
		}


		if (! basePointer) {
			memLoc      = "malloc";
			method		= VAR_MALLOC;
			basePointer = malloc(size);
		}

		if (debugLog) {
			if (basePointer) {
				debugLog->printf("Allocated %d bytes of VAR memory using %s.\n\n", size, memLoc);
			} else {
				debugLog->printf("Unable to allocate VAR memory.\n");
			}
		}
	} else {
		if (debugLog) {
			debugLog->println("VAR system not initialized.");
		}
	}
}



RenderDevice::VARSystem::~VARSystem() {

	switch (method) {
	case VAR_NVIDIA:
		glDisableClientState(GL_VERTEX_ARRAY_RANGE_WITHOUT_FLUSH_NV);
		wglFreeMemoryNV(basePointer);
		break;

	case VAR_MALLOC:
		free(basePointer);
		break;

	}

	areaList.deleteAll();
	basePointer       = NULL;
	size			  = 0;
	allocated         = 0;
}



VARArea* RenderDevice::VARSystem::createArea(size_t areaSize) {
	debugAssertM(basePointer, "Cannot allocate a VARArea before initializing the VAR system.");

	if (allocated + areaSize <= size) {
		
		VARArea* v = new VARArea((uint8*)basePointer + allocated, areaSize);
		allocated += areaSize;
		areaList.append(v);

		return v;

	} else {
		
		return NULL;

	}
}


void RenderDevice::VARSystem::sendIndices(RenderDevice::Primitive primitive, size_t indexSize, int numIndices, const void* index) const {
	GLenum i, p;

	switch (indexSize) {
	case sizeof(uint32):
		i = GL_UNSIGNED_INT;
		break;

	case sizeof(uint16):
		i = GL_UNSIGNED_SHORT;
		break;

	case sizeof(uint8):
		i = GL_UNSIGNED_BYTE;
		break;

	default:
		debugAssertM(false, "Indices must be either 8, 16, or 32-bytes each.");
	}


	switch (primitive) {
    case LINES:
		p = GL_LINES;
        break;

    case LINE_STRIP:
		p = GL_LINE_STRIP;
        break;

    case TRIANGLES:
		p = GL_TRIANGLES;
        break;

    case TRIANGLE_STRIP:
		p = GL_TRIANGLE_STRIP;
		break;

    case TRIANGLE_FAN:
		p = GL_TRIANGLE_FAN;
        break;

    case QUADS:
		p = GL_QUADS;
        break;

    case QUAD_STRIP:
		p = GL_QUAD_STRIP;
        break;
    }

	glDrawElements(p, numIndices, i, index);
}


void RenderDevice::VARSystem::beginIndexedPrimitives() const {
	glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT); 
}


void RenderDevice::VARSystem::setVertexArray(const class VAR& v) const {
	v.vertexPointer();
}


void RenderDevice::VARSystem::setNormalArray(const class VAR& v) const {
	v.normalPointer();
}


void RenderDevice::VARSystem::setColorArray(const class VAR& v) const {
	v.colorPointer();
}


void RenderDevice::VARSystem::setTexCoordArray(unsigned int unit, const class VAR& v) const {
	v.texCoordPointer(unit);
}


void RenderDevice::VARSystem::endIndexedPrimitives() const {
	glPopClientAttrib();
}



/////////////////////////////////////////////////////////////////////////////////////////////////////

VAR::VAR() : pointer(NULL), numElements(0), generation(0), elementSize(0), underlyingRepresentation(GL_FLOAT) {
}


bool VAR::ok() const {
	return pointer && (generation == area->generation);
}


// The following are called by the VARSystem.
void VAR::vertexPointer() const {
	debugAssert(ok());
	glEnableClientState(GL_VERTEX_ARRAY);
    debugAssertM(underlyingRepresentation != GL_UNSIGNED_INT, "OpenGL does not support GL_UNSIGNED_INT as a vertex format.");
    debugAssertM(underlyingRepresentation != GL_UNSIGNED_SHORT, "OpenGL does not support GL_UNSIGNED_SHORT as a vertex format.");
    debugAssertM(underlyingRepresentation != GL_UNSIGNED_BYTE, "OpenGL does not support GL_UNSIGNED_BYTE as a vertex format.");
	glVertexPointer(elementSize / sizeOfGLFormat(underlyingRepresentation), underlyingRepresentation, elementSize, pointer);
}


void VAR::normalPointer() const {
	debugAssert(ok());
	debugAssert((double)elementSize / sizeOfGLFormat(underlyingRepresentation) == 3.0);
    debugAssertM(underlyingRepresentation != GL_UNSIGNED_INT, "OpenGL does not support GL_UNSIGNED_INT as a normal format.");
    debugAssertM(underlyingRepresentation != GL_UNSIGNED_SHORT, "OpenGL does not support GL_UNSIGNED_SHORT as a normal format.");
    debugAssertM(underlyingRepresentation != GL_UNSIGNED_BYTE, "OpenGL does not support GL_UNSIGNED_BYTE as a normal format.");
	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(underlyingRepresentation, elementSize, pointer); 
}


void VAR::colorPointer() const {
	debugAssert(ok());
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(elementSize / sizeOfGLFormat(underlyingRepresentation), underlyingRepresentation, elementSize, pointer); 
}


void VAR::texCoordPointer(uint unit) const {
	debugAssert(ok());
	glClientActiveTextureARB(GL_TEXTURE0_ARB + unit);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(elementSize / sizeOfGLFormat(underlyingRepresentation), underlyingRepresentation, elementSize, pointer);
	glClientActiveTextureARB(GL_TEXTURE0_ARB);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////


VARArea::VARArea(void* _basePointer, size_t _size) :
   	basePointer(_basePointer), size(_size) {
    allocated     = 0;
	generation    = 1;
	peakAllocated = 0;
}


size_t VARArea::totalSize() const {
	return size;
}


size_t VARArea::freeSize() const {
	return size - allocated;
}


size_t VARArea::allocatedSize() const {
	return allocated;
}


size_t VARArea::peakAllocatedSize() const {
	return peakAllocated;
}


void VARArea::reset() {
	peakAllocated = iMax(peakAllocated, allocated);
	++generation;
	allocated = 0;
}


} // namespace
