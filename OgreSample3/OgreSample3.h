#ifndef _OGRESAMPLE3_
#define _OGRESAMPLE3_

#include "../CEGUIBase/CEGUIBaseApp.h"

class OgreSample3App : public CEGUIBaseApp
{
public:
	void createScene();
	void frameRenderingQueued(const Ogre::FrameEvent& evt);

protected:
	enum ShaderParam 
	{
		SP_SHININESS = 1,
		SP_DIFFUSE,
		SP_SPECULAR
	};

	Ogre::SceneNode * mLightPivot;
};

#endif