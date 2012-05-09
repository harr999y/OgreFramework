#ifndef _HYDRAXSAMPLE1_
#define _HYDRAXSAMPLE1_

#include "../CEGUIBase/CEGUIBaseApp.h"

#define SKYNUM 3

class HydraxRttListener : public Hydrax::RttManager::RttListener
{
public:
	HydraxRttListener(SkyX::SkyX * skyx,Hydrax::Hydrax * hydrax) : mSkyX(skyx), mHydrax(hydrax)
	{}

	void preRenderTargetUpdate(const Hydrax::RttManager::RttType& Rtt);
	void postRenderTargetUpdate(const Hydrax::RttManager::RttType& Rtt);
	SkyX::SkyX * mSkyX;
	Hydrax::Hydrax * mHydrax;
};

class BloomListener : public Ogre::CompositorInstance::Listener
{
public:
	BloomListener(SkyX::SkyX * skyx) : mSkyX(skyx)
	{}

	void notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
	{}

	void notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);

	SkyX::SkyX * mSkyX;
};

class HydraxSample1App : public CEGUIBaseApp
{
protected:
	Hydrax::Hydrax * mHydrax;
	int mCurrentSkyBox;
	Ogre::RaySceneQuery * mRaySceneQuery;
	Ogre::TerrainGroup * mTerrainGroup;
	Ogre::TerrainGlobalOptions * mTerrainGlobals;
	SkyX::SkyX * mSkyX;
	SkyX::ColorGradient mWaterGradient;
	SkyX::ColorGradient mSunGradient;
	SkyX::ColorGradient mAmbientGradient;
	bool mForceDisableShadows;
	float mLastPositionLength;

public:
	HydraxSample1App() : mCurrentSkyBox(0)
	{}
	void createScene();
	void createHydraX();
	void createSkyX();
	void frameRenderingQueued(const Ogre::FrameEvent& evt)
	{}
	void frameStarted(const Ogre::FrameEvent& evt);
	void frameEnded(const Ogre::FrameEvent& evt)
	{}
	
	void createTerrain();
	void updateEnvironmentLighting();
	void updateShadowFarDistance();
	bool keyPressed(const OIS::KeyEvent &arg);
};

#endif