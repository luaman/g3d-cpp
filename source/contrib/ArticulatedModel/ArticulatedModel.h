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

typedef ReferenceCountedPointer<class ArticulatedModel> ArticulatedModelRef;

/**
 A model composed of a heirarchy of rigid, textured sub-models.
 */
class ArticulatedModel : public ReferenceCountedObject {
public:

    class Pose {
    public:
        /** Mapping from names to coordinate frames (relative to parent).
            If a name is not present, its coordinate frame is assumed to
            be the identity.
         */
        Table<std::string, CoordinateFrame>     cframe;

        /** If false, the object's material is ignored during rendering. 
            Default is true. */
        bool                                    useMaterial;

        Pose() : useMaterial(true) {}
    };

    static const Pose DEFAULT_POSE;


    /** Beta API; subject to change in future releases.
        Illumination Equation:

        dst1 = underlying value in frame buffer
        evt = environment map
        ambUp, ambDn = ambient map up and down values (ideally, environment map convolved with a hemisphere)

        dst2 = dst1 * transmission + 
               evt[n] * reflection +
               lerp(ambDn, ambUp, n.y/2 + 0.5) * diffuse +
               emissive +
               SUM OVER LIGHTS {
                 light * (diffuse * NdotL +
                          specular * NdotH^specularExponent)}

        When choosing material properties, the transmission, diffuse, and specular terms
        should sum to less than 1.  The reflection and specular terms are technically the
        same value and should be equal; the difference is that specular only applies to
        lights and reflection to the environment (less lights), a concession to artistic
        control.

        Note that most translucent materials should be two-sided and have comparatively
        low diffuse terms.
        */
	class Material {
	public:

        /** Material property coefficients are specified as 
            a constant color times a texture map.  If the color
            is white the texture map entirely controls the result.
            If the color is black the term is disabled.  On graphics
            cards with few texture units the map will be ignored.*/
        class Component {
        public:
            Color3              constant;
            TextureRef          map;

            inline Component() : constant(Color3::black()), map(NULL) {}
            inline Component(const Color3& c) : constant(c), map(NULL) {}
            inline Component(double c) : constant(c,c,c), map(NULL) {}

            /** True if this material is definitely 0,0,0 everywhere */
            inline bool isBlack() const {
                return (constant.r == 0) && (constant.g == 0) && (constant.b == 0);
            }

            /** True if this material is definitely 1,1,1 everywhere */
            inline bool isWhite() const {
                return (constant.r == 1) && (constant.g == 1) && (constant.b == 1) && map.isNull();
            }
        };

        Component               diffuse;
        Component               emissive;
        Component               specular;
        Component               specularExponent;
        Component               transmission;
        Component               reflection;

        /** RGB*2-1 = tangent space normal, A = tangent space bump height.
          If NULL bump mapping is disabled. */
        TextureRef              normalMap;
        
        Material() : diffuse(1), emissive(0), 
            specular(0.25), specularExponent(60), 
            transmission(0), reflection(0) {
        }
	};

    class Part {
    public:

	    class TriList {
	    public:
		    Array<int>				indexArray;
		    Material				material;
    		RenderDevice::CullFace  cullFace;

            /** In the same space as the vertices */
            Sphere                  sphereBounds;
            Box                     boxBounds;

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
        VAR                         texCoord0VAR;

        MeshAlg::Geometry           geometry;
        Array<Vector2>              texCoordArray;
        Array<TriList>              triListArray;

        /** Indices into part array of sub-parts */
        Array<int>                  subPartArray;

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
    };

    friend class PosedArticulatedModel;


    /** Returns the index in partArray of the part with this name */
    Table<std::string, int>     partNameToIndex;
    Array<Part>                 partArray;

private:

    /** Called from the constructor */
    void init3DS(const std::string& filename, const Vector3& scale);

    /** Called from the constructor */
    void initIFS(const std::string& filename, const Vector3& scale);

public:

    std::string                 name;

    /** Appends one posed model per sub-part with geometry. */
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
