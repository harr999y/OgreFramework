#ifndef _AUDIODEME_
#define _AUDIODEME_

#include "../CEGUIBase/CEGUIBaseApp.h"
#include "FMODSoundManager.h"

class AudioDemo : public CEGUIBaseApp
{
public:
	void createScene();
	void frameRenderingQueued(const Ogre::FrameEvent& evt);
};

#endif
