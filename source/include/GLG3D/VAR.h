/**
  @file GLG3D/VAR.h

  @maintainer Morgan McGuire, morgan@graphics3d.com
  @created 2001-05-29
  @edited  2003-12-06
*/

#ifndef GLG3D_VAR_H
#define GLG3D_VAR_H

#include "GLG3D/RenderDevice.h"
#include "GLG3D/getOpenGLState.h"
#include "GLG3D/glFormat.h"
#include "GLG3D/VARArea.h"

namespace G3D {

/**
 A pointer to an array of vertices, colors, or normals in video memory.
 It is safe to copy these.  There is no destructor; it doesn't matter
 when you throw these objects away.  
 */
class VAR {
private:

    friend class RenderDevice;

	VARAreaRef	        area;

	/** For VBO_MEMORY, this is the offset.  For
        MAIN_MEMORY, this is a pointer to the block
        of uploaded memory */
	void*				_pointer;

	/** Size of one element */
	size_t				elementSize;

	int					numElements;

	uint64				generation;

    GLenum              underlyingRepresentation;

    /**
     The initial size this VAR was allocated with.
     */
    int                 maxSize;

	bool ok() const;

	void init(const void* sourcePtr, int _numElements, VARAreaRef _area,
        GLenum glformat, size_t eltSize);

	void update(const void* sourcePtr, int _numElements,
        GLenum glformat, size_t eltSize);

    /** Performs the actual memory transfer (like memcpy) */
    void uploadToCard(const void* sourcePtr, size_t size);

	// The following are called by RenderDevice
	void vertexPointer() const;

	void normalPointer() const;

	void colorPointer() const;

	void texCoordPointer(uint unit) const;

    void vertexAttribPointer(uint attribNum, bool normalize) const;

public:

	VAR();

	/**
	 Uploads the memory.  The element type is inferred from the
	 pointer type by the preprocessor.  Sample usage:

     
	  <PRE>
	    // Once at the beginning of the program
        VARAreaRef area = VARArea::create(1024 * 1024);

        //----------
        // Store data in main memory
	    Array<Vector3> vertex;
        Array<int>     index;
		
        //... fill out vertex & index arrays

        //------------
        // Upload to graphics card every frame
        area.reset();
		VAR varray(vertex, area);
        renderDevice->beginIndexedPrimitives();
    		renderDevice->setVertexArray(varray);
            renderDevice->sendIndices(RenderDevice::TRIANGLES, index);
        renderDevice->endIndexedPrimitives();
	  </PRE>
		See GLG3D_Demo for examples.
    */
	template<class T>
	VAR(const T* sourcePtr, int _numElements, VARArea* _area) {
		init(sourcePtr, _numElements, _area, glFormatOf(T), sizeof(T));
	}		

	template<class T>
	VAR(const Array<T>& source, VARArea* _area) {
		init(source.getCArray(), source.size(), _area, glFormatOf(T), sizeof(T));
	}

	template<class T>
	void update(const T* sourcePtr, int _numElements) {
		update(sourcePtr, _numElements, glFormatOf(T), sizeof(T));
	}

    template<class T>
	void update(const Array<T>& source) {
		update(source.getCArray(), source.size(), glFormatOf(T), sizeof(T));
	}

    /**
     TODO: remove
     Returns a pointer to the underlying memory.  This is potentially
     in AGP/video memory and may be slow (uncached) to write/read
     from.  Generally, you should use the RenderDevice methods like
     RenderDevice::setVertexPointer instead touching this method-- it is 
     provided only for when you need to perform an OpenGL call
     that is not supported by RenderDevice or to overwrite an existing
     vertex array (you should invoke finish on the internal VARArea
     before doing that).
     */
    void* pointer() {
        return _pointer;
    }
};

}

#endif
