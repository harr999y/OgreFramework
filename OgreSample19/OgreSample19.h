#ifndef _OGRESAMPLE19_
#define _OGRESAMPLE19_

#include "../CEGUIBase/CEGUIBaseApp.h"

using namespace Ogre;

class OgreSample19App : public CEGUIBaseApp
{
protected:
	const unsigned int NUM_MODELS;
	const Real ANIM_CHOP;
	std::vector<SceneNode*> mModelNodes;
	std::vector<AnimationState*> mAnimStates;
	std::vector<Real> mAnimSpeeds;

	Vector3 mSneakStartPos;
	Vector3 mSneakEndPos;

public:
	OgreSample19App() : NUM_MODELS(16) , ANIM_CHOP(8.042f)
	{}

	void frameRenderingQueued(const Ogre::FrameEvent& evt);
	void createScene();
	void tweakSneakAnim();
	void destroyScene();
};

#endif