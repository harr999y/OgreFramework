#include "../CEGUIBase/CEGUIBaseApp.h"
#include "../OgreChineseWithBillBoard/MovableText.h"
#include "MyCharacterController.h"
#include "UserAllocator.h"
#include "NxOgreGC.h"
//#include "MyCameraController.h"
#include "CharacterController.h"
#include "FMODSoundManager.h"

using namespace Ogre;

#define NUM_ANIMS 13
#define CHAR_HEIGHT 0.5
#define CAM_HEIGHT 0.5
#define RUN_SPEED 17
#define TURN_SPEED 500.0f
#define ANIM_FADE_SPEED 4.5f
#define JUMP_ACCEL 30.0f
#define GRAVITY 10.0f


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

public:
	class Jump
	{
	public:
		Jump() : mV0(0.0f), mJumpTime(0.0f), mJump(false), mLastHeight(0.0f)//, mJumpStartHeight(0.0f)
		{}

		float	mV0;
		float	mJumpTime;
		bool	mJump;
		float	mLastHeight;
		//float   mJumpStartHeight;

		void startJump(float v0)//,float jumpStartHeight)
		{
			if(mJump)	return;
			mJumpTime = 0.0f;
			mV0 = v0;
			mLastHeight = 0.0f;
			mJump = true;
			//mJumpStartHeight = jumpStartHeight;
		}
		void stopJump()
		{
			if(!mJump)	return;
				mJump = false;
		}
		float getHeight(float elapsedTime)
		{
			if(!mJump)	return 0.0f;
			mJumpTime += elapsedTime;
			float h = -70.9*mJumpTime*mJumpTime + mV0*mJumpTime;
			float deltaH = h - mLastHeight;
			mLastHeight = h;
			return deltaH;
		}
	};

public :
	SinbadCharacterController(Ogre::Camera * cam,NxScene * scene,Ogre::TerrainGroup * tg) : mScene(scene), mTerrainGroup(tg), /*mPitchLimit(false),*/ mCamera(cam), /*mLastPos(23),*/ mTransLeft(0.0f), mNeedRestore(false)
	{
		setupBody(cam->getSceneManager());
		setupCamera(cam);
		setupAnimations();
	}

	void update(Real deltaTime)
	{
		updateBody(deltaTime);
		updateAnimations(deltaTime);
		mCameraController->update(deltaTime);
	}

	void injectKeyPressed(const OIS::KeyEvent& evt);
	void injectKeyReleased(const OIS::KeyEvent& evt);
	void injectMouseMove(const OIS::MouseEvent& evt);
	void injectMousePressed(const OIS::MouseEvent& evt,OIS::MouseButtonID id);
	MyCharacterController * getController()
	{
		return mSinbadController;
	}

	//Return 0 if there is no need to restore,positive number to restore.
	float cameraPreviousTest(float translate = 0.0f,float yawAngle = 0.0f,float pitchAngle = 0.0f);

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
/*	SceneNode* mCameraPivot;
	SceneNode* mCameraOriginal;
	SceneNode* mCameraNode;*/

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

	NxControllerManager * mCCManager;
	MyCharacterController * mSinbadController;
	NxScene * mScene;
	Ogre::TerrainGroup * mTerrainGroup;

	Jump mJump;

	float mTransLeft;
	bool mNeedRestore;

	MyCameraController * mCameraController;
};

class OgitorSampleApp : public CEGUIBaseApp
{
	friend class OgitorSampleListener;

public:
	OgitorSampleApp() : 
	    mFly(false), 
	    mTerrainImported(true), 
	    mLoader(NULL),
		mChara(NULL),
		mSoundManager(NULL),
		mActiveCamera(NULL)
	{}

	~OgitorSampleApp()
	{
// 		SAFE_DELETE(mChara);
// 		SAFE_DELETE(mSoundManager);
// 		SAFE_DELETE(mLoader);
	}

	void createScene();
	void regCompositors();

	void frameRenderingQueued(const Ogre::FrameEvent& evt);
	bool keyPressed(const OIS::KeyEvent &arg);
	bool keyReleased(const OIS::KeyEvent &arg);
	bool mouseMoved(const OIS::MouseEvent &arg);
	bool mousePressed(const OIS::MouseEvent &arg,OIS::MouseButtonID id);
	void switchCamera(int idx);
	void createFrameListener();

	void playEvironSound();

private:
	std::vector<Ogre::String> mCamNames;
	DotSceneLoader * mLoader;
	bool mTerrainImported;
	bool mFly;
	Camera * mActiveCamera;
	//SinbadCharacterController * mChara;
	CharacterController * mChara;
	SoundManager * mSoundManager;
};


class OgitorSampleListener : public CEGUIBaseListener
{
public:
	OgitorSampleListener(CEGUIBaseApp * app,Ogre::Root *root,Ogre::SceneManager* sm,Ogre::RenderWindow* rw, OgreBites::SdkCameraMan *cm,
		CEGUI::OgreRenderer *or) : CEGUIBaseListener(app,root,sm,rw,cm,or)
	{}

	bool mouseMoved(const OIS::MouseEvent &arg);
	//bool mousePressed(const OIS::MouseEvent &arg,OIS::MouseButtonID id);
};
