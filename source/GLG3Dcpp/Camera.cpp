/**
  @file Camera.cpp

  @author Morgan McGuire, matrix@graphics3d.com
 
  @created 2001-04-15
  @edited  2003-05-27
*/

#include "GLG3D/Camera.h"
#include "GLG3D/RenderDevice.h"

namespace G3D {


Camera::Camera(RenderDevice* r) {
    debugAssert(r != NULL);
	renderDevice = r;
    nearPlane   = 0.5;
    farPlane    = 300;
	setFieldOfView(toRadians(55));
}


Camera::~Camera() {
}


CoordinateFrame Camera::getCoordinateFrame() const {
	return cframe;
}


void Camera::getCoordinateFrame(CoordinateFrame& c) const {
	c = cframe;
}


void Camera::setCoordinateFrame(const CoordinateFrame& c) {
	cframe = c;
}


void Camera::setProjectionAndCameraMatrix() const {
    
    int screenWidth  = renderDevice->getWidth();
    int screenHeight = renderDevice->getHeight();

    // OpenGL wants the renderDevice ratio (not the viewport or
    // screen ratio) here.
    double pixelAspect = renderDevice->getWidth() / (double)renderDevice->getHeight();

    double y = nearPlane * tan(fieldOfView / 2);
    double x = y * pixelAspect;

    double r, l, t, b, n, f;
    n = nearPlane;
    f = farPlane;
    r = x;
    l = -x;
    t = y;
    b = -y;

    renderDevice->setProjectionMatrix3D(l, r, b, t, n, f);
	renderDevice->setCameraToWorldMatrix(cframe);
}


void Camera::setFieldOfView(double angle) {
	debugAssert((angle < PI) && (angle > 0));

	fieldOfView = angle;

	// Solve for the corresponding image plane depth, as if the extent
	// of the film was 1x1.
	imagePlaneDepth = 1 / (2 * tan(angle / 2.0));
}
 

void Camera::setImagePlaneDepth(
    double                                  depth,
    double                                  width,
    double                                  height) {
	
    debugAssert(depth > 0);
	setFieldOfView(2 * atan(width / (2 * depth)));
}


double Camera::getImagePlaneDepth(
    double                                  width,
    double                                  height) const {

    // The image plane depth has been pre-computed for 
    // a 1x1 image.  Now that the image is width x height, 
    // we need to scale appropriately. 

    return imagePlaneDepth * height;
}


double Camera::getViewportWidth() const {
    return nearPlane / imagePlaneDepth;
}


double Camera::getViewportHeight() const {
    int screenWidth  = renderDevice->getWidth();
    int screenHeight = renderDevice->getHeight();

    return getViewportWidth() * 
        ((double)screenHeight / (double)screenWidth);
}


Ray Camera::worldRay(
    double                                  x,
    double                                  y) const {

    int screenWidth  = renderDevice->getWidth();
    int screenHeight = renderDevice->getHeight();

    Ray out;
    // Set the origin to 0
    out.origin = Vector3::ZERO3;

    double cx = screenWidth  / 2.0;
    double cy = screenHeight / 2.0;

    // TODO: figure out why we have to use the renderDevice height here, instead of the
    // VIEWPORT height
    out.direction =
        Vector3( (x - cx) * -CoordinateFrame::zLookDirection,
                -(y - cy),
                 getImagePlaneDepth(renderDevice->getWidth(), renderDevice->getHeight()) * CoordinateFrame::zLookDirection);

    out = cframe.toWorldSpace(out);

    // Normalize the direction (we didn't do it before)
    out.direction = out.direction.direction();

    return out;
}


Vector3 Camera::project(
    const Vector3&                      point) const {

    int screenWidth  = renderDevice->getWidth();
    int screenHeight = renderDevice->getHeight();

    Vector3 out = cframe.pointToObjectSpace(point);
    double w = out.z * CoordinateFrame::zLookDirection;

    if (w <= 0) {
        return Vector3::INF3;
    }
    debugAssert(w > 0);

    // Find where it hits an image plane of these dimensions
    double zImagePlane = getImagePlaneDepth((double)screenWidth, (double)screenHeight);

    // Recover the distance
    double rhw = zImagePlane / w;

    // Add the image center, flip the y axis
    out.x = screenWidth / 2.0 - (rhw * out.x * CoordinateFrame::zLookDirection);
    out.y = screenHeight / 2.0 - (rhw * out.y);
    out.z = rhw;

    return out;
}


double Camera::getZValue(
    double              x,
    double              y,
    int                 width,
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



void Camera::getClipPlanes(
    Plane*              clip) const {

    int screenWidth  = renderDevice->getWidth();
    int screenHeight = renderDevice->getHeight();

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
	clip[5] = Plane(Vector3(0,0,1), Vector3(0,0,-farPlane));

	// Now transform the planes to world space
	for (int p = 0; p < 6; ++p) {
		// Since there is no scale factor, we don't have to 
		// worry about the inverse transpose of the normal.
        Vector3 normal;
        Real d;

        clip[p].getEquation(normal, d);
		
		Vector3 newNormal = cframe.rotation * normal;
	    
		d = (newNormal * -d + cframe.translation).dot(newNormal);
		clip[p] = Plane(newNormal, newNormal * d);
	}
}



void Camera::get3DViewportCorners(
    Vector3& outUR,
    Vector3& outUL,
    Vector3& outLL,
    Vector3& outLR) const {

    const double sign            = CoordinateFrame::zLookDirection;
    const double w               = -sign * getViewportWidth() / 2;
    const double h               = getViewportHeight() / 2;
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

}
