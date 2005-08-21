#include "ModelData.h"

ModelData::ModelData() {}

ModelData::~ModelData() {}

ModelData* ModelData::create(const ArticulatedModelRef& model) {

    ModelData* m = new ModelData();

    m->type     = ARTICULATED;
    m->artmodel = model;

    return m;
}


ModelData* ModelData::create(const ASFModelRef& model) {

    ModelData* m = new ModelData();

    m->type      = ASF;
    m->asfmodel  = model;

    return m;
}


ModelData* ModelData::create(const MD2ModelRef& model) {

    ModelData* m = new ModelData();

    m->type      = MD2;
    m->md2model  = model;

    return m;
}


static GFontRef font() {
    static GFontRef f = GFont::fromFile(NULL, "dominant.fnt");
    return f;
}


static void drawLabels(RenderDevice* rd, const CoordinateFrame& cframe,  const ArticulatedModelRef& m, const ArticulatedModel::Part& part) {

    font()->draw3D(rd, part.name, cframe, 0.05);
    
    for (int i = 0; i < part.subPartArray.size(); ++i) {
        int p = part.subPartArray[i];
        CoordinateFrame cframe2 = cframe * m->partArray[p].cframe;
        drawLabels(rd, cframe2, m, m->partArray[p]);
    }
}


static void drawLabels(RenderDevice* rd, const CoordinateFrame& cframe, const ArticulatedModelRef& m) {
    for (int p = 0; p < m->partArray.size(); ++p) {
        const ArticulatedModel::Part& part = m->partArray[p];
        if (part.parent == -1) {
            // This is a root part, pose it
            drawLabels(rd, cframe, m, part);
        }
    }

}


void ModelData::drawLabels(RenderDevice* rd, const CoordinateFrame& cframe) const {
    switch (type) {
    case ARTICULATED:
        font()->draw3D(rd, artmodel->name, cframe);
        ::drawLabels(rd, cframe, artmodel);
        break;

    case ASF:
//        font()->draw3D(rd, asfmodel->name, cframe);
        ::drawLabels(rd, cframe, asfmodel->model);
        break;

    case MD2:
        font()->draw3D(rd, md2model->name(), cframe);
        break;

    default:
        debugAssertM(false, "Unsupported model type");
    }
}


void ModelData::pose(Array<PosedModelRef>& posedModels, const CoordinateFrame& cframe) {

    switch (type) {
    case ARTICULATED:
        artmodel->pose(posedModels, cframe, artpose);
        break;

    case ASF:
        asfmodel->pose(posedModels, cframe);
        break;

    case MD2:
        posedModels.append(md2model->pose(cframe, md2pose, md2material));
        break;

    default:
        debugAssertM(false, "Unsupported model type");
    }
}
