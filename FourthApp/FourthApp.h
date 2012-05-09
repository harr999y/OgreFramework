#ifndef _FOURTH_
#define _FOURTH_

#include "../BaseApp/BaseClasses.h"

class FourthApp:public BaseApp
{
public:
	void createScene();
	void createGrassMesh();

protected:
	Ogre::MeshPtr mGrassMesh;
};

#endif