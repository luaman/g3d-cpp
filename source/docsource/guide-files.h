/** @page guidefiles Working with Files & Networks

  @section filemanagement File Management

  G3D wraps most operating system file services to make them cross platform and easier to
  use.  

  G3D::filenameBaseExt, 
  G3D::filenameExt, and
  G3D::parseFilename process strings containing filenames.  To obtain directory listings
  (including wildcards), use G3D::getFiles, and G3D::getDirs. 

  G3D::copyFile,
  G3D::createDirectory,
  G3D::createTempFile,
  G3D::fileExists,
  G3D::fileLength, and
  G3D::resolveFilename, provide information about files and allow directory management.

  @section textfiles Text Files
  G3D::readFileAsString and G3D::writeStringToFile are the easy way to save and load text data.

  To help with parsing configuration and script files, the powerful G3D::TextInput provides 
  both basic text parsing and a tokenizer capable of parsing C++ and Java code.  G3D::TextOutput
  supports the mirror operations for writing nicely formatted text files with nested indenting.
 
  @section binaryfiles Binary Files

  G3D::BinaryInput and G3D::BinaryOutput simplify parsing and writing binary files.  They
  can handle both big and little endian and make appropriate corrections for the current
  platform.  These classes also offer gzip-level compression
  and decompression by setting a flag.

  Many G3D classes have serialize methods that can save the state of that class
  to a text or binary output and deserialize methods that can.  The overloaded
  G3D::serialize and G3D::deserialize functions provide the same functionality
  for built-in types like bool, float, and std::string.

  @section network Network

  G3D::NetworkDevice is to sockets as G3D::RenderDevice is to OpenGL-- it wraps them
  and simplifies state management.  Use it to create G3D::ReliableConduit (TCP) and
  G3D::LightweightConduit (UDP) connections between machines.  Both use the same 
  serialize/deserialize methods as the file I/O classes.

  The Discovery API (G3D::DiscoveryClient and G3D::DiscoveryServer) simplifies
  LAN server browsing and peer-to-peer networking. 

  These functions are demonstrated in the <A HREF="../demos/NetworkDemo">demo/NetworkDemo</A>. 

  @section images Images
    
  G3D::GImage can load many kinds of image files.  It is used internally by G3D::Texture.
 
  The G3D::computeNormalMap, G3D::flipRGBVertical, G3D::RGBtoARGB, G3D::RGBtoBGR, G3D::RGBtoBGRA, G3D::RGBtoRGBA, and G3D::RGBxRGBtoRGBA
  provide some basic image management services but we do not recommend using GImage for more
  advanced image processing.   (We chose the name "G"Image so
  you can introduce your own "Image" class without a name conflict).
  Consider the Open Source use ImageMagik for serious image work.

  @section models Models

    <CENTER><TABLE BORDER=1 WIDTH=75% BGCOLOR=#CCCCCC>
  <TR><TD>
  G3D does not provide a scene graph-- you must write that yourself even if it is as
  simple as an array of objects.  
    </TD></TR></TABLE></CENTER>

  
  G3D includes some (purely optional) structure that you can use to connect your 
  scene graph to the rendering and effect mechanisms.
 
  The pure virtual class, G3D::PosedModel, represents part of 
  a scene graph that has been prepared for rendering.  IFSModel and MD2Model load
  basic meshes and Quake 2 characters and can convert them to G3D::PosedModel instances.

  To aid in creation of meshes and parsing other file formats, <A HREF="../contrib/IFSBuilder">
  contrib/IFSBuilder</A> is an unsupported program that parses 3DS, OBJ, and other mesh
  files and creates some procedural meshes.  You will need to edit the source code to 
  use it.
*/