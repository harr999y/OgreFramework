#ifndef _NXOGRESAMPLECLOTHES_
#define _NXOGRESAMPLECLOTHES_

#include "../CEGUIBase/CEGUIBaseApp.h"

class NxOgreSample_ClothesApp : public CEGUIBaseApp
{
public:
	void createScene();
	void setupPhysX();
	void setupPipeScene();
	void frameRenderingQueued(const Ogre::FrameEvent& evt);
	void createFrameListener();
	bool mousePressed(const OIS::MouseEvent &arg,OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &arg,OIS::MouseButtonID id);
	void setupScene();
protected:
	NxOgre::World * mWorld;
	NxOgre::Scene * mScene;
	Critter::RenderSystem * mRenderSystem;
	unsigned int dragIndex;
	NxOgre::Cloth * mCloth;
	NxOgre::Cloth * mCloth2;
	Critter::Body * mMouseActor;
	NxOgre::Actor * mHitActor;
	NxActor * mHitActor2;
	NxDistanceJoint * mJoint;
};

class NxOgreSample_ClothesListener : public CEGUIBaseListener
{
public:
	NxOgreSample_ClothesListener(NxOgreSample_ClothesApp * app,Ogre::Root *root,Ogre::SceneManager* sm,Ogre::RenderWindow* rw, OgreBites::SdkCameraMan *cm,
		CEGUI::OgreRenderer *or) : CEGUIBaseListener(app,root,sm,rw,cm,or)
	{}
	bool mousePressed(const OIS::MouseEvent &arg,OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &arg,OIS::MouseButtonID id);
};

#endif