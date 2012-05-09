#ifndef _OGRESAMPLE1_
#define _OGRESAMPLE1_

#include "../CEGUIBase/CEGUIBaseApp.h"

class OgreSample1App : public CEGUIBaseApp
{
public :
	~OgreSample1App()
	{
		mPatchPass->setPolygonMode(Ogre::PM_SOLID);
		Ogre::MeshManager::getSingleton().remove(mPatch->getHandle());
	}

	struct PatchVertex
	{	
		float x,y,z;
		float nx,ny,nz;
		float u,v;
	};

	void createScene();
	void createGUI();
	Ogre::PatchMeshPtr mPatch;
	Ogre::Pass * mPatchPass;

	bool handleSlider(const CEGUI::EventArgs & args);
	bool updateGUI(const CEGUI::EventArgs& args);
};



#endif