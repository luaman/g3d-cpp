/**
 @file Draw.cpp
  
 @maintainer Morgan McGuire, matrix@graphics3d.com
 
 @created 2003-10-29
 @edited  2005-02-06
 */

#include "GLG3D/Draw.h"
#include "graphics3D.h"
#include "GLG3D/RenderDevice.h"
#include "GLG3D/GLCaps.h"
#include "G3D/Rect2D.h"

namespace G3D {

const int Draw::WIRE_SPHERE_SECTIONS = 26;
const int Draw::SPHERE_SECTIONS = 40;

const int Draw::SPHERE_PITCH_SECTIONS = 20;
const int Draw::SPHERE_YAW_SECTIONS = 40;

void Draw::poly2DOutline(const Array<Vector2>& polygon, RenderDevice* renderDevice, const Color4& color) {
    if (polygon.length() == 0) {
        return;
    }
    
    renderDevice->beginPrimitive(RenderDevice::LINE_STRIP);
        renderDevice->setColor(color);
        for (size_t i = 0; i < polygon.length(); ++i) {
            renderDevice->sendVertex(polygon[i]);
        }
        renderDevice->sendVertex(polygon[0]);
    renderDevice->endPrimitive();
}


void Draw::poly2D(const Array<Vector2>& polygon, RenderDevice* renderDevice, const Color4& color) {
    if (polygon.length() == 0) {
        return;
    }
    
    renderDevice->beginPrimitive(RenderDevice::TRIANGLE_FAN);
        renderDevice->setColor(color);
        for (size_t i = 0; i < polygon.length(); ++i) {
            renderDevice->sendVertex(polygon[i]);
        }
    renderDevice->endPrimitive();
}


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
    Vector3 x = cframe.rotation.getColumn(0).direction() * 2 * scale;
    Vector3 y = cframe.rotation.getColumn(1).direction() * 2 * scale;
    Vector3 z = cframe.rotation.getColumn(2).direction() * 2 * scale;

    Draw::arrow(c, x, renderDevice, xColor, scale);
    Draw::arrow(c, y, renderDevice, yColor, scale);
    Draw::arrow(c, z, renderDevice, zColor, scale);
  
    // Text label scale
    const double xx = -3;
    const double yy = xx * 1.4;

    // Project the 3D locations of the labels
    Vector4 xc2D = renderDevice->project(c + x * 1.1);
    Vector4 yc2D = renderDevice->project(c + y * 1.1);
    Vector4 zc2D = renderDevice->project(c + z * 1.1);

    // If coordinates are behind the viewer, transform off screen
    Vector2 x2D = (xc2D.w > 0) ? xc2D.xy() : Vector2(-2000, -2000);
    Vector2 y2D = (yc2D.w > 0) ? yc2D.xy() : Vector2(-2000, -2000);
    Vector2 z2D = (zc2D.w > 0) ? zc2D.xy() : Vector2(-2000, -2000);

    // Compute the size of the labels
    double xS = (xc2D.w > 0) ? clamp(10 * xc2D.w * scale, .1, 5) : 0;
    double yS = (yc2D.w > 0) ? clamp(10 * yc2D.w * scale, .1, 5) : 0;
    double zS = (zc2D.w > 0) ? clamp(10 * zc2D.w * scale, .1, 5) : 0;

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
            renderDevice->sendVertex(Vector2(  0,  0)  * yS + y2D);
            renderDevice->sendVertex(Vector2(  0,  0)  * yS + y2D);
            renderDevice->sendVertex(Vector2(  0, -yy) * yS + y2D);
            renderDevice->sendVertex(Vector2( xx,  yy) * yS + y2D);
            renderDevice->sendVertex(Vector2(  0,  0)  * yS + y2D);
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
        v = Vector3::unitX();
    } else {
        v = Vector3::unitY();
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

    lineSegment(LineSegment::fromTwoPoints(ray.origin, back), renderDevice, color, scale);

}


void Draw::capsule(
    const Capsule&       capsule, 
    RenderDevice*        renderDevice,
    const Color4&        solidColor,
    const Color4&        wireColor) {

    CoordinateFrame cframe(capsule.getPoint1());

    Vector3 Y = (capsule.getPoint2() - capsule.getPoint1()).direction();
    Vector3 X = (abs(Y.dot(Vector3::unitX())) > 0.9) ? Vector3::unitY() : Vector3::unitX();
    Vector3 Z = X.cross(Y).direction();
    X = Y.cross(Z);        
    cframe.rotation.setColumn(0, X);
    cframe.rotation.setColumn(1, Y);
    cframe.rotation.setColumn(2, Z);

    double radius = capsule.getRadius();
    double height = (capsule.getPoint2() - capsule.getPoint1()).length();

    Sphere sphere1(Vector3::zero(), radius);
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

        const double D = clamp(5.0 / ::pow((double)vertexArray.size(), .25), 0.1, .8) * scale;
        
        renderDevice->setLineWidth(1);
        renderDevice->beginPrimitive(RenderDevice::LINES);
            for (size_t v = 0; v < vertexArray.size(); ++v) {
                renderDevice->sendVertex(vertexArray[v] + normalArray[v] * D);
                renderDevice->sendVertex(vertexArray[v]);
            }
        renderDevice->endPrimitive();
        
        renderDevice->setLineWidth(2);
        renderDevice->beginPrimitive(RenderDevice::LINES);
            for (size_t v = 0; v < vertexArray.size(); ++v) {
                renderDevice->sendVertex(vertexArray[v] + normalArray[v] * D * .96);
                renderDevice->sendVertex(vertexArray[v] + normalArray[v] * D * .84);
            }
        renderDevice->endPrimitive();

        renderDevice->setLineWidth(3);
        renderDevice->beginPrimitive(RenderDevice::LINES);
            for (size_t v = 0; v < vertexArray.size(); ++v) {
                renderDevice->sendVertex(vertexArray[v] + normalArray[v] * D * .92);
                renderDevice->sendVertex(vertexArray[v] + normalArray[v] * D * .84);
            }
        renderDevice->endPrimitive();
    renderDevice->popState();
}


void Draw::vertexVectors(
    const Array<Vector3>&       vertexArray,
    const Array<Vector3>&       directionArray,
    RenderDevice*               renderDevice,
    const Color4&               color,
    double                      scale) {

    renderDevice->pushState();
        renderDevice->setColor(color);
        renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);

        const double D = clamp(5.0 / ::pow((double)vertexArray.size(), .25), 0.1, .8) * scale;
        
        renderDevice->setLineWidth(1);
        renderDevice->beginPrimitive(RenderDevice::LINES);
            for (size_t v = 0; v < vertexArray.size(); ++v) {
                renderDevice->sendVertex(vertexArray[v] + directionArray[v] * D);
                renderDevice->sendVertex(vertexArray[v]);
            }
        renderDevice->endPrimitive();
        
        renderDevice->setLineWidth(2);
        renderDevice->beginPrimitive(RenderDevice::LINES);
            for (size_t v = 0; v < vertexArray.size(); ++v) {
                renderDevice->sendVertex(vertexArray[v] + directionArray[v] * D * .96);
                renderDevice->sendVertex(vertexArray[v] + directionArray[v] * D * .84);
            }
        renderDevice->endPrimitive();

        renderDevice->setLineWidth(3);
        renderDevice->beginPrimitive(RenderDevice::LINES);
            for (size_t v = 0; v < vertexArray.size(); ++v) {
                renderDevice->sendVertex(vertexArray[v] + directionArray[v] * D * .92);
                renderDevice->sendVertex(vertexArray[v] + directionArray[v] * D * .84);
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
                renderDevice->sendVertex(v0 + d * i * 100.0);
            }

            // Off to infinity
            renderDevice->sendVertex(Vector4(d, 0));
        renderDevice->endPrimitive();
    renderDevice->popState();
}


void Draw::lineSegment(
    const LineSegment&  lineSegment,
    RenderDevice*       renderDevice,
    const Color4&       color,
    double              scale) {

    renderDevice->pushState();

        renderDevice->setShadeMode(RenderDevice::SHADE_SMOOTH);
        renderDevice->setColor(color);
        
        // Compute perspective line width
        Vector3 v0 = lineSegment.endPoint(0);
        Vector3 v1 = lineSegment.endPoint(1);

        Vector4 s0 = renderDevice->project(v0);
        Vector4 s1 = renderDevice->project(v1);

        double L = 2 * scale;
        if ((s0.w > 0) && (s1.w > 0)) {
            L = 15 * (s0.w + s1.w) / 2;
        } else if (s0.w > 0) {
            L = max(15 * s0.w, 10);
        } else if (s1.w > 0) {
            L = max(15 * s1.w, 10);
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
    const AABox&        _box,
    RenderDevice*       renderDevice,
    const Color4&       solidColor,
    const Color4&       wireColor) {

    box(_box.toBox(), renderDevice, solidColor, wireColor);
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
            } else {
                renderDevice->setCullFace(RenderDevice::CULL_BACK);
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

                    Vector3 v0 = cos(pitch0) * x + Vector3::unitY() * radius * sin(pitch0);
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

    // The first time this is invoked we create a series of quad strips in a vertex array.
    // Future calls then render from the array. 

    CoordinateFrame cframe = renderDevice->getObjectToWorldMatrix();

    // Auto-normalization will take care of normal length
    cframe.rotation = cframe.rotation * sphere.radius;
    cframe.translation += sphere.center;

    glPushAttrib(GL_ENABLE_BIT);
    glDisable(GL_NORMALIZE);
    glEnable(GL_RESCALE_NORMAL);

    renderDevice->pushState();
        renderDevice->setObjectToWorldMatrix(cframe);

        renderDevice->setColor(color);
        renderDevice->setShadeMode(RenderDevice::SHADE_SMOOTH);

        static bool initialized = false;
        static int numIndices = 0;
        static VAR vbuffer;
        static Array< Array<uint16> > stripIndexArray;

        if (! initialized) {
            // The normals are the same as the vertices for a sphere
            Array<Vector3> vertex;

            int s = 0;
            int i = 0;

            for (int p = 0; p < SPHERE_PITCH_SECTIONS; ++p) {
                const double pitch0 = p * G3D_PI / (double)SPHERE_PITCH_SECTIONS;
                const double pitch1 = (p + 1) * G3D_PI / (double)SPHERE_PITCH_SECTIONS;

                stripIndexArray.next();

                for (int y = 0; y <= SPHERE_YAW_SECTIONS; ++y) {
                    const double yaw = -y * G3D_PI * 2.0 / SPHERE_YAW_SECTIONS;

                    float cy = cos(yaw);
                    float sy = sin(yaw);
                    float sp0 = sin(pitch0);
                    float sp1 = sin(pitch1);

                    Vector3 v0 = Vector3(cy * sp0, cos(pitch0), sy * sp0);
                    Vector3 v1 = Vector3(cy * sp1, cos(pitch1), sy * sp1);

                    vertex.append(v0, v1);
                    stripIndexArray.last().append(i, i + 1);
                    i += 2;
                }
            }

            VARAreaRef area = VARArea::create(vertex.size() * sizeof(Vector3), VARArea::WRITE_ONCE);
            vbuffer = VAR(vertex, area);
            numIndices = vertex.size();
            initialized = true;
        }


        int start = top ? 0 : (SPHERE_PITCH_SECTIONS / 2);
        int stop = bottom ? SPHERE_PITCH_SECTIONS : (SPHERE_PITCH_SECTIONS / 2);

        renderDevice->beginIndexedPrimitives();
            renderDevice->setNormalArray(vbuffer);
            renderDevice->setVertexArray(vbuffer);
            for (int s = start; s < stop; ++s) {
                renderDevice->sendIndices(RenderDevice::QUAD_STRIP, stripIndexArray[s]);
            }
        renderDevice->endIndexedPrimitives();

    renderDevice->popState();

    glPopAttrib();
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
            } else {
                renderDevice->setCullFace(RenderDevice::CULL_BACK);
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


void Draw::fullScreenImage(const GImage& im, RenderDevice* renderDevice) {
    renderDevice->push2D();
        glPixelZoom((float)renderDevice->getWidth() / (float)im.width, 
                   -(float)renderDevice->getHeight() / (float)im.height);
        glRasterPos4d(0.0, 0.0, 0.0, 1.0);
        glDrawPixels(im.width, im.height, (im.channels == 3) ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, (const GLvoid*)im.byte());
    renderDevice->pop2D();
}


void Draw::rect2D(
    const Rect2D& rect,
    RenderDevice* rd,
    const Color4& color,
    const Vector2& texCoord0,
    const Vector2& texCoord1,
    const Vector2& texCoord2,
    const Vector2& texCoord3) {

    Draw::rect2D(rect, rd, color,
        Rect2D::xywh(0,0,texCoord0.x, texCoord0.y),
        Rect2D::xywh(0,0,texCoord1.x, texCoord1.y),
        Rect2D::xywh(0,0,texCoord2.x, texCoord2.y),
        Rect2D::xywh(0,0,texCoord3.x, texCoord3.y));
}


void Draw::rect2D(
    const Rect2D& rect,
    RenderDevice* rd,
    const Color4& color,
    const Rect2D& texCoord0,
    const Rect2D& texCoord1,
    const Rect2D& texCoord2,
    const Rect2D& texCoord3) {

    const Rect2D* tx[4];
    tx[0] = &texCoord0;
    tx[1] = &texCoord1;
    tx[2] = &texCoord2;
    tx[3] = &texCoord3;

    int N = iMin(4, GLCaps::numTextureCoords());

    rd->pushState();
    rd->setColor(color);
    rd->beginPrimitive(RenderDevice::QUADS);
        for (int i = 0; i < 4; ++i) {
            for (int t = 0; t < N; ++t) {
                rd->setTexCoord(t, tx[t]->corner(i));
            }
            rd->sendVertex(rect.corner(i));
        }
    rd->endPrimitive();
    rd->popState();
}


void Draw::rect2DBorder(
    const class Rect2D& rect,
    RenderDevice* rd,
    const Color4& color,
    double outerBorder,
    double innerBorder) {


    //
    //
    //   **************************************
    //   **                                  **
    //   * **                              ** *
    //   *   ******************************   *
    //   *   *                            *   *
    //
    //
    const Rect2D outer = rect.border(outerBorder);
    const Rect2D inner = rect.border(-innerBorder); 

    rd->pushState();
    rd->setColor(color);
    rd->beginPrimitive(RenderDevice::QUAD_STRIP);

    for (int i = 0; i < 5; ++i) {
        int j = i % 4;
        rd->sendVertex(outer.corner(j));
        rd->sendVertex(inner.corner(j));
    }

    rd->endPrimitive();
    rd->popState();
}


void Draw::frustum(
    const class GCamera::Frustum& frustum,
    RenderDevice* rd,
    const Color4& color,
    const Color4& wire) {

    // Draw edges
    rd->pushState();

    if (wire.a > 0) {
        rd->setColor(wire);
        for (size_t f = 0; f < frustum.faceArray.size(); ++f) {
            rd->beginPrimitive(RenderDevice::LINE_STRIP);
            for (size_t v = 0; v < 5; ++v) {
                rd->sendVertex(frustum.vertexPos[frustum.faceArray[f].vertexIndex[v % 4]]);
            }
            rd->endPrimitive();
        }
    }

    rd->setDepthWrite(false);
        rd->setCullFace(RenderDevice::CULL_NONE);
        rd->enableTwoSidedLighting();
        rd->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);
        rd->setColor(color);
        rd->beginPrimitive(RenderDevice::QUADS);
        for (size_t f = 0; f < frustum.faceArray.size(); ++f) {
            rd->setNormal(frustum.faceArray[f].plane.normal());
            for (int v = 0; v < 4; ++v) {
                rd->sendVertex(frustum.vertexPos[frustum.faceArray[f].vertexIndex[v]]);
            }
        }
        rd->endPrimitive();

    rd->popState();
    // TODO:face colors
}

}

