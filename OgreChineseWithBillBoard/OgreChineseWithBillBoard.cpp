#include "OgreChineseWithBillBoard.h"

using namespace Ogre;

void OgreChineseWithBillBoardApp::createScene()
{	
	//FontPtr font = FontManager::getSingleton().getByName("SdkTrays/Chinese");
	//font->load();
	mSceneMgr->setSkyBox(true, "Examples/SpaceSkyBox");
	for (int i = 0;i < 100;i++)
	{
		SceneNode * node = mSceneMgr->getRootSceneNode()->createChildSceneNode("s" + Ogre::StringConverter::toString(i));
		node->setPosition(Math::RangeRandom(-1000,1000),Math::RangeRandom(-1000,1000),Math::RangeRandom(-1000,1000));
		MovableText * mt = new MovableText(Ogre::StringConverter::toString(i),L"ò§çïÖ®Òí","SdkTrays/Chinese",30.0f);
		node->attachObject(mt);
	}
	mSceneMgr->setAmbientLight(ColourValue::White);
}