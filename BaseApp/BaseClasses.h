#ifndef _BASECLASSES_
#define _BASECLASSES_

#include "resource.h"

#include <OgreRoot.h>
#include <OgreCamera.h>
#include <OgreConfigFile.h>
#include <OgreEntity.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>	
#include <OgreRenderWindow.h>
#include <OgreLogManager.h>
#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>
#include <Ogre.h>
#include <OgreCompositorLogic.h>
#include <SdkTrays.h>
#include <SdkCameraman.h>
#include <OgrePrerequisites.h>
#include <OgreSharedPtr.h>
#include <OgreCustomCompositionPass.h>
#include <OgreDataStream.h>
#include <OgreD3D9RenderSystem.h>
#include <OgreD3D9DeviceManager.h>
#include <OgreD3D9Device.h>
#include <OgreD3D9RenderWindow.h>
#include <OgreTerrainGroup.h>
#include <OgreTerrain.h>

#include <NxOgre.h>
#include <NxOgreOGRE3D.h>
//#include <NxOgreAddonCharacterController.h>
#include <Critter.h>
#include <NxMath.h>
#include <NxPhysics.h>
#include <NxCooking.h>

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }

class BaseListener:public Ogre::FrameListener,public OIS::KeyListener,public OIS::MouseListener,public Ogre::WindowEventListener,
	public OgreBites::SdkTrayListener
{
public:
	BaseListener(Ogre::Root *root,Ogre::SceneManager* sm,Ogre::RenderWindow* rw,OgreBites::SdkCameraMan* cm):mRoot(root),mSceneMgr(sm),mWindow(rw),
		mSDKTrayMgr(0),mCameraMan(cm),mParamsPanel(0),mInputMgr(0),mKeyboard(0),mMouse(0),mCursorWasVisible(false),mShutDown(false)
	{}
	~BaseListener()
	{
// 		SAFE_DELETE(mSDKTrayMgr);
// 		SAFE_DELETE(mCameraMan);
// 		SAFE_DELETE(mParamsPanel);
// 		Ogre::WindowEventUtilities::removeWindowEventListener(mWindow,this);
 		windowClosed(mWindow);
	}

	virtual void initialise();
	virtual void setSDKTray();
	virtual void doSome();
	virtual void createGUI();
	
	virtual bool frameStarted(const Ogre::FrameEvent& evt);
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
	virtual bool frameEnded(const Ogre::FrameEvent& evt);

	virtual bool keyPressed(const OIS::KeyEvent &arg);
	virtual bool keyReleased(const OIS::KeyEvent &arg);
	virtual OIS::Keyboard* getKeyboard() const 
	{
		return mKeyboard;
	}

	virtual bool mouseMoved(const OIS::MouseEvent &arg);
	virtual bool mousePressed(const OIS::MouseEvent &arg,OIS::MouseButtonID id);
	virtual bool mouseReleased(const OIS::MouseEvent &arg,OIS::MouseButtonID id);

	/*virtual void buttonHit(OgreBites::Button* button)=0;
	virtual void itemSelected(OgreBites::SelectMenu* menu)=0;
	virtual void labelHit(OgreBites::Label* label)=0;
	virtual void sliderMoved(OgreBites::Slider* slider)=0;
	virtual void checkBoxToggled(OgreBites::CheckBox* box)=0;
	virtual void okDialogClosed(const Ogre::DisplayString& message)=0;
	virtual void yesNoDialogClosed(const Ogre::DisplayString& question, bool yesHit)=0;*/
	virtual void windowMoved(Ogre::RenderWindow* rw);                
	virtual void windowResized(Ogre::RenderWindow* rw);
	virtual bool windowClosing(Ogre::RenderWindow* rw);
	virtual void windowClosed(Ogre::RenderWindow* rw);
	virtual void windowFocusChange(Ogre::RenderWindow* rw);

protected:
	Ogre::Root* mRoot;
	Ogre::SceneManager* mSceneMgr;
	Ogre::RenderWindow* mWindow;

	OgreBites::SdkTrayManager * mSDKTrayMgr;
	OgreBites::SdkCameraMan* mCameraMan;
	OgreBites::ParamsPanel* mParamsPanel;
	bool mCursorWasVisible;
	bool mShutDown;

	OIS::InputManager* mInputMgr;
	OIS::Keyboard* mKeyboard;
	OIS::Mouse* mMouse;
};

class BaseApp
{
public:
	BaseApp():mRoot(0),mWindow(0),mSceneMgr(0),mCameraMan(0),mResourcesCfg(""),
		mPluginsCfg("")
	{}
	~BaseApp()
	{
// #ifdef _DEBUG  /// δ֪bug.
// 		//SAFE_DELETE(mRoot);
// #endif
	}

	virtual void run();
	virtual bool setup();
	virtual bool configure();
	virtual void chooseSceneManager();
	virtual void createCamera();
	virtual void createViewport();
	virtual void loadResources();
	virtual void createScene();
	virtual void destroyScene();
	virtual void setupResources();
	virtual void createResourceListener();
	virtual void createFrameListener();
	virtual void createGUISystem();
	virtual void doSome();
	virtual void initialiseGUI();
	virtual void createGUI();
	virtual bool keyPressed(const OIS::KeyEvent &arg);
	virtual bool keyReleased(const OIS::KeyEvent &arg);
	virtual bool mouseMoved(const OIS::MouseEvent &arg);
	virtual bool mousePressed(const OIS::MouseEvent &arg,OIS::MouseButtonID id);
	virtual bool mouseReleased(const OIS::MouseEvent &arg,OIS::MouseButtonID id);

protected:
	Ogre::Root *mRoot;
	Ogre::RenderWindow *mWindow;
	Ogre::SceneManager *mSceneMgr;
	OgreBites::SdkCameraMan* mCameraMan;

	Ogre::String mResourcesCfg;
	Ogre::String mPluginsCfg;
};

#endif