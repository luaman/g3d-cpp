/**
  @file Rect2D.h
 
  @maintainer Morgan McGuire, matrix@graphics3d.com
 
  @created 2003-11-13
  @created 2004-02-23

  Copyright 2000-2004, Morgan McGuire.
  All rights reserved.
 */

#ifndef G3D_RECT2D_H
#define G3D_RECT2D_H

#include "G3D/Vector2.h"

namespace G3D {

/**
 If you are using this class for pixel rectangles, keep in mind that the last
 pixel you can draw to is at x0() + width() - 1.
 */
class Rect2D {
private:
    Vector2 min, max;

    /**
     Returns true if the whole polygon is clipped.
     @param p Value of the point
     @param axis Index [0 or 1] of the axis to clip along?
     @param clipGreater Are we clipping greater than or less than the line?
     @param inPoly Polygon being clipped
     @param outPoly The clipped polygon
     */
    template<class T>
    static bool clipSide2D(
        const double p, bool clipGreater, int axis, 
        const Array<T>& inPoly, Array<T>& outPoly) {

        outPoly.clear();
        int i0 = -1;

        Vector2 pt1;
        bool c1 = true;

        double negate = clipGreater ? -1 : 1;

        // Find a point that is not clipped
        for (i0 = 0; (i0 < inPoly.length()) && c1; ++i0) {
            pt1 = inPoly[i0];       
            c1  = (negate * pt1[axis]) < (negate * p);
        }

        // We incremented i0 one time to many
        --i0;

        if (c1) {
            // We could not find an unclipped point
            return true;
        }

        outPoly.append(pt1);

        // for each point in inPoly,
        //     if the point is outside the side and the previous one was also outside, continue
        //     if the point is outside the side and the previous one was inside, cut the line
        //     if the point is inside the side and the previous one was also inside, append the points
        //     if the point is inside the side and the previous one was outside, cut the line    
        for (int i = 1; i <= inPoly.length(); ++i) {
            Vector2 pt2 = inPoly[(i + i0) % inPoly.length()];
            bool    c2  = (negate * pt2[axis]) < (negate * p);

            if (c1 ^ c2) {

                if (!c1 && c2 && (i > 1)) {
                    // Unclipped to clipped trasition and not the first iteration
                    outPoly.append(pt1);
                }

                // only one point is clipped, find where the line crosses the clipping plane


                double alpha;
                if (pt2[axis] == pt1[axis]) {
                    alpha = 0;
                } else {
                    alpha = (p - pt1[axis]) / (pt2[axis] - pt1[axis]);
                }
                outPoly.append(pt1.lerp(pt2, alpha));
            } else if (! (c1 || c2) && (i != 1)) {
                // neither point is clipped (don't do this the first time 
                // because we appended the first pt before the loop)
                outPoly.append(pt1);
            }
        
            pt1 = pt2;
            c1 = c2;
        }

        return false;
    }

public:

    inline double width() const {
        return max.x - min.x;
    }

    inline double height() const {
        return max.y - min.y;
    }

    inline double x0() const {
        return min.x;
    }

    inline double x1() const {
        return max.x;
    }

    inline double y0() const {
        return min.y;
    }

    inline double y1() const {
        return max.y;
    }

    inline Rect2D() : min(0, 0), max(0, 0) {}

    inline static Rect2D xyxy(double x0, double y0, double x1, double y1) {
        Rect2D r;
        
        r.min.x = G3D::min(x0, x1);
        r.min.y = G3D::min(y0, y1);
        r.max.x = G3D::max(x0, x1);
        r.max.y = G3D::max(y0, y1);

        return r;
    }

    inline static Rect2D xyxy(const Vector2& v0, const Vector2& v1) {
        Rect2D r;

        r.min = v0.min(v1);
        r.max = v0.max(v1);

        return r;
    }

    inline static Rect2D xywh(double x, double y, double w, double h) {
        return xyxy(x, y, x + w, y + h);
    }

    inline bool contains(const Vector2& v) {
        return (v.x >= min.x) && (v.y >= min.y) && (v.x <= max.x) && (v.y <= max.y);
    }

    inline Rect2D operator*(double s) {
        return xyxy(min.x * s, min.y * s, max.x * s, max.y * s);
    }

    inline Rect2D operator/(double s) {
        return xyxy(min * s, max * s);
    }

    inline Rect2D operator+(const Vector2& v) {
        return xyxy(min + v, max + v);
    }

    inline Rect2D operator-(const Vector2& v) {
        return xyxy(min - v, max - v);
    }

    inline bool operator==(const Rect2D& other) {
        return (min == other.min) && (max == other.max);
    }

    inline bool operator!=(const Rect2D& other) {
        return (min != other.min) || (max != other.max);
    }


    /** 
     Clips so that the rightmost point of the outPoly is at rect.x1 (e.g. a 800x600 window produces
     rightmost point 799, not 800).  The results are suitable for pixel rendering if iRounded.
     Templated so that it will work for Vector2,3,4 (the z and w components are interpolated linearly).
     The template parameter must define T.lerp and contain x and y components.

     If the entire polygon is clipped by a single side, the result will be empty.
     The result might also have zero area but not be empty.
     */
    template<class T>
    void clip(const Array<T>& inPoly, Array<T>& outPoly) const {

        const bool greaterThan = true;
        const bool lessThan    = false;
        const int  X = 0;
        const int  Y = 1;

        Array<T> temp;

        bool entirelyClipped =
          clipSide2D(x0(), lessThan,    X,    inPoly,  temp)    ||
          clipSide2D(x1(), greaterThan, X,    temp,    outPoly) ||
          clipSide2D(y0(), lessThan,    Y,    outPoly, temp)    || 
          clipSide2D(y1(), greaterThan, Y,    temp,    outPoly);

        if (entirelyClipped) {
            outPoly.clear();
        }
    }

};

}

#endif
