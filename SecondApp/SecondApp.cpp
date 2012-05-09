#include "SecondApp.h"

void SecondApp::createScene()
{
	// Set ambient light
	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));
	mSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);

	// World geometry
	mSceneMgr->setWorldGeometry("terrain.cfg");
	
}

void SecondApp::createFrameListener()
{
	SecondListener * listener = new SecondListener(mRoot,mSceneMgr,mWindow,mCameraMan,mGUIRenderer);
	listener->initialise();
}

void SecondApp::createGUISystem()
{
	mGUIRenderer = &(CEGUI::OgreRenderer::bootstrapSystem());
	initialiseGUI();

	CEGUI::SchemeManager::getSingleton().createAll("TaharezLook.scheme","schemes");
	CEGUI::MouseCursor::getSingleton().setImage("TaharezLook","MouseArrow");
}

void SecondApp::initialiseGUI()
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

void SecondApp::doSome()
{}

bool SecondListener::frameStarted(const Ogre::FrameEvent &evt)
{

	Ogre::Vector3 camPos = mCameraMan->getCamera()->getPosition();
	Ogre::Ray ray(Ogre::Vector3(camPos.x,6000.0f,camPos.z),Ogre::Vector3::NEGATIVE_UNIT_Y);
	mRaySceneQuery->setRay(ray);
	mRaySceneQuery->setQueryMask(0);
	//mRaySceneQuery->setQueryMask(ACTIVITIES_MASK);
	mRaySceneQuery->setSortByDistance(false);
	Ogre::RaySceneQueryResult &result = mRaySceneQuery->execute();
	Ogre::RaySceneQueryResult::iterator itr = result.begin();
	if(itr != result.end())
	{
		if(itr->worldFragment)
		{
			Ogre::Real height = itr->worldFragment->singleIntersection.y;
			if((height + 120.0f) != camPos.y)
				mCameraMan->getCamera()->setPosition(camPos.x,height+120.0f,camPos.z);
		}
	}

	mRaySceneQuery->setQueryMask(ACTIVITIES_MASK);
	CEGUI::Point point = CEGUI::MouseCursor::getSingleton().getPosition();
	ray = mCameraMan->getCamera()->getCameraToViewportRay(point.d_x/mWindow->getWidth(),point.d_y/mWindow->getHeight());
	mRaySceneQuery->setRay(ray);
	//mRaySceneQuery->setQueryTypeMask(ACTIVITIES_MASK);

	mRaySceneQuery->setSortByDistance(true);
	mRayQueryResult = &(mRaySceneQuery->execute());

	return true;
}

bool SecondListener::mouseMoved(const OIS::MouseEvent &arg)
{
	if(mLMouseDown)
	{
		if(mCurrentNode)
		{
			Ogre::RaySceneQueryResult::iterator itr = mRayQueryResult->begin();

			while(itr != mRayQueryResult->end())
			{
				if(itr->worldFragment)			
				{
					mCurrentNode->setPosition(itr->worldFragment->singleIntersection);
					break;
				}
				itr++;
			}
		}
	}
	else if(mRMouseDown)
	{
		mCameraMan->getCamera()->yaw(Ogre::Degree(-arg.state.X.rel * mRotateSpeed));
		mCameraMan->getCamera()->pitch(Ogre::Degree(-arg.state.Y.rel * mRotateSpeed));
	}

	CEGUI::System::getSingleton().injectMouseMove(arg.state.X.rel*1.6f,arg.state.Y.rel*1.6f);
	return true;
}

bool SecondListener::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if(id == OIS::MB_Left)
	{
		Ogre::RaySceneQueryResult::iterator itr = mRayQueryResult->begin();

		while(itr != mRayQueryResult->end())
		{			
			if(itr->movable && itr->movable->getQueryFlags() != 0xffffffff)	
			{
				mCurrentNode = itr->movable->getParentSceneNode();
				mCurrentNode->showBoundingBox(true);
				break;
			}
			itr++;
		}

		mLMouseDown = true;
	}
	else if(id == OIS::MB_Right)
	{
		mRMouseDown = true;
		CEGUI::MouseCursor::getSingleton().hide();
	}
	return true;
}

bool SecondListener::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if(id == OIS::MB_Left)
	{
		if(!mCurrentNode)
		{
			Ogre::RaySceneQueryResult::iterator itr = mRayQueryResult->begin();
			while(itr != mRayQueryResult->end())
			{
				if(itr->worldFragment)
				{
					char name[16];
					sprintf(name,"Robot%d",mCount++);

					Ogre::Entity * ent = mSceneMgr->createEntity(name,"robot.mesh");
					ent->setQueryFlags(ACTIVITIES_MASK);
					Ogre::SceneNode * node = mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::String(name),itr->worldFragment->singleIntersection);
					node->attachObject(ent);
					break;
				}
				itr++;
			}
		}
		mLMouseDown = false;
		if(mCurrentNode)
		{
			mCurrentNode->showBoundingBox(false);
			mCurrentNode = 0;
		}
	}
	else if(id == OIS::MB_Right)
	{
		mRMouseDown = false;
		CEGUI::MouseCursor::getSingleton().show();
	}

	return true;
}

void SecondListener::setGUI()
{}

void SecondListener::doSome()
{
	mRaySceneQuery = mSceneMgr->createRayQuery(Ogre::Ray());
	//mSceneMgr->setWorldGeometryRenderQueue();
	//mRaySceneQuery->setQueryMask(0x00000000);
	//mRaySceneQuery->setWorldFragmentType(Ogre::SceneQuery::WFT_SINGLE_INTERSECTION);
	//mRaySceneQuery->setQueryTypeMask(Ogre::SceneManager::USER_TYPE_MASK_LIMIT);
	mCameraMan->setTopSpeed(mMoveSpeed);
}