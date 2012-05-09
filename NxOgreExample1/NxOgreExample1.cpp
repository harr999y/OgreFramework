#include "NxOgreExample1.h"

void NxOgreExample1::createScene()
{
	using namespace Ogre;
	mWorld = NxOgre::World::createWorld();
	NxOgre::SceneDescription sceneDesc;
	sceneDesc.mGravity = NxOgre::Vec3(0,-9.8f,0);
	sceneDesc.mName = "Test";
	mScene = mWorld->createScene(sceneDesc);
	mScene->getMaterial(0)->setStaticFriction(0.5f);
	mScene->getMaterial(0)->setDynamicFriction(0.5f);
	mScene->getMaterial(0)->setRestitution(0.1f);
	mRenderSystem = new OGRE3DRenderSystem(mScene);
	mCube = mRenderSystem->createBody(new NxOgre::Box(10,10,10),NxOgre::Vec3(20,335,0),"robot.mesh");
	mCube->addForce(NxOgre::Vec3(-800,-200,0));
	//mCube->getEntity()->getParentNode()->setScale(10,10,10);
	mScene->createSceneGeometry(new NxOgre::PlaneGeometry(0,NxOgre::Vec3(0,1,0)),Matrix44_Identity);

	MovablePlane *plane = new MovablePlane("Plane");
	plane->d = 0;
	plane->normal = Vector3::UNIT_Y;
	Ogre::MeshManager::getSingleton().createPlane("PlaneMesh", 
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
		*plane, 120, 120, 1, 1, true, 1, 3, 3, Vector3::UNIT_Z);
	Entity *planeEnt = mSceneMgr->createEntity("PlaneEntity", "PlaneMesh");
	planeEnt->setMaterialName("Examples/GrassFloor");

	Ogre::SceneNode* mPlaneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	mPlaneNode->attachObject(planeEnt);
	mPlaneNode->scale(100, 100, 100);

	mVisualDebugger = mWorld->getVisualDebugger();
	mVisualDebuggerRenderable = new OGRE3DRenderable(NxOgre::Enums::RenderableType_VisualDebugger);
	mVisualDebugger->setRenderable(mVisualDebuggerRenderable);
	mVisualDebbugerNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	mVisualDebbugerNode->attachObject(mVisualDebuggerRenderable);
	mVisualDebugger->setVisualisationMode(NxOgre::Enums::VisualDebugger_ShowAll);

	//mVolume = mScene->createVolume(new NxOgre::Box(255),NxOgre::Matrix44(NxOgre::Vec3(0,0,0)),this,NxOgre::Enums::VolumeCollisionType_All);

	mKB = mRenderSystem->createKinematicBody(new NxOgre::Box(120,100,120),NxOgre::Vec3(0,12,0),"robot.mesh");
	//mRenderSystem->createBody(new NxOgre::Box(1,1,1),NxOgre::Vec3(0,14,0),"Cube.mesh");
	mYPos = mKB->getGlobalPosition().y;
	mDown = false;
}

void NxOgreExample1::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
	NxOgre::TimeController::getSingleton()->advance(evt.timeSinceLastFrame);
	mVisualDebugger->draw();
	mVisualDebbugerNode->needUpdate();

	if (mDown)
	{
		mYPos -= 2.5f * evt.timeSinceLastFrame;
	}
	else
	{
		mYPos += 2.5f * evt.timeSinceLastFrame;
	}

	mKB->moveGlobalPosition(NxOgre::Vec3(0,mYPos,0));
	if (mYPos > 20)
	{
		mDown = true;
	}
	else if (mYPos < 0)
	{
		mDown = false;
	}

	if (GetAsyncKeyState(VK_SPACE) & 0xff)
	{
		OGRE3DBody * cube = mRenderSystem->createBody(new NxOgre::Box(1,1,1),NxOgre::Vec3(mCameraMan->getCamera()->getPosition().x,mCameraMan->getCamera()->getPosition().y,mCameraMan->getCamera()->getPosition().z),"fish.mesh");
		cube->addForce(NxOgre::Vec3(mCameraMan->getCamera()->getDirection().x * 10000,mCameraMan->getCamera()->getDirection().y * 10000,mCameraMan->getCamera()->getDirection().z * 10000),NxOgre::Enums::ForceMode_Force,true);
	}
	CEGUIBaseApp::frameRenderingQueued(evt);
}

void NxOgreExample1::onVolumeEvent(NxOgre::Volume* volume, NxOgre::Shape* volumeShape, NxOgre::RigidBody* collision_body, NxOgre::Shape* rigidBodyShape, unsigned int collisionEvent)
{
	if (collisionEvent == NxOgre::Enums::VolumeCollisionType_OnEnter)
	{
		NxOgre::Actor * actor = static_cast<NxOgre::Actor*>(collision_body);
		float y = (9.81 * actor->getMass()) + (-actor->getLinearVelocity().y * actor->getMass()) + (10 - actor->getGlobalPosition().y * actor->getMass());
		actor->addForce(NxOgre::Vec3(0,200,0),NxOgre::Enums::ForceMode_Impulse);
	}
}