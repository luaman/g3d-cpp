/**
 @file IFSBuilder/MD2.h

 Quake MD2 file structure.

 http://www.ugrad.cs.jhu.edu/~dansch/md2/

 @maintainer Morgan McGuire, morgan@cs.brown.edu
 @created 2003-02-21
 @edited  2003-02-27
 */

#ifndef MD2_H
#define MD2_H

// Prevent the compiler from adding extra space to the MD2 headers
#pragma pack(push, MD2)
#pragma pack(1)
struct MD2ModelHeader {
   int magic; 
   int version; 
   int skinWidth; 
   int skinHeight; 
   int frameSize; 
   int numSkins; 
   int numVertices; 
   int numTexCoords; 
   int numTriangles; 
   int numGlCommands; 
   int numFrames; 
   int offsetSkins; 
   int offsetTexCoords; 
   int offsetTriangles; 
   int offsetFrames; 
   int offsetGlCommands; 
   int offsetEnd; 
};

// Unused in this program
struct MD2Vertex {
   byte vertex[3];
   byte lightNormalIndex;
};
  

struct MD2Frame {
   float scale[3];
   float translate[3];
   char name[16];
   //triangleVertex_t vertices[1];
};


// Unused in this program
struct MD2Triangle {
   short vertexIndices[3];
   short textureIndices[3];
};

#pragma pack(pop, MD2)

#endif
