#ifndef GLFORMAT_H
#define GLFORMAT_H

#include "glheaders.h"
#include "G3D/g3dmath.h"

/** A macro that maps G3D types to OpenGL formats (e.g. glFormat(Vector3) == GL_FLOAT).
    Used by the vertex array infrastructure. */
// This implementation is designed to meet the following constraints:
//   1. Work around the many MSVC++ partial template bugs
//   2. Work for primitive types (e.g. int)
#define glFormatOf(T) G3D::_internal::_GLFormat<T>::x()

namespace G3D {
namespace _internal {


template<class T> class _GLFormat {
public:
    static GLenum x() {
        return GL_NONE;
    }
};

/**
 Macro to define partial specializations for types.
 */
#define MAP(G3DType, GLType)                 \
template<> class _GLFormat<G3DType> {        \
public:                                      \
    static GLenum x()  {                     \
        return GLType;                       \
    }                                        \
};

MAP( Vector2,       GL_FLOAT)
MAP( Vector3,       GL_FLOAT)
MAP( Vector4,       GL_FLOAT)
MAP( Vector3uint16, GL_UNSIGNED_SHORT)
MAP( Color3uint8,   GL_UNSIGNED_BYTE)
MAP( Color3,        GL_FLOAT)
MAP( Color4,        GL_FLOAT)
MAP( uint8,         GL_UNSIGNED_BYTE)
MAP( uint16,        GL_UNSIGNED_SHORT)
MAP( uint32,        GL_UNSIGNED_INT)
MAP( int8,          GL_BYTE)
MAP( int16,         GL_SHORT)
MAP( int32,         GL_INT)
MAP( float,         GL_FLOAT)
MAP( double,        GL_DOUBLE)

#undef MAP

}
}

#endif