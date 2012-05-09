#ifndef _CHARACTERCONTROLLER_
#define _CHARACTERCONTROLLER_

#include "Ogre.h"
#include "NxScene.h"
#include "windows.h"
#include "MyCameraController.h"
#include "MyCharacterController.h"
#include "../OgreChineseWithBillBoard/MovableText.h"
#include "OgreTerrainGroup.h"
#include "UserAllocator.h"
#include "CEGUI.h"

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
		if(!mJump)	
			return 0.0f;

		mJumpTime += elapsedTime;
		float h = -70.9*mJumpTime*mJumpTime + mV0*mJumpTime;
		float deltaH = h - mLastHeight;
/*
		CEGUI::Logger::getSingleton().logEvent(CEGUI::String("mLastHeight") + CEGUI::PropertyHelper::floatToString(mLastHeight));
		CEGUI::Logger::getSingleton().logEvent(CEGUI::String("currentHeight") + CEGUI::PropertyHelper::floatToString(h));
		CEGUI::Logger::getSingleton().logEvent(CEGUI::String("deltaHeight") + CEGUI::PropertyHelper::floatToString(deltaH));
		CEGUI::Logger::getSingleton().logEvent(CEGUI::String("\n"));*/

		mLastHeight = h;
		return deltaH;
	}

	bool isJumping()
	{
		return mJump;
	}
};

#define ANIM_FADE_SPEED 4.5f
#define RUN_SPEED 17
#define TURN_SPEED 500.0f

enum ANIMS
{
	STAND,
	ATTACK,
	DANCE,
	RUN,
	JUMPSTART,
	JUMPING,
	JUMPEND,
	JUMPENDRUN,
	HANDSCLOSED,

	ANIMS_NUM
};

class CharacterController 
{
private:
	Jump mJump;
	MyCameraController * mCameraController;
	MyCharacterController * mCharacterController;
	NxScene * mNxScene;
	Ogre::Camera * mCamera;
	Vector3 mKeyDirection;
	Vector3 mGoalDirection;
	std::vector<bool> mFadingIn;
	std::vector<bool> mFadingOut;
	StringVector mAnimationNames;
	unsigned mActiveAnimationID;
	Entity * mBodyEntity;
	SceneNode * mBodyNode;
	std::vector<AnimationState*> mAnimations;
	typedef map<String, unsigned>::type AnimationNameIDMap;
	AnimationNameIDMap mAnimationNameIDs;
	NxControllerManager * mCCManager;
	TerrainGroup * mTerrainGroup;
	float mTimer;
	float mAttackTimer;
	AnimationStateSet * mAnimationStateSet;
	Entity * mWeapon;
	Entity * mShield;
	RibbonTrail * mSwordTrail;
	String mWeaponName;
	String mShieldName;
	std::vector<bool> mUpperBodyInvalid;
	std::vector<bool> mLowerBodyInvalid;
	bool mIsShifting;
	
public:
	CharacterController(Ogre::Camera * camera,NxScene * scene,TerrainGroup * tg) : 
	  mNxScene(scene), 
	  mCamera(camera), 
	  mTerrainGroup(tg), 
	  mTimer(0.0f), 
	  mAttackTimer(0.0f),
	  mSwordTrail(NULL),
	  mIsShifting(false)
	{
		setup();
	}

	void setup();
	void setupBody();
	void setupCameraController();
	void setupAnimations();
	void setupConfigFile();
	void update(float deltaTime);
	void updateBody(float deltaTime);
	void updateAnimations(float deltaTime);
	void fadeAnimations(float deltaTime);

	void injectMouseMove(const OIS::MouseEvent &evt);
	void injectMousePressed(const OIS::MouseEvent &evt,OIS::MouseButtonID id);
	void injectMouseReleased(const OIS::MouseEvent &evt,OIS::MouseButtonID id);
	void injectKeyPressed(const OIS::KeyEvent &evt);
	void injectKeyReleased(const OIS::KeyEvent &evt);

	void changeAnimation(unsigned id,bool loop = false,bool notStopPrevious = false);
	void handleWeapon();
	void handleShield();

	void setUpperBonesState(unsigned id,bool valid);
	void setLowerBonesState(unsigned id,bool valid);
};

//////////////////////////////////////////////////////////////////////////
//·ÏÆú
/*
class Helpler
{
public:
	static int findStringVector(StringVector& sv,String string)
	{
		for (unsigned i=0;i < sv.size();i++)
		{
			if (sv[i] == string)
				return i;
		}
		return -1;
	}
};*/
//////////////////////////////////////////////////////////////////////////

#endif