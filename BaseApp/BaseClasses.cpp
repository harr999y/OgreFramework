#include "BaseClasses.h"
//#include <crtdbg.h>

using namespace Ogre;

void BaseApp::run()
{
	mResourcesCfg = "../config/resources.cfg";

#ifdef _DEBUG
	mPluginsCfg = "../config/plugins_d.cfg";
#else
	mPluginsCfg = "../config/plugins.cfg";
#endif

	if(!setup())
		return;
	
	mRoot->startRendering();

	destroyScene();
}

void BaseApp::chooseSceneManager()
{
	mSceneMgr = mRoot->createSceneManager(Ogre::ST_GENERIC);
}

bool BaseApp::configure()
{
	if(!mRoot->restoreConfig())
	{
		if(!mRoot->showConfigDialog())
			return false;
	}
	mWindow = mRoot->initialise(true,"OGRE_Framework Render Window");
		
// 	HWND hwnd;
// 	mWindow->getCustomAttribute("Window",(void *)&hwnd);
// 	LONG iconID = (LONG)LoadIcon(GetModuleHandle(0),MAKEINTRESOURCE(IDI_APPICON));
// 	SetClassLong(hwnd,GCL_HICONSM,iconID);

	return true;
}

void BaseApp::createCamera()
{
	Ogre::Camera* cam = mSceneMgr->createCamera("PlayerCam");
	cam->setPosition(0,0,0);
	cam->lookAt(0,0,0);
	//cam->setAutoAspectRatio(true);
	cam->setNearClipDistance(1);
	cam->setFarClipDistance(10000);
	mCameraMan = new OgreBites::SdkCameraMan(cam);
	mCameraMan->setTopSpeed(1000);
}

void BaseApp::createFrameListener()
{	
	BaseListener * base = new BaseListener(mRoot,mSceneMgr,mWindow,mCameraMan);
	base->initialise();
}

void BaseApp::createViewport()
{
	Ogre::Camera* cam = mSceneMgr->getCamera("PlayerCam");
	Ogre::Viewport * vp = mWindow->addViewport(cam);
	vp->setBackgroundColour(ColourValue(0.0f,0.0f,0.0f,0.0f));
	cam->setAspectRatio(Ogre::Real(vp->getActualWidth())/Ogre::Real(vp->getActualHeight()));
}

void BaseApp::createResourceListener()
{}

void BaseApp::createScene()
{}

void BaseApp::destroyScene()
{}

void BaseApp::loadResources()
{
	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();	
}

void BaseApp::setupResources()
{
	ConfigFile cf;
	cf.load(mResourcesCfg);

	Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

	String secName,typeName,archName;
	while(seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		ConfigFile::SettingsMultiMap * settings = seci.getNext();
		ConfigFile::SettingsMultiMap::iterator i;
		for(i=settings->begin();i!=settings->end();++i)
		{
			typeName = i->first;
			archName = i->second;
			ResourceGroupManager::getSingleton().addResourceLocation(archName,typeName,secName);
		}
	}
}

void BaseApp::createGUISystem()
{}

void BaseApp::doSome()
{}

void BaseApp::initialiseGUI()
{}

void BaseApp::createGUI()
{}

bool BaseApp::setup()
{
	mRoot = new Root(mPluginsCfg);
	setupResources();
	if(!configure())
		return false;
	loadResources();
	//assert(_CrtCheckMemory()); 
	chooseSceneManager();
	createCamera();
	createViewport();
	createGUISystem();

	doSome();

	createFrameListener();
	
	TextureManager::getSingleton().setDefaultNumMipmaps(5);

	createScene();

	return true;
}

void BaseListener::initialise()
{
	Ogre::LogManager::getSingleton().logMessage("Being Iniltilizing OIS");
	OIS::ParamList pl;
	size_t windowHnd  = 0;
	std::ostringstream windowHndStr;

	mWindow->getCustomAttribute("WINDOW",&windowHnd);
	windowHndStr << windowHnd;
	pl.insert(std::make_pair(std::string("WINDOW"),windowHndStr.str()));

	mInputMgr = OIS::InputManager::createInputSystem(pl);

	mKeyboard = static_cast<OIS::Keyboard*> (mInputMgr->createInputObject(OIS::OISKeyboard,true));
	mMouse = static_cast<OIS::Mouse*> (mInputMgr->createInputObject(OIS::OISMouse,true));

	mMouse->setEventCallback(this);
	mKeyboard->setEventCallback(this);

	windowResized(mWindow);

	Ogre::WindowEventUtilities::addWindowEventListener(mWindow,this);

	//setSDKTray();

	doSome();

	mRoot->addFrameListener(this);
}

void BaseListener::setSDKTray()
{
	mSDKTrayMgr = new OgreBites::SdkTrayManager("InterfaceName",mWindow,mMouse,this);
	mSDKTrayMgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);
	mSDKTrayMgr->hideCursor();
	
	Ogre::DisplayStringVector items;
	items.push_back(L"摄像机.pX");
	items.push_back(L"摄像机.pY");
	items.push_back(L"摄像机.pZ");
	items.push_back("");
	items.push_back(L"摄像机.oW");
	items.push_back(L"摄像机.oX");
	items.push_back(L"摄像机.oY");
	items.push_back(L"摄像机.oZ");
	items.push_back("");
	items.push_back(L"纹理过滤方式");
	items.push_back(L"渲染模式");

	mParamsPanel = mSDKTrayMgr->createParamsPanel(OgreBites::TL_NONE,"DetailsPanel",200,items);
	mParamsPanel->setParamValue(9,"Bilinear");
	mParamsPanel->setParamValue(10,"Solid");
	mParamsPanel->hide();
}

void BaseListener::createGUI()
{}

bool BaseListener::frameEnded(const Ogre::FrameEvent &evt)
{
	return true;
}

void BaseListener::doSome()
{}

bool BaseListener::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
	if(mWindow->isClosed())
		return false;
	if(mShutDown)
		return false;

	mMouse->capture();
	mKeyboard->capture();

	mSDKTrayMgr->frameRenderingQueued(evt);

	if(!mSDKTrayMgr->isDialogVisible())
	{
		mCameraMan->frameRenderingQueued(evt);
		if(mParamsPanel->isVisible())
		{
			mParamsPanel->setParamValue(0,Ogre::StringConverter::toString(mCameraMan->getCamera()->getDerivedPosition().x));
			mParamsPanel->setParamValue(1,Ogre::StringConverter::toString(mCameraMan->getCamera()->getDerivedPosition().y));
			mParamsPanel->setParamValue(2,Ogre::StringConverter::toString(mCameraMan->getCamera()->getDerivedPosition().z));
			mParamsPanel->setParamValue(4,Ogre::StringConverter::toString(mCameraMan->getCamera()->getDerivedOrientation().w));
			mParamsPanel->setParamValue(5,Ogre::StringConverter::toString(mCameraMan->getCamera()->getDerivedOrientation().x));
			mParamsPanel->setParamValue(6,Ogre::StringConverter::toString(mCameraMan->getCamera()->getDerivedOrientation().y));
			mParamsPanel->setParamValue(7,Ogre::StringConverter::toString(mCameraMan->getCamera()->getDerivedOrientation().z));
		}
	}
	return true;
}

bool BaseListener::frameStarted(const Ogre::FrameEvent &evt)
{
	return true;
}

bool BaseListener::keyPressed(const OIS::KeyEvent &arg)
{
	if(mSDKTrayMgr->isDialogVisible())
		return true;

	if(arg.key == OIS::KC_F)
		mSDKTrayMgr->toggleAdvancedFrameStats();
	else if(arg.key == OIS::KC_G)
	{
		if(mParamsPanel->getTrayLocation() == OgreBites::TL_NONE)
		{
			mSDKTrayMgr->moveWidgetToTray(mParamsPanel,OgreBites::TL_TOPRIGHT,0);
			mParamsPanel->show();
		}
		else
		{
			mSDKTrayMgr->removeWidgetFromTray(mParamsPanel);
			mParamsPanel->hide();
		}
	}
	else if(arg.key == OIS::KC_T)
	{
		Ogre::String newVal;
		Ogre::TextureFilterOptions tfo;
		unsigned int aniso;

		switch(mParamsPanel->getParamValue(9).asUTF8()[0])
		{
		case 'B':
			newVal = "Trilinear";
			tfo = Ogre::TFO_TRILINEAR;
			aniso = 1;
			break;
		case 'T':
			newVal = "Anisotropic";
			tfo = Ogre::TFO_ANISOTROPIC;
			aniso = 8;
			break;
		case 'A':
			newVal = "None";
			tfo = Ogre::TFO_NONE;
			aniso =1;
			break;
		default:
			newVal = "Bilinear";
			tfo = Ogre::TFO_BILINEAR;
			aniso = 1;
		}

		Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(tfo);
		Ogre::MaterialManager::getSingleton().setDefaultAnisotropy(aniso);
		mParamsPanel->setParamValue(9,newVal);
	}
	else if (arg.key == OIS::KC_R)
	{
		Ogre::String newVal;
		Ogre::PolygonMode pm;

		switch (mSceneMgr->getCamera("PlayerCam")->getPolygonMode())
		{
		case Ogre::PM_SOLID:
			newVal ="Wireframe";
			pm = Ogre::PM_WIREFRAME;
			break;
		case Ogre::PM_WIREFRAME:
			newVal = "Points"; 
			pm = Ogre::PM_POINTS;
			break;
		default:
			newVal = "Solid";
			pm = Ogre::PM_SOLID;
		}

		mSceneMgr->getCamera("PlayerCam")->setPolygonMode(pm);
		mParamsPanel->setParamValue(10,newVal);
	}
	else if(arg.key==OIS::KC_F5)
	{
		Ogre::TextureManager::getSingleton().reloadAll();
	}
	else if(arg.key == OIS::KC_SYSRQ)
	{
		mWindow->writeContentsToTimestampedFile("screenshot",".jpg");
	}
	else if(arg.key == OIS::KC_ESCAPE)
	{
		mShutDown = true;
		return true;
	}

	mCameraMan->injectKeyDown(arg);
	return true;
}

bool BaseListener::keyReleased(const OIS::KeyEvent &arg)
{
	mCameraMan->injectKeyUp(arg);
	return true;
}

bool BaseListener::mouseMoved(const OIS::MouseEvent &arg)
{
	if(mSDKTrayMgr->injectMouseMove(arg))
		return true;
	mCameraMan->injectMouseMove(arg);
	return true;
}

bool BaseListener::mousePressed(const OIS::MouseEvent &arg,OIS::MouseButtonID id)
{
	if (mSDKTrayMgr->injectMouseDown(arg,id))
		return true;
	mCameraMan->injectMouseDown(arg,id);
	return true;
}

bool BaseListener::mouseReleased(const OIS::MouseEvent &arg,OIS::MouseButtonID id)
{
	if(mSDKTrayMgr->injectMouseUp(arg,id))
		return true;
	mCameraMan->injectMouseUp(arg,id);
	return true;
}

void BaseListener::windowResized(Ogre::RenderWindow* rw)
{
	unsigned int width,height,depth;
	int left,top;
	rw->getMetrics(width,height,depth,left,top);

	const OIS::MouseState &ms=mMouse->getMouseState();
	ms.width = width;
	ms.height = height;
}

void BaseListener::windowClosed(Ogre::RenderWindow* rw)
{
	if(rw == mWindow)
	{
		if(mInputMgr)
		{
			mInputMgr->destroyInputObject(mMouse);
			mInputMgr->destroyInputObject(mKeyboard);

			OIS::InputManager::destroyInputSystem(mInputMgr);
			mInputMgr=0;
		}
	}
}

void BaseListener::windowMoved(Ogre::RenderWindow* rw)
{}

bool BaseListener::windowClosing(Ogre::RenderWindow* rw)
{
	return true;
}

void BaseListener::windowFocusChange(Ogre::RenderWindow* rw)
{}

bool BaseApp::keyPressed(const OIS::KeyEvent &arg)
{
	return true;
}

bool BaseApp::keyReleased(const OIS::KeyEvent &arg)
{
	return true;
}

bool BaseApp::mouseMoved(const OIS::MouseEvent &arg)
{
	return true;
}

bool BaseApp::mousePressed(const OIS::MouseEvent &arg,OIS::MouseButtonID id)
{
	return true;
}

bool BaseApp::mouseReleased(const OIS::MouseEvent &arg,OIS::MouseButtonID id)
{
	return true;
}