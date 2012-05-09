#ifndef _OGRESAMPLE2_
#define _OGRESAMPLE2_
#include "../CEGUIBase/CEGUIBaseApp.h"

class OgreSample2App:public CEGUIBaseApp
{
public:
	~OgreSample2App()
	{
		Ogre::MeshManager::getSingleton().remove("floor");
	}
	void createScene();
	void frameRenderingQueued(const Ogre::FrameEvent& evt);
protected:
	Ogre::AnimationState * mAnimState;
};

#endif