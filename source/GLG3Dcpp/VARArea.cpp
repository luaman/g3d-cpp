/**
 @file VARArea.cpp
 
 Implementation of the vertex array system used by RenderDevice.

 @maintainer Morgan McGuire, morgan@graphics3d.com
 
 @created 2003-01-08
 @edited  2005-02-06
 */

#include "GLG3D/RenderDevice.h"
#include "G3D/Log.h"
#include "GLG3D/glheaders.h"
#include "GLG3D/getOpenGLState.h"
#include "GLG3D/VAR.h"
#include "GLG3D/VARArea.h"
#include "GLG3D/GLCaps.h"

namespace G3D {

VARArea::Mode VARArea::mode = VARArea::UNINITIALIZED;

size_t VARArea::_sizeOfAllVARAreasInMemory = 0;

VARAreaRef VARArea::create(size_t s, UsageHint h) {
    return new VARArea(s, h);
}


VARArea::VARArea(size_t _size, UsageHint hint) : size(_size) {

    // See if we've determined the mode yet.
    if (mode == UNINITIALIZED) {
        if (GLCaps::supports_GL_ARB_vertex_buffer_object() &&
            (glGenBuffersARB != NULL) && 
            (glBufferDataARB != NULL) &&
            (glDeleteBuffersARB != NULL)) {
            mode = VBO_MEMORY;
        } else {
            mode = MAIN_MEMORY;
        }
    }

    _sizeOfAllVARAreasInMemory += size;

    switch (mode) {
    case VBO_MEMORY:
        {
            glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);
                glGenBuffersARB(1, &glbuffer);
                glBindBufferARB(GL_ARRAY_BUFFER_ARB, glbuffer);

                GLenum usage;

                switch (hint) {
                case WRITE_EVERY_FRAME:
                    usage = GL_STREAM_DRAW_ARB;
                    break;

                case WRITE_ONCE:
                    usage = GL_STATIC_DRAW_ARB;
                    break;

                case WRITE_EVERY_FEW_FRAMES:
                    usage = GL_DYNAMIC_DRAW_ARB;
                    break;

                default:
                    usage = GL_STREAM_DRAW_ARB;
                    debugAssertM(false, "Fell through switch");
                }

                // Load some (undefined) data to initialize the buffer
                glBufferDataARB(GL_ARRAY_BUFFER_ARB, size, NULL, usage);
    debugAssertGLOk();    

                // The basePointer is always NULL for a VBO
                basePointer = NULL;

            glPopClientAttrib();
    debugAssertGLOk();    
        }
        break;

    case MAIN_MEMORY:
        // Use the base pointer
        glbuffer = 0;
        basePointer = malloc(size);
        debugAssert(basePointer);
        break;

    default:
        alwaysAssertM(false, "Fell through switch.");
        glbuffer = 0;
        basePointer = NULL;
    }

    milestone     = NULL;
    allocated     = 0;
	generation    = 1;
	peakAllocated = 0;
}


VARArea::~VARArea() {
    _sizeOfAllVARAreasInMemory -= size;

    switch (mode) {
    case VBO_MEMORY:
        // Delete the vertex buffer
        glDeleteBuffersARB(1, &glbuffer);
        glbuffer = 0;
        break;

    case MAIN_MEMORY:
        // Free the buffer
        free(basePointer);
        basePointer = NULL;
        break;

    default:
        alwaysAssertM(false, "Fell through switch.");
    }
}


void VARArea::finish() {
    if (milestone != (MilestoneRef)NULL) {
        renderDevice->waitForMilestone(milestone);
        milestone = NULL;
    }
}


void VARArea::reset() {
    finish();
	++generation;
	allocated = 0;
}


} // namespace
