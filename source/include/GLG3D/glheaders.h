/**
 @file G3D/glheaders.h

 #includes the OpenGL headers

 @maintainer Morgan McGuire, matrix@graphics3d.com

 @created 2002-08-07
 @edited  2003-12-06

 Copyright 2002-2003, Morgan McGuire.
 All rights reserved.
*/

#ifndef G3D_GLHEADERS_H
#define G3D_GLHEADERS_H

#ifdef _WIN32
  #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
  #endif
  #include "windows.h"
#endif

#include "../GL/gl.h"
#define GL_GLEXT_PROTOTYPES
#include "../GL/glext.h"

#ifdef G3D_WIN32
//    #define WGL_WGLEXT_PROTOTYPES
    #include "../GL/wglext.h"
#endif

#if defined(G3D_LINUX) || defined(G3D_OSX)
//    #define GLX_GLXEXT_PROTOTYPES
	#include "../GL/glxext.h"
	#include "../GL/glx.h"
#endif
#include "../GL/glext.h"

#include "../glh/glut.h"

// OpenGL extensions


extern PFNGLMULTITEXCOORD2FARBPROC         glMultiTexCoord2fARB;

extern PFNGLMULTITEXCOORD1FARBPROC         glMultiTexCoord1fARB;
extern PFNGLMULTITEXCOORD1DARBPROC         glMultiTexCoord1dARB;

extern PFNGLMULTITEXCOORD2FVARBPROC        glMultiTexCoord2fvARB;
extern PFNGLMULTITEXCOORD2DVARBPROC        glMultiTexCoord2dvARB;

extern PFNGLMULTITEXCOORD3FVARBPROC        glMultiTexCoord3fvARB;
extern PFNGLMULTITEXCOORD3DVARBPROC        glMultiTexCoord3dvARB;

extern PFNGLMULTITEXCOORD4FVARBPROC        glMultiTexCoord4fvARB;
extern PFNGLMULTITEXCOORD4DVARBPROC        glMultiTexCoord4dvARB;
extern PFNGLACTIVETEXTUREARBPROC           glActiveTextureARB;
extern PFNGLCLIENTACTIVETEXTUREARBPROC     glClientActiveTextureARB;

extern PFNGLVERTEXARRAYRANGENVPROC         glVertexArrayRangeNV;
extern PFNGLFLUSHVERTEXARRAYRANGENVPROC    glFlushVertexArrayRangeNV;

extern PFNGLCOMPRESSEDTEXIMAGE2DARBPROC    glCompressedTexImage2DARB;
extern PFNGLGETCOMPRESSEDTEXIMAGEARBPROC   glGetCompressedTexImageARB;

extern PFNGLVERTEXATTRIBPOINTERARBPROC     glVertexAttribPointerARB;
extern PFNGLENABLEVERTEXATTRIBARRAYARBPROC glEnableVertexAttribArrayARB;
extern PFNGLDISABLEVERTEXATTRIBARRAYARBPROC glDisableVertexAttribArrayARB;

extern PFNGLPOINTPARAMETERFARBPROC         glPointParameterfARB;
extern PFNGLPOINTPARAMETERFVARBPROC        glPointParameterfvARB;

typedef BOOL (APIENTRY * PFNWGLGLSWAPINTERVALEXTPROC) (GLint interval);
typedef BOOL (WINAPI * PFNWGLCHOOSEPIXELFORMATARBPROC) (HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int* piFormats, UINT* nNumFormats);

#ifndef _WIN32
    extern PFNWGLGLSWAPINTERVALEXTPROC         wglSwapIntervalEXT;
    extern PFNWGLCHOOSEPIXELFORMATARBPROC      wglChoosePixelFormatARB;
    extern PFNWGLALLOCATEMEMORYNVPROC          wglAllocateMemoryNV;
    extern PFNWGLFREEMEMORYNVPROC              wglFreeMemoryNV;
#endif
extern PFNGLVERTEXARRAYRANGENVPROC         glVertexArrayRangeNV;

extern PFNGLMULTIDRAWARRAYSEXTPROC glMultiDrawArraysEXT;
extern PFNGLMULTIDRAWELEMENTSEXTPROC glMultiDrawElementsEXT;

  
#ifdef GL_NV_fence
extern PFNGLGENFENCESNVPROC				   glGenFencesNV;
extern PFNGLDELETEFENCESNVPROC			   glDeleteFencesNV;
extern PFNGLSETFENCENVPROC				   glSetFenceNV;
extern PFNGLFINISHFENCENVPROC			   glFinishFenceNV;
#endif


extern PFNGLGENPROGRAMSNVPROC              glGenProgramsNV;
extern PFNGLDELETEPROGRAMSNVPROC           glDeleteProgramsNV;
extern PFNGLBINDPROGRAMNVPROC              glBindProgramNV;
extern PFNGLLOADPROGRAMNVPROC              glLoadProgramNV;
extern PFNGLTRACKMATRIXNVPROC              glTrackMatrixNV;
extern PFNGLPROGRAMPARAMETER4FVNVPROC      glProgramParameter4fvNV;
extern PFNGLGETPROGRAMPARAMETERFVNVPROC    glGetProgramParameterfvNV;
extern PFNGLGETPROGRAMPARAMETERDVNVPROC    glGetProgramParameterdvNV;

extern PFNGLGENPROGRAMSARBPROC                     glGenProgramsARB;
extern PFNGLBINDPROGRAMARBPROC                     glBindProgramARB;
extern PFNGLDELETEPROGRAMSARBPROC                  glDeleteProgramsARB;
extern PFNGLPROGRAMSTRINGARBPROC                   glProgramStringARB;
extern PFNGLPROGRAMENVPARAMETER4FARBPROC           glProgramEnvParameter4fARB;
extern PFNGLPROGRAMLOCALPARAMETER4FARBPROC         glProgramLocalParameter4fARB;
extern PFNGLPROGRAMLOCALPARAMETER4FVARBPROC        glProgramLocalParameter4fvARB;
extern PFNGLPROGRAMENVPARAMETER4DVARBPROC          glProgramEnvParameter4dvARB;
extern PFNGLPROGRAMLOCALPARAMETER4DVARBPROC        glProgramLocalParameter4dvARB;

extern PFNGLCOMBINERPARAMETERFVNVPROC               glCombinerParameterfvNV;
extern PFNGLCOMBINERPARAMETERFNVPROC                glCombinerParameterfNV;
extern PFNGLCOMBINERPARAMETERIVNVPROC               glCombinerParameterivNV;
extern PFNGLCOMBINERPARAMETERINVPROC                glCombinerParameteriNV;
extern PFNGLCOMBINERINPUTNVPROC                     glCombinerInputNV;
extern PFNGLCOMBINEROUTPUTNVPROC                    glCombinerOutputNV;
extern PFNGLFINALCOMBINERINPUTNVPROC                glFinalCombinerInputNV;
extern PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC       glGetCombinerInputParameterfvNV;
extern PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC       glGetCombinerInputParameterivNV;
extern PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC      glGetCombinerOutputParameterfvNV;
extern PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC      glGetCombinerOutputParameterivNV;
extern PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC  glGetFinalCombinerInputParameterfvNV;
extern PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC  glGetFinalCombinerInputParameterivNV;
extern PFNGLCOMBINERSTAGEPARAMETERFVNVPROC          glCombinerStageParameterfvNV;
extern PFNGLGETCOMBINERSTAGEPARAMETERFVNVPROC       glGetCombinerStageParameterfvNV;

extern PFNGLACTIVESTENCILFACEEXTPROC       glActiveStencilFaceEXT;

extern PFNWGLALLOCATEMEMORYNVPROC           wglAllocateMemoryNV;
extern PFNWGLFREEMEMORYNVPROC           wglFreeMemoryNV;


extern PFNGLBINDBUFFERARBPROC glBindBufferARB;
extern PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB;
extern PFNGLGENBUFFERSARBPROC glGenBuffersARB;
extern PFNGLISBUFFERARBPROC glIsBufferARB;

/*typedef void (APIENTRY * PFNGLBUFFERDATAARBPROC) (GLenum target, GLsizeiptrARB size, const GLvoid *data, GLenum usage);
typedef void (APIENTRY * PFNGLBUFFERSUBDATAARBPROC) (GLenum target, GLintptrARB offset, GLsizeiptrARB size, const GLvoid *data);
typedef void (APIENTRY * PFNGLGETBUFFERSUBDATAARBPROC) (GLenum target, GLintptrARB offset, GLsizeiptrARB size, GLvoid *data);
typedef GLvoid* (APIENTRY * PFNGLMAPBUFFERARBPROC) (GLenum target, GLenum access);
typedef GLboolean (APIENTRY * PFNGLUNMAPBUFFERARBPROC) (GLenum target);
typedef void (APIENTRY * PFNGLGETBUFFERPARAMETERIVARBPROC) (GLenum target, GLenum pname, GLint *params);
typedef void (APIENTRY * PFNGLGETBUFFERPOINTERVARBPROC) (GLenum target, GLenum pname, GLvoid* *params);
*/



#ifndef GL_CLAMP_TO_BORDER_SGIS
#define GL_CLAMP_TO_BORDER_SGIS           0x812D
#endif

#ifndef  GL_TEXTURE_BINDING_3D
#define  GL_TEXTURE_BINDING_3D   0x806A 
#endif

#endif
