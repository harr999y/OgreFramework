#include "ThirdApp.h"

void ThirdApp::createGUISystem()
{
	mGUIRenderer = &(CEGUI::OgreRenderer::bootstrapSystem());
	//MessageBox(NULL,L"孩子啊，GUI的问题",L"孩子啊，CEGUI真变态啊",MB_OK);

	CEGUI::SchemeManager::getSingleton().createAll("TaharezLook.scheme","schemes");
	CEGUI::MouseCursor::getSingleton().setImage("TaharezLook","MouseArrow");
}

void ThirdApp::initialiseGUI()
{
	//1.Initialise default resource groups
	CEGUI::Imageset::setDefaultResourceGroup("imagesets");
	CEGUI::Font::setDefaultResourceGroup("fonts");
	CEGUI::Scheme::setDefaultResourceGroup("schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeel");
	CEGUI::WindowManager::setDefaultResourceGroup("layouts");
	CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");

	//2. setup default group for validation schemas
    CEGUI::XMLParser* parser = CEGUI::System::getSingleton().getXMLParser();
    if (parser->isPropertyPresent("SchemaDefaultResourceGroup"))
        parser->setProperty("SchemaDefaultResourceGroup", "schemas");      

}

void ThirdListener::doSome()
{
	mRect = new SelectionRectangle("xxx");
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(mRect);
	
	mVolQuery = mSceneMgr->createPlaneBoundedVolumeQuery(PlaneBoundedVolumeList());
}