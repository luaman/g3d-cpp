/** @file ArticulatedModel.h
    @author Morgan McGuire morgan3d@sf.net

    This prototype model class will be included in the main API for future 
    versions of G3D.  It is currently in a beta state while we iron out some
    bugs.  E-mail morgan3d@sf.net if you have patches. 

  TODO: hierarchy
  TODO: smoothing groups
  TODO: proper shader
*/
#ifndef G3D_ARTICULATEDMODEL
#define G3D_ARTICULATEDMODEL

#include "SuperShader.h"


typedef ReferenceCountedPointer<class ArticulatedModel> ArticulatedModelRef;

/**
 A model composed of a heirarchy of rigid, textured sub-models.
 */
class ArticulatedModel : public ReferenceCountedObject {
private:

    friend class PosedArticulatedModel;

    /** Classification of a graphics card. 
        FIXED_FUNCTION  Use OpenGL fixed function lighting only.
        PS14            
        PS20            Use pixel shader 2.0 (full feature)
     */
    enum GraphicsProfile {
        UNKNOWN = 0,
        FIXED_FUNCTION,
        PS20};

    /** Measures the capabilities of this machine */
    inline static GraphicsProfile profile() {
        static GraphicsProfile p = UNKNOWN;

        if (p == UNKNOWN) {
            if (GLCaps::supports_GL_ARB_shader_objects()) {
                p = PS20;
            } else {
                p = FIXED_FUNCTION;
            }
p = FIXED_FUNCTION; // TODO: remove
        }

        return p;
    }

public:

    class Pose {
    public:
        /** Mapping from names to coordinate frames (relative to parent).
            If a name is not present, its coordinate frame is assumed to
            be the identity.
         */
        Table<std::string, CoordinateFrame>     cframe;

        Pose() {}
    };

    static const Pose DEFAULT_POSE;

    class Part {
    public:

	    class TriList {
	    public:
		    Array<int>				indexArray;
            SuperShader::Material	material;
    		RenderDevice::CullFace  cullFace;

            /** In the same space as the vertices */
            Sphere                  sphereBounds;
            Box                     boxBounds;

            SuperShaderRef          shader;

            TriList() : cullFace(RenderDevice::CULL_BACK) {}

            /** Recomputes the bounds */
            void computeBounds(const Part& parentPart);
	    };


        /** Each part must have a unique name */
        std::string                 name;

        /** Root position, relative to parent. Has already been applied to vertices!*/
        CoordinateFrame             cframe;

        CoordinateFrame             keyframe;

        VAR                         vertexVAR;
        VAR                         normalVAR;
        VAR                         tangentVAR;
        VAR                         texCoord0VAR;

        MeshAlg::Geometry           geometry;
        Array<Vector2>              texCoordArray;
        Array<Vector3>              tangentArray;
        Array<TriList>              triListArray;

        /** Indices into part array of sub-parts */
        Array<int>                  subPartArray;

        /** All faces.  Used for updateNormals and rendering without materials. */
        Array<int>                  indexArray;

        /**
         Does not restore rendering state when done.
         @param Net frame of parent.
         */
        void render(RenderDevice* rd, const CoordinateFrame& parent, const Pose& pose) const;

        void pose(
            ArticulatedModelRef     model,
            int                     partIndex,
            Array<PosedModelRef>&   posedArray,
            const CoordinateFrame&  parent, 
            const Pose&             posex) const;

        /** Some parts have no geometry because they are interior nodes in the hierarchy */
        inline bool hasGeometry() const {
            return geometry.vertexArray.size() > 0;
        }

        /** When geometry.vertexArray has been changed, invoke to recompute
            geometry.normalArray and the tangent array. */
        void updateNormals();

        /** When geometry or texCoordArray is changed, invoke to update
            (or allocate for the first time) the VAR data.*/
        void updateVAR();

        /** Invoke when the trilist materials have changed to recompute the shaders. */
        void updateShaders();
    };

    friend class PosedArticulatedModel;

    /** Returns the index in partArray of the part with this name */
    Table<std::string, int>     partNameToIndex;
    Array<Part>                 partArray;

    /** Update normals, var, and shaders on all Parts.  If you modify Parts explicitly,
        invoke this afterward to update dependent state. (slow)*/
    void updateAll();

private:

    /** Called from the constructor */
    void init3DS(const std::string& filename, const Vector3& scale);

    /** Called from the constructor */
    void initIFS(const std::string& filename, const Vector3& scale);

public:

    std::string                 name;

    /** Appends one posed model per sub-part with geometry.

        If the lighting environment is NULL the system will
        default using to whatever fixed function state is enabled
        (e.g., with renderDevice->setLight).  If the lighting
        environment is specified, the SuperShader will be used,
        providing detailed illuminaton.
    */
    void pose(
        Array<PosedModelRef>&   posedModelArray, 
        const CoordinateFrame&  cframe = CoordinateFrame(),
        const Pose&             pose = DEFAULT_POSE);

    /** 
      Supports 3DS, IFS, PLY2 file formats.  The format of a file is detected by the extension. 
      @param scale Transform all vertices by this scale factor on load
      */
    static ArticulatedModelRef fromFile(const std::string& filename, const Vector3& scale);

    static ArticulatedModelRef fromFile(const std::string& filename, float scale = 1.0) {
        return fromFile(filename, Vector3(scale, scale, scale));
    }
};

#endif G3D_ARTICULATEDMODEL
