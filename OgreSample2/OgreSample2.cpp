#include "OgreSample2.h"

void OgreSample2App::createScene()
{
	using namespace Ogre;
	// setup some basic lighting for our scene
	mSceneMgr->setAmbientLight(ColourValue(0.3f, 0.3f, 0.3f));
	mSceneMgr->createLight()->setPosition(20, 80, 50);

	mSceneMgr->setSkyBox(true, "Examples/MorningSkyBox");

	// create an ogre head entity and attach it to a node
	Entity* head = mSceneMgr->createEntity("Head", "ogrehead.mesh");
	SceneNode* headNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	headNode->attachObject(head);

	mCameraMan->setStyle(OgreBites::CS_MANUAL);
	mCameraMan->getCamera()->setAutoTracking(true,headNode);
	SceneNode * camNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	camNode->attachObject(mCameraMan->getCamera());

	Animation * anim = mSceneMgr->createAnimation("CameraTrack",10);
	anim->setInterpolationMode(Animation::IM_SPLINE);

	NodeAnimationTrack * track = anim->createNodeTrack(0,camNode);
	track->createNodeKeyFrame(0)->setTranslate(Vector3(200,0,0));
	track->createNodeKeyFrame(2.5)->setTranslate(Vector3(0, -50, 100));
	track->createNodeKeyFrame(5)->setTranslate(Vector3(-500, 100, 0));
	track->createNodeKeyFrame(7.5)->setTranslate(Vector3(0, 200, -300));
	track->createNodeKeyFrame(10)->setTranslate(Vector3(200, 0, 0));

	mAnimState = mSceneMgr->createAnimationState("CameraTrack");
	mAnimState->setEnabled(true);
	mAnimState->setLoop(true);
	CEGUI::MouseCursor::getSingleton().hide();
	mCameraMan->getCamera()->setPosition(0.0f,0.0f,0.f);
}

void OgreSample2App::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
	mAnimState->addTime(evt.timeSinceLastFrame);
}