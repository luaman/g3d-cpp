/**
  @file GCamera.cpp

  @author Morgan McGuire, matrix@graphics3d.com
 
  @created 2001-04-15
  @edited  2003-11-13
*/

#include "G3D/GCamera.h"
#include "G3D/Rect2D.h"

namespace G3D {


GCamera::GCamera() {
    nearPlane   = 0.1;
    farPlane    = 300;
	setFieldOfView(toRadians(55));
}


GCamera::~GCamera() {
}


CoordinateFrame GCamera::getCoordinateFrame() const {
	return cframe;
}


void GCamera::getCoordinateFrame(CoordinateFrame& c) const {
	c = cframe;
}


void GCamera::setCoordinateFrame(const CoordinateFrame& c) {
	cframe = c;
}


void GCamera::setFieldOfView(double angle) {
	debugAssert((angle < G3D_PI) && (angle > 0));

	fieldOfView = angle;

	// Solve for the corresponding image plane depth, as if the extent
	// of the film was 1x1.
	imagePlaneDepth = 1 / (2 * tan(angle / 2.0));
}
 

void GCamera::setImagePlaneDepth(
    double                                  depth,
    const class Rect2D&                     viewport) {
	
    debugAssert(depth > 0);
	setFieldOfView(2 * atan(viewport.width() / (2 * depth)));
}


double GCamera::getImagePlaneDepth(
    const class Rect2D&                     viewport) const {

    // The image plane depth has been pre-computed for 
    // a 1x1 image.  Now that the image is width x height, 
    // we need to scale appropriately. 

    return imagePlaneDepth * viewport.height();
}


double GCamera::getViewportWidth(const Rect2D& viewport) const {
    return nearPlane / imagePlaneDepth;
}


double GCamera::getViewportHeight(const Rect2D& viewport) const {
    return getViewportWidth(viewport) * viewport.height() / viewport.width();
}


Ray GCamera::worldRay(
    double                                  x,
    double                                  y,
    const Rect2D&                           viewport) const {

    int screenWidth  = viewport.width();
    int screenHeight = viewport.height();

    Ray out;
    // Set the origin to 0
    out.origin = Vector3::ZERO3;

    double cx = screenWidth  / 2.0;
    double cy = screenHeight / 2.0;

    out.direction =
        Vector3( (x - cx) * -CoordinateFrame::zLookDirection,
                -(y - cy),
                 getImagePlaneDepth(viewport) * CoordinateFrame::zLookDirection);

    out = cframe.toWorldSpace(out);

    // Normalize the direction (we didn't do it before)
    out.direction = out.direction.direction();

    return out;
}


Vector3 GCamera::project(
    const Vector3&                      point,
    const Rect2D&                       viewport) const {


    int screenWidth  = viewport.width();
    int screenHeight = viewport.height();

    Vector3 out = cframe.pointToObjectSpace(point);
    double w = out.z * CoordinateFrame::zLookDirection;

    if (w <= 0) {
        return Vector3::INF3;
    }
    debugAssert(w > 0);

    // Find where it hits an image plane of these dimensions
    double zImagePlane = getImagePlaneDepth(viewport);

    // Recover the distance
    double rhw = zImagePlane / w;

    // Add the image center, flip the y axis
    out.x = screenWidth / 2.0 - (rhw * out.x * CoordinateFrame::zLookDirection);
    out.y = screenHeight / 2.0 - (rhw * out.y);
    out.z = rhw;

    return out;
}


double GCamera::worldToScreenSpaceArea(double area, double z, const Rect2D& viewport) const {

    if (z >= 0) {
        return inf;
    }

    double zImagePlane = getImagePlaneDepth(viewport);

    return area * square(zImagePlane / z);
}


/*
double GCamera::getZValue(
    double              x,
    double              y,
    const class Rect2D&                     viewport    int                 width,
    int                 height,
    double              lineOffset) const {

    double depth = renderDevice->getDepthBufferValue((int)x, (int)(height - y));

    double n = -nearPlane;
    double f = -farPlane;

    // Undo the hyperbolic scaling.
    // Derivation:
    //                  a = ((1/out) - (1/n)) / ((1/f) - (1/n))
    //              depth = (1-a) * lineOffset) + (a * 1)
    //
    //              depth = lineOffset + a * (-lineOffset + 1)
    //              depth = lineOffset + (((1/z) - (1/n)) / ((1/f) - (1/n))) * (1 - lineOffset)
    // depth - lineOffset = (((1/z) - (1/n)) / ((1/f) - (1/n))) * (1 - lineOffset)
    //
    //(depth - lineOffset) / (1 - lineOffset) = (((1/z) - (1/n)) / ((1/f) - (1/n)))
    //((1/f) - (1/n)) * (depth - lineOffset) / (1 - lineOffset) = ((1/z) - (1/n))  
    //(((1/f) - (1/n)) * (depth - lineOffset) / (1 - lineOffset)) + 1/n = (1/z) 
    //
    // z = 1/( (((1/f) - (1/n)) * (depth - lineOffset) / (1 - lineOffset)) + 1/n)

    if (f >= inf) {
        // Infinite far plane
        return  1 / (((-1/n) * (depth - lineOffset) / (1 - lineOffset)) + 1/n);
    } else {
        return  1 / ((((1/f) - (1/n)) * (depth - lineOffset) / (1 - lineOffset)) + 1/n);
    }
}
*/


void GCamera::getClipPlanes(
    const Rect2D&       viewport,
    Plane*              clip) const {

    double screenWidth  = viewport.width();
    double screenHeight = viewport.height();

	// First construct the planes.  Do this in the order of near, left,
    // right, bottom, top, far so that early out clipping tests are likely
    // to end quickly.

	double fovx = screenWidth * fieldOfView / screenHeight;

	// Near (recall that nearPlane, farPlane are positive numbers, so
	// we need to negate them to produce actual z values.)
	clip[0] = Plane(Vector3(0,0,-1), Vector3(0,0,-nearPlane));

    // Right
    clip[1] = Plane(Vector3(-cos(fovx/2), 0, -sin(fovx/2)), Vector3::ZERO);

	// Left
	clip[2] = Plane(Vector3(-clip[1].normal().x, 0, clip[1].normal().z), Vector3::ZERO);

    // Top
    clip[4] = Plane(Vector3(0, -cos(fieldOfView/2), -sin(fieldOfView/2)), Vector3::ZERO);

	// Bottom
	clip[3] = Plane(Vector3(0, -clip[4].normal().y, clip[4].normal().z), Vector3::ZERO);

    // Far
	clip[5] = Plane(Vector3(0, 0, 1), Vector3(0, 0, -farPlane));

	// Now transform the planes to world space
	for (int p = 0; p < 6; ++p) {
		// Since there is no scale factor, we don't have to 
		// worry about the inverse transpose of the normal.
        Vector3 normal;
        float d;

        clip[p].getEquation(normal, d);
		
		Vector3 newNormal = cframe.rotation * normal;
	    
        if (isFinite(d)) {
    		d = (newNormal * -d + cframe.translation).dot(newNormal);
    		clip[p] = Plane(newNormal, newNormal * d);
        } else {
            // When d is infinite, we can't multiply 0's by it without
            // generating NaNs.
            clip[p] = Plane::fromEquation(newNormal.x, newNormal.y, newNormal.z, d);
        }
	}
}


void GCamera::get3DViewportCorners(
    const Rect2D& viewport,
    Vector3& outUR,
    Vector3& outUL,
    Vector3& outLL,
    Vector3& outLR) const {

    const double sign            = CoordinateFrame::zLookDirection;
    const double w               = -sign * getViewportWidth(viewport) / 2;
    const double h               = getViewportHeight(viewport) / 2;
    const double z               = -sign * getNearPlaneZ();

    // Compute the points
    outUR = Vector3( w,  h, z);
    outUL = Vector3(-w,  h, z);
    outLL = Vector3(-w, -h, z);
    outLR = Vector3( w, -h, z);

    // Take to world space
    outUR = cframe.pointToWorldSpace(outUR);
    outUL = cframe.pointToWorldSpace(outUL);
    outLR = cframe.pointToWorldSpace(outLR);
    outLL = cframe.pointToWorldSpace(outLL);
}


void GCamera::setPosition(const Vector3& t) { 
    cframe.translation = t;
}


void GCamera::lookAt(const Vector3& position, const Vector3& up) { 
    cframe.lookAt(position, up);
}

}
