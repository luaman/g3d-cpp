/**
 @file Draw.cpp
  
 @maintainer Morgan McGuire, matrix@graphics3d.com
 
 @created 2003-10-29
 @edited  2003-10-29
 */

#include "GLG3D/Draw.h"
#include "graphics3D.h"
#include "GLG3D/RenderDevice.h"

namespace G3D {

const int Draw::WIRE_SPHERE_SECTIONS = 26;
const int Draw::SPHERE_SECTIONS = 40;


void Draw::axes(
    RenderDevice*       renderDevice,
    const Color4&       xColor,
    const Color4&       yColor,
    const Color4&       zColor,
    double              scale) {

    axes(CoordinateFrame(), renderDevice, xColor, yColor, zColor, scale);
}


void Draw::arrow(
    const Vector3&      start,
    const Vector3&      direction,
    RenderDevice*       renderDevice,
    const Color4&       color,
    double              scale) {
    ray(Ray::fromOriginAndDirection(start, direction), renderDevice, color, scale);
}


void Draw::axes(
    const CoordinateFrame& cframe,
    RenderDevice*       renderDevice,
    const Color4&       xColor,
    const Color4&       yColor,
    const Color4&       zColor,
    double              scale) {

    Vector3 c = cframe.translation;
    Vector3 x = cframe.rotation.getColumn(0).direction() * 2;
    Vector3 y = cframe.rotation.getColumn(1).direction() * 2;
    Vector3 z = cframe.rotation.getColumn(2).direction() * 2;

    Draw::arrow(c, x, renderDevice, xColor, scale);
    Draw::arrow(c, y, renderDevice, yColor, scale);
    Draw::arrow(c, z, renderDevice, zColor, scale);
  
    // Text label scale
    const double xx = -3;
    const double yy = xx * 1.4;

    Vector3 xc2D = renderDevice->project(c + x * 1.1);
    Vector3 yc2D = renderDevice->project(c + y * 1.1);
    Vector3 zc2D = renderDevice->project(c + z * 1.1);

    // If coordinates are behind the viewer, transform off screen
    Vector2 x2D = (xc2D.z > 0) ? xc2D.xy() : Vector2(-1000, -1000);
    Vector2 y2D = (yc2D.z > 0) ? yc2D.xy() : Vector2(-1000, -1000);
    Vector2 z2D = (zc2D.z > 0) ? zc2D.xy() : Vector2(-1000, -1000);

    double xS = (xc2D.z > 0) ? clamp(10 * xc2D.z, .1, 5) : 0;
    double yS = (yc2D.z > 0) ? clamp(10 * yc2D.z, .1, 5) : 0;
    double zS = (zc2D.z > 0) ? clamp(10 * zc2D.z, .1, 5) : 0;

    renderDevice->push2D();
        renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);
        renderDevice->setLineWidth(2);

        renderDevice->beginPrimitive(RenderDevice::LINES);
            // X
            renderDevice->setColor(xColor);
            renderDevice->sendVertex(Vector2(-xx,  yy) * xS + x2D);
            renderDevice->sendVertex(Vector2( xx, -yy) * xS + x2D);
            renderDevice->sendVertex(Vector2( xx,  yy) * xS + x2D);
            renderDevice->sendVertex(Vector2(-xx, -yy) * xS + x2D);

            // Y
            renderDevice->setColor(yColor);
            renderDevice->sendVertex(Vector2(-xx,  yy) * yS + y2D);
            renderDevice->sendVertex(Vector2(  0,  0) * yS + y2D);
            renderDevice->sendVertex(Vector2(  0,  0) * yS + y2D);
            renderDevice->sendVertex(Vector2(  0, -yy) * yS + y2D);
            renderDevice->sendVertex(Vector2( xx,  yy) * yS + y2D);
            renderDevice->sendVertex(Vector2(  0,  0) * yS + y2D);
        renderDevice->endPrimitive();

        renderDevice->beginPrimitive(RenderDevice::LINE_STRIP);
            // Z
            renderDevice->setColor(zColor);    
            renderDevice->sendVertex(Vector2( xx,  yy) * zS + z2D);
            renderDevice->sendVertex(Vector2(-xx,  yy) * zS + z2D);
            renderDevice->sendVertex(Vector2( xx, -yy) * zS + z2D);
            renderDevice->sendVertex(Vector2(-xx, -yy) * zS + z2D);
        renderDevice->endPrimitive();
    renderDevice->pop2D();
}


void Draw::ray(
    const Ray&          ray,
    RenderDevice*       renderDevice,
    const Color4&       color,
    double              scale) {

    Vector3 tip = ray.origin + ray.direction;
    // Create a coordinate frame at the tip
    Vector3 u = ray.direction.direction();
    Vector3 v;
    if (u.x == 0) {
        v = Vector3::UNIT_X;
    } else {
        v = Vector3::UNIT_Y;
    }
    Vector3 w = u.cross(v).direction();
    v = w.cross(u).direction();
    Vector3 back = tip - u * 0.3 * scale;

    renderDevice->pushState();
        renderDevice->setShadeMode(RenderDevice::SHADE_SMOOTH);
        renderDevice->setColor(color);

        double r = scale * .1;
        // Arrow head
        renderDevice->beginPrimitive(RenderDevice::TRIANGLES);
            renderDevice->setNormal(u);
            for (int a = 0; a < SPHERE_SECTIONS; ++a) {
                double angle0 = a * G3D_PI * 2.0 / SPHERE_SECTIONS;
                double angle1 = (a + 1) * G3D_PI * 2.0 / SPHERE_SECTIONS;
                Vector3 dir0 = cos(angle0) * v + sin(angle0) * w;
                Vector3 dir1 = cos(angle1) * v + sin(angle1) * w;

                renderDevice->setNormal(dir0);
                renderDevice->sendVertex(tip);

                renderDevice->sendVertex(back + dir0 * r);

                renderDevice->setNormal(dir1);
                renderDevice->sendVertex(back + dir1 * r);
            }
        renderDevice->endPrimitive();

        // Back of arrow head
        renderDevice->beginPrimitive(RenderDevice::TRIANGLE_FAN);
            renderDevice->setNormal(-u);
            for (int a = 0; a <= SPHERE_SECTIONS; ++a) {
                double angle = a * G3D_PI * 2.0 / SPHERE_SECTIONS;
                Vector3 dir = sin(angle) * v + cos(angle) * w;
                renderDevice->sendVertex(back + dir * r);
            }
        renderDevice->endPrimitive();

    renderDevice->popState();

    lineSegment(LineSegment::fromTwoPoints(ray.origin, back), renderDevice, color);

}


void Draw::capsule(
    const Capsule&       capsule, 
    RenderDevice*        renderDevice,
    const Color4&        solidColor,
    const Color4&        wireColor) {

    CoordinateFrame cframe(capsule.getPoint1());

    Vector3 Y = (capsule.getPoint2() - capsule.getPoint1()).direction();
    Vector3 X = (abs(Y.dot(Vector3::UNIT_X)) > 0.9) ? Vector3::UNIT_Y : Vector3::UNIT_X;
    Vector3 Z = X.cross(Y).direction();
    X = Y.cross(Z);        
    cframe.rotation.setColumn(0, X);
    cframe.rotation.setColumn(1, Y);
    cframe.rotation.setColumn(2, Z);

    double radius = capsule.getRadius();
    double height = (capsule.getPoint2() - capsule.getPoint1()).length();

    Sphere sphere1(Vector3::ZERO, radius);
    Sphere sphere2(Vector3(0, height, 0), radius);

    Vector3 top(0, height, 0);

    renderDevice->pushState();

        renderDevice->setObjectToWorldMatrix(renderDevice->getObjectToWorldMatrix() * cframe);
        renderDevice->setShadeMode(RenderDevice::SHADE_SMOOTH);

        if (solidColor.a > 0) {
            int numPasses = 1;

            if (solidColor.a < 1) {
                // Multiple rendering passes to get front/back blending correct
                renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);
                numPasses = 2;
                renderDevice->setCullFace(RenderDevice::CULL_FRONT);
                renderDevice->disableDepthWrite();
            }

            renderDevice->setColor(solidColor);
            for (int k = 0; k < numPasses; ++k) {
                sphereSection(sphere1, renderDevice, solidColor, false, true);
                sphereSection(sphere2, renderDevice, solidColor, true, false);

                // Cylinder faces
                renderDevice->beginPrimitive(RenderDevice::QUAD_STRIP);
                    for (int y = 0; y <= SPHERE_SECTIONS; ++y) {
                        const double yaw0 = y * G3D_PI * 2.0 / SPHERE_SECTIONS;
                        Vector3 v0 = Vector3(cos(yaw0), 0, sin(yaw0));

                        renderDevice->setNormal(v0);
                        renderDevice->sendVertex(v0 * radius);
                        renderDevice->sendVertex(v0 * radius + top);
                    }
                renderDevice->endPrimitive();

                renderDevice->setCullFace(RenderDevice::CULL_BACK);
            }

        }

        if (wireColor.a > 0) {
            renderDevice->enableDepthWrite();
            renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);

            wireSphereSection(sphere1, renderDevice, wireColor, false, true);
            wireSphereSection(sphere2, renderDevice, wireColor, true, false);

            // Line around center
            renderDevice->setColor(wireColor);
            Vector3 center(0, height / 2, 0);
            renderDevice->setLineWidth(2);
            renderDevice->beginPrimitive(RenderDevice::LINES);
                for (int y = 0; y < WIRE_SPHERE_SECTIONS; ++y) {
                    const double yaw0 = y * G3D_PI * 2.0 / WIRE_SPHERE_SECTIONS;
                    const double yaw1 = (y + 1) * G3D_PI * 2.0 / WIRE_SPHERE_SECTIONS;

                    Vector3 v0(cos(yaw0), 0, sin(yaw0));
                    Vector3 v1(cos(yaw1), 0, sin(yaw1));

                    renderDevice->setNormal(v0);
                    renderDevice->sendVertex(v0 * radius + center);
                    renderDevice->setNormal(v1);
                    renderDevice->sendVertex(v1 * radius + center);
                }

                // Edge lines
                for (int y = 0; y < 8; ++y) {
                    const double yaw = y * G3D_PI / 4;
                    const Vector3 x(cos(yaw), 0, sin(yaw));
        
                    renderDevice->setNormal(x);
                    renderDevice->sendVertex(x * radius);
                    renderDevice->sendVertex(x * radius + top);
                }
            renderDevice->endPrimitive();
        }

    renderDevice->popState();
}


void Draw::vertexNormals(
    const MeshAlg::Geometry&    geometry,
    RenderDevice*               renderDevice,
    const Color4&               color,
    double                      scale) {

    renderDevice->pushState();
        renderDevice->setColor(color);
        renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);

        const Array<Vector3>& vertexArray = geometry.vertexArray;
        const Array<Vector3>& normalArray = geometry.normalArray;

        const double D = clamp(5.0 / pow(vertexArray.size(), .25), 0.1, .8) * scale;
        
        renderDevice->setLineWidth(1);
        renderDevice->beginPrimitive(RenderDevice::LINES);
            for (int v = 0; v < vertexArray.size(); ++v) {
                renderDevice->sendVertex(vertexArray[v] + normalArray[v] * D);
                renderDevice->sendVertex(vertexArray[v]);
            }
        renderDevice->endPrimitive();
        
        renderDevice->setLineWidth(2);
        renderDevice->beginPrimitive(RenderDevice::LINES);
            for (int v = 0; v < vertexArray.size(); ++v) {
                renderDevice->sendVertex(vertexArray[v] + normalArray[v] * D * .96);
                renderDevice->sendVertex(vertexArray[v] + normalArray[v] * D * .84);
            }
        renderDevice->endPrimitive();

        renderDevice->setLineWidth(3);
        renderDevice->beginPrimitive(RenderDevice::LINES);
            for (int v = 0; v < vertexArray.size(); ++v) {
                renderDevice->sendVertex(vertexArray[v] + normalArray[v] * D * .92);
                renderDevice->sendVertex(vertexArray[v] + normalArray[v] * D * .84);
            }
        renderDevice->endPrimitive();
    renderDevice->popState();
}


void Draw::line(
    const Line&         line,
    RenderDevice*       renderDevice,
    const Color4&       color) {

    renderDevice->pushState();
        renderDevice->setShadeMode(RenderDevice::SHADE_SMOOTH);
        renderDevice->setColor(color);
        renderDevice->setLineWidth(2);
        renderDevice->setDepthTest(RenderDevice::DEPTH_LEQUAL);
        renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);

        Vector3 v0 = line.point();
        Vector3 d  = line.direction();
        renderDevice->beginPrimitive(RenderDevice::LINE_STRIP);
            // Off to infinity
            renderDevice->sendVertex(Vector4(-d, 0));

            for (int i = -10; i <= 10; i += 2) {
                renderDevice->sendVertex(v0 + i * d * 100);
            }

            // Off to infinity
            renderDevice->sendVertex(Vector4(d, 0));
        renderDevice->endPrimitive();
    renderDevice->popState();
}


void Draw::lineSegment(
    const LineSegment&  lineSegment,
    RenderDevice*       renderDevice,
    const Color4&       color) {

    renderDevice->pushState();

        renderDevice->setShadeMode(RenderDevice::SHADE_SMOOTH);
        renderDevice->setColor(color);
        
        // Compute perspective line width
        Vector3 v0 = lineSegment.endPoint(0);
        Vector3 v1 = lineSegment.endPoint(1);

        Vector3 s0 = renderDevice->project(v0);
        Vector3 s1 = renderDevice->project(v1);

        double L = 2;
        if ((s0.z > 0) && (s1.z > 0)) {
            L = 15 * (s0.z + s1.z) / 2;
        } else if (s0.z > 0) {
            L = max(15 * s0.z, 10);
        } else if (s1.z > 0) {
            L = max(15 * s1.z, 10);
        }

        renderDevice->setLineWidth(L);

        // Find the object space vector perpendicular to the line
        // that points closest to the eye.
        Vector3 eye = renderDevice->getObjectToWorldMatrix().pointToObjectSpace(renderDevice->getCameraToWorldMatrix().translation);
        Vector3 E = eye - v0;
        Vector3 V = v1 - v0;
        Vector3 U = E.cross(V);
        Vector3 N = V.cross(U).direction();

        renderDevice->setDepthTest(RenderDevice::DEPTH_LEQUAL);
        renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);

        renderDevice->beginPrimitive(RenderDevice::LINES);        
            renderDevice->setNormal(N);
            renderDevice->sendVertex(v0);
            renderDevice->sendVertex(v1);
        renderDevice->endPrimitive();
    renderDevice->popState();
}



void Draw::box(
    const Box&          box,
    RenderDevice*       renderDevice,
    const Color4&       solidColor,
    const Color4&       wireColor) {

    renderDevice->pushState();
        renderDevice->setShadeMode(RenderDevice::SHADE_SMOOTH);

        if (solidColor.a > 0) {
            int numPasses = 1;

            if (solidColor.a < 1) {
                // Multiple rendering passes to get front/back blending correct
                renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);
                numPasses = 2;
                renderDevice->setCullFace(RenderDevice::CULL_FRONT);
                renderDevice->disableDepthWrite();
            }

            renderDevice->setColor(solidColor);
            for (int k = 0; k < numPasses; ++k) {
                renderDevice->beginPrimitive(RenderDevice::QUADS);
                    for (int i = 0; i < 6; ++i) {
                        Vector3 v0, v1, v2, v3;
                        box.getFaceCorners(i, v0, v1, v2, v3);

                        Vector3 n = (v1 - v0).cross(v3 - v0);
                        renderDevice->setNormal(n.direction());
                        renderDevice->sendVertex(v0);
                        renderDevice->sendVertex(v1);
                        renderDevice->sendVertex(v2);
                        renderDevice->sendVertex(v3);
                    }
                renderDevice->endPrimitive();
                renderDevice->setCullFace(RenderDevice::CULL_BACK);
            }
        }

        if (wireColor.a > 0) {
            renderDevice->enableDepthWrite();
            renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);
            renderDevice->setColor(wireColor);
            renderDevice->setLineWidth(2);

            Vector3 c = box.getCenter();
            Vector3 v;

            renderDevice->setDepthTest(RenderDevice::DEPTH_LEQUAL);
            renderDevice->beginPrimitive(RenderDevice::LINES);

                // Wire frame
                for (int i = 0; i < 8; i += 4) {
                    for (int j = 0; j < 4; ++j) {
                        v = box.getCorner(i + j);
                        renderDevice->setNormal((v - c).direction());
                        renderDevice->sendVertex(v);

                        v = box.getCorner(i + ((j + 1) % 4));
                        renderDevice->setNormal((v - c).direction());
                        renderDevice->sendVertex(v);
                    }
                }

                for (int i = 0; i < 4; ++i) {
                    v = box.getCorner(i);
                    renderDevice->setNormal((v - c).direction());
                    renderDevice->sendVertex(v);

                    v = box.getCorner(i + 4);
                    renderDevice->setNormal((v - c).direction());
                    renderDevice->sendVertex(v);
                }

            renderDevice->endPrimitive();
        }
    renderDevice->popState();
}


void Draw::wireSphereSection(
    const Sphere&       sphere,
    RenderDevice*       renderDevice,
    const Color4&       color,
    bool                top,
    bool                bottom) {
    
    int sections = WIRE_SPHERE_SECTIONS;
    int start = top ? 0 : (sections / 2);
    int stop = bottom ? sections : (sections / 2);

    renderDevice->pushState();
        renderDevice->setShadeMode(RenderDevice::SHADE_SMOOTH);
        renderDevice->setColor(color);
        renderDevice->setLineWidth(2);
        renderDevice->setDepthTest(RenderDevice::DEPTH_LEQUAL);
        renderDevice->setCullFace(RenderDevice::CULL_BACK);
        renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);

        double radius = sphere.radius;
        const Vector3& center = sphere.center;

        // Wire frame
        for (int y = 0; y < 8; ++y) {
            const double yaw = y * G3D_PI / 4;
            const Vector3 x(cos(yaw) * radius, 0, sin(yaw) * radius);
            //const Vector3 z(-sin(yaw) * radius, 0, cos(yaw) * radius);

            renderDevice->beginPrimitive(RenderDevice::LINE_STRIP);
                for (int p = start; p <= stop; ++p) {
                    const double pitch0 = p * G3D_PI / (sections * 0.5);

                    Vector3 v0 = cos(pitch0) * x + Vector3::UNIT_Y * radius * sin(pitch0);
                    renderDevice->setNormal(v0.direction());
                    renderDevice->sendVertex(v0 + center);
                }
            renderDevice->endPrimitive();
        }


        int a = bottom ? -1 : 0;
        int b = top ? 1 : 0; 
        for (int p = a; p <= b; ++p) {
            const double pitch = p * G3D_PI / 6;

            renderDevice->beginPrimitive(RenderDevice::LINE_STRIP);
                for (int y = 0; y <= sections; ++y) {
                    const double yaw0 = y * G3D_PI / 13;
                    Vector3 v0 = Vector3(cos(yaw0) * cos(pitch), sin(pitch), sin(yaw0) * cos(pitch)) * radius;
                    renderDevice->setNormal(v0.direction());
                    renderDevice->sendVertex(v0 + center);
                }
            renderDevice->endPrimitive();
        }

    renderDevice->popState();
}


void Draw::sphereSection(
    const Sphere&       sphere,
    RenderDevice*       renderDevice,
    const Color4&       color,
    bool                top,
    bool                bottom) {

    int sections = SPHERE_SECTIONS;
    int start = top ? 0 : (sections / 2);
    int stop = bottom ? sections : (sections / 2);

    renderDevice->pushState();
        renderDevice->setColor(color);
        renderDevice->setShadeMode(RenderDevice::SHADE_SMOOTH);

        for (int p = start; p < stop; ++p) {
            const double pitch0 = p * G3D_PI / (double)sections;
            const double pitch1 = (p + 1) * G3D_PI / (double)sections;

            renderDevice->beginPrimitive(RenderDevice::QUAD_STRIP);
            for (int y = 0; y <= SPHERE_SECTIONS; ++y) {
                const double yaw = -y * G3D_PI * 2.0 / SPHERE_SECTIONS;

                Vector3 v0 = Vector3(cos(yaw) * sin(pitch0), cos(pitch0), sin(yaw) * sin(pitch0));
                Vector3 v1 = Vector3(cos(yaw) * sin(pitch1), cos(pitch1), sin(yaw) * sin(pitch1));

                renderDevice->setNormal(v0);
                renderDevice->sendVertex(v0 * sphere.radius + sphere.center);

                renderDevice->setNormal(v1);
                renderDevice->sendVertex(v1 * sphere.radius + sphere.center);
            }
            renderDevice->endPrimitive();
        }

    renderDevice->popState();       
}


void Draw::sphere(
    const Sphere&       sphere,
    RenderDevice*       renderDevice,
    const Color4&       solidColor,
    const Color4&       wireColor) {

    if (solidColor.a > 0) {
        renderDevice->pushState();

            int numPasses = 1;

            if (solidColor.a < 1) {
                numPasses = 2;
                renderDevice->setCullFace(RenderDevice::CULL_FRONT);
                renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);
                renderDevice->disableDepthWrite();
            }

            if (wireColor.a > 0) {
                renderDevice->setPolygonOffset(3);
            }

            for (int k = 0; k < numPasses; ++k) {
                sphereSection(sphere, renderDevice, solidColor, true, true);
                renderDevice->setCullFace(RenderDevice::CULL_BACK);
            }
        renderDevice->popState();
    }

    if (wireColor.a > 0) {
        wireSphereSection(sphere, renderDevice, wireColor, true, true);
    }
}

}
