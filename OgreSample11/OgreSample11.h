#ifndef _OGRESAMPLE11_
#define _OGRESAMPLE11_

#include "../CEGUIBase/CEGUIBaseApp.h"

using namespace Ogre;

class OgreSample11App : public CEGUIBaseApp , public RenderTargetListener
{
public:
	OgreSample11App() : NUM_FISH(30),NUM_FISH_WAYPOINTS(10),FISH_PATH_LENGTH(200),FISH_SCALE(2)
	{}
	void createScene();
	void frameRenderingQueued(const Ogre::FrameEvent& evt);
	void preRenderTargetUpdate(const RenderTargetEvent& evt);
	void postRenderTargetUpdate(const RenderTargetEvent& evt);

protected:
	const unsigned int NUM_FISH;
	const unsigned int NUM_FISH_WAYPOINTS;
	const unsigned int FISH_PATH_LENGTH; 
	const Real FISH_SCALE;
	std::vector<Entity*> mSurfaceEnts;
	std::vector<Entity*> mSubmergedEnts;
	RenderTarget * mRefractionTarget;
	RenderTarget * mReflectionTarget;
	Plane mWaterPlane;
	Entity * mWater;
	std::vector<SceneNode *> mFishNodes;
	std::vector<AnimationState *> mFishAnimStates;
	std::vector<SimpleSpline> mFishSplines;
	Real mFishAnimTime;
};
#endif