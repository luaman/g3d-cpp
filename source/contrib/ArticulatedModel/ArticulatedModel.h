/** @file ArticulatedModel.h
    @author Morgan McGuire morgan3d@sf.net

    This prototype model class will be included in the main API for future 
    versions of G3D.  It is currently in a beta state while we iron out some
    bugs.  E-mail morgan3d@sf.net if you have patches. 
*/
#ifndef G3D_ARTICULATEDMODEL
#define G3D_ARTICULATEDMODEL

#include "Load3DS.h"

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
    };

    static const Pose DEFAULT_POSE;

private:

    Load3DS                     load;

    class Part {
    public:

        /** Each part must have a unique name */
        std::string             name;

        /** Root position, relative to parent. Has already been applied to vertices!*/
        CoordinateFrame         cframe;

        CoordinateFrame         keyframe;

        Array<Vector3>          vertexArray;
        Array<Vector2>          texCoordArray;
        Array<int>              indexArray;

        /** Indices into part array of sub-parts */
        Array<int>              subPartArray;

        TextureRef              texture1;

        /**
         Does not restore rendering state when done.
         @param Net frame of parent.
         */
        void render(RenderDevice* rd, const CoordinateFrame& parent, const Pose& pose) const;
    };

    /** Returns the index in partArray of the part with this name */
    Table<std::string, int>     partNameToIndex;
    Array<Part>                 partArray;

    /** Called from the constructor */
    void init(const std::string& filename);

public:

    PosedModelRef pose();

    void render(RenderDevice* rd, const Pose& pose = DEFAULT_POSE);

    /**  Supports 3DS, IFS, PLY file formats, detected by the extension. */
    static ArticulatedModelRef fromFile(const std::string& filename);
};

#endif G3D_ARTICULATEDMODEL
