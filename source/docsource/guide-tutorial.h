/** @page guidetutorial Tutorial

@section Introduction

<img src="splash.jpg" width=600></img>

The goal of this assignment is to set up, compile, run and debug a 3D scene
using the G3D library. This handout walks you through the process, but it
intentionally written to force you to refer to the online documentation available
on the course web page. Except for maybe the very last part of the assignment,
you shouldn't have to do any math, just a lot of reading.

You should make extensive use of the G3D documentation at:
http://g3d-cpp.sourceforge.net/html/index.html 
(and also in your local G3D installation, in html/index.html in your G3D installation.)

That index page lists all of the classes in G3D by functionality. It has links
to alphabetic lists as well. Look at <a href="map.png">the Namespace Map</a> 
for an architectural view of G3D. You may also want to look at the OpenGL
man pages because many G3D calls are designed to be similar to an equivalent
OpenGL call. Use either man or:
http://www.3dlabs.com/support/developer/GLmanpages/

G3D also includes some source code demos that may help if you need to brush
up on your OpenGL-style rendering. See the demos directory of your G3D installation.
The GLG3D_Demo will be particularly useful to you at this stage.

If you're doing this tutorial as the first assignment in a class, you can bet that 
future assignments will assume that you are familiar with the G3D library
and can create 3D scenes, detect collisions, and handle user input. With a solid 
grasp of G3d, you won't have to learn new support code for each project and 
will have a powerful, reliable base of code on which to build).

\section prereqs Prerequisites

To make best use of this tutorial, we recommend that you are already comfortable with 
<ul>
<li>C++</li>
<li>C++ templates (you can fake it if you don't know this will yet)</li>
<li>Compiling and running c++ projects on the architecture of your choice</li>
<li>OpenGL: you don't need to be an expert, but you should feel okay about setting up lighting and drawing primitives</li>
</ul>
In addition, you should have a complete and working installation of G3D, including
the packages it requires. See \link guideinstall \endlink for installation instructions. The
directory in which you installed G3D will be referred to as G3D_ROOT for the duration of this tutorial. 

You will need the following packages and tools installed. See your teaching assistant
or sysadmin if you have problems installing or locating these tools. 
<ul>
<li>Linux</li>
	<ul>
	<li><a href="http://ice.sourceforge.net/">iCompile</a></li>
	<li><a href="http://www.libsdl.org/index.php">SDL</a>, Simple DirectMedia Layer. 
		There should be a /usr/share/doc/libsdl1.2-dev directory on your machine</li>
	</ul>
<li>Windows</li>
	<ul>
	<li><a href="http://www.libsdl.org/index.php">SDL</a>, Simple DirectMedia Layer</li>
	<li>Microsoft Visual C++ 6.0 or Microsoft Visual Studio .NET 2003 with Microsoft Visual C++ 7.0</li>
	</ul>
</ul>

You should build and run a few of the demos to insure that your installation is A-OK. 


\section setup Project Setup
	@subsection linux Linux Project Setup
		Create a new directory for your project. 

Copy the icompile script where ever you downloaded it 
to yor new project directory. You will use this script instead of a Makefile (though you
may write your own Makefile if you prefer). iCompile automatically compiles 
and links every C++ file in the directory it's run from, so you won't need 
to edit it to refer to each source file (as you would with a Makefile). 
It needs the following environment variables, which you can set in your 
<tt>.cshrc</tt> file using setenv. Modify the paths as appropriate for
your installation:

<tt>
<pre>
INCLUDE ~me/libraries/g3d/include:/usr/include/SDL
LIBRARY ~me/libraries/g3d/linux-lib
LD_LIBRARY_PATH $LIBRARY
CC gcc-3.2
CXX g++-3.2
</pre>
</tt>
		
	@subsection windowsprojsetup Windows Project Setup

Even if your course requires you to hand in code that compiles and runs correctly on
Linux, you may want to use Windows to develop your code because of its more
featureful development environment. G3D works equally well, if not better, under
MSVC: the library integrates into the IDE, it runs faster, and you get the nicer
MSVC debugger.

Copy SDL.dll, glut32.dll, and zlib1.dll from G3D_ROOT/libraries/g3d/demos
to your project directory. You must do this for every G3D project. 

@subsubsection vc6 Microsoft Visual C++ 6.0

Launch MSVC 6.0. Make a new empty project by selecting "New.." under
the "File" menu, switching to the "Projects" tab and selecting the "Win32
Application" template. Give the project a name. 

Next, you need to add two additional paths to the project options for libraries
and include files. Bring up the "Option" window from the "Tools" menu. Select
the "Include files" item in the "Show directories for:" drop-down list, and add
two new entries for the SDL and G3D libraries. If you have used default installation locations,
these will be c:/SDL-1.2.7 and G3D_ROOT/win32-lib.
Next, select the "Include files" item under the "Show directories for:" drop-down list, 
and add the two new paths or SDL and G3D.

@subsubsection vc7 Microsoft Visual Studio .NET 2003

Go to Tools:Options. Select the Projects heading, and within that heading, Visual C++
Directories. In that panel, select "Show Directories For: Include files." Add the g3d
include directory, G3D_ROOT/include, and the SDL include directory, probably C:/SDL-1.2.7/include
In the same panel, select "Show Directories For: Library Files."
Add the g3d library directory, G3D_ROOT/win32-7-lib.

Make a new, empty Win32 Project. (File -> New -> Project, select
Visual C++ Project, then "Win32 Project". Give it a name, and click okay.
This brings up the Win32 Application Wizard. You need to tell it to make an
empty project. Do this by clcking on "Application Settings" in the Wizard
window, and checking the "Empty project" checkbox. Click "Finish."
 
@section emptyworld Create an Empty World
Create a main.cpp file and an App.h file with the following respective contents:

@subsection main_cpp main.cpp
<pre>
#include <G3DAll.h>
#include "Demo.h"
#include "App.h"

void App::main()
{
	Demo(this).run(); 
}

int main(int argc, char** argv) 
{
	GAppSettings settings;
	settings.window.width = 800;
	settings.window.height = 600;

	App app(settings);
	app.setDebugMode(true); 
	app.debugController.setActive(true);
	
	app.run();
	return 0;
}
</pre>


@subsection app_h App.h
<pre>
#ifndef App_H
#define App_H
#include <G3DAll.h>

class App: public GApp {
	public:
		App(const GAppSettings& settings): GApp(settings) fg; 
		void main();
};

#endif
</pre>

@subsection more Filling out the new project

<i>Note about namespaces:</i> All G3D classes are defined in the namespace G3D,
so we'll explicitly include that in this handout (ie: G3D::GApp). Nevertheless,
the G3DAll.h file includes the line using namespace G3D so you won't have to
explicitly type the namespace in your program.

Next, define a class called <tt>Demo</tt> that inherits from <tt>G3D::GApplet</tt>. The constructor
should take a <tt>App*</tt> as a parameter and pass it to the parent's contructor.
Your <tt>Demo</tt> class should store the passed-in <tt>App*</tt> in a member variable of its own.
Override the <tt>G3D::GApplet::doGraphics</tt> method to clear the screen by calling
the <tt>G3D::RenderDevice::clear</tt> method on your app's renderDevice. Set
the projection and camera matrices from your app's <tt>debugCamera</tt> using the
<tt>G3D::RenderDevice::setProjectionAndCameraMatrix</tt> method. To get an idea
of there the origin lies, render a default set of axes using <tt>G3D::Draw::axes</tt>
Compile and run the program with <tt>compile --run</tt> on linux, or <tt>F5</tt> on windows. 
You should see a gray background with a set of colored axes in the middle. (You may have to move
the mouse around to see the axes.)

In debug mode, you get a few things for free. For example, you can control the
camera with standard first-person game controls:
<ul>
<li>Use the mouse to tilt the camera sideways and up/down</li>
<li>Hold theWkey to move the camera forward, the S key to move it backward</li>
<li>Hold the A and D keys to translate the camera left and right</li>
<li>Press the Esc key to quit the program</li>
</ul>

If you have never played first-person games before, fly around to gain some
familiarity with this navigation method.

If movement is too quick, take a look at G3D::ManualCameraController's methods
to slow things down to something manageable.

@section sky Add a Sky and Lighting

<img src="tutorial-sky.jpg" width=400>

Next, we are going to add a sky and lighting to the scene. Add a member
variable of type G3D::SkyRef (not SkyRef*) to your <tt>Demo</tt> class. By the naming
conventions used across G3D, any class that ends with the Ref suffix is a
reference-counted pointer: use it as a pointer (i.e. use -> or the * operator),
but do not delete it explicitly.
Override the G3D::GApplet::init method in your <tt>Demo</tt> class, and have your
implementation initialize the G3D::SkyRef with a G3D::Sky instance as follows:
<pre>
sky = Sky::create(_app->renderDevice, _app->dataDir + "sky/");
</pre>

Your <tt>Demo</tt>'s init method will be called for you automatically.
Now modify <tt>Demo::doGraphics</tt>. After you have set up the camera, but before
you draw the axes, create and initialize a local variable of type
G3D::LightingParameters and call G3D::Sky::render with these lighting parameters
(hint: you might want to use G3D::toSeconds as well). Consult the
documentation for the details about these functions.

Enable lighting for the scene using G3D::RenderDevice::enableLighting. Use
methods in the G3D::RenderDevice and G3D::GLight classes to set the ambient
light color and set a directional light (use light 0); the G3D::LightingParameters
object will tell you the ambient light color, light direction, and light color.
After the G3D::Draw::axes call, disable lighting by calling
G3D::RenderDevice::disableLighting. Finally, call G3D::Sky::renderLensFlare.
Run the program again: you should see a cloudy sky as a backdrop to the axes.


@section groundplane Create a Ground Plane

In this part of the tutorial, you will add a 14m by 7m flat, horizontal textured
rectangle below the axes in your scene.3 You will use immediate mode rendering. 
That is, instead of having a plane object, you'll just send the vertices
for the plane every time the scene is drawn.

We want the ground plane to be affected by lighting, so you need to insert
this code after the light has been enabled but before it is disabled. To render
the ground plane, use methods in G3D::RenderDevice that correspond
to similarly-named OpenGL calls you are familiar with. For example, use
G3D::RenderDevice::beginPrimitive(G3D::RenderDevice::QUADS) instead
of <tt>glBegin(GL QUADS)</tt>, G3D::RenderDevice::setNormal instead of <tt>glNormal</tt>,
G3D::RenderDevice::sendVertex instead of <tt>glVertex</tt>, and so on. Consult the
overview documentation on G3D::RenderDevice if you get stuck.

Texture your ground plane. Add a new member variable of type
G3D::TextureRef to the Demo class and initialize the texture in your Demo::init
method, using G3D::Texture::fromFile. You can use any .JPG, .BMP, .TGA
or .PCX ¯le you like for this. Modify your rendering code in doGraphics to
assign this texture to texture unit 0, and supply appropriate texture coordinates
using G3D::RenderDevice. Again, the G3D calls correspond directly to
OpenGL calls.

It will be easier to debug the texture coordinates if you change the default camera
position to look down at the ground plane. You can do this in <tt>Demo::init</tt>
by calling G3D::GCamera methods on your app's <tt>debugCamera</tt>.

@section scenegraph Create a Scene Graph

G3D is low level and does not provide a scene graph structure. You must make
your own based on the needs of your project. Fortunately, the data structures
and rendering support provided are powerful enough that it is easy to make a
scene graph.

Make a pure virtual <tt>Entity</tt> class as follows:

<pre>
class Entity {
public:
	bool selected;
	Entity() : selected(false) {}
	virtual ~Entity() {}
	virtual void render(RenderDevice*) = 0;

	// Returns amount of time to intersection starting at ray.origin and traveling with ray.velocity.
	virtual RealTime getIntersectionTime(const Ray&) = 0;
};
</pre>


Make a subclass of <tt>Entity</tt> called <tt>SphereEntity</tt> with a constructor 
that takes a color, a center position and a radius. Use the G3D::Draw class to help
you implement SphereEntity::render. Use G3D::CollisionDetection or
G3D::Ray::intersectionTime to help you implement
SphereEntity::getIntersectionTime.
To take care of the rendering, use either the G3D::Sphere class or load the
<tt>sphere.ifs</tt> model from the GApp::dataDir + "ifs" directory using IFSModel
(though we specifically recommend against using multiple inheritance from both
Entity and G3D::Sphere).

Add a new member to the <tt>Demo</tt> class of type G3D::Array<Entity*>. This array
will contain references to all the objects in your scene (this is a one-level scenegraph).
In <tt>Demo::init</tt>, create three new <tt>SphereEntity</tt>'s and append them to
the array. You should release these objects by calling G3D::Array::deleteAll
from <tt>Demo::cleanup</tt>, which you need to override from G3D::GApplet (it will
be called automatically when the applet is closed). In <tt>Demo::doGraphics</tt>, iterate
through your array, calling Entity::render on each object. G3D::Array
overloads the square brackets operator for accessing the elements inside the
array, so you can do <tt>myArray[0]->doSomething();</tt>.

Run the program to verify that the scene looks the way you expected. You may
find the following helpful for debugging: G3D::debugPrintf, G3D::debugAssertM,
G3D::GApp::debugPrintf, G3D::Draw.

Next, make your <tt>SphereEntity</tt>'s draw differently when the <tt>selected</tt> flag is true.
You'll use this in the next section. 

Choose whatever appearance you prefer to represent this state. For example,
you might change the color or the fourth argument to G3D::Draw::sphere, or
use G3D::RenderDevice::setRenderMode to add wireframe.

@section interaction User Interaction

You will now make it possible to select objects in your scene with the mouse.
First of all you want the mouse pointer to be visible on screen. Delete the
<tt>debugController.setActive</tt> line from your main.cpp file.

This will disable the first-person camera controls that were explained earlier.
While the application is running, you can switch between first-person navigation
and the mouse cursor by pressing the Tab key.

You are going to override the G3D::GApplet::doLogic method, and inside your
implementation you will detect mouse click events. The
<tt>G3D::GApp::userInput->keyPressed(SDL LEFT MOUSE KEY)</tt> (G3D::GApp::userInput->keyPressed) method will return
true when there was a mouse click. In that case, get the mouse pointer
coordinates from the G3D::UserInput object. (Note that <tt>G3D::UserInput::get*</tt> methods are deprecated. 
You can still get the mouse position with G3D::UserInput::mouseXY(), 
G3D::UserInput::getX(), and G3D::UserInput::getY(), but one of these methods will be more useful
than the others. 

You will map these 2D coordinates to the 3D ray that goes through that pixel from the center of projection.
(Hint: look at G3D::GCamera documentation and
G3D::RenderDevice::getViewport.) Use this ray and the
<tt>Entity::getIntersectionTime</tt> methods to set the selected flag on the first
object hit by the ray. Make sure to unselect all other objects.

@section crazy Going Crazy
Now that you have learned to fly around and click on objects to select them, do
something creative to make your scene more interesting. Here are some ideas
to get you started. You are encouraged to make up your own ideas as well:
<ul>
<li> Replace some of the spheres with a teapot (G3D::IFSModel), a bunny, or
even an animated Darth Maul (G3D::MD2Model).</li>
<li>Make the objects in the scene move over time</li>
<li>Replace the ground texture with one rendered on the fly using
G3D::Texture::copyFromScreen</li>
<li>Make the spheres reflect the sky (G3D::Sky::getEnvironmentMap,
G3D::RenderDevice::configureReflectionMap)</li>
<li>Make your selection method more interesting by implementing the 
<a href="http://www.flipcode.com/articles/article_objectoutline.shtml">hardware accelerated
object outlining technique by Max McGuire</a>. G3D::RenderDevice has several 
methods that support the stencil buffer.</li>

@section onwards Onwards and Upwards

Now that you've gotten your feet wet with G3D, here are some more resources that 
you might find useful:
<ul>
<li><a href="http://sourceforge.net/projects/g3d-cpp/">SourceForge g3d project</a>, including
<a href="http://sourceforge.net/forum/?group_id=76879">forums</a> and a <a href="http://sourceforge.net/tracker/?group_id=76879&atid=548562">bugtracker</a>.
<li>The \link guidewelcome Manual \endlink goes into more detail on particularly interesting parts of G3D. 
<li>The <tt>contrib</tt> directory of your G3D installation contains interesting/useful examples and tools.</li>
</ul>

Tutorial questions, comments, feedback? Mail to sbshine at users.sourceforge.net. 
 */