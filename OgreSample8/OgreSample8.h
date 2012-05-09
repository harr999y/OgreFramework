#ifndef _OGRESAMPLE8_
#define _OGRESAMPLE8_

#include "../CEGUIBase/CEGUIBaseApp.h"

using namespace Ogre;
using namespace CEGUI;

class OgreSample8App : public CEGUIBaseApp
{
public:
	void createScene();
	void frameRenderingQueued(const Ogre::FrameEvent& evt);
	bool handleItemSelected1(const CEGUI::EventArgs& args);
	bool handleItemSelected2(const CEGUI::EventArgs& args);
	void createGUI();

protected:
	void loadResources();
	void setupModes();
	
	std::map<Ogre::String,Ogre::StringVector> mPossibilities;
	SceneNode * mObjectNode;
	SceneNode * mLightPivot1;
	SceneNode * mLightPivot2;
	CEGUI::Combobox * mMeshMenu;
	Combobox * mMaterialMenu;
};

#endif