/**
 @file Collision_Demo/buildScene.cpp

 @maintainer Morgan McGuire, matrix@graphics3d.com
 @created 2003-02-07
 @edited  2003-05-23
 */

#include <G3DAll.h>
#include "Model.h"
#include "Object.h"
#include "Scene.h"

extern Scene* scene;

/**
  This scene tests whether the system is robust to energy increasing through
  a collision.   The single sphere starts slightly above the surface.  Ideally,
  it should appear to be resting.  It is hard to simulate this scene without
  the sphere bouncing higher and higher through successive iterations.
  */
void insertRestingContactSpheres() { 
    // This sphere must stay at rest
    scene->insertDynamic(new SimSphere(Sphere(Vector3(4, 1.01, 3), 1), Vector3(0, 0, 0), Color3::blue()));

    // This sphere must become at rest on the surface
    scene->insertDynamic(new SimSphere(Sphere(Vector3(4, .49, 4.5), .5), Vector3(0, 0, 0), Color3::blue()));

    // This sphere must come to rest (given non-unit restitution)
    scene->insertDynamic(new SimSphere(Sphere(Vector3(4, 2, 1), .5), Vector3(0, 0, 0), Color3::blue()));
}


/**
 Constructs a tray for objects to sit on top of.
 */
void insertTray() {
    // Ground tray
    double wallHeight = 4;
    Color3 trayColor(Color3::gray());
    scene->insertStatic(new BoxObject(Box(Vector3(-14, -0.5, -10), Vector3(14, 0, 10)), trayColor));
    scene->insertStatic(new BoxObject(Box(Vector3(-14, 0, -10), Vector3(-13, wallHeight, 10)), trayColor));
    scene->insertStatic(new BoxObject(Box(Vector3(13, 0, -10), Vector3(14, wallHeight, 10)), trayColor));
    scene->insertStatic(new BoxObject(Box(Vector3(-13, 0, -10), Vector3(13, wallHeight, -9)), trayColor));
    scene->insertStatic(new BoxObject(Box(Vector3(-13, 0, 9), Vector3(13, wallHeight, 10)), trayColor));
}


/**
 The sphere beings touching the ground of the tray.  The challenge is to keep the ball on the
 surface of the tray, without using up all of the energy on micro-collisions or getting stuck
 in the ground.
 */
void insertRollingContactSpheres() {
    // Sphere on ground
    scene->insertDynamic(new SimSphere(Sphere(Vector3(-10, .25, -2), .25), Vector3(16, 0, 4), Color3::blue()));

    // Sphere on ramp
    scene->insertDynamic(new SimSphere(Sphere(Vector3(-2.25, 4.75, 9.4), .25), Vector3(0, 0, 0), Color3::blue()));
}


void insertSpiralSlide() {
    int i;
    for (i = 0; i < 41; ++i) {
        double angle = G3D_PI * i / 10.0;
        double angle2 = G3D_PI * (i - 0.6) / 10.0;

        // Outer spiral
        CoordinateFrame c;
        Box b(Vector3(-1, -1, -.1), Vector3(1, 1, .1));
        c.translation = Vector3(cos(angle) * 2.9, i / 3.5 + 1.5, sin(angle) * 2.9);
        c.lookAt(Vector3(cos(angle2) * 1.5, i / 3.5 + 2.2, sin(angle2) * 1.5));
        scene->insertStatic(new BoxObject(c.toWorldSpace(b), (Color3::yellow() + Color3::white()) / 2));

        // Inner inner spiral
        {
            Box b(Vector3(-.3, -.3, -.1), Vector3(.25, .25, .1));
            c.translation = Vector3(cos(angle) * 1.2, i / 3.5 + 1, sin(angle) * 1.2);
            c.lookAt(Vector3(cos(angle2) * 3, i / 3.5 + 2, sin(angle2) * 3));
            scene->insertStatic(new BoxObject(c.toWorldSpace(b), (Color3::yellow() + Color3::white()) / 2));
        }
    }

    scene->insertDynamic(new SimSphere(Sphere(Vector3(1.9, 13, -1), .75), Vector3(-2,-.5,-2), Color3::blue()));
}


/**
 Two slanted green ramps.
 */
void insertRamps() {
    {
        Box b(Vector3(-1, 0, -5), Vector3(1, .25, 5.5));
        CoordinateFrame c;
        c.lookAt(Vector3(0, 1, 2));
        c.translation = Vector3(-2.5, 2.25, 5.5);
        scene->insertStatic(new BoxObject(c.toWorldSpace(b), (Color3::green() + Color3::white()) / 2));
    }

    // Corner ramp  
    {
        Box b(Vector3(-1, 0, -5), Vector3(1, .25, 5.5));
        CoordinateFrame c;
        c.lookAt(Vector3(-2, 2, -2));
        c.translation = Vector3(-11.2, 2.85, -7.2);
        scene->insertStatic(new BoxObject(c.toWorldSpace(b), (Color3::green() + Color3::white()) / 2));
    }     
}

void buildScene() {
    //    scene->insertStatic(new SphereObject(Sphere(Vector3(-8, 1, 4), 1), (Color3::red() + Color3::white()) / 2));
    
    scene->insertStatic(new CapsuleObject(Capsule(Vector3(-9, 1, 4), Vector3(-9, 4, 4), 1), (Color3::red() + Color3::white()) / 2));

    scene->insertStatic(new BoxObject(Box(Vector3(6, 0, 0), Vector3(7, 1, 8)), (Color3::green() + Color3::white()) / 2));
    scene->insertStatic(new GeneralObject(Model::getModel("cow.ifs"), CoordinateFrame(Vector3(-7,1.7,4)), Color3::yellow()));
    scene->insertStatic(new GeneralObject(Model::getModel("p51-mustang.ifs"), CoordinateFrame(Vector3(10,1,2)), Color3::orange()));
    scene->insertStatic(new GeneralObject(Model::getModel("knot.ifs"), CoordinateFrame(Vector3(7,2.2,-4)), (Color3::blue() + Color3::white()) / 2));

    insertTray();
    insertRamps();
    insertSpiralSlide();
    insertRollingContactSpheres();
    insertRestingContactSpheres();
   
    //scene->insertDynamic(new SimSphere(Sphere(Vector3(-4, 3, 4), .25), Vector3(-1, 0, 0), Color3::blue()));
    
    // Spheres
    
    int i;
    for (i = 0; i < 10; ++i) {
        scene->insertDynamic(new SimSphere(Sphere(Vector3(0, 7, 0), .25), Vector3::random() * 10, Color3::blue()));
    }   
    
    
    /*
    {
        CoordinateFrame c;
        c.lookAt(Vector3(0,-1,0), Vector3::UNIT_X);
        c.translation = Vector3(5.25,4,0);
        scene->insertStatic(new GeneralObject(Model::getModel("triangle.ifs"), c, (Color3::blue() + Color3::white()) / 2));
    }*/
}
