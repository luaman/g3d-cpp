/**
  @file VARArea.h
  @maintainer Morgan McGuire, morgan@graphics3d.com
  @created 2003-08-09
  @edited  2003-08-09
*/

#ifndef GLG3D_VARAREA_H
#define GLG3D_VARAREA_H

#include "graphics3D.h"
#include "GLG3D/RenderDevice.h"
#include "GLG3D/Milestone.h"

namespace G3D {

/**
 A memory chunk of VAR space (call RenderDevice::createVARArea to 
 allocate, don't ever deallocate manually).

 <P> A large buffer is allocated in video memory when the VAR system
 is initialized.  This buffer can be partitioned into multiple
 VARAreas.  Vertex arrays are uploaded to these areas by creating VAR
 objects.  Once used, those vertex arrays are dropped from memory by
 calling reset() on the corresponding VARArea.

 <P> Typically, two VARAreas are created.  One is a dynamic area that
 is reset every frame, the other is a static area that is never reset.
 */
// Deleting a VARArea does not free the memory associated with it.
class VARArea {
private:
	friend class VAR;
    friend class RenderDevice;
    friend class RenderDevice::VARSystem;

    /**
     The milestone is used for finish().  It is created
     by RenderDevice::setVARAreaMilestones.  If NULL, there
     is no milestone pending.
     */
    MilestoneRef        milestone;

	/** Number of bytes allocated. */
	size_t				allocated;

	/**
	 These prevent vertex arrays that have been freed from
	 accidentally being used.
	 */
	uint64				generation;

	/** The maximum size of this area that was ever used. */
	size_t				peakAllocated;

    RenderDevice*       renderDevice;

	/** Pointer to the memory. */
	void*				basePointer;

	/** Total  number of bytes in this area. */
	size_t				size;
	

	VARArea(
        RenderDevice*   _renderDevice,
        void*           _basePointer,
        size_t          _size);

public:

    ~VARArea();

	size_t totalSize() const;

	size_t freeSize() const;

	size_t allocatedSize() const;

	size_t peakAllocatedSize() const;

    /**
     Blocks the CPU until all rendering calls referencing this area have completed.
     */
    void finish();

	/** Finishes, then frees all VAR memory inside this area.*/ 
	void reset();
};


} // namespace

inline unsigned int hashCode(const G3D::VARArea* v) {
    return (unsigned int)v;
}

#endif
