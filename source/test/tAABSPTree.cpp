#include "../include/G3DAll.h"


static void testSerialize() {
    AABSPTree<Vector3> tree;
    int N = 1000;

    for (int i = 0; i < N; ++i) {
        tree.insert(Vector3::random());
    }
    tree.balance();

    // Save the struture
    BinaryOutput b("test-bsp.dat", G3D_LITTLE_ENDIAN);
    tree.serializeStructure(b);
    b.commit();

}


static void testBoxIntersect() {

	AABSPTree<Vector3> tree;

	// Make a tree containing a regular grid of points
	for (int x = -5; x <= 5; ++x) {
		for (int y = -5; y <= 5; ++y) {
			for (int z = -5; z <= 5; ++z) {
				tree.insert(Vector3(x, y, z));
			}
		}
	}
	tree.balance();

	AABox box(Vector3(-1.5, -1.5, -1.5), Vector3(1.5, 1.5, 1.5));

	AABSPTree<Vector3>::BoxIntersectionIterator it = tree.beginBoxIntersection(box);
	const AABSPTree<Vector3>::BoxIntersectionIterator end = tree.endBoxIntersection();

	int hits = 0;
	while (it != end) { 
		const Vector3& v = *it;

		debugAssert(box.contains(v));

		++hits;
		++it;
	}

	debugAssertM(hits == 3*3*3, "Wrong number of intersections found in testBoxIntersect for AABSPTree");
}


void testAABSPTree() {
	printf("AABSPTree ");

	testBoxIntersect();
	testSerialize();

	printf("passed\n");
}
