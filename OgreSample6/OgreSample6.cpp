#include "OgreSample6.h"

void OgreSample6App::createScene()
{
	mSceneMgr->setSkyDome(true, "Examples/CloudySky");

	// setup some basic lighting for our scene
	mSceneMgr->setAmbientLight(ColourValue(0.3f, 0.3f, 0.3f));
	mSceneMgr->createLight()->setPosition(20, 80, 50);

	createCubeMap();

	// create an ogre head, give it the dynamic cube map material, and place it at the origin
	mHead = mSceneMgr->createEntity("CubeMappedHead", "ogrehead.mesh");
	mHead->setMaterialName("Examples/DynamicCubeMap");
	mSceneMgr->getRootSceneNode()->attachObject(mHead);
	mPivot = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	Entity* fish = mSceneMgr->createEntity("Fish", "fish.mesh");
	mFishSwim = fish->getAnimationState("swim");
	mFishSwim->setEnabled(true);

	// create a child node at an offset and attach a regular ogre head and a nimbus to it
	SceneNode* node = mPivot->createChildSceneNode(Ogre::Vector3(-60, 10, 0));
	node->setScale(7, 7, 7);
	node->yaw(Degree(90));
	node->attachObject(fish);

	// create a floor mesh resource
	MeshManager::getSingleton().createPlane("floor", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Plane(Ogre::Vector3::UNIT_Y, -30), 1000, 1000, 10, 10, true, 1, 8, 8, Ogre::Vector3::UNIT_Z);

	// create a floor entity, give it a material, and place it at the origin
	Entity* floor = mSceneMgr->createEntity("Floor", "floor");
	floor->setMaterialName("Examples/BumpyMetal");
	mSceneMgr->getRootSceneNode()->attachObject(floor);
}

void OgreSample6App::createCubeMap()
{
	mCubeCamera = mSceneMgr->createCamera("CubeMapCamera");
	mCubeCamera->setFOVy(Degree(90));
	mCubeCamera->setAspectRatio(1);
	mCubeCamera->setFixedYawAxis(false);
	mCubeCamera->setNearClipDistance(5);

	TexturePtr tex = TextureManager::getSingleton().createManual("dyncubemap",ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,TEX_TYPE_CUBE_MAP,128,128,0,PF_R8G8B8,TU_RENDERTARGET);

	for (size_t i = 0;i<6;i++)
	{
		mTargets[i] = tex->getBuffer(i)->getRenderTarget();
		mTargets[i]->addViewport(mCubeCamera)->setOverlaysEnabled(false);
		mTargets[i]->addListener(this);
	}
}

void OgreSample6App::frameRenderingQueued(const FrameEvent &evt)
{
	mPivot->yaw(Radian(evt.timeSinceLastFrame));
	mFishSwim->addTime(evt.timeSinceLastFrame * 3.0f);
}

void OgreSample6App::preRenderTargetUpdate(const RenderTargetEvent &evt)
{
	mHead->setVisible(false);
	mCubeCamera->setOrientation(Quaternion::IDENTITY);
	if (evt.source == mTargets[0])
	{
		mCubeCamera->yaw(Degree(-90));
	} 
	else if (evt.source == mTargets[1])
	{
		mCubeCamera->yaw(Degree(90));
	}
	else if (evt.source == mTargets[2])
	{
		mCubeCamera->pitch(Degree(90));
	}
	else if (evt.source == mTargets[3])
	{
		mCubeCamera->pitch(Degree(-90));
	}
	else if (evt.source == mTargets[5])
	{
		mCubeCamera->yaw(Degree(180));
	}
}

void OgreSample6App::postRenderTargetUpdate(const Ogre::RenderTargetEvent &evt)
{
	mHead->setVisible(true);
}