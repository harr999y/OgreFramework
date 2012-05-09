#ifndef _OGRESAMPLE9_
#define _OGRESAMPLE9_

#include "../CEGUIBase/CEGUIBaseApp.h"

using namespace Ogre;

class OgreSample9App : public CEGUIBaseApp
{
protected:
	SceneNode * mPenguinNode;
	AnimationState * mPenguinAnimState;

public:
	void frameRenderingQueued(const Ogre::FrameEvent& evt);
	void createScene();
	void createFrameListener();
};

class OgreSample9Listener : public CEGUIBaseListener
{
public:
	OgreSample9Listener(CEGUIBaseApp * app,Ogre::Root *root,Ogre::SceneManager* sm,Ogre::RenderWindow* rw, OgreBites::SdkCameraMan *cm,CEGUI::OgreRenderer *or)  : CEGUIBaseListener(app,root,sm,rw,cm,or),TEXTURE_SIZE(128),SQE_BRUSH_RADIUS(Math::Sqr(12)),mTexBuf(NULL),mPlaneSize(0.0f),mBrushPos(0.0f,0.f),mTimeSinceLastFreeze(0.0f),mWiping(false)
	{}
	bool frameRenderingQueued(const Ogre::FrameEvent& evt);
	void updateTexture(Ogre::uint8 freezeAmount);
	void doSome();
	bool mousePressed(const OIS::MouseEvent &arg,OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &arg,OIS::MouseButtonID id);
protected:
	const unsigned int TEXTURE_SIZE;
	const unsigned int SQE_BRUSH_RADIUS;
	HardwarePixelBufferSharedPtr mTexBuf;
	Real mPlaneSize;
	RaySceneQuery * mCursorQuery;
	Vector2 mBrushPos;
	Real mTimeSinceLastFreeze;
	bool mWiping;
};

#endif