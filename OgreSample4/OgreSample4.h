#ifndef _OGRESAMPLE4_
#define _OGRESAMPLE4_

#include "../CEGUIBase/CEGUIBaseApp.h"
using namespace Ogre;

#define NUM_ANIMS 13
#define CHAR_HEIGHT 5
#define CAM_HEIGHT 2
#define RUN_SPEED 27
#define TURN_SPEED 500.0f
#define ANIM_FADE_SPEED 7.5f
#define JUMP_ACCEL 30.0f
#define GRAVITY 90.0f

class SinbadCharacterController
{
private :
	enum AnimID
	{
		ANIM_IDLE_BASE,
		ANIM_IDLE_TOP,
		ANIM_RUN_BASE,
		ANIM_RUN_TOP,
		ANIM_HANDS_CLOSED,
		ANIM_HANDS_RELAXED,
		ANIM_DRAW_SWORDS,
		ANIM_SLICE_VERTICAL,
		ANIM_SLICE_HORIZONTAL,
		ANIM_DANCE,
		ANIM_JUMP_START,
		ANIM_JUMP_LOOP,
		ANIM_JUMP_END,
		ANIM_NONE
	};

public :
	SinbadCharacterController(Ogre::Camera * cam)
	{
		setupBody(cam->getSceneManager());
		setupCamera(cam);
		setupAnimations();
	}

	void addTime(Real deltaTime)
	{
		updateBody(deltaTime);
		updateAnimations(deltaTime);
		updateCamera(deltaTime);
	}
	void injectKeyDown(const OIS::KeyEvent& evt);
	void injectKeyUp(const OIS::KeyEvent& evt);
	void injectMouseMove(const OIS::MouseEvent& evt);
	void injectMouseDown(const OIS::MouseEvent& evt,OIS::MouseButtonID id);

private :
	void setupBody(SceneManager* sceneMgr);
	void setupAnimations();
	void setupCamera(Camera* cam);
	void updateBody(Real deltaTime);
	void updateAnimations(Real deltaTime);
	void fadeAnimations(Real deltaTime);
	void updateCamera(Real deltaTime);
	void updateCameraGoal(Real deltaYaw,Real deltaPitch,Real deltaZoom);
	void setBaseAnimation(AnimID id,bool reset = false);
	void setTopAnimation(AnimID id,bool reset = false);

	Camera* mCamera;
	SceneNode* mBodyNode;
	SceneNode* mCameraPivot;
	SceneNode* mCameraGoal;
	SceneNode* mCameraNode;
	Real mPivotPitch;
	Entity* mBodyEnt;
	Entity* mSword1;
	Entity* mSword2;
	RibbonTrail* mSwordTrail;
	AnimationState* mAnims[NUM_ANIMS];
	AnimID mBaseAnimID;
	AnimID mTopAnimID;
	bool mFadingIn[NUM_ANIMS];
	bool mFadingOut[NUM_ANIMS];
	bool mSwordsDrawn;
	Vector3 mKeyDirection;
	Vector3 mGoalDirection;
	Real mVerticalVelocity;
	Real mTimer;
};

class OgreSample4Listener : public CEGUIBaseListener
{
public :
	OgreSample4Listener(CEGUIBaseApp * app,Ogre::Root *root,Ogre::SceneManager* sm,Ogre::RenderWindow* rw, OgreBites::SdkCameraMan *cm,
		CEGUI::OgreRenderer *or) : CEGUIBaseListener(app,root,sm,rw,cm,or)
	{
		mChara = new SinbadCharacterController(mCameraMan->getCamera());
/*
		Ogre::Entity * ent = mSceneMgr->createEntity("阿拉伯人B身.mesh");
		ent->setMaterialName("阿拉伯人B身");
		Ogre::SceneNode * node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		node->attachObject(ent);
		mAnimationState = ent->getAnimationState("休闲");
		mAnimationState->setEnabled(true);
		mAnimationState->setLoop(true);*/

	}
	~OgreSample4Listener()
	{
		if(mChara)
			delete mChara;
	}
	bool frameStarted(const Ogre::FrameEvent& evt);
	bool keyPressed(const OIS::KeyEvent& arg);
	bool keyReleased(const OIS::KeyEvent& arg);
	virtual bool mouseMoved(const OIS::MouseEvent &arg);
	virtual bool mousePressed(const OIS::MouseEvent &arg,OIS::MouseButtonID id);

protected :
	SinbadCharacterController* mChara;
	Ogre::AnimationState * mAnimationState;
};

class OgreSample4App : public CEGUIBaseApp
{
public :
	void createScene();
	virtual void createFrameListener();
	~OgreSample4App()
	{
		MeshManager::getSingleton().remove("floor");
	}
};

#endif