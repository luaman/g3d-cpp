/**
 @file CoordinateFrame.h

 @maintainer Morgan McGuire, matrix@graphics3d.com
 @cite Portions by Laura Wollstadt
 @cite Portions based on Dave Eberly's Magic Software Library
       at <A HREF="http://www.magic-software.com">http://www.magic-software.com</A>
 
 @created 2001-03-04
 @edited  2003-09-28

 Copyright 2000-2003, Morgan McGuire.
 All rights reserved.
*/

#ifndef G3D_COORDINATEFRAME_H
#define G3D_COORDINATEFRAME_H

#include "G3D/Vector3.h"
#include "G3D/Vector4.h"
#include "G3D/Ray.h"
#include "G3D/Matrix3.h"
#include "G3D/Sphere.h"
#include "G3D/Box.h"
#include "G3D/Triangle.h"
#include <math.h>
#include <string>
#include <stdio.h>
#include <cstdarg>
#include <assert.h>


namespace G3D {

/**
 An RT transformation.
 */
class CoordinateFrame {

public:

    /**
     The direction an object "looks" relative to its own axes.
     */
    static const Real				zLookDirection;

    /**
     Takes object space points to world space.
     */
    Matrix3							rotation;

    /**
     Takes object space points to world space.
     */
    Vector3							translation;

    /**
     Initializes to the identity coordinate frame.
     */
    inline CoordinateFrame() : 
        rotation(Matrix3::IDENTITY), translation(Vector3::ZERO) {
    }

	CoordinateFrame(const Vector3& _translation) :
        rotation(Matrix3::IDENTITY), translation(_translation) {
	}

    CoordinateFrame(const Matrix3 &rotation, const Vector3 &translation) :
        rotation(rotation), translation(translation) {
    }

    CoordinateFrame(class BinaryInput& b);

    void deserialize(class BinaryInput& b);
    void serialize(class BinaryOutput& b) const;

    CoordinateFrame(const CoordinateFrame &other) :
        rotation(other.rotation), translation(other.translation) {}

    /**
      Computes the inverse of this coordinate frame.
     */
    inline CoordinateFrame inverse() const {
        CoordinateFrame out;
        out.rotation = rotation.transpose();
        out.translation = out.rotation * -translation;
        return out;
    }

    virtual ~CoordinateFrame() {}


    /**
     Produces an XML serialization of this coordinate frame.
     */
    std::string toXML() const {
        char buffer[1024];

        int count = sprintf(buffer, "<COORDINATEFRAME>\n  %lf,%lf,%lf,%lf,\n  %lf,%lf,%lf,%lf,\n  %lf,%lf,%lf,%lf,\n  %lf,%lf,%lf,%lf\n</COORDINATEFRAME>\n",
                            rotation[0][0], rotation[0][1], rotation[0][2], translation.x,
                            rotation[1][0], rotation[1][1], rotation[1][2], translation.y,
                            rotation[2][0], rotation[2][1], rotation[2][2], translation.z,
                            0.0, 0.0, 0.0, 1.0);
        assert(count < 1024);

        std::string s = buffer;
        return s;
    }


    /*
     Returns the heading as an angle in radians, where
    north is 0 and west is PI/2
     */
    inline Real getHeading() const {
        Vector3 look = rotation.getColumn(2);
        Real angle = (Real) atan2( -look.z, look.x);
        return angle;
    }

    /*
     Takes the coordinate frame into object space.
     this->inverse() * c
     */
    inline CoordinateFrame toObjectSpace(const CoordinateFrame& c) const {
        return this->inverse() * c;
    }

    inline Vector4 toObjectSpace(const Vector4& v) const {
        return this->inverse().toWorldSpace(v);
    }

    inline Vector4 toWorldSpace(const Vector4& v) const {
        return Vector4(rotation * Vector3(v.x, v.y, v.z) + translation * v.w, v.w);
    }

    /**
     Transforms the point into world space.
     */
    inline Vector3 pointToWorldSpace(const Vector3& v) const {
        return rotation * v + translation;
    }

    /**
     Transforms the point into object space.
     */
    inline Vector3 pointToObjectSpace(const Vector3& v) const {
        return this->inverse().pointToWorldSpace(v);
    }

    /**
     Transforms the vector into world space (no translation).
     */
    inline Vector3 vectorToWorldSpace(const Vector3& v) const {
        return rotation * v;
    }

    Ray toObjectSpace(const Ray& r) const;
    Ray toWorldSpace(const Ray& r) const;

    /**
     Transforms the vector into object space (no translation).
     */
    inline Vector3 vectorToObjectSpace(const Vector3 &v) const {
        return rotation.transpose() * v;
    }

    Box toWorldSpace(const Box& b) const;

    Sphere toWorldSpace(const Sphere& b) const;

    Triangle toWorldSpace(const Triangle& t) const;

    Box toObjectSpace(const Box& b) const;

    Sphere toObjectSpace(const Sphere& b) const;

    Triangle toObjectSpace(const Triangle& t) const;

    CoordinateFrame operator*(const CoordinateFrame &other) const {
        return CoordinateFrame(rotation * other.rotation,
                               pointToWorldSpace(other.translation));
    }

    void lookAt(const Vector3& target);

    void lookAt(
        const Vector3&  target,
        Vector3         up);

	inline Vector3 getLookVector() const {
		return rotation.getColumn(2) * zLookDirection;
	}

    /**
     If a viewer looks along the look vector, this is the viewer's "left"
     */
    inline Vector3 getLeftVector() const {
		return -rotation.getColumn(0);
	}

    inline Vector3 getRightVector() const {
		return rotation.getColumn(0);
	}

    /**
     Uses Quat.lerp to interpolate between two coordinate frames.
     */
    CoordinateFrame lerp(
        const CoordinateFrame&  other,
        double                  alpha) const;

};

} // namespace

#endif
