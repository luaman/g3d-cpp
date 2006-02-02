/** @page guidemeshes Working with Meshes

  \htmlonly
<TABLE BORDER=0 WIDTH=80%><TR><TD><I><FONT FACE="Arial">
<A HREF="guidevar.html"><IMG SRC="backarrow.gif" BORDER=0 ALIGN=MIDDLE>
Vertex Arrays</A></I></FONT></TD><TD ALIGN=RIGHT><FONT FACE="Arial"><I>
<A HREF="guideui.html">
User Input <IMG SRC="forwardarrow.gif" BORDER=0 ALIGN=MIDDLE></A></I></FONT></TD></TR></TABLE>
\endhtmlonly

    A mesh is a collection of faces, edges and vertexes that can be used 
to render 3D objects.  There is such a variety of mesh implementations that
%G3D provides only the basic building blocks for implementating a mesh.  The
rest of the implementation is left to the developer's own design.  The 
following tutorial will review the resources that %G3D provides and give an 
overview of how to use each.

@section meshalg G3D::MeshAlg
    
<P> G3D::MeshAlg class provides much of the common functionality necessary to
implement a custom mesh class.  These calculations include the ability to compute
common vertex attributes such as normals, binormals and tangents for tangent space
calculations; and mesh boundaries, and backfaces.  This class also provides 
basic functions for mesh analysis to include the calculation of min/mean/median/max 
face area and min/mean/median/max edge length.
<P> The basic function set in G3D::MeshAlg accepts an array of vertices and an
array of integers that corresponds to the mesh vertices and their transversal order.  
The vertices are assumed to define a triangle-based mesh.  If it is necessary to 
convert from another type of mesh to a triangle mesh, then the class function 
G3D::MeshAlg::toIndexedTriList can be used for the conversion.  It will accept an 
array of vertices in any %G3D defined Primitive and convert them to an indexed list 
of triangles.
<P> Another set of useful features in this class are the functions computeWeld
and WeldAdjacency which will collapese vertices that are within a defined radius of each
other.  This can be used to downsample a model or find unique vertices.
  
@section meshbuilder G3D::MeshBuilder
    
<P> G3D::MeshBuilder is a class built to optimize the creation of water-tight meshes
from a set of polygons.  It allows polygons to be added in the form of triangles and
quads.  After the models have been written, the commit function can be used to create
an array of vertices and a corresponding index array to be used with G3D::MeshAlg.

@section models G3D::IFSModel and G3D::PosedModel

<P> G3D::IFSModel allows simple access to loading/saving IFS models.  It is
important to note that the IFSModel class does not include texture coordinates, so it is
best used for scientific work and not representing textured models.  The IFSModel can 
be used in conjunction with MeshBuilder to save a set of points to a file.  The class 
documentation provides further explanation of class functionality.  
<P> G3D::PosedModel is essentially a snapshot of a specific model geometry.  Most models have
a variety of geometric poses, but these poses can be based on skeletal animation or several other 
algorithms.  G3D::PosedModel is intended to be the base class for user-defined object where 
these algorithms could come together and produce a conventional mesh that may be easily rendered.
By convention, every model implementation should provide a pose method, which returns an 
implementation specific child of PosedModel as its output.  This derived PosedModel then can 
be used to render the mesh.  G3D::MD2Model is an excellent demonstration for creating a
custom mesh model.
    
@section examples Contrib Examples

<A HREF="../contrib/IFSBuilder">contrib/IFSBuilder</A> parses 3DS, SM, PLY, and OBJ files.

<IMG SRC="curveeditor.jpg" WIDTH = 200>
<A HREF="../contrib/CurveEditor">contrib/CurveEditor</A> demonstrates using G3D::MeshBuilder,
 G3D::IFSModel::save and G3D::cyclicCatmullRomSpline to create meshes in a primitive
 interactive editor.

  */