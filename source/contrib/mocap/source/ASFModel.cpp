/**
 @file ASFModel.cpp
 @maintainer Morgan McGuire
 @cite Thanks to Jason Mallios and Chad Jenkins for help with the implementation.
 */
#include "ASFModel.h"
#include "AMUtil.h"


ASFModel::Node::~Node() {
    childArray.deleteAll();
    childArray.clear();
}


std::string ASFModel::readUntilASFBlockName(TextInput& text) {
    Token t = text.read();
    while ((t.type() != Token::SYMBOL) ||
           (t.string() != ":")) {
	    t = text.read();
    }

    return text.readSymbol();
}


Vector3 ASFModel::readASFVec3(TextInput& text) {
    Vector3 v;
    for (int i = 0; i < 3; ++i) {
        v[i] = text.readNumber();
    }

    return v;
}


void ASFModel::readOrder(TextInput& text, Array<Order>& order) {

    text.readSymbol("order");

    order.resize(6);

    for (int i = 0; i < 6; ++i) {
        std::string s = text.readSymbol();

        debugAssert(s.length() == 2);

        // There are two characters.  The first is R or T, which
        // we map to 0 and 3.  The second is X, Y, or Z, which
        // we map to 0, 1, or 2.  The sum of the two mappings
        // is the enum value.  Could have used a Table instead; this
        // is an elegant alternative.

        order[i] = (Order)(((s[0] - 'T') * 3 / 2) + (s[1] - 'X'));
    }
}


void ASFModel::readHeirarchy(TextInput& text, Table<std::string, Array<std::string> >& 
children) {
    std::string blockName = ASFModel::readUntilASFBlockName(text);        
    debugAssert(blockName == "hierarchy");

    text.readSymbol("begin");

    // Maps parent name to direct child names
    Array<std::string> list;

    Token t = text.peek();
    debugAssert(t.type() == Token::SYMBOL);
    std::string parent = t.string();

    while (parent != "end") {
        list.fastClear();

        // Consume the parent token
        text.read();
        int line = t.line();

        // Read until the end of the line
        while (text.peekLineNumber() == line) {
            list.append(text.readSymbol());
        }

        children.set(parent, list);

        // Read the next token
        t = text.peek();
        debugAssert(t.type() == Token::SYMBOL);
        parent = t.string();
    }
}


ASFModel::Bone* ASFModel::readBone(TextInput& text, float torad) {
    text.readSymbol("begin");
    std::string s = text.readSymbol();

    ASFModel::Bone* bone = new Bone();

    while (s != "end") {
        if (s == "id") {

            bone->id = text.readNumber();

        } else if (s == "name") {

            bone->name = text.readSymbol();
             
        } else if (s == "direction") {

            bone->direction = ASFModel::readASFVec3(text).direction();

        } else if (s == "length") {

            bone->length = text.readNumber() / 14;
            debugAssert(bone->length > 0);

        } else if (s == "axis") {

            bone->axis = ASFModel::readASFVec3(text);
            text.readSymbol();

        } else if (s == "dof") {

            // Consume the dof data
            Token t = text.read();
            while ((t.type() == Token::SYMBOL) && 
                   ((t.string()[0] == 'r') ||
                    (t.string()[0] == 't')) &&
                   ((t.string()[1] >= 'x') &&
                   (t.string()[1] <= 'z'))) {

                debugAssertM(t.string()[0] == 'r', "ASFModel only support rotations joints");
                bone->limit.free[t.string()[1] - 'x'] = 1.0;

                t = text.read();
            }

            // Put back the extra token we just read
            text.push(t);

        } else if (s == "limits") {
            
            Token t = text.read();
            while ((t.type() == Token::SYMBOL) && (t.string() == "(")) {
                float lo = text.readNumber();
                float hi = text.readNumber();

                text.readSymbol(")");

                t = text.read();
            }
            text.push(t);
        
        } else {
            debugAssertM(false, std::string("Unknown symbol: '") + s + "'");
        }

        s = text.readSymbol();

    }

    return bone;
}


ASFModelRef ASFModel::create(const std::string& filename) {
    return new ASFModel(filename);
}


ASFModel::ASFModel(const std::string& filename) {
    loadBones(filename);
    buildModel();
}


void ASFModel::loadBones(const std::string& filename) {
    TextInput::Options options;
    options.cppComments = false;
    options.otherCommentCharacter = '#';

	TextInput text(filename, options);

    // File consists of blocks preceeded by colons.

    // Multiply by toRad to convert the file's angle into radians
    double torad = 1.0;

    // Order of transformation parameters in the file
    Array<ASFModel::Order> order;

    std::string blockName;

    ////////////////////////////////////////////////////////////////
    blockName = ASFModel::readUntilASFBlockName(text);
    debugAssert(blockName == "version");
    float version = text.readNumber();


    ////////////////////////////////////////////////////////////////
    blockName = ASFModel::readUntilASFBlockName(text);
    debugAssert(blockName == "name");
    name = text.readSymbol();


    ////////////////////////////////////////////////////////////////
    blockName = ASFModel::readUntilASFBlockName(text);
    debugAssert(blockName == "units");
    text.readSymbol("mass");
    text.readNumber();

    text.readSymbol("length");
    text.readNumber();

    std::string angleUnits = text.readSymbol();
    if (angleUnits == "deg") {
        torad = G3D_PI / 180.0;
    }

    ////////////////////////////////////////////////////////////////
    blockName = ASFModel::readUntilASFBlockName(text);
    debugAssert(blockName == "documentation");
    // Skip the documentation chunk.


    ////////////////////////////////////////////////////////////////
    blockName = ASFModel::readUntilASFBlockName(text);        
    debugAssert(blockName == "root");

    ASFModel::readOrder(text, order);
    
    text.readSymbol("axis");
    std::string axis = text.readSymbol();

    text.readSymbol("position");
    
    Vector3 position = ASFModel::readASFVec3(text);

    text.readSymbol("orientation");
    Vector3 orientation = ASFModel::readASFVec3(text) * torad;

    root.cframe =
        (Matrix3::fromEulerAnglesZYX(orientation.z, orientation.y, orientation.x), 
         position);


    ////////////////////////////////////////////////////////////////
    blockName = ASFModel::readUntilASFBlockName(text);        
    debugAssert(blockName == "bonedata");

    Token t = text.peek();
    while ((t.type() == Token::SYMBOL) && (t.string() == "begin")) {
        Bone* b = ASFModel::readBone(text, torad);
        boneTable.set(b->name, b);
        t = text.peek();
    }

    ////////////////////////////////////////////////////////////////
    Table<std::string, Array<std::string> > children;

    ASFModel::readHeirarchy(text, children);

    // Assemble the bones into a heirarchy
    typedef Table<std::string, Array<std::string> >::Iterator It;
    It cur = children.begin();
    const It end = children.end();

    while (cur != end) {
        const std::string& parentName       = cur->key;
        const Array<std::string>& childName = cur->value;

        Node* parent;

        if (parentName == "root") {
            parent = &root;
        } else {
            parent = boneTable[parentName];
        }

        parent->childArray.resize(childName.size());
        for (int i = 0; i < childName.size(); ++i) {
            Bone* bone = boneTable[childName[i]];
            parent->childArray[i] = bone;
            bone->parent = parent;
        }

        ++cur;
    }
}


void ASFModel::pose(Array<PosedModelRef>& posedModels, const CoordinateFrame& cframe) {
    model->pose(posedModels, cframe);
}


void ASFModel::buildModel() {

    model = ArticulatedModel::createEmpty();    
    model->name = name;

    typedef Table<std::string, Bone* >::Iterator It;
    It       cur = boneTable.begin();
    const It end = boneTable.end();

    SuperShader::Material material;
    material.emit.constant = Color3::black();
    material.diffuse.constant = Color3::orange();
    material.specular.constant = Color3::yellow();

    while (cur != end) {
        const Bone* bone = cur->value;

        ArticulatedModel::Part& part = model->partArray.next();        
        part.name = cur->key;
        model->partNameToIndex.set(part.name, model->partArray.size() - 1);

        ++cur;
    }

    // Iterate through again to connect up the heirarchy.    
    for (cur = boneTable.begin(); cur != end; ++cur) {
        const Bone* bone = cur->value;
        int i = model->partNameToIndex[cur->key];
        ArticulatedModel::Part& part = model->partArray[i];

        if ((bone->parent == NULL) || (bone->parent->name == "root")) {
            part.parent = -1;
        } else {
            // This node has a parent
            int p = model->partNameToIndex[bone->parent->name];
            ArticulatedModel::Part& parent = model->partArray[p];
            Bone* parentBone = dynamic_cast<Bone*>(bone->parent);

            // Record the parent
            part.parent = p;
            // Record this child
            parent.subPartArray.append(i);

            part.cframe = CoordinateFrame(
                parentBone->direction * parentBone->length);
        }

        conoid(Vector3(0, 0, bone->length), Vector2(0.01, 0.01), 
               Vector3(0, 0, 0), Vector2(.03, .03), 
               bone->direction,
               material,
               part);
    }

    model->updateAll();
}


ASFModel::~ASFModel() {
}
