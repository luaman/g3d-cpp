/**
 @page guideshaders Shaders

  Older graphics cards provide only a fixed-function pipeline in which lighting,
  texturing, and transformation is hardwired.  The range of effects that can
  be achieved on such cards is limited.  Newer cards (GeForce3/Radeon and later) 
  have a programmable pipeline that allows C-like code to be downloaded onto 
  the card and executed for each vertex and pixel.  The latest cards are 
  programmable-only and have no dedicated circuits for the old fixed-function
  routines.  Instead they emulate the fixed-function pipeline (which is slowly
  disappearing from OpenGL and DirectX altogether!).

  The programs that execute on the graphics card are called Vertex and Pixel 
  Shaders.  Well-written shaders are just as fast as the fixed-function pipeline
  but offer more flexibility and are easier to write and debug than fixed function
  code.  Most new games are written for programmable cards only, although for
  the widest possible compatibility it is good to have both full-featured 
  programmable and limited-effect fixed-function pathways in rendering code.

  Using shaders often requires configuring a G3D::RenderDevice in a specific
  way immediately before a group of primitives are rendered, and then restoring
  the old state.  You can subclass the G3D::Shader class to provide methods
  to execute immediately before and after a group of primitives are rendered.
  Most frequently, G3D::Shader::beforePrimitives calls 
  G3D::RenderDevice::setVertexAndPixelShader with appropriate arguments (e.g.
  the light source in object space).  G3D::SimpleShader is provided for convenience 
  when this is the <I>only</I> functionality needed.

  It is possible to write shaders without using the G3D::Shader class.  Even with G3D::Shader, it is 
  <I>not</I> possible to access the programmable pipeline without using one of the 
  other support classes listed below.

  <TABLE ALIGN=RIGHT WIDTH=50%>
  <TD><TD>
  <IMG SRC="ParallaxBump_Demo_screen.jpg" WIDTH=300>
  </TD></TR>
  <TR><TD>
  <CENTER>
  The Parallax Bump Mapping Demo (demos/parallax) uses G3D::VertexProgram and G3D::PixelProgram with Cg.
  </CENTER>
  </TD>
  </TR>
  </TABLE>

  @section supported Supported Languages

  G3D supports shaders written in the OpenGL shading language GLSL, in NVIDIA's 
  nearly identical Cg language, and in OpenGL assembly (DirectX uses a different
  language  called HLSL that is nearly identical to Cg and GLSL but is not 
  directly supported).  <B>We recommend GLSL for shader development</B> because
  it is the most widely supported and easiest to use of the shader languages.
   

  GLSL programs are loaded from strings or from files with G3D::VertexAndPixelShader.

  Assembly programs are loaded from strings or from files with G3D::VertexProgram
  and G3D::PixelProgram.

  Cg programs must first be compiled to assembly.  The assembly can then be loaded
  with G3D::VertexProgram and G3D::PixelProgram, which know how to parse the special
  comments that Cg adds at the top of a file.
  
  @section tools Tools
	RenderMonkey (http://www.ati.com/developer/rendermonkey/index.html) and the
    OpenGL Shader Designer (http://www.typhoonlabs.com/) are high-level development
    environments you can use to debug your shaders before incorporating them
    into a G3D project.

    To use Cg shaders you will need the compiler from the 
    Cg Toolkit (http://developer.nvidia.com/page/cg_main.html).  Cg can also be 
    used to help debug problems with GLSL shaders using the "-oglsl" flag.

  @section sample Sample Shaders
    Hundreds of sample shaders are available in all languages.
    The OpenGL Shading Language book (http://www.aw-bc.com/catalog/academic/product/0,1144,0321197895,00.html)
    is the official GLSL spec and contains many examples.  NVIDIA, ATI, and 3DLabs 
    each provide SDKs with tons more.
    
    Communit forums on programmable cards and sample shaders are also available at
    http://www.shadertech.com/

*/