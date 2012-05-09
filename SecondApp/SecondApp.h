#ifndef _SENCOND_
#define _SENCOND_

#include "../BaseApp/BaseClasses.h"
#include <CEGUISystem.h>
#include <CEGUISchemeManager.h>
#include <CEGUIOgreRenderer.h>
#include <CEGUIImageset.h>
#include <CEGUIDefaultResourceProvider.h>
#include <CEGUIFont.h>
#include <falagard/CEGUIFalWidgetLookManager.h>
#include <CEGUIWindowManager.h>
#include <CEGUIScriptModule.h>
#include <CEGUIXMLParser.h>

class SecondApp:public BaseApp
{
protected:
	CEGUI::OgreRenderer* mGUIRenderer;

public:
	SecondApp():mGUIRenderer(0)
	{}
	~SecondApp()
	{
		/*if(mGUISystem)
			mGUISystem->destroySingletons();
		if(mGUIRenderer)
			mGUIRenderer->destroySystem*/
	}

	void createFrameListener();
	void createScene();
	void createGUISystem();
	void doSome();
	void initialiseGUI();
};

class SecondListener:public BaseListener
{
protected:
	Ogre::RaySceneQuery * mRaySceneQuery;
	bool mLMouseDown,mRMouseDown;
	int mCount;
	float mMoveSpeed;
	float mRotateSpeed;
	Ogre::SceneNode * mCurrentNode;
	CEGUI::Renderer * mGUIRenderer;
	Ogre::RaySceneQueryResult * mRayQueryResult;

public:
	SecondListener(Ogre::Root *root,Ogre::SceneManager* sm,Ogre::RenderWindow* rw,OgreBites::SdkCameraMan* cm,CEGUI::Renderer* rd)
		:BaseListener(root,sm,rw,cm),mGUIRenderer(rd),mLMouseDown(false),mRMouseDown(false),mCurrentNode(0),mCount(0),mMoveSpeed(350.f),
		mRotateSpeed(0.3f),mRaySceneQuery(0),mRayQueryResult(0)
	{}
	~SecondListener()
	{
		mSceneMgr->destroyQuery(mRaySceneQuery);
	}
	bool mouseMoved(const OIS::MouseEvent & arg);
	bool mousePressed(const OIS::MouseEvent & arg,OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent & arg,OIS::MouseButtonID id);
	bool frameStarted(const Ogre::FrameEvent &evt);
	void setGUI();
	void doSome();
	bool initialiseGUI();

	enum QueryTypeFlags
	{
		ACTIVITIES_MASK = 1<<0
	};
};

#endif