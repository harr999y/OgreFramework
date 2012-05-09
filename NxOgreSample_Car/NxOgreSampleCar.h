#ifndef _NXOGRESAMPLE_CAR_
#define _NXOGRESAMPLE_CAR_

#include "../CEGUIBase/CEGUIBaseApp.h"
#include "NxWheelDesc.h"

class NxOgreSampleCarApp : public CEGUIBaseApp
{
public:
	NxOgreSampleCarApp() : mMat(NULL), mSteerAngle(0.0)
	{}

	void createScene();
	void frameRenderingQueued(const Ogre::FrameEvent& evt);
	void setupPhysX();
	void createCar();
	NxWheelShape * attachCarWheel(NxWheelDesc & wheelDesc,bool left);
	void createTerrain();

protected:
	NxOgre::World * mWorld;
	NxOgre::Scene * mScene;
	Critter::RenderSystem * mRenderSystem;
	Critter::Body * mCar;
	Ogre::TerrainGroup * mTerrainGroup;
	Ogre::TerrainGlobalOptions * mTerrainGlobals;

	NxWheelShape * wheelLeftFront;  
	NxWheelShape * wheelRightFront;
	NxWheelShape * wheelLeftRear;
	NxWheelShape * wheelRightRear;

	NxMaterial * mMat;

	NxReal mSteerAngle;
};

#endif