/**
 @file ASFModel.h
 @maintainer Morgan McGuire
 @cite Thanks to Jason Mallios and Chad Jenkins for help with the implementation.
 @cite http://www.motionrealityinc.com/software/asfamcspec.html
 */
#ifndef ASFModel_H
#define ASFModel_H

#include <G3DAll.h>
#include "../../ArticulatedModel/ArticulatedModel.h"

typedef ReferenceCountedPointer<class ASFModel> ASFModelRef;

class ASFModel : public ReferenceCountedObject {
public:

    class Bone;

    class Node {
    public:
        std::string         name;
        Node*               parent;
        Array<Bone*>        childArray;

        Node() : name("Node"), parent(NULL) {}

        virtual ~Node();
    };


    /** It is conventional in body dynamics to describe both the joint closer
        to the root and the bone itself in a bone structure, as is done here.*/
    class Bone : public Node {
    public:
        Vector3                 direction;

        Vector3                 axis;

        float                   length;

        int                     id;

        /** Rotation limits (as Euler angles).*/
        class Limit {
        public:
            Vector3 lo;
            Vector3 hi;

            /** 1 for components that can move, 0 if not free. */ 
            Vector3 free;

            inline Limit() : lo(-Vector3::inf()), hi(Vector3::inf()), free(0.0f, 0.0f, 0.0f) {}
        };

        Limit limit;

        inline Bone() : direction(0, 1, 0), axis(1, 0, 0), length(1), id(-1) {
            name = "Bone";
        }
    };


    class Pose {
    public:
        CoordinateFrame         root;

        Array<Vector3>          frame;

        /** out = this + alpha * (b - this);*/
        void lerp(const Pose& b, float alpha, Pose& out);
    };


    class Motion {
    public:
        Array<std::string>      boneName;
        Array<Pose>             pose;
    };

    Table<std::string, Motion>  motionTable;

    class Root : public Node {
    public:
        CoordinateFrame     cframe;

        Root() {
            name = "root";
        }
    };

    /** The bone heirarchy is mirrored inside the articulated model. */
    Root                        root;

    Table<std::string, Bone*>   boneTable;

    std::string                 name;

private:

    /**
     Reads until :<name> and return name.
     */
    static std::string readUntilASFBlockName(TextInput& text);

    static Vector3 readASFVec3(TextInput& text);

    static void readHeirarchy(TextInput& text, Table<std::string, Array<std::string> >& children);

    /** Order of components in ASF file */
    enum Order {RX = 0, RY, RZ, TX, TY, TZ};
    static void readOrder(TextInput& text, Array<Order>& order);

    static Bone* readBone(TextInput& text, float torad);

public:
    ArticulatedModelRef model;
private:

    ASFModel(const std::string& filename);

    /** Called from the constructor to set the boneTable and root.  Creates no geometry. */
    void loadBones(const std::string& filename);

    /** Called from the constructor to create the ArticulatedModel from the bone heirarchy. */
    void buildModel();

public:

    ~ASFModel();

    static ASFModelRef create(const std::string& filename);

    void pose(Array<PosedModelRef>& posedModels, const CoordinateFrame& cframe);

};





#endif
