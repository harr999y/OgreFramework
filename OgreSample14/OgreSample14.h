#ifndef _OGRESAMPLE14_
#define _OGRESAMPLE14_

#include "../CEGUIBase/CEGUIBaseApp.h"

using namespace Ogre;

class OgreSample14App : public CEGUIBaseApp
{
public:
	void createScene();
	void frameRenderingQueued(const Ogre::FrameEvent& evt);
	void createLights();

protected:
	AnimationState * mGreenLightAnimState;
	AnimationState * mYellowLightAnimState;
};

#endif