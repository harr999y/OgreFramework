#include "FourthApp.h"

using namespace Ogre;

void FourthApp::createScene()
{
	createGrassMesh();
	mSceneMgr->setAmbientLight(ColourValue(1, 1, 1));

	mCameraMan->getCamera()->setPosition(150, 50, 150);
	mCameraMan->getCamera()->lookAt(0, 0, 0);

	Entity *robot = mSceneMgr->createEntity("robot", "robot.mesh");
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(robot);

	Plane plane;
	plane.normal = Vector3::UNIT_Y;
	plane.d = 0;
	MeshManager::getSingleton().createPlane("floor",
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,
		450,450,10,10,true,1,50,50,Vector3::UNIT_Z);
	Entity* pPlaneEnt = mSceneMgr->createEntity("plane", "floor");
	pPlaneEnt->setMaterialName("Examples/GrassFloor");
	pPlaneEnt->setCastShadows(false);
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(pPlaneEnt);

	Entity * grass = mSceneMgr->createEntity("Grass","GrassMesh");

	Ogre::StaticGeometry *sg =mSceneMgr->createStaticGeometry("GrassArea");
	const int size =375;
	const int amount = 60;
	sg->setRegionDimensions(Vector3(size,size,size));
	sg->setOrigin(Vector3(-(size/2),0,(-size/2)));
		
	for(int x=-size/2;x<size/2;x+=(size/amount))
	{
		for(int z=-size/2;z<size/2;z+=(size/amount))
		{
			Real r = size/amount/2.0f;
			Vector3 pos(x+Math::RangeRandom(-r,r),0,z+Math::RangeRandom(-r,r));
			Vector3 scale(1.0f,Math::RangeRandom(0.9f,1.1f),1.0f);
			Quaternion orientation;
			orientation.FromAngleAxis(Degree(Math::RangeRandom(0.0f,359.0f)),Vector3::UNIT_Y);

			sg->addEntity(grass,pos,orientation,scale);
		}
	}
	sg->build();		
}

void FourthApp::createGrassMesh()
{
	const float width = 25;
	const float height = 30;
	ManualObject mo("GrassObject");
	Vector3 vec(width/2.0f,0.0f,0.0f);
	Quaternion rot;
	rot.FromAngleAxis(Degree(60),Vector3::UNIT_Y);

	mo.begin("Examples/GrassBlades",RenderOperation::OT_TRIANGLE_LIST);
	for(int i=0;i<3;i++)
	{
		mo.position(-vec.x,height,-vec.z);
		mo.textureCoord(0.0f,0.0f);

		mo.position(-vec.x,height,vec.z);
		mo.textureCoord(0.0f,1.0f);

		mo.position(vec.x,0.0f,-vec.z);
		mo.textureCoord(1.0f,0.0f);

		mo.position(vec.x,0.0f,vec.z);
		mo.textureCoord(1.0f,1.0f);	
		int offset = i*4;
		mo.triangle(offset,offset+3,offset+1);
		mo.triangle(offset,offset+2,offset+3);

		vec = rot * vec;
	}
	mo.end();

	mo.convertToMesh("GrassMesh");
}