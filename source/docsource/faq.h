/** @page faq General FAQ


There are several reoccuring questions that appear at the <A HREF="http://sourceforge.net/forum/forum.php?forum_id=262426">G3D forums</A>. This document answers many of the most common questions. Questions closer related to building problems may be answered in the \link guidecompiling Compiling and Linking Guide \endlink and \link errorfaq Build Error FAQ \endlink.
<P>

<B>G3D::Matrix4 vs. OpenGL Matrices</B>
<P>
User may notice passing G3D::Matrix4 as a float* to OpenGL's matrix functions has unexpected results. Similarly, copying an OpenGL float* matrix into a G3D::Matrix4 seems to store the values strangely. This is because G3D stores matrices in row major order and OpenGL stores them column major. Basically, rather than store elements contiguously by their columns, they, they are ordered by their rows. There are technical reasons to use either ordering, and it is not uncommon to encounter lively discussions arguing each representation's merits. For G3D, row major ordering was chosen for increased accessing performance at the cost of breaking an OpenGL convention. Fortunately, the conversion between OpenGL's column major ordering and G3D's row major ordering is simple: G3D::Matrix4::transpose will return a row major matrix in column major, and a column major matrix row major. Better yet, G3D's OpenGL extension will automatically perform this conversion using the G3D::CoordinateFrame objects with G3D::glLoadMatrix, G3D::glLoadInvMatrix, G3D::glMultInvMatrix, G3D::glMultMatrix, G3D::glGetFloat, and G3D::glGetMatrix. For most applications, G3D::CoordinateFrame is a better choice for rigid-body transformations than G3D::Matrix4.
<P>

<B>P-Buffers and G3D</B><P>
There is no extended functionality supporting p-buffers in G3D. This does not prevent you from using them through standard OpenGL calls, though. Using p-buffers is a recognizably difficult process, principally due to the extension's design. Fortunately, a new p-buffer specification is under work by the OpenGL review board. G3D intends to support this new specification.

<P><B>UNICODE Font Support</B><P>
G3D supports rendering fonts through G3D::GFont, but only through the ASCII 8-bit characters. To implement extended characters, one might attempt extending G3D::GFont for extended characters. The FreeType generates little 2D bitmaps that you have to pack into a texture (using G3D::Texture the way G3D::GFont does) for rendering.

<P><B>The Next Release, What's Coming, What's Going</B><P>
One way to check on the overall progress of a release is to check the changelog.h in CVS for fixes and features added. This is in the cpp/source/docsource directory: http://cvs.sourceforge.net/viewcvs.py/g3d-cpp/cpp/source/docsource/

Deprecated functionality is removed at major releases. (from 6.xx to 7.00)
Because G3D builds as a static lib, there is no code bloat to your final executable. I would not be concerned about the deprecated routines-- just avoid using them in new code. The list of all deprecated entry points is:
http://g3d-cpp.sourceforge.net/html/deprecated.html

<P><B>The Demos Run Slow</B><P>
Most of the G3D demos are designed to stress high-end hardware, and will likely choke lower-end systems. The most significant component affecting demo performance is the video card; even with a high-speed processor, a low-end graphics card will likely be the bottleneck in the rendering pipeline. Low performance in these demos doesn't mean G3D is "slow." The demos are intentionally constructed to test against the computational limits using methods which may not accurately represent the "real" 3D applications.

<P><B>What are Index Arrays?</B><P>
Say you have a shape with four distinct vertices, A, B, C, D, and the triangles you want to render are ABC and BCD. 
with sendVertex, you'd: 
 
sendVertex(A); sendVertex(B); sendVertex(C); 
sendVertex(B); sendVertex(C); sendVertex(D); 
 
with vertex arrays you make a VAR that contains A,B,C,D, and an Array<int> indexArray that contains 0,1,2 1,2,3. 

In the VAR_Demo you will see an example of this in the 'main.cpp' file. The "Model" object has an array of Vector3 for vertex locations. Then, a VAR is created with a Vector3 array (one for verticies and one for normals). 

<P><B>Using G3D with other libraries</B><P>
G3D is a middle-level API that is used to encapsulate and assist with common graphics tasks. It also has a very thin high-level layer (G3D::App, G3D::Applet). This can be simply ignored if you intend use some other high-level functionality, such as a Windows app you're writing or a scenegraph library. In this way, the library will make calls to G3D to do the OpenGL rendering.

However, some of these libraries even include their own render functions. In these cases, you can still use G3D to augment OpenGL functionality, but this may interfere with your library's rendering. The conditions are specific to the library, and the particular application. In any case, G3D's other non-OpenGL graphics functionality remains available; the geometry, image, and math API's may be useful depending on your needs.

Most libraries with orthogonal functionality (such as audio libraries) should operate adjacently with G3D without conflict.

For more details, see \link guideintro Working with Other Libraries \endlink.
 
*/