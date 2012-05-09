#ifndef _NXOGRESAMPLE_SOFTBODY_
#define _NXOGRESAMPLE_SOFTBODY_

#include "../CEGUIBase/CEGUIBaseApp.h"

class NxOgreSample_SoftBodyApp : public CEGUIBaseApp
{
public:
	NxOgreSample_SoftBodyApp() : mSelectedVertex(-1)
	{}

	void createScene();
	void setupPhysX();
	void setupScene();
	void frameRenderingQueued(const Ogre::FrameEvent& evt);
	bool mousePressed(const OIS::MouseEvent &arg,OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &arg,OIS::MouseButtonID id);
	bool keyPressed(const OIS::KeyEvent &arg);
	void createFrameListener();

private:
	NxOgre::World * mWorld;
	NxOgre::Scene * mScene;
	NxOgre::SoftBody * mSoftBody;
	//Critter::Body * m
	Critter::RenderSystem * mRenderSystem;

	unsigned mSelectedVertex;
};

class NxOgreSample_SoftBodyListener : public CEGUIBaseListener
{
public:
	NxOgreSample_SoftBodyListener(NxOgreSample_SoftBodyApp * app,Ogre::Root *root,Ogre::SceneManager* sm,Ogre::RenderWindow* rw, OgreBites::SdkCameraMan *cm,
		CEGUI::OgreRenderer *or) : CEGUIBaseListener(app,root,sm,rw,cm,or)
	{}
	bool mousePressed(const OIS::MouseEvent &arg,OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &arg,OIS::MouseButtonID id);
	bool keyPressed(const OIS::KeyEvent &arg);
};


#endif