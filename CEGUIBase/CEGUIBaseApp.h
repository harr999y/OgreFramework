#ifndef _CEGUIBASE_
#define _CEGUIBASE_

#include "../BaseApp/BaseClasses.h"

//CCEGUI
#include <CEGUISystem.h>
#include <CEGUISchemeManager.h>
#include <CEGUIOgreRenderer.h>
#include <CEGUIImageset.h>
#include <CEGUIDefaultResourceProvider.h>
#include <CEGUIFont.h>
#include <CEGUIWindowManager.h>
#include <CEGUIScriptModule.h>
#include <CEGUIXMLParser.h>
#include <CEGUIGeometryBuffer.h>
#include <CEGUIRenderingSurface.h>
#include <CEGUIRenderingRoot.h>
#include <CEGUIUDim.h>
#include <CEGUIFontManager.h>
#include <CEGUIBase.h>
#include <CEGUIPropertyHelper.h>
#include <CEGUIInputEvent.h>
#include <CEGUI.h>
#include <Hydrax.h>
#include <SkyX.h>
#include <PagedGeometryAll.h>
#include "DotSceneLoader.h"

//using namespace Ogre;

class CEGUIBaseApp;

#define DELTA_MOUSE_SPEED_CHANGED 0.05f

LRESULT CALLBACK ChineseHook(int nCode,WPARAM wParam,LPARAM lParam);
static HHOOK gHook = NULL;

class CEGUIRQListener : public Ogre::RenderQueueListener
{
public:
	CEGUIRQListener(CEGUI::OgreRenderer* renderer,Ogre::uint8 queueID,bool postQueue) :
	  mRenderer(renderer),mQueueID(queueID),mPostQueue(postQueue)
	  {}

	  virtual void renderQueueStarted(Ogre::uint8 queueGroupId, const Ogre::String& invocation, 
		  bool& skipThisInvocation)
	  {
		  if (!mPostQueue && mQueueID == queueGroupId)
		  {
			  CEGUI::System::getSingleton().renderGUI();
		  }
	  }

	  void setTargetRenderQueue(Ogre::uint8 queueID)
	  {
		  mQueueID = queueID;
	  }

	  void setPostRenderQueue(bool postQueue)
	  {
		  mPostQueue = postQueue;
	  }

protected:
	CEGUI::OgreRenderer * mRenderer;
	Ogre::uint8 mQueueID;
	bool mPostQueue;
};


class CEGUIBaseListener:public BaseListener
{
public:
	CEGUIBaseListener(CEGUIBaseApp * app,Ogre::Root *root,Ogre::SceneManager* sm,Ogre::RenderWindow* rw, OgreBites::SdkCameraMan *cm,
		CEGUI::OgreRenderer *or): BaseListener(root,sm,rw,cm),mOgreRenderer(or),mRMouseDown(false),mRotateSpeed(0.3f),mFPS(0),mApp(app),mDeltaSpeedMouseChanged(0.0f),mMouseSpeed(1.0f),mFps(0),mFpsGeometry(NULL),mFpsTime(0.0f),mSpeedGeometry(NULL),mMouseSpeedChanged(false),mLMouseDown(false),mCursorFollow(false),mLeftArrow(false),mRightArrow(false),mArrowTimePassed(0.0f),mMouseIn(true)
	{}
	
	~CEGUIBaseListener()
	{
		//if(mOverlay)
			//delete mOverlay;
	}

	enum CopyState
	{
		COPY,
		CUT,
		PASTE,
		ALL
	};

	virtual void initialise();
	virtual void doSome();
	virtual void createGUI();

	virtual int getFps() const; 
	virtual float getMouseSpeed() const;
	virtual bool getMouseSpeedChanged() const
	{
		return mMouseSpeedChanged;
	}

	virtual bool frameStarted(const Ogre::FrameEvent& evt);
	virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt);
	virtual bool frameEnded(const Ogre::FrameEvent& evt);

	virtual bool keyPressed(const OIS::KeyEvent &arg);
	virtual bool keyReleased(const OIS::KeyEvent &arg);

	virtual bool mouseMoved(const OIS::MouseEvent &arg);
	virtual bool mousePressed(const OIS::MouseEvent &arg,OIS::MouseButtonID id);
	virtual bool mouseReleased(const OIS::MouseEvent &arg,OIS::MouseButtonID id);

	virtual void windowResized(Ogre::RenderWindow* rw);

	virtual bool updateAll(const CEGUI::EventArgs& args);
	virtual void updateFps(const Ogre::FrameEvent &evt);
	virtual void updateSpeed(const Ogre::FrameEvent &evt);
	virtual void updateArrow(const Ogre::FrameEvent &evt);

	virtual void handleCopyPasteCut(CopyState copyState);

	//virtual void IMMFollow();

protected:
	Ogre::OverlayContainer * mOverlay;
	CEGUI::OgreRenderer * mOgreRenderer;

	CEGUI::MouseButton convertOISButtonToCegui(int buttonID);

	CEGUI::GeometryBuffer * mFpsGeometry;
	CEGUI::GeometryBuffer * mSpeedGeometry;

	int mFps;
	int mFPS;
	float mFpsTime;
	float mMouseSpeed;
	bool mMouseSpeedChanged;
	float mDeltaSpeedMouseChanged;

	bool mRMouseDown;
	bool mLMouseDown;
	float mRotateSpeed;

	bool mCursorFollow;

	bool mLeftArrow;
	bool mRightArrow;
	float mArrowTimePassed;

	bool mMouseIn;

	CEGUI::String mBuff;

	CEGUIBaseApp * mApp;
};

class CEGUIBaseApp:public BaseApp
{
public:
	CEGUIBaseApp(): mOgreRenderer(NULL),mListener(NULL),mRootWindow(NULL)
	{}
	~CEGUIBaseApp()
	{
		//UnhookWindowsHookEx(gHook);
		//mOgreRenderer->destroySystem();
	}

	virtual void createGUISystem();
	virtual void doSome();
	virtual void initialiseGUI();
	virtual void createGUI();
	virtual void createScene();
	virtual bool updateGUI(const CEGUI::EventArgs& args);
	virtual void createFrameListener();
	virtual void frameRenderingQueued(const Ogre::FrameEvent& evt);
	virtual void frameStarted(const Ogre::FrameEvent& evt);
	virtual void frameEnded(const Ogre::FrameEvent& evt);

protected:
	CEGUI::OgreRenderer * mOgreRenderer;
	CEGUIBaseListener * mListener;
	CEGUI::DefaultWindow * mRootWindow;
	CEGUIRQListener * mCEGUIRenderQueueListener;
};
 

#endif