/**
 @file MD2Model.cpp

 @maintainer Morgan McGuire, matrix@graphics3d.com
 @created 2003-08-07
 @edited  2003-09-11

 */

#if defined(G3D_WIN32) && defined(SSE)
    #include <xmmintrin.h>
#endif

#include "GLG3D/VARArea.h"
#include "GLG3D/MD2Model.h"
#include "GLG3D/VAR.h"

namespace G3D {

const int       MD2Model::Face::NONE             = -100;

MD2Model*       MD2Model::interpolatedModel      = NULL;
MD2Model::Pose  MD2Model::interpolatedPose;
VARArea*        MD2Model::varArea[MD2Model::NUM_VAR_AREAS];
int             MD2Model::nextVarArea            = MD2Model::NONE_ALLOCATED;
const GameTime  MD2Model::PRE_BLEND_TIME         = 1.0 / 8.0;
const double    MD2Model::hangTimePct            = 0.1;

const MD2Model::MD2AnimInfo MD2Model::animationTable[MD2Model::MAX_ANIMATIONS] = 
{
    // first, last, fps
    {   0,  39,  9, true },    // STAND
    {  40,  45, 10, true },    // RUN
    {  46,  53, 10, false },   // ATTACK
    {  54,  57,  7, false },   // PAIN_A
    {  58,  61,  7, false },   // PAIN_B
    {  62,  65,  7, false },   // PAIN_C
    {  66,  71,  7, false },   // JUMP_DOWN
    {  72,  83,  7, false },   // FLIP
    {  84,  94,  7, false },   // SALUTE
    {  95, 111, 10, false },   // FALLBACK
    { 112, 122,  7, false },   // WAVE
    { 123, 134,  6, false },   // POINT
    { 135, 153, 10, true },    // CROUCH_STAND
    { 154, 159,  7, true },    // CROUCH_WALK
    { 160, 168, 10, false },   // CROUCH_ATTACK
    { 169, 172,  7, false },   // CROUCH_PAIN
    { 173, 177,  5, false },   // CROUCH_DEATH
    { 178, 183,  7, false },   // DEATH_FALLBACK
    { 184, 189,  7, false },   // DEATH_FALLFORWARD
    { 190, 197,  7, false },   // DEATH_FALLBACKSLOW
    // JUMP is not in the table; it is handled specially
};


bool MD2Model::Pose::operator==(const MD2Model::Pose& other) const {
    return (animation == other.animation) && fuzzyEq(time, other.time);
}


void MD2Model::computeFrameNumbers(const MD2Model::Pose& pose, int& kf0, int& kf1, double& alpha) {

    if (pose.time < 0) {
        Animation a = pose.animation;
        
        if (pose.animation == JUMP) {
            a = JUMP_UP;
        } else {
        }

        debugAssert(iAbs(a) < MAX_ANIMATIONS);

        if (a >= 0) {
            kf1 = animationTable[a].first;
        } else {
            kf1 = animationTable[-a].last;
        }

        alpha = clamp(0, 1 + (pose.time / PRE_BLEND_TIME), 1);

        if ((pose.preFrameNumber >= 0) && (pose.preFrameNumber < 197)) {
            kf0 = pose.preFrameNumber;
        } else {
            // Illegal pose number; just hold the first frame
            kf0 = kf1;
        }

        return;
    }


    // Assume time is positive
    if (pose.animation == JUMP) {
        // Jump is special because it is two animations pasted together

        // Time to jump up (== time to jump down)
        GameTime upTime = animationLength(JUMP_UP);

        // Make the time on the right interval
        GameTime time = iWrap(pose.time * 1000, 1000 * upTime * (2 + hangTimePct)) / 1000.0;

        if (time < upTime) {
            // Jump up
            computeFrameNumbers(Pose(JUMP_UP, time), kf0, kf1, alpha);
        } else if (time < upTime * (1 + hangTimePct)) {
            // Hold at the peak
            computeFrameNumbers(Pose(JUMP_UP, upTime), kf0, kf1, alpha);
        } else {
            // Jump down
            computeFrameNumbers(Pose(JUMP_DOWN, time - upTime * (1 + hangTimePct)), kf0, kf1, alpha);
        }

        return;
    }

    Animation a = (Animation)iAbs(pose.animation);
    debugAssert(a < MAX_ANIMATIONS);

    // Figure out how far between frames we are
    const MD2AnimInfo& anim = animationTable[a];
    GameTime time = pose.time;
    GameTime len = animationLength(a);

    if (pose.animation < 0) {
        // Run the animation backwards
        time = len - time;
    }

    int totalFrames = anim.last - anim.first + 1;

    // Number of frames into the animation
    double frames = time * anim.fps;

    int iframes   = iFloor(frames);

    if (anim.loops) {
        kf0   = anim.first + iWrap(iframes, totalFrames);
        kf1   = anim.first + iWrap((iframes + 1), totalFrames);
        alpha = frames - iframes;
    } else {
        kf0   = anim.first + iMin(iframes, totalFrames - 1);
        kf1   = anim.first + iMin((iframes + 1), totalFrames - 1);
        alpha = frames - iframes;
    }
}


MD2Model::Pose MD2Model::choosePose(
    const MD2Model::Pose& currentPose,
    bool crouching,
    bool movingForward,
    bool movingBackward,
    bool attack,
    bool jump,
    bool flip,
    bool salute,
    bool fallback,
    bool wave,
    bool point,
    bool death1,
    bool death2,
    bool death3,
    bool pain1,
    bool pain2,
    bool pain3) const {

    Pose pose(currentPose);

    if (animationDeath(pose.animation)) {
        // Can't recover from a death pose
        return pose;
    }

    if (death1 || death2 || death3) {
        // Death interrupts anything
        pose.preFrameNumber = getFrameNumber(currentPose);
        pose.time           = -PRE_BLEND_TIME;
        if (crouching) {
            pose.animation  = CROUCH_DEATH;
        } else if (death1) {
            pose.animation  = DEATH_FALLBACK;
        } else if (death2) {
            pose.animation  = DEATH_FALLFORWARD;
        } else if (death3) {
            pose.animation  = DEATH_FALLBACKSLOW;
        }
        return pose;
    }

    if ((pain1 || pain2 || pain3) && ! animationPain(pose.animation)) {
        // Pain interrupts anything but death
        pose.preFrameNumber = getFrameNumber(currentPose);
        pose.time           = -PRE_BLEND_TIME;
        if (crouching) {
            pose.animation  = CROUCH_PAIN;
        } else if (pain1) {
            pose.animation  = PAIN_A;
        } else if (pain2) {
            pose.animation  = PAIN_B;
        } else if (pain3) {
            pose.animation  = PAIN_C;
        }
        return pose;
    }

    // End of an animation
    if (! animationLoops(pose.animation) &&
        (pose.time >= animationLength(pose.animation))) {
        pose.animation = STAND;
    }


    // Run
    if (movingForward) {
        if ((! animationRunForward(pose.animation)) && animationInterruptible(pose.animation)) {
            // Start running
            pose.animation = RUN;
        }
    } else if (movingBackward) {
        if ((! animationRunBackward(pose.animation)) && animationInterruptible(pose.animation)) {
            // Start running
            pose.animation = RUN_BACKWARD;
        }
    } else {
        if (animationRun(pose.animation)) {
            // Stop running
            pose.animation = STAND;
        }
    }

    if (animationInterruptible(pose.animation)) {

        if (attack) {
            pose.animation = ATTACK;
        } else if (jump && ! animationJump(pose.animation)) {
            pose.animation = JUMP;
        } else if (flip) {
            pose.animation = FLIP;
        } else if (salute) {
            pose.animation = SALUTE;
        } else if (fallback) {
            pose.animation = FALLBACK;
        } else if (wave) {
            pose.animation = WAVE;
        } else if (point) {
            pose.animation = POINT;
        }
    }

    // Crouch
    if (crouching) {
        // Move to a crouch if necessary
        switch (pose.animation) {
        case STAND:
            pose.animation = CROUCH_STAND;
            break;

        case RUN:
            pose.animation = CROUCH_WALK;
            break;

        case RUN_BACKWARD:
            pose.animation = CROUCH_WALK_BACKWARD;
            break;

        case ATTACK:
            pose.animation = CROUCH_ATTACK;
            break;

        default:;
        // We don't allow crouch during pain or other actions
        }
    } else {

        // Move to a crouch if necessary
        switch (pose.animation) {
        case CROUCH_STAND:
            pose.animation = STAND;
            break;

        case CROUCH_WALK:
            pose.animation = RUN;
            break;

        case CROUCH_WALK_BACKWARD:
            pose.animation = RUN_BACKWARD;
            break;

        case CROUCH_ATTACK:
            pose.animation = ATTACK;
            break;

        default:;
        // We don't allow standing up during pain or other actions
        }
    }

    // Blend in old animation
    if (pose.animation != currentPose.animation) {
        pose.preFrameNumber = getFrameNumber(currentPose);
        pose.time           = -PRE_BLEND_TIME;
    }
    return pose;
}


bool MD2Model::animationRun(Animation a) {
    return (iAbs(a) == RUN) || (iAbs(a) == CROUCH_WALK);
}


bool MD2Model::animationRunForward(Animation a) {
    return (a == RUN) || (a == CROUCH_WALK);
}


bool MD2Model::animationRunBackward(Animation a) {
    return (a == RUN_BACKWARD) || (a == CROUCH_WALK_BACKWARD);
}


bool MD2Model::animationStand(Animation a) {
    return (a == STAND) || (a == CROUCH_STAND);
}


bool MD2Model::animationAttack(Animation a) {
    return (a == ATTACK) || (a == CROUCH_ATTACK);
}


bool MD2Model::animationJump(Animation a) {
    return (a == JUMP) || (a == JUMP_UP) || (a == JUMP_DOWN);
}


bool MD2Model::animationInterruptible(Animation a) {
    return (! animationAttack(a)) && (! animationDeath(a)) && (! animationPain(a));
}


bool MD2Model::animationPain(Animation a) {
    return (a == CROUCH_PAIN) || ((a >= PAIN_A) && (a <= PAIN_C));
}


bool MD2Model::animationCrouch(Animation a) {
    return (a >= CROUCH_STAND) && (a <= CROUCH_DEATH);
}


bool MD2Model::animationDeath(Animation a) {
    return (a >= CROUCH_DEATH) && (a <= DEATH_FALLBACKSLOW);
}


bool MD2Model::animationLoops(Animation a) {
    if (a == JUMP) {
        return false;
    }

    a = (Animation)iAbs(a);
    debugAssert(a < MAX_ANIMATIONS);
    return animationTable[a].loops;
}


GameTime MD2Model::animationLength(Animation a) {
    if (a == JUMP) {
        return animationLength(JUMP_DOWN) * (2 + hangTimePct);
    }

    a = (Animation)iAbs(a);
    debugAssert(a < MAX_ANIMATIONS);

    const MD2AnimInfo& info = animationTable[a];

    if (info.loops) {
        return (info.last - info.first + 1) / (double) info.fps;
    } else {
        return (info.last - info.first) / (double) info.fps;
    }
}


int MD2Model::getFrameNumber(const Pose& pose) const {
    // Return the frame we're about to go to.
    int kf0, kf1;
    double alpha;
    computeFrameNumbers(pose, kf0, kf1, alpha);
    return kf1;
}


static const int maxVARVerts = 1600; 

void MD2Model::allocateVertexArrays(RenderDevice* renderDevice) {

    size_t size = maxVARVerts * (sizeof(Vector3) * 2 + sizeof(Vector2));

    if (renderDevice->freeVARSize() < size * NUM_VAR_AREAS) {
        // Not enough VAR memory (maybe none)
        return;
    }

    for (int i = 0; i < NUM_VAR_AREAS; ++i) {
        varArea[i] = renderDevice->createVARArea(size);
    }

    if (varArea[NUM_VAR_AREAS - 1] == NULL) {
        nextVarArea = NONE_ALLOCATED;
        Log::common()->println("\n*******\nCould not allocate vertex arrays.");
        return;
    }

    nextVarArea = 0;
}


void MD2Model::render(RenderDevice* renderDevice, const Pose& pose) {
    if (! initialized) {
        return;
    }

    bool tooBig = (maxVARVerts < keyFrame[0].vertexArray.size());
    bool useVAR = (nextVarArea != NONE_ALLOCATED) && ! tooBig;
   
    if (! useVAR && ! tooBig) {
        // Try to allocate some memory
        allocateVertexArrays(renderDevice);
        useVAR = (nextVarArea != NONE_ALLOCATED);
    }

    getGeometry(pose, interpolatedFrame);

    renderDevice->pushState();

        renderDevice->setShadeMode(RenderDevice::SHADE_SMOOTH);

        // Quake's triangles are backwards of OpenGL in our coordinate
        // system, so cull front faces instead of back faces.

        const Array<Vector3>& vertexArray   = interpolatedFrame.vertexArray;
        const Array<Vector3>& normalArray   = interpolatedFrame.normalArray;

        if (useVAR) {

            // Upload the arrays (System::memcpy is no faster than memcpy for VAR memory)
            varArea[nextVarArea]->reset();

            VAR varTexCoord(_texCoordArray, varArea[nextVarArea]);
            VAR varNormal  (normalArray,   varArea[nextVarArea]);
            VAR varVertex  (vertexArray,   varArea[nextVarArea]);

            renderDevice->setTextureMatrix(0, texFrame);

            renderDevice->beginIndexedPrimitives();
                renderDevice->setTexCoordArray(0, varTexCoord);
                renderDevice->setNormalArray(varNormal);
                renderDevice->setVertexArray(varVertex);
                renderDevice->sendIndices(RenderDevice::TRIANGLES, indexArray);
            renderDevice->endIndexedPrimitives();
            
            nextVarArea = (nextVarArea + 1) % NUM_VAR_AREAS;

        } else {

            // No VAR available; use the default rendering path

            renderDevice->setCullFace(RenderDevice::CULL_FRONT);
            for (int p = 0; p < primitiveArray.size(); ++p) {
    
                const Primitive&          primitive = primitiveArray[p];
                const int                 n         = primitive.pvertexArray.size();
                const Primitive::PVertex* pvertex   = primitive.pvertexArray.getCArray();

                renderDevice->beginPrimitive(primitive.type);

                    const Vector3* normal = normalArray.getCArray();
                    const Vector3* vertex = vertexArray.getCArray();

                    for (int i = 0; i < n; ++i) {
                        const int v = pvertex[i].index;
                        renderDevice->setTexCoord(0, pvertex[i].texCoord);
                        renderDevice->setNormal(normal[v]);
                        renderDevice->sendVertex(vertex[v]);
                    }
                renderDevice->endPrimitive();
            }
        }

    renderDevice->popState();
    
}


void MD2Model::debugRenderWireframe(RenderDevice* renderDevice, const Pose& pose) {
    if (! initialized) {
        return;
    }
    getGeometry(pose, interpolatedFrame);

    renderDevice->pushState();
        renderDevice->setDepthTest(RenderDevice::DEPTH_LEQUAL);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        renderDevice->setPolygonOffset(-.1);
        renderDevice->setColor(Color3::BLACK);
        
        renderDevice->beginPrimitive(RenderDevice::TRIANGLES);
        for (int i = 0; i < indexArray.size(); ++i) {
            renderDevice->sendVertex(interpolatedFrame.vertexArray[indexArray[i]]);
        }
        renderDevice->endPrimitive();

        renderDevice->setPolygonOffset(0.0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    renderDevice->popState();
}


void MD2Model::debugRenderNormals(RenderDevice* renderDevice, const Pose& pose) {
    if (! initialized) {
        return;
    }
    getGeometry(pose, interpolatedFrame);

    renderDevice->pushState();
        renderDevice->setLineWidth(1.5);
        renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);
        renderDevice->setColor(Color3::RED);
        
        renderDevice->beginPrimitive(RenderDevice::LINES);
            for (int v = 0; v < interpolatedFrame.vertexArray.size(); ++v) {
                renderDevice->sendVertex(interpolatedFrame.vertexArray[v]);
                renderDevice->sendVertex(interpolatedFrame.vertexArray[v] + interpolatedFrame.normalArray[v] * .3);
            }
        renderDevice->endPrimitive();
    renderDevice->popState();
}


const Sphere& MD2Model::boundingSphere(Animation a) const {
    if (! initialized) {
        static Sphere s(Vector3::ZERO, 0);
        return s;
    }
    return animationBoundingSphere[(Animation)iAbs(a)];
}


const Box& MD2Model::boundingBox(Animation a) const {
    if (! initialized) {
        static Box b(Vector3::ZERO, Vector3::ZERO);
        return b;
    }
    return animationBoundingBox[(Animation)iAbs(a)];
}


const Sphere& MD2Model::boundingSphere() const {
    if (! initialized) {
        static Sphere s(Vector3::ZERO, 0);
        return s;
    }
    return _boundingSphere;
}


const Box& MD2Model::boundingBox() const {
    if (! initialized) {
        static Box b(Vector3::ZERO, Vector3::ZERO);
        return b;
    }
    return _boundingBox;
}


const Array<MD2Model::Face>& MD2Model::faces() const {
    return faceArray;
}


const Array<MD2Model::Edge>& MD2Model::geometricEdges() const {
    return edgeArray;
}

const Array< Array<int> >& MD2Model::adjacentFaces() const {
    return adjacentFaceArray;
}


size_t MD2Model::mainMemorySize() const {

    size_t frameSize   = keyFrame.size() * (sizeof(PackedGeometry)  + (sizeof(Vector3) + sizeof(uint8)) * keyFrame[0].vertexArray.size());
    size_t indexSize   = indexArray.size() * sizeof(int);
    size_t faceSize    = faceArray.size() * sizeof(Face);
    size_t texSize     = _texCoordArray.size() * sizeof(Vector2int16);
    size_t valentSize  = adjacentFaceArray.size() * sizeof(Array<int>);
    for (int i = 0; i < adjacentFaceArray.size(); ++i) {
        valentSize += adjacentFaceArray[i].size() * sizeof(int);
    }

    size_t primitiveSize  = primitiveArray.size() * sizeof(Primitive);
    for (int i = 0; i < primitiveArray.size(); ++i) {
        primitiveSize += primitiveArray[i].pvertexArray.size() * sizeof(Primitive::PVertex);
    }

    size_t edgeSize    = edgeArray.size() * sizeof(Edge);

    return sizeof(MD2Model) + frameSize + indexSize + faceSize + valentSize + primitiveSize + texSize;
}


MD2Model::Geometry MD2Model::interpolatedFrame;

#ifdef G3D_WIN32
    // Supress warnings about mutating ebx and ebp; we 
    // push and pop them during the asm code of getGeometry
    // to get 2 extra registers
    #pragma warning( push )
    #pragma warning( disable : 4731 )
#endif

void MD2Model::getGeometry(const Pose& pose, Geometry& out) const {
    if (! initialized) {
        return;
    }
    
    const int numVertices = keyFrame[0].vertexArray.size();

    out.vertexArray.resize(numVertices, DONT_SHRINK_UNDERLYING_ARRAY);
    out.normalArray.resize(numVertices, DONT_SHRINK_UNDERLYING_ARRAY);

    if ((interpolatedModel == this) && (pose == interpolatedPose)) {
        // We're being asked to recompute a pose we have cached.
        if (&out != &interpolatedFrame) {
            // Copy from the cache
            System::memcpy(out.vertexArray.getCArray(), interpolatedFrame.vertexArray.getCArray(), sizeof(Vector3) * interpolatedFrame.vertexArray.size());
            System::memcpy(out.normalArray.getCArray(), interpolatedFrame.normalArray.getCArray(), sizeof(Vector3) * interpolatedFrame.vertexArray.size());
        }

        return;
    }

    if (&out == &interpolatedFrame) {
        // Make a note about what the cache contains
        interpolatedPose  = pose;
        interpolatedModel = const_cast<MD2Model*>(this);
    }

    double alpha;
    int i0, i1;

    computeFrameNumbers(pose, i0, i1, alpha);

    if ((i0 >= keyFrame.size()) || (i1 >= keyFrame.size())) {
        // This animation is not supported by this model.
        i0 = 0;
        i1 = 0;
        alpha = 0;
    }

    const PackedGeometry& frame0 = keyFrame[i0];
    const PackedGeometry& frame1 = keyFrame[i1];

    // To use SSE both the compiler and the processor must support it
    #if defined(SSE) && defined(G3D_WIN32)
        bool useSSE = System::hasSSE();
    #else
        bool useSSE = false;
    #endif

    if (! useSSE) {
        const Vector3*  v0 = frame0.vertexArray.getCArray();
        const Vector3*  v1 = frame1.vertexArray.getCArray();
    
        const uint8*    n0 = frame0.normalArray.getCArray();
        const uint8*    n1 = frame1.normalArray.getCArray();

        Vector3*        vI = out.vertexArray.getCArray();
        Vector3*        nI = out.normalArray.getCArray();

        for (int v = numVertices - 1; v >= 0; --v) {
            vI[v] = v0[v].lerp(alpha, v1[v]);
            nI[v] = normalTable[n0[v]].lerp(alpha, normalTable[n1[v]]);
        }

        return;
    }


    // Put this code inside an ifdef because it won't compile
    // on non-SSE compilers otherwise.
    #if defined(SSE) && defined(G3D_WIN32)
        // The high performance path interpolates between two arrays of floats.
        // We first convert the vertices and normals into this format.

        // n0 and v0 are first moved to the interpolated frame then
        // lerped in place

        const int numFloats = numVertices * 3;
    
        static Array<Vector3> normal1;

        normal1.resize(numVertices, DONT_SHRINK_UNDERLYING_ARRAY);

        {
            // Expand the normals out
            const uint8* n0Index = frame0.normalArray.getCArray();
            const uint8* n1Index = frame1.normalArray.getCArray();

            Vector3* n0Vector = out.normalArray.getCArray();
            Vector3* n1Vector = normal1.getCArray();

            for (int i = numVertices - 1; i >= 0; --i) {
                n0Vector[i] = normalTable[n0Index[i]];
                n1Vector[i] = normalTable[n1Index[i]];
            }
        }

        const float* v0 = reinterpret_cast<const float*>(frame0.vertexArray.getCArray());
        const float* v1 = reinterpret_cast<const float*>(frame1.vertexArray.getCArray());

        const float* n1 = reinterpret_cast<const float*>(normal1.getCArray());

        // Interpolated values
        float* vI = reinterpret_cast<float*>(out.vertexArray.getCArray());
        float* nI = reinterpret_cast<float*>(out.normalArray.getCArray());

        // Copy over vertices so we can mutate them in place
        System::memcpy(vI, v0, numFloats * sizeof(float));

        alwaysAssertM(((int)v1 % 16) == 0, "SEE array not aligned to 16-bytes");
        alwaysAssertM(((int)vI % 16) == 0, "SEE array not aligned to 16-bytes");
        alwaysAssertM(((int)n1 % 16) == 0, "SEE array not aligned to 16-bytes");
        alwaysAssertM(((int)nI % 16) == 0, "SEE array not aligned to 16-bytes");

        const int num128 = numFloats / sizeof(float);

        debugAssert(frame1.vertexArray.size() * 3 == numFloats);
        debugAssert(normal1.size() * 3 == numFloats);

        // Spread alpha across a vector
        const __m128 alpha128 = _mm_set_ps(alpha, alpha, alpha, alpha);

        // Our goal:
        //   vI = vI + (v1 - vI) * alpha
        //   nI = nI + (n1 - nI) * alpha
        __asm {
            mov     ecx,  num128
            movaps  xmm7, alpha128

            // Set up source and destination registers
            mov     edi, vI
            mov     esi, v1

            mov     edx, nI
            mov     eax, n1

        beginLoop:
            // Load vI and nI
            movaps  xmm0, [edi]
            movaps  xmm2, [edx]

            // Load v1 and n1
            movaps  xmm1, [esi]
            movaps  xmm3, [eax]

            // Compute (v1 - v0) and (n1 - n0)
            subps   xmm1, xmm0
            subps   xmm3, xmm2

            // Multiply each by alpha
            mulps   xmm1, xmm7
            mulps   xmm3, xmm7

            // Add the totals to v0 and n0
            addps   xmm0, xmm1
            addps   xmm2, xmm3

            // Store the results back in vI and nI
            movaps  [edi], xmm0
            movaps  [edx], xmm2

            // Increment all pointers
            add     edi, 16
            add     esi, 16
            add     edx, 16
            add     eax, 16

            dec     ecx
            jnz     beginLoop
        }
       
        // The last few floats may have been missed by the previous loop.
        for (int i = num128 * 4; i < numFloats; ++i) {
            vI[i] = vI[i] + (v1[i] - vI[i]) * alpha;
            nI[i] = nI[i] + (n1[i] - nI[i]) * alpha;
        }

    #endif
}

#ifdef G3D_WIN32
    #pragma warning( pop )
#endif

}