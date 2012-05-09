#ifndef _OGRESAMPLE16_
#define _OGRESAMPLE16_

#include "../CEGUIBase/CEGUIBaseApp.h"

class OgreSample16App : public CEGUIBaseApp
{
protected:
	Ogre::SceneNode * mNode;

public:
	void createScene();
	void frameRenderingQueued(const Ogre::FrameEvent& evt);
};

#endif