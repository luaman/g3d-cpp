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
   http://citeseer.nj.nec.com/meyer02discrete.html 

   @author Tomer Moscovich
   @maintainer Morgan McGuire

  TODO: reduce redundant data storage
  TODO: pass values that need to be computed in rather than recomputing them
  TODO: use edge adjacency
  TODO: convert to use Vertex
*/
class Curvatures {
public:
    Curvatures() {}

    /**
      Takes an array of vertices, and an array of triangle indices to traverse them
      You may get both of these from a G3D::PosedModel.
      */
    Curvatures(const Array< Vector3 >& _vertexArray, 
                           const Array< int >& _indexArray);
    /**
     Gaussian curvature,
     the product of the principal curvatures.
    */
    void gaussianCurvature(Array<double>& gaussianCurvaturePerVertex) const;

    /**
     Mean curvature at each point on a mesh.
     Mean curvature is the mean of the normal curvatures,
     and also the average of the principal curvatures.
    */
    void meanCurvature(Array<double>& meanCurvaturePerVertex) const;

    /** Returns the Principal Curvatures for each vertex in _vertexArray */
    void principalCurvature(
        const Array<double>& gaussianCurvature,
        const Array<double>& meanCurvature,
        Array<double>&       k1,
        Array<double>&       k2) const;

    /** Returns the Principal Direction for each vertex in _vertexArray
      @cite principal directions based on Taubing G. "Estimating the Tensor of Curvature of a
      Surface from a Polyhedral Approximation. Proc. ICCV 1995.
      http://citeseer.nj.nec.com/taubin95estimating.html
    */
    void principalDirections(Array<Vector3>& T1, Array<Vector3>& T2) const;

protected:

    MeshAlg::Geometry               geometry;
    Array<MeshAlg::Face>            faces;
    Array<MeshAlg::Edge>            edges;
    Array<MeshAlg::Vertex>          vertices;

    /**
     adjacentVertices[v] is an array of the 1-neighbors of vertex v.
     Computed by initAdjacentVertices.
     */
    Array< Array<int> >             adjacentVertices;

    /** angle at [vertex][face] */
    Array< Table<int, float> >      angleInner;

    /** angle at [adjacent vertex][face] */
    Array< Table<int, float> >      angleLeft;  

    /** angle at [other adjacent vertex][face] */
    Array< Table<int, float> >      angleRight; 

    /** cotan of angles opposite each segment [i][j] */
    Array< Table<int, double> >     cota; 
    Array< Table<int, double> >     cotb;

    /** 
     Area of the Voronoi region surrounding a vertex generalized to account
     for obtuse triangles.
    */
    Array<double> AMixed;


    /**
     For each vertex i, calculate the angle at i for each adjacent face
     */
    void initAngles();

    /** For each vertex i and an adjacent vertex j, find the cotans of 
        the two angles (a and b) opposite edge ij. */
    void initCotans();

    void initAdjacentVertices();

    void initAMixed();

    /**
     "Givens" rotation (based on Golub and Loan)
     solves [c s; -s c]'*[a b]' = [r 0]' for c and s 
     */
    void givens(double& cost, double& sint, double a, double b) const;

    /**
     Returns a set of the 1-neighbors of the vertex with index v.
     */
    void get1Neighbors(int v, Set<int>& ringSet) const;
};


/** Angle between two vectors */
inline float vectorAngle(const Vector3& a, const Vector3& b){
    double d = a.dot(b);
    double m = sqrt(a.squaredLength() * b.squaredLength());
    return acos(d / m);
}


void Curvatures::get1Neighbors(int v, Set<int>& ringSet) const{

    ringSet.clear();

    const Array<int>& neighborFaces = vertices[v].faceIndex;
    if (neighborFaces.size() > 0) {
        // For each face
        for (int j = neighborFaces.size() - 1; j >= 0; --j) {
            const int f = neighborFaces[j];
            const MeshAlg::Face& face = faces[f];

            // Insert all vertices of this face
            for (int i = 0; i < 3; ++i) {
                ringSet.insert(face.vertexIndex[i]);  
            }
        }

        // ringSet should contain 1-neighbors only, so remove the vertex itself.
        ringSet.remove(v);
    }
}


Curvatures::Curvatures(
    const Array< Vector3 >& _vertexArray, 
    const Array< int >&     _indexArray) {

    // Compute Mesh Properties
    geometry.vertexArray = _vertexArray;
    MeshAlg::computeAdjacency(geometry.vertexArray, _indexArray,
             faces, edges, vertices);

    Array<Vector3> faceNormalArray;
    MeshAlg::computeNormals(geometry.vertexArray, faces,
            vertices, geometry.normalArray, faceNormalArray);

    // init tables
    initAdjacentVertices();
    initAngles();
    initCotans();
    initAMixed();
}

static const mod3[] = {0, 1, 2, 0, 1};

inline double cot(double angle) {
    return 1.0 / tan(angle);
}

void Curvatures::initAdjacentVertices() {
    const int n = geometry.vertexArray.size();

    Set<int> ringSet;

    adjacentVertices.resize(n, DONT_SHRINK_UNDERLYING_ARRAY);
    for (int v = 0; v < n; ++v) {
        get1Neighbors(v, ringSet);
        ringSet.getMembers(adjacentVertices[v]);
    }
}

void Curvatures::initAngles() {
    const int n = geometry.vertexArray.size();

    angleInner.resize(n);
    angleLeft.resize(n);
    angleRight.resize(n);

    for (int v = 0; v < geometry.vertexArray.size(); ++v) {
         for (int j = 0; j < vertices[v].faceIndex.size(); ++j) {
            const int f = vertices[v].faceIndex[j];
            const MeshAlg::Face& face = faces[f];

            // 1 calculate cosine of angle at vertex
            
            // 1.1 find vertex b
            int i;
            if (face.vertexIndex[0] == v) {
                i = 0;
            } else if (face.vertexIndex[1] == v) {
                i = 1;
            } else {
                i = 2;
            }

            // 1.2 assign geometric vertices
            const Vector3& C = geometry.vertexArray[face.vertexIndex[i]];
            const Vector3& L = geometry.vertexArray[face.vertexIndex[mod3[i + 1]]];
            const Vector3& R = geometry.vertexArray[face.vertexIndex[mod3[i + 2]]];

            // 1.3 calculate cos of angle 
            angleInner[v].set(f, vectorAngle(L - C, R - C));
            angleLeft[v]. set(f, vectorAngle(C - L, R - L));
            angleRight[v].set(f, vectorAngle(C - R, L - R));
         }
    }
}



void Curvatures::initCotans() {
    cota.resize(geometry.vertexArray.size());
    cotb.resize(geometry.vertexArray.size());

    Array< MeshAlg::Face > aFaces;
    Array< int > aFacesIdx;

    // For each edge, calc cota/cotb
    for(int k = 0; k < edges.size(); ++k) { 

        int i = edges[k].vertexIndex[0];
        int j = edges[k].vertexIndex[1];
        aFaces.clear();
        aFacesIdx.clear();

        // Find faces that share this edge
        const MeshAlg::Vertex& vertex = vertices[i];

        for (int x = 0; x < vertex.faceIndex.size(); ++x) {
            const int f = vertex.faceIndex[x];
            const MeshAlg::Face& face = faces[f];

            if (face.containsVertex(j)) {
                aFaces.append(face);
                aFacesIdx.append(f);
            }
        }

        debugAssertM(aFaces.size() >= 2, "initCotans: Fewer than 2 faces share an edge."); 

        int a, b;

        // Find the points who's angle we need (ie not i and j)
        for (int v = 0; v < 3; ++v) {
            if ( (aFaces[0].vertexIndex[v] !=i) && (aFaces[0].vertexIndex[v] != j) ) {
                a = aFaces[0].vertexIndex[v];
            }

            if ( (aFaces[1].vertexIndex[v] !=i) && (aFaces[1].vertexIndex[v] != j) ) {
                b = aFaces[1].vertexIndex[v];
            }
        }

        // Find cota cotb
        if (angleInner[a].containsKey(aFacesIdx[0])) {
            //TODO Don't edges go both ways?
            cota[i].set(j, cot(angleInner[a][aFacesIdx[0]]));
            cotb[j].set(i, cot(angleInner[a][aFacesIdx[0]]));
        } else
            debugPrintf("\n1nokey kijab %d %d %d %d %d", k, i, j, a, b); 

        if (angleInner[b].containsKey(aFacesIdx[1])) {
            //TODO Don't edges go both ways?
            cotb[i].set(j, cot(angleInner[b][aFacesIdx[1]]));
            cota[j].set(i, cot(angleInner[b][aFacesIdx[1]]));  
        } else {
            debugPrintf("\n2nokey kijab %d %d %d %d %d", k, i, j, a, b);
        }
    }
}


void Curvatures::initAMixed() {

    // Init Amixed
    AMixed.resize(geometry.vertexArray.size());
    for (int v = 0; v < AMixed.size(); ++v) {
        AMixed[v] = 0.0;
    }

    // For each vertex
    for (int v = 0; v < geometry.vertexArray.size(); ++v) {  
        
        // For each face
        for (int j = 0; j < vertices[v].faceIndex.size(); ++j) { 

            const int f = vertices[v].faceIndex[j];
            const MeshAlg::Face& face = faces[f];

            int iCenter = 0;

            // Circle around the vertices of the face until we find the index
            // matching v.
            if (face.vertexIndex[0] == v) {
                iCenter = 0;
            } else if (face.vertexIndex[1] == v) {
                iCenter = 1;
            } else {
                iCenter = 2;
            }

            const int iLeft  = mod3[iCenter + 1];
            const int iRight = mod3[iCenter + 2];

            // Get the geometry of triangle CLR
            const Vector3& C = geometry.vertexArray[face.vertexIndex[iCenter]];
            const Vector3& L = geometry.vertexArray[face.vertexIndex[iLeft]];
            const Vector3& R = geometry.vertexArray[face.vertexIndex[iRight]];

            const double thetaC = angleInner[v][f];
            const double thetaL = angleLeft[face.vertexIndex[iLeft]][f];
            const double thetaR = angleLeft[face.vertexIndex[iRight]][f];

            // Figure out which angle is obtuse

            if (abs(thetaC) > G3D_HALF_PI) {

                // Angle at v is obtuse

                const double area = (L - C).cross(R - C).length() / 2.0;
                AMixed[v] += area / 2.0;

            } else if ((abs(thetaL) > G3D_HALF_PI) || 
                       (abs(thetaR) > G3D_HALF_PI)) {   

                // The triangle is obtuse
                const double area = (L - C).cross(R - C).length() / 2.0;
                AMixed[v] += area / 4.0;

            } else { 
                
                // The triangle is *not* obtuse
                double lenLeft   = (L - C).squaredLength();
                double lenRight  = (R - C).squaredLength();

                // Voronoi region
                AMixed[v] += 
                    0.125 * (lenLeft  * cot(thetaL) +   
                             lenRight * cot(thetaR));
            }
        }
    }
}


void Curvatures::gaussianCurvature(Array<double>& curvatures) const {
    curvatures.resize(geometry.vertexArray.size(), DONT_SHRINK_UNDERLYING_ARRAY);

    // For each vertex
    for (int v = 0; v < curvatures.size(); ++v) {  
         double theta = 0.0;

         // For each face
         for (int j = 0; j < vertices[v].faceIndex.size(); ++j) {
            const int f = vertices[v].faceIndex[j];
            theta += abs(angleInner[v][f]);  
         }

         curvatures[v] = (G3D_TWO_PI - theta) / AMixed[v];
    }
}


void Curvatures::meanCurvature(Array<double>& curvatures) const {

    curvatures.resize(geometry.vertexArray.size(), DONT_SHRINK_UNDERLYING_ARRAY);

    // Set of indices of adjacent vertices
    Set<int> ringSet;

    // For each vertex
    for (int v = 0; v < curvatures.size(); ++v) {  
         // Position of the current vertex
         const Vector3& vPos = geometry.vertexArray[v];

         // Iterate over each edge exiting v, summing K's

         // Mean Curvature Normal
         Vector3 K = Vector3::ZERO;

         const Array<int>& neighbor = adjacentVertices[v];

         for (int n = 0; n < neighbor.size(); ++n) {
             // Index of vertex at the other end of the edge
             const int v2 = neighbor[n];

             // K(xi) = 1/(2*Amixed)*sum(cota+cotb)*(Xj-Xi)
             K += (geometry.vertexArray[v2] - vPos) * (cota[v][v2] + cotb[v][v2]);
         }

         K /= 2.0 * AMixed[v];
         curvatures[v] = 0.5 * K.length();
    }
}


void Curvatures::principalCurvature(
    const Array<double>&    kg,
    const Array<double>&    kh,
    Array<double>&          k1,
    Array<double>&          k2) const {

    // For each vertex
    for(int v = geometry.vertexArray.size() - 1; v >= 0; --v) {  
       const double sdxi = sqrt(max(kh[v] * kh[v] - kg[v], 0.0));       
       k1.append(kh[v] + sdxi);
       k2.append(kh[v] - sdxi);
    }
}


void Curvatures::principalDirections(Array<Vector3>& T1, Array<Vector3>& T2) const {
    const int n = geometry.vertexArray.size();

    T1.resize(n, DONT_SHRINK_UNDERLYING_ARRAY);
    T2.resize(n, DONT_SHRINK_UNDERLYING_ARRAY);

    // vertex, neighbor, and vj - vi
    Vector3 vi, vj, dvji;     

    // Setup for Householder matrix
    Vector3 W;                

    // Householder matrix
    Matrix3 Q    = Matrix3::ZERO;
    Matrix3 M    = Matrix3::ZERO;
    Matrix3 QMQ  = Matrix3::ZERO;

    // Holds a vector for a N*N' that yields a 3x3 matrix
    Matrix3 NMat = Matrix3::ZERO; 

    // weight of k and T
    Array<float> weight;    

    // normal curvature in direction vj
    Array<float> k;   
    
    // normalized projection of vj-vi onto tangent plane
    Array<Vector3> T; 

    // For each vertex
    for (int v = 0; v < n; ++v) {
        vi = geometry.vertexArray[v];

        // Calculate weights (proportional to inverse of |vi-vj|
        // (Hameiri'03 modification of Taubin)
        const Array<int>& ring = adjacentVertices[v];
        double wSum = 0.0;
        weight.resize(ring.size(), DONT_SHRINK_UNDERLYING_ARRAY);
        for (int j = 0; j < ring.size(); ++j) {
            vj = geometry.vertexArray[ring[j]];
            weight[j] = (vi - vj).length();
            wSum += weight[j];
        }

        // Make weights sum to 1
        for(int j = 0; j < weight.size(); ++j) {
            weight[j] /= wSum;
        }
    
        // Calc unit length normalized projection of vj-vi onto the tangent plane
        // T_ij = normalize((I-N_vi*N_vi')(vi-vj))

        // for matrix rep of vertex normal
        NMat = Matrix3::ZERO;

        NMat.setColumn(0, geometry.normalArray[v]);
        T.clear();
        T.resize(ring.size());
        for (int j = 0; j < T.size(); ++j) { 
            vj = geometry.vertexArray[ring[j]];
            T[j] = ((Matrix3::IDENTITY - NMat * NMat.transpose()) * (vi - vj)).direction();
        }

        // Calc normal curvature in direction of vi,vj
        k.clear();
        k.resize(ring.size());
        for (int j = 0; j < k.size(); ++j) {
            dvji = geometry.vertexArray[ring[j]] - vi;
            const Vector3& N = geometry.normalArray[v];
            k[j] =  (2.0 * N.dot(dvji)) / dvji.squaredLength();
        }

        // Estimate M as weighted sum over neighborhood
        M = Matrix3::ZERO;
        for (int j = 0; j < ring.size(); ++j) {
            // Holds a vector for a T*T' that yields a 3x3 matrix
            Matrix3 TMat = Matrix3::ZERO;
            TMat.setColumn(0, T[j]);
            M = M + weight[j] * k[j] * (TMat * TMat.transpose()); 
        }

        // Calculate W for Householder matrix
        double EminusN = (Vector3::UNIT_X - geometry.normalArray[v]).length();
        double EplusN  = (Vector3::UNIT_X + geometry.normalArray[v]).length();

        if (EminusN > EplusN) {
            W = (Vector3::UNIT_X - geometry.normalArray[v]) / EminusN; 
        } else {
            W = (Vector3::UNIT_X + geometry.normalArray[v]) / EplusN; 
        }
        
        // Holds a vector for a W*W' that yields a 3x3 matrix
        Matrix3 WMat = Matrix3::ZERO;
        WMat.setColumn(0, W);

        // Calc Householder matrix Q
        Q = Matrix3::IDENTITY - (WMat * WMat.transpose()) * 2.0;
         
        // Q'MQ contain the upper bit of M?
        QMQ = Q.transpose() * M * Q;

        // Find theta using Givens rotation
        double cost, sint;
        givens(cost, sint, QMQ[1][1], M[2][1]);

        // Principal directions (TODO: reversed?)
        T1[v] = cost * Q.getColumn(1) - sint * Q.getColumn(2);
        T2[v] = sint * Q.getColumn(1) + cost * Q.getColumn(2);
    }
}


void Curvatures::givens(double& cost, double& sint, double a, double b) const {
    double t;

    if (b == 0) {
        cost = 1.0;
        sint = 1.0;
    } else if (abs(b) > abs(a)) {
        t    = -a / b;
        sint = 1.0 / sqrt(1.0 + t * t);
        cost = sint*t;
    } else {
        t    = -b / a;
        cost = 1.0 / sqrt(1.0 + t * t);
        sint = cost*t;
    }
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

    Array<double>               principleCurvature[2];
    Array<Vector3>              principleDirection[2];

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
    curvature.principalCurvature(gaussianCurvature, meanCurvature, principleCurvature[0], principleCurvature[1]);
    curvature.principalDirections(principleDirection[0], principleDirection[1]);
    
}


void Mesh::render(App* app, RenderDevice* renderDevice) {
    renderDevice->pushState();
        renderDevice->setShadeMode(RenderDevice::SHADE_SMOOTH);

        renderDevice->enableLighting();

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

    mesh.import(IFSModel::create(app->dataDir + "ifs/cow.ifs")->pose(CoordinateFrame()));
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

    dataDir = "d:/libraries/g3d-6_02/data/";

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

