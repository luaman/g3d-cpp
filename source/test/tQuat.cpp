#include "../include/G3DAll.h"

static void testMatrixConversion() {

    {
        // This is a known corner case
        Matrix3 M = Matrix3::fromAxisAngle(Vector3::unitY(), toRadians(180));
        Quat q(M);
        Matrix3 M2 = q.toRotationMatrix();

		for (int r = 0; r < 3; ++r) {
			for (int c = 0; c < 3; ++c) {
				debugAssert(abs(M[r][c] - M2[r][c]) < 0.0005);
			}
		}
    }

    {
        // This is a known corner case (near the one above)
        Matrix3 M(-0.99999988, 0, 0,
                   0, 1, 0,
                   0, 0, -0.99999988);
        Quat q(M);
        Matrix3 M2 = q.toRotationMatrix();

		for (int r = 0; r < 3; ++r) {
			for (int c = 0; c < 3; ++c) {
				debugAssert(abs(M[r][c] - M2[r][c]) < 0.0005);
			}
		}
    }


    // Round trip M->q->M
	for (int i = 0; i < 100; ++i) {
		Matrix3 M = Matrix3::fromAxisAngle(Vector3::random(), random(0, G3D_TWO_PI));
		Quat q(M);
		Matrix3 M2 = q.toRotationMatrix();

		for (int r = 0; r < 3; ++r) {
			for (int c = 0; c < 3; ++c) {
				debugAssert(abs(M[r][c] - M2[r][c]) < 0.0005);
			}
		}
	}

	// Round trip q->M->q
	for (int i = 0; i < 100; ++i) {
		Quat q1 = Quat::fromAxisAngleRotation(Vector3::random(), random(0, G3D_TWO_PI));
		Matrix3 M = q1.toRotationMatrix();
		Quat q2(M);

		debugAssert(q1.fuzzyEq(q2) || q1.fuzzyEq(-q2));
	}
}


static void testSlerp() {
	for (int i = 0; i < 100; ++i) {
		Vector3 axis = Vector3::random();
		float a0 = random(0, G3D_PI);
		float a1 = random(a0, G3D_PI);
		float a2 = (a0 + a1) / 2;
		Quat q0 = Quat::fromAxisAngleRotation(axis, a0);
		Quat q1 = Quat::fromAxisAngleRotation(axis, a1);
		Quat q2 = Quat::fromAxisAngleRotation(axis, a2);

		Quat rq2 = q0.slerp(q1, 0.5);

		float ra2;
		Vector3 raxis;
		rq2.toAxisAngleRotation(raxis, ra2);

		debugAssert(fuzzyEq(ra2, a2));
		debugAssert(raxis.fuzzyEq(axis));
		debugAssert(rq2.fuzzyEq(q2));
	}
}


void testQuat() {
	printf("Quat ");

	testMatrixConversion();
	testSlerp();


	printf("passed\n");
}
