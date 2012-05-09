#ifndef _OGRESAMPLE6_
#define _OGRESAMPLE6_

#include "../CEGUIBase/CEGUIBaseApp.h"

using namespace Ogre;
using namespace CEGUI;

class OgreSample6App:public CEGUIBaseApp,public RenderTargetListener
{
public:
	void frameRenderingQueued(const FrameEvent& evt);
	void preRenderTargetUpdate(const RenderTargetEvent& evt);
	void postRenderTargetUpdate(const Ogre::RenderTargetEvent& evt);
	void createScene();
	void createCubeMap();
	void cleanupScene();

protected:
	Entity* mHead;
	Camera* mCubeCamera;
	Ogre::RenderTarget * mTargets[6];
	SceneNode* mPivot;
	AnimationState* mFishSwim;
};

#endif