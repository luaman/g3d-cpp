/** @page guideconcepts Concepts

  This section details some high-level concepts that will make the library easier to use.

  @section imm Immediate Mode Graphics

  G3D's structure is patterned on that of OpenGL and DirectX.  The major difference
  is that it provides easier to use state and resource management.  Graphics are
  primarily at the immediate mode level: programs render using 
  G3D::RenderDevice by clearing the screen,
  configuring state for transformation and lighting, and then sending a number of triangles
  to the graphics card.  Triangles are specified by a series of vertices, each 
  with its own texture coordinate, color, and surface normal.  Animation is achieved by
  drawing frames in rapid succession.
  
  The basic concepts of immediate mode graphics are not explained in this manual or
  in the G3D documentation.  Refer to the OpenGL manual for an introduction to immediate mode
  hardware graphics rendering and to a textbook like <I>Computer Graphics: Principles and
  Practice</I> for information about lighting and transformations.

  @section coordinates Coordinate Systems

  RenderDevice uses separate matrices for the object-to-world, world-to-camera, and perspective
  transformations (the concatenation of the first two is the equivalent of the OpenGL ModelView
  matrix).

  The default coordinate system is right-handed with Y=up, X=right, and Z=towards viewer.  
  Objects "look" along their negative z-axis.

  @section ref Reference Counting
  
  G3D uses reference counting to automatically manage memory for key resources, like G3D::Textures.
  These classes are allocated with static factory methods (G3D::Texture::fromFile) instead
  of <CODE>new</CODE> and pointers are stored in Ref types (G3D::TextureRef instead of G3D::Texture*). 
  You can use the Ref type as if it were a regular pointer, sharing a single instance over 
  multiple Ref's, dynamically casting, and invoking methods with the arrow (<CODE>-\></CODE>).

  Reference counted objects automatically maintain a count of the number of pointers to them.
  When that pointer count reaches zero the object could never be used again, so it automatically
  deletes itself.  This is both convenient for objects that have no natural "owner" to delete 
  them.  It also helps avoid memory leaks because memory management is automatic.

  <CENTER><TABLE BORDER=1 WIDTH=75% BGCOLOR=#CCCCCC>
  <TR><TD>If a class has a Ref type, never create a raw pointer to it and never call <CODE>delete</CODE>
  on an instance of it.</TD></TR></TABLE></CENTER>
  
  You can create your own reference counted classes using:
  
    <PRE>
       typedef G3D::ReferenceCountedPointer<class MyClass> MyClassRef;
       class MyClass : public G3D::ReferenceCountedObject { ... };
    </PRE>


  @section stl STL vs. G3D

   In general, we recommend using STL classes like <CODE>std::string</CODE> wherever possible--
   they are standardized and do their job well.  However, for some data structures G3D provides
   alternatives because the STL implementation is not appropriate for graphics use. 
   G3D::Array, G3D::Queue, G3D::Table, and G3D::Set are written in the style of the STL with
   iterators and mostly the same methods.  However they are optimized for access patterns that 
   we have observed to be common to real-time 3D programs, are slightly easier to use, and 
   obey constraints imposed by other graphics APIs.  For example, G3D::Array guarantees that
   the base pointer is aligned to a 16-byte boundary, which is necessary for working with 
   MMX and SIMD instructions.  These classes also contain fixes for some bugs in older 
   versions of the STL.

   The G3D::System class provides platform-independent access to low-level properties of the
   platform and CPU.  It also has highly optimized routines for timing (at the cycle level)
   and memory operations like System::memcpy.

  @section layer Layered APIs
  (see namespace map)

 */
