#ifndef _NXOGRESAMPLETERRAIN_
#define _NXOGRESAMPLETERRAIN_

#include "../CEGUIBase/CEGUIBaseApp.h"
#include "NxTruck.h"

class NxOgreSample_TerrainApp : public CEGUIBaseApp
{
public:
	void createScene();
	void frameRenderingQueued(const Ogre::FrameEvent& evt);
	void setupPhysX();
	void destroyScene();
	virtual bool keyPressed(const OIS::KeyEvent &arg);
	void createFrameListener();
protected:
	Critter::RenderSystem * mRenderSystem;
	Ogre::Light * mLight;
	Ogre::TerrainGroup * mTerrainGroup;
	Ogre::TerrainGlobalOptions * mTerrainGlobals;
	NxOgre::World * mWorld;
	NxOgre::Scene * mScene;
	Truck * mTruck;
};

class NxOgreSample_TerrainListener : public CEGUIBaseListener
{
public:
	NxOgreSample_TerrainListener(NxOgreSample_TerrainApp * app,Ogre::Root *root,Ogre::SceneManager* sm,Ogre::RenderWindow* rw, OgreBites::SdkCameraMan *cm,
		CEGUI::OgreRenderer *or) : CEGUIBaseListener(app,root,sm,rw,cm,or)
	{}
	bool keyPressed(const OIS::KeyEvent &arg);
};

#endif