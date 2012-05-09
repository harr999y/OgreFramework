#include "Ogre.h"
#include "NxScene.h"
#include "NxRay.h"
#include "OgreTerrainGroup.h"

//std::pair<Ogre::SceneNode *,NxActor *> NodeActorPair;

using namespace Ogre;
class MyCameraController
{
private:
	/*
	NxRevoluteJoint * mJoint;
	NxSpringDesc * mSpringDesc;
	Ogre::SceneManager * mSceneMgr;
	NxRay mRay;
	NodeActorPair * mCaracter;
	NodeActorPair * mCamera;*/

	SceneNode * mCameraPivot;
	SceneNode * mCameraNode;
	SceneNode * mCameraOriginal;
	SceneNode * mBodyNode;
	Camera * mCamera;
	NxRaycastHit mHitPoint;
	NxScene * mNxScene;
	bool bViewRestore;
	float mPivotPitch;
	Ogre::TerrainGroup * mTerrainGroup;

public:
	//MyCameraController(Ogre::SceneManager * sceneMgr,NxScene * scene,NodeActorPair * caracter,NodeActorPair * camera) : mSceneMgr(sceneMgr), mNxScene(scene), mCamera(caracter)
	MyCameraController(Camera * camera,SceneNode * body,NxScene * scene,TerrainGroup * tg) : mCamera(camera), mBodyNode(body), mNxScene(scene), mTerrainGroup(tg), bViewRestore(false), mPivotPitch(0.0f)
	{
		setup();
	}

	void setup();
	void update(float deltaTime);
	void updateGoal(float pitchAngle,float yawAngle,float deltaZoom);

	SceneNode * getCameraPivot()
	{
		return mCameraPivot;
	}
};
