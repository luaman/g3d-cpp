/**
  @file demos/main.cpp

  This is a sample main.cpp to get you started with G3D.  It is
  designed to make writing an application easy.  Although the
  GApp/GApplet infrastructure is helpful for most projects,
  you are not restricted to using it-- choose the level of
  support that is best for your project (see the G3D Map in the
  documentation).

  @author Morgan McGuire, matrix@graphics3d.com
 */

#include <G3DAll.h>

#if G3D_VER != 60200
    #error Requires G3D 6.02
#endif


/**
   Discrete Curvature Operators for triangular meshes.

   @cite  Based on Meyer, Desbrun, Schroder, and Barr "Discrete 
   Differential-Geometry Operators for Triangulated 2-Manifolds." 
   http://citeseer.nj.nec.com/meyer02discrete.html */
class Curvatures {
public:
    Curvatures() {}

    /**
      Takes an array of vertices, and an array of triangle indices to traverse them
      You may get both of these from a G3D::PosedModel.
      */
    Curvatures(const Array< Vector3 >& _vertexArray, 
                           const Array< int >& _indexArray);

    /** Returns the Principal Curvatures for each vertex in _vertexArray */
    void principalCurvature( Array< double >& k1, Array< double >& k2) const;

    /** Returns the Principal Direction for each vertex in _vertexArray */
    void principalDirections( Array< Vector3 >& T1, Array< Vector3 >& T2) const;
   
    void gaussianCurvature( Array< double >& curvatures ) const;

    void meanCurvature( Array< double >& curvatures ) const;

protected:

    MeshAlg::Geometry      geometry;
    Array< MeshAlg::Face > faces;
    Array< MeshAlg::Edge > edges;
    Array< Array< int > >  adjacentFaces;

    /** angle at [vertex][face] */
    Array< Table< int, float > > angleInner;

    /** angle at [adjacent vertex][face] */
    Array< Table< int, float > > angleLeft;  

    /** angle at [other adjacent vertex][face] */
    Array< Table< int, float > > angleRight; 

    /** cotan of angles opposite each segment [i][j] */
    Array< Table< int, double > > cota; 
    Array< Table< int, double > > cotb;

    /** Area of "Voronoi" region surrounding each vertex [i] */
    Array< double > AMixed;

    void initAngles();
    void initCotans();
    void initAMixed();
    void givens(float &cost, float &sint, float a, float b) const;
    void toString(Vector3 v) const;
    void toString(Matrix3 m) const;

};

/** Angle between two vectors */
inline float vectorAngle(const Vector3& a, const Vector3& b){
    double d = a.dot(b);
    double m = sqrt(a.squaredLength() * b.squaredLength());
    return acos(d / m);
}


Curvatures::Curvatures(
    const Array< Vector3 >& _vertexArray, 
    const Array< int >&     _indexArray) {

    // Compute Mesh Properties
    geometry.vertexArray = _vertexArray;
    MeshAlg::computeAdjacency(geometry.vertexArray, _indexArray,
             faces, edges, adjacentFaces);

    Array<Vector3> faceNormalArray;
    MeshAlg::computeNormals(geometry.vertexArray, faces,
            adjacentFaces, geometry.normalArray, faceNormalArray);

    // init tables
    initAngles();
    initCotans();
    initAMixed();
}


// For each vertex i, calculate the angle at i for each adjacent face
void Curvatures::initAngles() {
    angleInner.resize(geometry.vertexArray.size()); // init
    angleLeft.resize(geometry.vertexArray.size());
    angleRight.resize(geometry.vertexArray.size());
    Vector3 inner, left, right;
    int faceIdx;

    for(int i = 0; i < geometry.vertexArray.size(); ++i) {  // For each vertex
         for(int j = 0; j < adjacentFaces[i].size(); ++j) {// For each face
            faceIdx = adjacentFaces[i][j];
            //1 calculate cosine of angle at vertex
            
            //1.1 find vertex b
            int v;
            if(faces[faceIdx].vertexIndex[0] == i)      v = 0;
            else if(faces[faceIdx].vertexIndex[1] == i) v = 1;
            else                                        v = 2;

            //1.2 assign geometric vertices
            inner = geometry.vertexArray[faces[faceIdx].vertexIndex[v]];
            left  = geometry.vertexArray[faces[faceIdx].vertexIndex[(v+1)%3]];
            right = geometry.vertexArray[faces[faceIdx].vertexIndex[(v+2)%3]];


            //1.3 calculate cos of angle 
            angleInner[i].set(faceIdx, vectorAngle( left-inner, right-inner ));
            angleLeft[i].set( faceIdx, vectorAngle( inner-left, right-left ));
            angleRight[i].set(faceIdx, vectorAngle( inner-right, left-right ));
         }
    }
}


// For each vertex i and an adjacent vertex j, find the cotans of 
// the two angles (a and b) opposite edge ij.
void Curvatures::initCotans() {
    cota.resize(geometry.vertexArray.size());
    cotb.resize(geometry.vertexArray.size());

    int i, j, a, b;
    //Vector3 xi, xj, xa, xb;
    Array< MeshAlg::Face > aFaces;
    Array< int > aFacesIdx;

    // For each edge, calc cota/cotb
    for(int k = 0; k < edges.size(); ++k) { 

        i = edges[k].vertexIndex[0];
        j = edges[k].vertexIndex[1];
        aFaces.clear();
        aFacesIdx.clear();

        // Find faces that share this edge
        for(int f = 0; f < adjacentFaces[i].size(); ++f) {
            if( (faces[adjacentFaces[i][f]].vertexIndex[0] == j) ||
                (faces[adjacentFaces[i][f]].vertexIndex[1] == j) ||
                (faces[adjacentFaces[i][f]].vertexIndex[2] == j) ) {
                aFaces.append(faces[adjacentFaces[i][f]]);
                aFacesIdx.append(adjacentFaces[i][f]);
            }
        }

        debugAssertM(aFaces.size() >= 2, "initCotans: Fewer than 2 faces share an edge."); 
        // Find the points who's angle we need (ie not i and j)
        for(int v = 0; v < 3; ++v) {
            if( (aFaces[0].vertexIndex[v] !=i) && (aFaces[0].vertexIndex[v] != j) )
                a = aFaces[0].vertexIndex[v];
            if( (aFaces[1].vertexIndex[v] !=i) && (aFaces[1].vertexIndex[v] != j) )
                b = aFaces[1].vertexIndex[v];
        }

        // Find cota cotb
        if (angleInner[a].containsKey(aFacesIdx[0])) {
            cota[i].set(j, 1/tan(angleInner[a][aFacesIdx[0]]));
            cotb[j].set(i, 1/tan(angleInner[a][aFacesIdx[0]])); //TODO Don't edges go both ways?
        } else
            debugPrintf("\n1nokey kijab %d %d %d %d %d", k, i, j, a, b); 

        if (angleInner[b].containsKey(aFacesIdx[1])) {
            cotb[i].set(j, 1/tan(angleInner[b][aFacesIdx[1]]));
            cota[j].set(i, 1/tan(angleInner[b][aFacesIdx[1]]));  //TODO Don't edges go both ways?
        } else {
            debugPrintf("\n2nokey kijab %d %d %d %d %d", k, i, j, a, b);
        }
    }
}

// Area of the Voronoi region surrounding a vertex generalized to account
// for obtuse triangles.
void Curvatures::initAMixed() {
    Triangle tri;
    MeshAlg::Face face;
    int faceIdx;

    int v, left, right;

    // length of opposite sides
    float lenLeft, lenRight;

    // Init Amixed
    for (int i = 0; i < geometry.vertexArray.size(); ++i) {
        AMixed.append(0);
    }

    // For each vertex
    for (int i = 0; i < geometry.vertexArray.size(); ++i) {  
        
        // For each face
        for (int j = 0; j < adjacentFaces[i].size(); ++j) { 
            faceIdx = adjacentFaces[i][j];
            face = faces[faceIdx];

            //find vertex i
            if (face.vertexIndex[0] == i) {
                v = 0;
            } else if (face.vertexIndex[1] == i) {
                v = 1;
            } else {
                v = 2;
            }

            left  = (v + 1) % 3;
            right = (v + 2) % 3;

            // get geometry 
            tri = Triangle(geometry.vertexArray[face.vertexIndex[v]],
                           geometry.vertexArray[face.vertexIndex[left]],  //"Left"
                           geometry.vertexArray[face.vertexIndex[right]]); //"Right"

            if (fabs(angleInner[i][faceIdx]) > G3D_HALF_PI) {

                // Angle at i is obtuse
                AMixed[i] += tri.area() / 2.0;

            } else if ((fabs(angleLeft[ face.vertexIndex[left]][faceIdx]) > G3D_HALF_PI) || 
                       (fabs(angleRight[face.vertexIndex[right]][faceIdx]) > G3D_HALF_PI)) {   

                // The triangle is obtuse
                AMixed[i] += tri.area() / 4.0;

            } else { 
                
                // tri is not obtuse 
                lenLeft   = (tri.vertex(0)-tri.vertex(1)).squaredLength();
                lenRight  = (tri.vertex(0)-tri.vertex(2)).squaredLength();

                // Voronoi region
                AMixed[i] += 
                    0.125 * (lenLeft  * 1.0 / tan(angleLeft[face.vertexIndex[left]][faceIdx]) +   
                             lenRight * 1.0 / tan(angleRight[face.vertexIndex[right]][faceIdx]));
            }
        }
    }
}


// Gaussian curvature.
// (The product of the principal curvatures.)
void Curvatures::gaussianCurvature(Array< double >& curvatures) const {
    curvatures.clear();

    // For each vertex
    for (int i = 0; i < geometry.vertexArray.size(); ++i) {  
         double theta = 0.0;

         // For each face
         for (int j = 0; j < adjacentFaces[i].size(); ++j) {
            theta += fabs(angleInner[i][adjacentFaces[i][j]]);  
         }

         curvatures.append( (G3D_TWO_PI - theta) / AMixed[i] );  
    }
}

// Mean curvature at each point on a mesh.
// Mean curvature is the mean of the normal curvatures,
// and also the average of the principal curvatures.
void Curvatures::meanCurvature(Array< double >& curvatures) const {

    curvatures.clear();

    Set< int > ringSet;
    Array< int > ring;

    // Mean Curvature Normal
    Vector3 K; 

    // For each vertex
    for (int v = 0; v < geometry.vertexArray.size(); ++v) {  

         // Find N1 ring of vertices
         ringSet.clear();

         
         const Array<int>& neighborFaces = adjacentFaces[v];

         if (neighborFaces.size() > 0) {
             // For each face
             for (int f = neighborFaces.size() - 1; f >= 0; --f) {
                 const MeshAlg::Face& face = faces[neighborFaces[f]];

                 for (int i = 0; i < 3; ++i) {
                     ringSet.insert(face.vertexIndex[i]);  
                 }
             }

             // ringSet contains N1 ring only.  Remove the vertex itself.
             ringSet.remove(v);
         }

         ring = ringSet.getMembers();
         K = Vector3(0, 0, 0);
         
         // for each edge exiting v
         for(int j = 0; j < ring.size(); ++j) {
             // K(xi) = 1/(2*Amixed)*sum(cota+cotb)*(Xj-Xi)
             K += (cota[v][ring[j]] + cotb[v][ring[j]]) * 
                  (geometry.vertexArray[ring[j]] - geometry.vertexArray[v]);
         }
         K /= 2.0 * AMixed[v];
         curvatures.append( 0.5 * K.length() );

    }
}


void Curvatures::principalCurvature(Array< double >& k1, Array< double >& k2) const {

    // mean and gaussian curvature
    Array< double > kh, kg;
    gaussianCurvature(kg);
    meanCurvature(kh);

    // For each vertex
    for(int i = 0; i < geometry.vertexArray.size(); ++i) {  
       double dxi = kh[i] * kh[i] - kg[i];
       if (dxi < 0.0) {
            // threshold at 0
           dxi = 0.0;
       }
       
       double sdxi = sqrt(dxi);
       k1.append(kh[i] + sdxi);
       k2.append(kh[i] - sdxi);
    }
}


// principal directions based on Taubing G. "Estimating the Tensor of Curvature of a
// Surface from a Polyhedral Approximation. Proc. ICCV 1995.
// http://citeseer.nj.nec.com/taubin95estimating.html
void Curvatures::principalDirections( Array< Vector3 >& T1, Array< Vector3 >& T2) const {
    T1.clear();
    T2.clear();

    Set< int > ringSet;

    // 1-ring neighbors of vi
    Array< int > ring;        

    // vertex, neighbor, and vj-vi
    Vector3 vi, vj, dvji;     

    // Setup for Householder matrix
    Vector3 W;                

    // Householder matrix
    Matrix3 Q    = Matrix3::ZERO;
    Matrix3 M    = Matrix3::ZERO;
    Matrix3 QMQ  = Matrix3::ZERO;

    // Holds a vector for a N*N' that yields a 3x3 matrix
    Matrix3 NMat = Matrix3::ZERO; 

    // Holds a vector for a T*T' that yields a 3x3 matrix
    Matrix3 TMat = Matrix3::ZERO; 

    // Holds a vector for a W*W' that yields a 3x3 matrix
    Matrix3 WMat = Matrix3::ZERO; 

    // weight of k and T
    Array< float > weight;    

    // normal curvature in direction vj
    Array< float > k;   
    
    // normalized projection of vj-vi onto tangent plane
    Array< Vector3 > T; 
    float EminusN, EplusN;
    float cost, sint;
    float wSum;

    // For each vertex
    for (int i = 0; i < geometry.vertexArray.size(); ++i) {
        vi = geometry.vertexArray[i];

        // Find N1 ring of vertices
        ring.clear();
        ringSet.clear();

        // For each face
        for(int j = 0; j < adjacentFaces[i].size(); ++j) {
            // add all verts
            ringSet.insert(faces[adjacentFaces[i][j]].vertexIndex[0]);  
            ringSet.insert(faces[adjacentFaces[i][j]].vertexIndex[1]);
            ringSet.insert(faces[adjacentFaces[i][j]].vertexIndex[2]);
        }

        // ringSet contains N1 ring only
        ringSet.remove(i); 
        ring = ringSet.getMembers();

        // Calculate weights (proportional to inverse of |vi-vj|
        // (Hameiri'03 modification of Taubin)
        wSum = 0;
        weight.clear();
        for(int j = 0; j < ring.size(); ++j) {
            vj = geometry.vertexArray[ring[j]];
            weight.append((vi-vj).length());
            wSum += weight[j];
        }

        // Make weights sum to 1
        for(int j = 0; j < ring.size(); ++j) {
            weight[j] /= wSum;
        }
    
        // Calc unit length normalized projection of vj-vi onto the tangent plane
        // T_ij = normalize((I-N_vi*N_vi')(vi-vj))

        // for matrix rep of vertex normal
        NMat = Matrix3::ZERO;

        NMat.setColumn(0, geometry.normalArray[i]);
        T.clear();
        for (int j = 0; j < ring.size(); ++j) { 
            vj = geometry.vertexArray[ring[j]];
            T.append( (Matrix3::IDENTITY - NMat*NMat.transpose())*(vi-vj) );
            T[j] /= T[j].length();
        }

        // Calc normal curvature in direction of vi,vj
        k.clear();
        for (int j = 0; j < ring.size(); ++j) {
            dvji =  geometry.vertexArray[ring[j]] - vi;
            k.append((2*geometry.normalArray[i].dot(dvji))/(dvji.length()*dvji.length()));
        }

        // Estimate M as weighted sum over neighborhood
        M = Matrix3::ZERO;
        for (int j = 0; j < ring.size(); ++j) {
            TMat = Matrix3::ZERO;
            TMat.setColumn(0, T[j]);
            M = M + weight[j]*k[j]*(TMat*TMat.transpose()); 
        }

        // Calculate W for Householder matrix
        EminusN = (Vector3::UNIT_X - geometry.normalArray[i]).length();
        EplusN  = (Vector3::UNIT_X + geometry.normalArray[i]).length();

        if (EminusN > EplusN) {
            W = (Vector3::UNIT_X - geometry.normalArray[i]) / EminusN; 
        } else {
            W = (Vector3::UNIT_X + geometry.normalArray[i]) / EplusN; 
        }
        WMat = Matrix3::ZERO;
        WMat.setColumn(0, W);

        // Calc Householder matrix Q
        Q = Matrix3::IDENTITY - 2.0 * (WMat * WMat.transpose());
         
        // Q'MQ contain the upper bit of M?
        QMQ = Q.transpose() * M * Q;

        // Find theta using Givens rotation
        givens(cost, sint, QMQ[1][1], M[2][1] );

        // Principal directions (TODO: reversed?)
        T1.append( cost * Q.getColumn(1) - sint * Q.getColumn(2) );
        T2.append( sint * Q.getColumn(1) + cost * Q.getColumn(2) );
    }
}



// Givens rotation (based on Golub and Loan)
// solves [c s; -s c]'*[a b]' = [r 0]' for c and s 
void Curvatures::givens(float &cost, float &sint, float a, float b) const {
    double t;

    if (b == 0) {
        cost = 1.0;
        sint = 1.0;
    } else if (abs(b)>abs(a)) {
        t    = -a / b;
        sint = 1.0 / sqrt(1.0 + t * t);
        cost = sint*t;
    } else {
        t    = -b / a;
        cost = 1.0 / sqrt(1.0 + t * t);
        sint = cost*t;
    }
}


void Curvatures::toString(Vector3 v) const {
    debugPrintf("[%f %f %f]", v[0], v[1], v[2]);
}

void Curvatures::toString(Matrix3 m) const {
    debugPrintf("[%f %f %f; %f %f %f; %f %f %f]", m[0][0], m[0][1], m[0][2], 
                                                  m[1][0], m[1][1], m[1][2], 
                                                  m[2][0], m[2][1], m[2][2]);
}



////////////////////////////////////////////////////////////////////////







class App : public GApp {
protected:
    void main();
public:
    SkyRef              sky;

    App(const GAppSettings& settings);
};


class Mesh {
public:
    MeshAlg::Geometry           geometry;
    Array<int>                  indexArray;

    Array<MeshAlg::Face>        faceArray;
    Array<MeshAlg::Edge>        edgeArray;
    Array<MeshAlg::Vertex>      vertexArray;

    Array<MeshAlg::Face>        weldedFaceArray;
    Array<MeshAlg::Edge>        weldedEdgeArray;
    Array<MeshAlg::Vertex>      weldedVertexArray;

    Array<Vector3>              faceNormalArray;

    Sphere                      boundingSphere;
    Box                         boundingBox;

    int                         numBoundaryEdges;
    int                         numWeldedBoundaryEdges;

    std::string                 name;

    /** Per-vertex arrays.  On the range [0 (flat), inf (crease)] */
    Array<double>               gaussianCurvature;

    /** On the range [-inf to inf] */
    Array<double>               meanCurvature;

    enum RenderStyle {WHITE, GAUSSIAN, MEAN} renderStyle;

    Mesh() : renderStyle(WHITE) {}
    Mesh(const PosedModelRef& pm) : renderStyle(WHITE) {
        import(pm);
    }

    void import(const PosedModelRef& pm);

    void render(class App* app, RenderDevice* renderDevice);
};


/**
 This simple demo applet uses the debug mode as the regular
 rendering mode so you can fly around the scene.
 */
class Demo : public GApplet {
public:

    Mesh                mesh;

    // Add state that should be visible to this applet.
    // If you have multiple applets that need to share
    // state, put it in the App.

    class App*          app;

    Demo(App* app);    

    virtual void init();

    virtual void doLogic();

	virtual void doNetwork();

    virtual void doSimulation(SimTime dt);

    virtual void doGraphics();

    virtual void cleanup();

};



void Mesh::import(const PosedModelRef& pm) {
    geometry            = pm->objectSpaceGeometry();
    indexArray          = pm->triangleIndices();

    faceArray           = pm->faces();
    edgeArray           = pm->edges();
    vertexArray         = pm->vertices();

    weldedFaceArray     = pm->weldedFaces();
    weldedEdgeArray     = pm->weldedEdges();
    weldedVertexArray   = pm->weldedVertices();

    pm->getObjectSpaceFaceNormals(faceNormalArray);

    boundingSphere      = pm->objectSpaceBoundingSphere();
    boundingBox         = pm->objectSpaceBoundingBox();

    numBoundaryEdges    = pm->numBoundaryEdges();
    numWeldedBoundaryEdges = pm->numWeldedBoundaryEdges();

    name                = pm->name();

    Curvatures curvature(geometry.vertexArray, indexArray);
    curvature.gaussianCurvature(gaussianCurvature);
    curvature.meanCurvature(meanCurvature);
}


void Mesh::render(App* app, RenderDevice* renderDevice) {
    renderDevice->pushState();
        renderDevice->setShadeMode(RenderDevice::SHADE_SMOOTH);

        renderDevice->setColor(Color3::WHITE);
        renderDevice->beginPrimitive(RenderDevice::TRIANGLES);

            double L, H;
            L = inf;
            H = -inf;

            for (int a = 0; a < indexArray.size(); ++a) {
                int i = indexArray[a];
                renderDevice->setNormal(geometry.normalArray[i]);

                switch (renderStyle) {
                case GAUSSIAN:
                    {
                        double c = gaussianCurvature[i] * 0.1;
                        L = min(c, L);  H = max(c, H);

                        c = clamp(sign(c) * sqrt(abs(c)), -1, 1) * 0.5 + 0.5;
                        Color3 col = (Color3::WHITE).lerp(Color3::GREEN * 0.7, c);
                        renderDevice->setColor(col);
                    }
                    break;

                case MEAN:
                    {
                        double c = meanCurvature[i];
                        L = min(c, L);  H = max(c, H);

                        c = clamp((c - 20) / 40, 0, 1);
                        c = clamp(sqrt(c), 0, 1);
                        Color3 col = Color3::WHITE.lerp(Color3::RED, c);
                        renderDevice->setColor(col);
                    }
                    break;
                }

                renderDevice->sendVertex(geometry.vertexArray[i]);
            }
        renderDevice->endPrimitive();

        app->debugPrintf("[%g, %g]", L, H);
        // Draw the edges
        renderDevice->setColor(Color3::BLACK);
        renderDevice->setLineWidth(0.5);
        renderDevice->setBlendFunc(RenderDevice::BLEND_SRC_ALPHA, RenderDevice::BLEND_ONE_MINUS_SRC_ALPHA);

        renderDevice->beginPrimitive(RenderDevice::LINES);
            for (int e = 0; e < edgeArray.size(); ++e) {
                for (int a = 0; a < 2; ++a) {
                    int i = edgeArray[e].vertexIndex[a];

                    renderDevice->setNormal(geometry.normalArray[i]);
                    renderDevice->sendVertex(geometry.vertexArray[i] + geometry.normalArray[i] * 0.001);
                }
            }
        renderDevice->endPrimitive();
    renderDevice->popState();
}




Demo::Demo(App* _app) : GApplet(_app), app(_app) {
}


void Demo::init()  {
    // Called before Demo::run() beings
    app->debugCamera.setPosition(Vector3(0,0,1));
    app->debugCamera.lookAt(Vector3::ZERO);

    mesh.import(IFSModel::create(app->dataDir + "ifs/elephant.ifs")->pose(CoordinateFrame()));
}


void Demo::cleanup() {
    // Called when Demo::run() exits
}


void Demo::doNetwork() {
	// Poll net messages here
}


void Demo::doSimulation(SimTime dt) {
	// Add physical simulation here
}


void Demo::doLogic() {
    if (app->userInput->keyPressed(SDLK_ESCAPE)) {
        // Even when we aren't in debug mode, quit on escape.
        endApplet = true;
        app->endProgram = true;
    }

    app->debugPrintf("Shading: (M)ean, (G)aussian, (V)anilla");

    if (app->userInput->keyPressed('g')) {
        mesh.renderStyle = Mesh::GAUSSIAN;
    } else if (app->userInput->keyPressed('m')) {
        mesh.renderStyle = Mesh::MEAN;
    } else if (app->userInput->keyPressed('v')) {
        mesh.renderStyle = Mesh::WHITE;
    }
	// Add other key handling here
}


void Demo::doGraphics() {

    LightingParameters lighting(G3D::toSeconds(11, 00, 00, AM));
    app->renderDevice->setProjectionAndCameraMatrix(app->debugCamera);

    // Cyan background
    app->renderDevice->setColorClearValue(Color3(.1, .5, 1));

    app->renderDevice->clear(app->sky.isNull(), true, true);
    if (! app->sky.isNull()) {
        app->sky->render(lighting);
    }

    // Setup lighting
    app->renderDevice->enableLighting();
		app->renderDevice->setLight(0, GLight::directional(lighting.lightDirection, lighting.lightColor));
		app->renderDevice->setAmbientLightColor(lighting.ambient);


//    	Draw::axes(CoordinateFrame(Vector3(0, 0, 0)), app->renderDevice);

    app->renderDevice->disableLighting();
        mesh.render(app, app->renderDevice);

    if (! app->sky.isNull()) {
        app->sky->renderLensFlare(lighting);
    }
}


void App::main() {
	setDebugMode(true);
	debugController.setActive(false);

    dataDir = "z:/libraries/g3d-6_02/data/";

    // Load objects here
    sky = Sky::create(renderDevice, dataDir + "sky/");
    
    Demo(this).run();
}


App::App(const GAppSettings& settings) : GApp(settings) {
}


int main(int argc, char** argv) {
    GAppSettings settings;
    settings.window.width       = 400;
    settings.window.height      = 400;
    settings.window.fsaaSamples = 4;
    App(settings).run();
    return 0;
}

