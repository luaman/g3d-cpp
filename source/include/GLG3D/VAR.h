/**
  @file GLG3D/VAR.h

  @maintainer Morgan McGuire, morgan@graphics3d.com
  @created 2001-05-29
  @edited  2003-04-08
*/

#ifndef GLG3D_VAR_H
#define GLG3D_VAR_H

#include "GLG3D/RenderDevice.h"
#include "GLG3D/glFormat.h"

namespace G3D {

/**
 A pointer to an array of vertices, colors, or normals in video memory.
 It is safe to copy these.  There is no destructor; it doesn't matter
 when you throw these objects away.  
 */
class VAR {
private:

	class VARArea*		area;

	/** Pointer to the block of uploaded memory */
	void*				pointer;

	/** Size of one element */
	size_t				elementSize;

	/** Pointer to the block of uploaded memory */
	int					numElements;

	uint64				generation;

    GLenum              underlyingRepresentation;

	bool ok() const;

	template<class T>
	void init(const T* sourcePtr, int _numElements, VARArea* _area) {

		numElements = _numElements;
		area		= _area;
        underlyingRepresentation = glFormatOf(T);

		debugAssert(area);
		debugAssert(area->basePointer);

		elementSize = sizeof(T);

        debugAssertM(
            (elementSize % sizeOfGLFormat(underlyingRepresentation)) == 0,
            "Sanity check failed on OpenGL data format; you may"
            " be using an unsupported type in a vertex array.");

		pointer = (uint8*)area->basePointer + area->allocated;

		// Ensure that the next memory address is 8-byte aligned
		pointer = ((uint8*)pointer +      
   			       ((8 - (size_t)pointer % 8) % 8));

		generation = area->generation;
		
		size_t size = elementSize * numElements;
		debugAssert(size + area->allocated <= area->size);
		area->allocated = (size_t)pointer + size - (size_t)area->basePointer;

		// Upload the data
		memcpy(pointer, sourcePtr, size);
	}


	friend class RenderDevice::VARSystem;

	// The following are called by the VARSystem.
	void vertexPointer() const;


	void normalPointer() const;


	void colorPointer() const;


	void texCoordPointer(uint unit) const;

public:

	VAR();

	/**
	 Uploads the memory.  The element type is inferred from the
	 pointer type by the preprocessor.  Sample usage:

     
	  <PRE>
	    VARArea* dynamicArea = renderDevice->createVARArea(1024 * 1024);
	    Array<Vector3> vertex;
		//...
		VAR varray(vertex, dynamicArea);
		
	  </PRE>
		See GLG3D_Demo for examples.
    */
	template<class T>
	VAR(const T* sourcePtr, int _numElements, VARArea* _area) {
		init(sourcePtr, _numElements, _area);
	}		


	template<class T>
	VAR(const Array<T>& source, VARArea* _area) {
		init(source.getCArray(), source.size(), _area);
	}		
};

}

#endif
