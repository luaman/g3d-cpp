
/**
  @file demos/main.cpp

  This is a sample main.cpp to get you started with G3D.  It is
  designed to make writing an application easy.  Although the
  GApp/GApplet infrastructure is helpful for most projects,
  you are not restricted to using it-- choose the level of
  support that is best for your project (see the G3D Map in the
  documentation).

  @author Morgan McGuire, matrix@graphics3d.com
 */

#include <G3DAll.h>
#include "../contrib/Win32Window/Win32Window.h"
#include "../contrib/Win32Window/Win32Window.cpp"

#if G3D_VER < 60400
    #error Requires G3D 6.04
#endif

class App : public GApp {
protected:
    void main();
public:
    SkyRef              sky;

    App(const GAppSettings& settings);
};


/**
 This simple demo applet uses the debug mode as the regular
 rendering mode so you can fly around the scene.
 */
class Demo : public GApplet {
public:

    // Add state that should be visible to this applet.
    // If you have multiple applets that need to share
    // state, put it in the App.

    class App*					app;

    Demo(App* app);    

    virtual void init();

    virtual void doLogic();

    virtual void doGraphics();

};


Demo::Demo(App* _app) : GApplet(_app), app(_app) {

}


void Demo::init()  {
   
	// Called before Demo::run() beings
    app->debugCamera.setPosition(Vector3(0, 20, 0));
    app->debugCamera.lookAt(Vector3(0, 0, 0), -Vector3::UNIT_Z);
}



void Demo::doLogic() {
    if (app->userInput->keyPressed(SDLK_ESCAPE)) {
        // Even when we aren't in debug mode, quit on escape.
        endApplet = true;
        app->endProgram = true;
    }
}


void Demo::doGraphics() {
    LightingParameters lighting(G3D::toSeconds(11, 00, 00, AM));
    app->renderDevice->setProjectionAndCameraMatrix(app->debugCamera);

    // Cyan background
    app->renderDevice->setColorClearValue(Color3(.1, .5, 1));

    app->renderDevice->clear(true, true, true);

	
	Draw::axes(CoordinateFrame(Vector3(0, 0, 0)), app->renderDevice);

}



/** Computes an array of */
void getViewFrustumFaces(Array< Array<Vector4> > face);


/**
 Returns the set of planes bounding the region that can cast
 shadows visible to the camera, i.e. the set of all points
 that can shadow points in the view frustum.

 @param light  World space firection to the light source (which must be a directional light)
 @param camera The camera (defines the view frustum)
 @param plane  The output array of planes
 @param viewport The window-space viewport.  Used only to compute the aspect ratio of the frustum.
 */
void computeVisibleShadowCasterVolume(
    const GCamera&      camera,
    const Rect2D&       viewport,
    const Vector3&      light,
    Array<Plane>&       plane) {
    
    plane.clear();

    // The volume is the convex hull of the vertices definining the view
    // frustum and the light source point at infinity.  This volume
    // consists of allthe planes in the view frustum that have the light
    // source in their positive half space (i.e. N dot L > 0) and
    // planes containing view frustum edges (with two finite vertices)
    // in only one such plane and the light source point.

    Array<Plane> frustum;
    camera.getClipPlanes(viewport, frustum);

    // Remove planes that would cull the light
    for (int p = 0; p < frustum.size(); ++p) {
        if (frustum[p].normal().dot(light) <= 0) {
            frustum.fastRemove(p);
        }
    }


    const double x               = camera.getViewportWidth(viewport) / 2;
    const double y               = camera.getViewportHeight(viewport) / 2;
    const double z               = camera.getNearPlaneZ();
    const double w               = z / camera.getFarPlaneZ();

    (All in camera space):

    class Edge {
    public:
        /** Edge is directed from v[0] to v[1] */
        int         vertexIndex[2];
        Edge(){}
        Edge(int v0, int v1) {
            vertexIndex[0] = v0;
            vertexIndex[1] = v1;
        }
        bool equals(int v0, int v1) const {
            return (vertexIndex[0] == v0) &&
                   (vertexIndex[1] == v1);
        }
    };

    class Face {
    public:
        /** Counter clockwise */
        int         vertexIndex[4];
    };


    Array<Vector4>  vertexPos;
    
    // Near face (ccw from UR)
    vertexPos.append(
        Vector4( x,  y, z, 1),
        Vector4(-x,  y, z, 1),
        Vector4(-x, -y, z, 1),
        Vector4( x, -y, z, 1));

    // Far face (ccw from UR, from origin)
    vertexPos.append(
        Vector4( x,  y, z, w),
        Vector4(-x,  y, z, w),
        Vector4(-x, -y, z, w),
        Vector4( x, -y, z, w));

    // Transform to world space
    CoordinateFrame cframe;
    camera.getCoordinateFrame(cframe);
    for (int v = 0; v < vertexPos.size(); ++v) {
        vertexPos[v] = cframe.toWorldSpace(vertexPos[v]);
    }


    Array<Face> frustum;
    Face face;
    
    // Near plane (wind backwards so normal faces into frustum)
    face.vertexIndex[0] = 3;
    face.vertexIndex[1] = 2;
    face.vertexIndex[2] = 1;
    face.vertexIndex[3] = 0;
    frustum.append(face);

    // Far plane
    face.vertexIndex[0] = 4;
    face.vertexIndex[1] = 5;
    face.vertexIndex[2] = 6;
    face.vertexIndex[3] = 7;
    frustum.append(face);

    // Right plane
    face.vertexIndex[0] = 0;
    face.vertexIndex[1] = 4;
    face.vertexIndex[2] = 7;
    face.vertexIndex[3] = 3;
    frustum.append(face);

    // Left plane
    face.vertexIndex[0] = 5;
    face.vertexIndex[1] = 1;
    face.vertexIndex[2] = 2;
    face.vertexIndex[3] = 6;
    frustum.append(face);

    // Top plane
    face.vertexIndex[0] = 1;
    face.vertexIndex[1] = 5;
    face.vertexIndex[2] = 4;
    face.vertexIndex[3] = 0;
    frustum.append(face);

    // Bottom plane
    face.vertexIndex[0] = 2;
    face.vertexIndex[1] = 3;
    face.vertexIndex[2] = 7;
    face.vertexIndex[3] = 6;
    frustum.append(face);

    // Boundary of the portion of the frustum that is on the convex hull
    // of (frustum + light).  The edges are directed ccw to close the frustum.
    // We do not include edges entirely at infinity since they only give
    // rise to planes at infinity.
    Array<Edge> boundary;

    // Collect planes that do not cull the light source and find the
    // boundary.
    for (int f = 0; f < frustum.size(); ++f) {
        const Face& face = frustum[f];
        int v0 = face.vertexIndex[0];
        int v1 = face.vertexIndex[1];
        int v2 = face.vertexIndex[2];
        Vector3 N = (vertexPos[v1] - vertexPos[v0]).cross(vertexPos[v2] - vertexPos[v0]);
        if (N.dot(L) > 0) {
            // This plane is on the convex hull
            plane.append(Plane(N, vertexPos[v0]));

            // Collect finite and semi-finite edges
            for (int v = 0; v < 4; ++v) {
                int a = face.vertexIndex[v];
                int b = face.vertexIndex[(v + 1) % 4];
                
                if ((vertexPos[a].w != 0) || (vertexPos[b].w != 0)) {
                    // This edge is at least semi-finite.  Search
                    // the boundary array for this edge.  If we
                    // find it, this edge is not on the boundary since
                    // its complement already added this edge to the boundary,
                    // so we remove both.  If not found, this edge is on the
                    // boundary so we add it to the array (to be potentially
                    // removed by a later complement).
                    
                    bool found = false;
                    for (int e = 0; e < boundary.size(); ++e) {
                        if ((boundary[e].equals(a, b)) {
                            boundary.fastRemove(e);
                            found = true;
                            break;
                        }
                    }

                    if (! found) {
                        // Add the reverse of this edge to the boundary
                        boundary.append(Edge(b, a));
                    }
                }
            }
        }
    }

    // Construct planes that contain the boundary edges and the light source
    for (int e = 0; e < boundary.size(); ++e) {
        const Vector4& A = vertexPos[boundary[e].vertexIndex[0]];
        const Vector4& B = vertexPos[boundary[e].vertexIndex[1]];
        // Create plane containing ABL
        plane.append(Plane(A, B, L));
    }

}

void App::main() {
	setDebugMode(true);
	debugController.setActive(false);
    
    Demo(this).run();
}


App::App(const GAppSettings& settings) : GApp(settings, new Win32Window(settings.window)) {
}


int main(int argc, char** argv) {
    GAppSettings settings;
    App(settings).run();
    return 0;
}
