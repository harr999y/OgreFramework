#ifndef _NXOGRESAMPLE_RIGIDBODIES_
#define _NXOGRESAMPLE_RIGIDBODIES_

#include "../CEGUIBase/CEGUIBaseApp.h"

class NxOgreSample_RigidBodiesApp : public CEGUIBaseApp
{
public:
	void createScene();
	void setupPhysX();
	void frameRenderingQueued(const Ogre::FrameEvent& evt);
	void destroyScene();


};

#endif