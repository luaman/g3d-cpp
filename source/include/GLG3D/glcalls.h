/**
 @file glcalls.h

 Maps OpenGL calls to G3D data types

 @maintainer Morgan McGuire, matrix@graphics3d.com

 @created 2002-08-07
 @edited  2003-07-07

 Copyright 2002, Morgan McGuire.
 All rights reserved.
*/

#ifndef G3D_GLCALLS_H
#define G3D_GLCALLS_H

#include "graphics3D.h"
#include "G3D/platform.h"
#include "GLG3D/glheaders.h"

namespace G3D {

inline void glMultiTexCoord(GLint unit, G3D::Real v) {
	glMultiTexCoord1fARB(unit, v);
}


inline void glMultiTexCoord(GLint unit, const G3D::Vector2& v) {
	glMultiTexCoord2fvARB(unit, (const float*)&v);
}


inline void glMultiTexCoord(GLint unit, const G3D::Vector3& v) {
	glMultiTexCoord3fvARB(unit, (const float*)&v);
}


inline void glMultiTexCoord(GLint unit, const G3D::Vector4& v) {
	glMultiTexCoord4fvARB(unit, (const float*)&v);
}


inline void glVertex(const G3D::Vector2& v) {
	glVertex2fv((const float*)&v);
}


inline void glVertex(const G3D::Vector3& v) {
	glVertex3fv((const float*)&v);
}


inline void glVertex(const G3D::Vector4& v) {
	glVertex4fv((const float*)&v);
}


inline void glColor(const G3D::Color3 &c) {
	glColor3fv((const float*)&c);
}


inline void glColor(const G3D::Color4& c) {
	glColor4fv((const float*)&c);
}


inline void glColor(G3D::Real r, G3D::Real g, G3D::Real b, G3D::Real a) {
	glColor4f(r, g, b, a);
}


inline void glColor(G3D::Real r, G3D::Real g, G3D::Real b) {
	glColor3f(r, g, b);
}


inline void glNormal(const G3D::Vector3 &n) {
	glNormal3fv((const float*)&n);
}


inline void glTexCoord(const G3D::Vector3 &t) {
	glTexCoord3fv((const float *)&t);
}


inline void glTexCoord(const G3D::Vector2 &t) {
	glTexCoord2fv((const float *)&t);
}


inline void glTexCoord(const G3D::Real t) {
	glTexCoord1f((const float)t);
}


/**
 Loads a coordinate frame into the current OpenGL matrix slot.
 */
void glLoadMatrix(const CoordinateFrame& cf);

/**
 Loads the inverse of a coordinate frame into the current OpenGL matrix slot.
 */
void glLoadInvMatrix(const CoordinateFrame& cf);

/**
 Multiplies the current GL matrix slot by the inverse of a matrix.
 */
void glMultInvMatrix(const CoordinateFrame& cf);

/**
 Multiplies the current GL matrix slot by this matrix.
 */
void glMultMatrix(const CoordinateFrame& cf);

/** platform independent version of wglGetProcAddress */
inline void * glGetProcAddress(const char * name){
    #ifdef G3D_WIN32
	    return (void *) wglGetProcAddress(name);
    #else
	    return (void *) glXGetProcAddressARB((const GLubyte*)name);
    #endif
}

/**
 Returns a texture matrix appropriate for reflection vectors
 based on the current modelview matrix.

  Example:
 <PRE>
    renderDevice->setTexture(0, cubemap);

	glEnable(GL_TEXTURE_CUBE_MAP_ARB);
    for (int i = 0; i < 4; ++i) {
    	glTexGeni(GL_S + i, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP_ARB);
	    glEnable(GL_TEXTURE_GEN_S + i);
    }

    renderDevice->setTextureMatrix(0, reflectionMatrix());
    renderDevice->setColor(Color3::WHITE);
	glutSolidSphere(3, 200, 100);

    for (int i = 0; i < 4; ++i) {
	    glDisable(GL_TEXTURE_GEN_S + i);
    }
	glDisable(GL_TEXTURE_CUBE_MAP_ARB);
 */
CoordinateFrame reflectionMatrix();

} // namespace

#endif

