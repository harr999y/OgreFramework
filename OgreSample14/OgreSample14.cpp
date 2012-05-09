#include "OgreSample14.h"

void OgreSample14App::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	mGreenLightAnimState->addTime(evt.timeSinceLastFrame);
	mYellowLightAnimState->addTime(evt.timeSinceLastFrame);
}

void OgreSample14App::createScene()
{
	Entity* head = mSceneMgr->createEntity("Head", "ogrehead.mesh");
	mSceneMgr->getRootSceneNode()->attachObject(head);
	createLights();
}

void OgreSample14App::createLights()
{
	mSceneMgr->setAmbientLight(ColourValue(0.1f, 0.1f, 0.1f));

	NameValuePairList params;
	params["numberOfChains"] = "2";
	params["maxElements"] = "80";
	RibbonTrail * trail = (RibbonTrail*)mSceneMgr->createMovableObject("RibbonTrail",&params);
	mSceneMgr->getRootSceneNode()->attachObject(trail);
	trail->setMaterialName("Examples/LightRibbonTrail");
	trail->setTrailLength(400);

	SceneNode* node;
	Animation* anim;
	NodeAnimationTrack* track;
	Light* light;
	BillboardSet* bbs;

	// create a light node
	node = mSceneMgr->getRootSceneNode()->createChildSceneNode(Vector3(50, 30, 0));

	// create a 14 second animation with spline interpolation
	anim = mSceneMgr->createAnimation("Path1", 14);
	anim->setInterpolationMode(Animation::IM_SPLINE);

	track = anim->createNodeTrack(1, node);  // create a node track for our animation

	// enter keyframes for our track to define a path for the light to follow
	track->createNodeKeyFrame(0)->setTranslate(Vector3(50, 30, 0));
	track->createNodeKeyFrame(2)->setTranslate(Vector3(100, -30, 0));
	track->createNodeKeyFrame(4)->setTranslate(Vector3(120, -80, 150));
	track->createNodeKeyFrame(6)->setTranslate(Vector3(30, -80, 50));
	track->createNodeKeyFrame(8)->setTranslate(Vector3(-50, 30, -50));
	track->createNodeKeyFrame(10)->setTranslate(Vector3(-150, -20, -100));
	track->createNodeKeyFrame(12)->setTranslate(Vector3(-50, -30, 0));
	track->createNodeKeyFrame(14)->setTranslate(Vector3(50, 30, 0));

	// create an animation state from the animation and enable it
	mYellowLightAnimState = mSceneMgr->createAnimationState("Path1");
	mYellowLightAnimState->setEnabled(true);

	// set initial settings for the ribbon trail and add the light node
	trail->setInitialColour(0, 1.0f, 0.8f, 0);
	trail->setColourChange(0, 0.5f, 0.5f, 0.5f, 0.5f);
	trail->setInitialWidth(0, 5);
	trail->addNode(node);

	// attach a light with the same colour to the light node
	light = mSceneMgr->createLight();
	light->setDiffuseColour(trail->getInitialColour(0));
	node->attachObject(light);

	// attach a flare with the same colour to the light node
	bbs = mSceneMgr->createBillboardSet(1);
	bbs->createBillboard(Vector3::ZERO, trail->getInitialColour(0));
	bbs->setMaterialName("Examples/Flare");
	node->attachObject(bbs);

	// create a second light node
	node = mSceneMgr->getRootSceneNode()->createChildSceneNode(Vector3(-50, 100, 0));

	// create a 10 second animation with spline interpolation
	anim = mSceneMgr->createAnimation("Path2", 10);
	anim->setInterpolationMode(Animation::IM_SPLINE);

	track = anim->createNodeTrack(1, node);  // create a node track for our animation

	// enter keyframes for our track to define a path for the light to follow
	track->createNodeKeyFrame(0)->setTranslate(Vector3(-50, 100, 0));
	track->createNodeKeyFrame(2)->setTranslate(Vector3(-100, 150, -30));
	track->createNodeKeyFrame(4)->setTranslate(Vector3(-200, 0, 40));
	track->createNodeKeyFrame(6)->setTranslate(Vector3(0, -150, 70));
	track->createNodeKeyFrame(8)->setTranslate(Vector3(50, 0, 30));
	track->createNodeKeyFrame(10)->setTranslate(Vector3(-50, 100, 0));

	// create an animation state from the animation and enable it
	mGreenLightAnimState = mSceneMgr->createAnimationState("Path2");
	mGreenLightAnimState->setEnabled(true);

	trail->setInitialColour(1,0.0f,1.0f,0.4f);
	trail->setColourChange(1,0.5f,0.5f,0.5f,0.5f);
	trail->addNode(node);

	// attach a light with the same colour to the light node
	light = mSceneMgr->createLight();
	light->setDiffuseColour(trail->getInitialColour(1));
	node->attachObject(light);

	// attach a flare with the same colour to the light node
	bbs = mSceneMgr->createBillboardSet(1);
	bbs->createBillboard(Vector3::ZERO, trail->getInitialColour(1));
	bbs->setMaterialName("Examples/Flare");
	node->attachObject(bbs);
}