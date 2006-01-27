/**
 @page guidefbo Framebuffer Class Tutorial
  \htmlonly
<TABLE BORDER=0 WIDTH=80%><TR><TD><I><FONT FACE="Arial">
<A HREF="guideshaders.html"><IMG SRC="backarrow.gif" BORDER=0 ALIGN=MIDDLE>
Shaders</A></I></FONT></TD><TD ALIGN=RIGHT> </TD></TR></TABLE>
\endhtmlonly

    @section overview Framebuffer Overview

    G3D's Framebuffer class implements OpenGL's framebuffer object extension.  The framebuffer object
    circumvents the need to copy-from-screen in order to render to a texture by implementing
    direct texture rendering.  Furthermore, the framebuffer allows rendering onto other images 
    such as renderbuffers which can contain depth and stencil data.  As an added bonus, 
    multiple images can be attached to a single framebuffer.  This makes implementing multiple render 
    targets a trivial task.

    It is very important to note the definitions of an image, a texture and a renderbuffer when talking about
    framebuffers.   An image is a set of texel data.  A texture is an image that can be written to and read from 
    in the context of texture operations.  A renderbuffer is an image that can be written to and read from through
    the framebuffer, but <i>cannot</i> be used in texture operations.  Both textures and renderbuffers are images,
    so the term image will be used to apply to the those objects as a collective.

    The framebuffer is surprisingly simple to use.  A new framebuffer is created simply by:
    
    <PRE>
        FramebufferRef fb = Framebuffer::create("New Framebuffer");
    </PRE>

    Once it is created, a framebuffer cannot be immediately used for rendering.   It must first
    have at least one image attached to it.  This image need not be a color buffer.  An image is attached
    to the framebuffer through the following code:

    <PRE>
        fb->set(Framebuffer::COLOR_ATTACHMENT0, someBuffer);
    </PRE>

    The argument someBuffer represents an image that will be used as a rendering target.  In order to enable
    rendering to the framebuffer, the RenderDevice operation setFramebuffer must be utilized:

    <PRE>
        renderDevice->setFramebuffer(fb);
    </PRE>

    Rendering operations may now commence.  Each framebuffer uses the current OpenGL context for all rendering
    operations, so no further initialization is necessary if the framebuffer wishes to use the 
    current OpenGL context.   Unfortunately, this also means that the current viewport parameters are also
    still present when the new framebuffer is rendered to.  If the current window display viewport is 800x600 and
    the rendering texture is 256x256, there will be discrepencies. In order to prevent this, a glViewport operation
    is required to set the correct viewport size after setting the framebuffer.

    In order to render to the window display framebuffer again, call setFramebuffer with a null argument as follows:

    <PRE>
        renderDevice->setFramebuffer(NULL);
    </PRE>

    In summary, the Framebuffer should be used as follows:

    <PRE>
        FramebufferRef fb = Framebuffer::create("New Framebuffer");
        Texture someBuffer = Texture::createEmpty("Some Buffer", 256, 256, TextureFormat::RGBA8);
        fb->set (Framebuffer::COLOR_ATTACHMENT0, someBuffer);

        renderDevice->setFramebuffer(fb);
        glViewport (0,0,256,256);
            ... Render code here. ...
        renderDevice->setFramebuffer(NULL);
        glViewport(0, 0, renderDevice->width(), renderDevice->height());
    </PRE>

    @section fb_completeness Framebuffer Completeness

    There are few other important things to keep in mind when rendering to a framebuffer.  At this time,
    every framebuffer attachment must be identical in size and resolution.  That means that you cannot
    mix a 256x256 RGBA8 texture with a 512x512 RGBA32F texture in the same framebuffer.  If you attempt
    this kind of madness, then you will receive a framebuffer completeness error.  A similar error will
    be generated if you attempt to render to a framebuffer without an attachment.
*/

