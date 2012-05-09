#include "OgreSample1.h"

void OgreSample1App::createScene()
{
	using namespace Ogre;
	// setup some basic lighting for our scene
	mSceneMgr->setAmbientLight(ColourValue(0.5, 0.5, 0.5));
	mSceneMgr->createLight()->setPosition(100, 100, 100);

	// define the control point vertices for our patch
	PatchVertex verts[9] =
	{
		{-50, -35, -50, -0.5, 0.5, 0.0, 0.0, 0.0},
		{  0,   0, -50,  0.0, 0.5, 0.0, 0.5, 0.0},
		{ 50,  35, -50,  0.5, 0.5, 0.0, 1.0, 0.0},
		{-50,   0,   0, -0.5, 0.5, 0.0, 0.0, 0.5},
		{  0,   0,   0,  0.0, 0.5, 0.0, 0.5, 0.5},
		{ 50,   0,   0,  0.5, 0.5, 0.0, 1.0, 0.5},
		{-50,  35,  50, -0.5, 0.5, 0.0, 0.0, 1.0},
		{  0,   0,  50,  0.0, 0.5, 0.0, 0.5, 1.0},
		{ 50, -35,  50,  0.5, 0.5, 0.0, 1.0, 1.0}
	};	

	Ogre::VertexDeclaration * decl = Ogre::HardwareBufferManager::getSingleton().createVertexDeclaration();
	decl->addElement(0,0,VET_FLOAT3,VES_POSITION);
	decl->addElement(0,sizeof(float)*3,VET_FLOAT3,VES_NORMAL);
	decl->addElement(0,sizeof(float)*6,VET_FLOAT2,VES_TEXTURE_COORDINATES,0);

	mPatch = Ogre::MeshManager::getSingleton().createBezierPatch("patch",Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,(float *)verts,decl,3,3,5,5,Ogre::PatchSurface::VS_BOTH);

	mPatch->setSubdivision(0);

	Ogre::Entity * ent = mSceneMgr->createEntity("Patch","patch");
	ent->setMaterialName("Examples/BumpyMetal");
	mSceneMgr->getRootSceneNode()->attachObject(ent);

	mPatchPass = ent->getSubEntity(0)->getMaterial()->getTechnique(0)->getPass(0);

	mCameraMan->setTopSpeed(100);
	Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);
}

void OgreSample1App::createGUI()
{
	using namespace CEGUI;
	CEGUI::WindowManager & winMgr = CEGUI::WindowManager::getSingleton();
	CEGUI::SchemeManager::getSingleton().create("TaharezLook.scheme");
	CEGUI::SchemeManager::getSingleton().create("WindowsLook.scheme");
	CEGUI::FontManager::getSingleton().create("chinese-26.font");
	//CEGUI::System::getSingleton().setDefaultFont("Chinese");
	CEGUI::System::getSingleton().setDefaultMouseCursor("WindowsLook","MouseArrow");
	mRootWindow->addChildWindow(winMgr.loadWindowLayout("OgreSample1.layout"));

	//CEGUI::System::getSingleton().setGUISheet(winMgr.getWindow("Root"));

	winMgr.getWindow("Root/Slider")->subscribeEvent(Slider::EventValueChanged,Event::Subscriber(&OgreSample1App::handleSlider,this));
	winMgr.getWindow("Root/Show/fps")->subscribeEvent(RenderingSurface::EventRenderQueueStarted,Event::Subscriber(&OgreSample1App::updateGUI,this));
	winMgr.getWindow("Root/Show/speed")->subscribeEvent(RenderingSurface::EventRenderQueueStarted,Event::Subscriber(&OgreSample1App::updateGUI,this));

	WindowManager::getSingleton().getWindow("Root/Show/speed")->setText("1.00");
}

bool OgreSample1App::updateGUI(const CEGUI::EventArgs &args)
{
	using namespace CEGUI;
	if (static_cast<const RenderQueueEventArgs&>(args).queueID != RQ_OVERLAY)
		return false;

	static int fps = 0;
	if(fps != mListener->getFps())
	{
		fps = mListener->getFps();
		WindowManager::getSingleton().getWindow("Root/Show/fps")->setText(PropertyHelper::intToString(fps));
	}
	if(mListener->getMouseSpeedChanged())
	{
		float speed = mListener->getMouseSpeed();
		char a[5] = "\0";
		sprintf(a,"%.2f",speed);
		WindowManager::getSingleton().getWindow("Root/Show/speed")->setText(a);
	}
	return true;
}

bool OgreSample1App::handleSlider(const CEGUI::EventArgs &args)
{
	using namespace CEGUI;
	Slider* window = static_cast<Slider *>(static_cast<const WindowEventArgs &>(args).window);
	float value = window->getCurrentValue();
	mPatch->setSubdivision(value);
	return true;
}
