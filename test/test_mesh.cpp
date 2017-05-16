#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <test/testutil.h>
#include "test_mesh.h"
#include <stdio.h>
#include <cstdint>
#include <sstream>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>

void TestMesh::setUp()
{
    mesh = new Mesh();
}

void TestMesh::tearDown()
{
    delete mesh;
}

void TestMesh::testMeshing()
{
    mesh->readSTL("../meshes/bunny.stl");
    CPPUNIT_ASSERT(mesh->basicValidity());
    CPPUNIT_ASSERT(!mesh->manifoldValidity()); // bunny has known holes in the bottom
    
    
}

void TestMesh::testMeshingSphere(){
	mesh->readSTL("../meshes/sphere.stl");
    CPPUNIT_ASSERT(mesh->basicValidity());
    CPPUNIT_ASSERT(mesh->manifoldValidity());
}

void TestMesh::testMeshingCube(){
	mesh->readSTL("../meshes/cube.stl");
    CPPUNIT_ASSERT(mesh->basicValidity());
    CPPUNIT_ASSERT(mesh->manifoldValidity());
}

void TestMesh::testMeshingTorus(){
	mesh->readSTL("../meshes/torus.stl");
	CPPUNIT_ASSERT(mesh->basicValidity());
    CPPUNIT_ASSERT(mesh->manifoldValidity());
}

void TestMesh::testEulerTorus(){
	mesh->readSTL("../meshes/torus.stl");
	int G = 1;
	int eulchar = 2 - 2*G;
	CPPUNIT_ASSERT(eulchar == mesh->getEuler());
}


void TestMesh::testEulerCharacteristic(){
	mesh->readSTL("../meshes/sphere.stl");
	int G = 0; //sphere has genus of zero
	int eulchar = 2 - 2*G;
	CPPUNIT_ASSERT(eulchar == mesh->getEuler());
}

void TestMesh::testDanglingVert(){
	mesh->readSTL("../meshes/cube.stl");
	vector<cgp::Point> verts = mesh->getVerts();
	cgp::Point point = verts[0];
	point.x = point.x - 20;
	point.y = point.y - 20;
	verts.push_back(point);
	mesh->setVerts(verts);
	CPPUNIT_ASSERT(!mesh->basicValidity());
}

void TestMesh::testEdgeBounds(){
	mesh->readSTL("../meshes/cube.stl");
	vector<Edge> edges = mesh->getEdges();
	Edge tempedge;
	vector<cgp::Point> verts = mesh->getVerts();
	cgp::Point point1 = verts[0];
	point1.x = point1.x - 20;
	point1.y = point1.y + 80;
	cgp::Point point2 = verts[1];
	point2.x = point2.x + 50;
	point2.y = point2.y - 20;
	verts.push_back(point1);
	tempedge.v[0] = verts.size()-1;
	verts.push_back(point2);
	tempedge.v[1] = verts.size()-1;
	edges.push_back(tempedge);
	mesh->setVerts(verts);
	CPPUNIT_ASSERT(mesh->checkEdgeBound(edges));
	CPPUNIT_ASSERT(!mesh->basicValidity());
}



//#if 0 /* Disabled since it crashes the whole test suite */
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestMesh, TestSet::perCommit());
//#endif
