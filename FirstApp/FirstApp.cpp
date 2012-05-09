#include "FirstApp.h"

void FirstApp::createFrameListener()
{
	using namespace Ogre;
	Entity *ent;
	SceneNode *node;

	ent = mSceneMgr->createEntity("robot","robot.mesh");
	node = mSceneMgr->getRootSceneNode( )->createChildSceneNode("robot",Ogre::Vector3(0.0f,0.0f,25.0f));
	node->attachObject(ent);

	FirstListener *listener=new FirstListener(mRoot,mSceneMgr,mWindow,mCameraMan,static_cast<Ogre::SceneNode *>(mSceneMgr->getRootSceneNode()->getChild("robot")),mSceneMgr->getEntity("robot"),mWalkList);
	listener->initialise();
}

void FirstListener::nextLocation()
{
	if(mWalkList.empty())
	{
		mWalking =false;
		m_bNeedWalk = false;
		return;
	}
	mDestination = mWalkList.front();
	mWalkList.pop_front();
	mDirection = mDestination - mSceneNode->getPosition();
	mDistance = mDirection.normalise();	

	mWalked = 0.0f;
	mWalking = true;

}

void FirstListener::doSome()
{
	/*mAnimationState = mEntity->getAnimationState("Idle");
	mAnimationState->setLoop(true);
	mAnimationState->setEnabled(true);*/

	mSDKTrayMgr->showCursor();
	mSDKTrayMgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);

	mWalkSpeed = 35;
	mDirection = Ogre::Vector3::ZERO;
	m_bNeedWalk = true;
}

/*bool First_Listener::mouseMoved(const OIS::MouseEvent &arg)
{
	mSDKTrayMgr->injectMouseMove(arg);
	return true;
}*/


bool FirstListener::mousePressed(const OIS::MouseEvent &arg,OIS::MouseButtonID id)	
{
	Ogre::Vector2 point(arg.state.X.abs,arg.state.Y.abs);
	Ogre::Ray ray = mCameraMan->getCamera()->getCameraToViewportRay(point.x/arg.state.width,point.y/arg.state.height);
	std::pair<bool,Ogre::Real> pr = ray.intersects(Ogre::Plane(Ogre::Vector3::UNIT_Y,0));
	
	if(pr.first)
	{
		Ogre::Vector3 pt = ray.getPoint(pr.second);
		mWalkList.clear();
		mWalkList.push_front(pt);
		Ogre::Entity * ent = mSceneMgr->createEntity("facial.mesh");
		Ogre::SceneNode * node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		node->attachObject(ent);
		node->setPosition(pt+Ogre::Vector3(0.0f,20.0f,0.f));
		mWalking = false;
		if(!m_bNeedWalk)
			m_bNeedWalk=true;
	}
	return true;
}

bool FirstListener::frameStarted(const Ogre::FrameEvent &evt)
{
	if(m_bNeedWalk)
	{	
		if(!mWalking)
		{
			nextLocation();
			mEntity->getAnimationState("Die")->setEnabled(false);
			mAnimationState = mEntity->getAnimationState("Walk");
			mAnimationState->setLoop(true);
			mAnimationState->setEnabled(true);
			Ogre::Vector3 src = mSceneNode->getOrientation() * Ogre::Vector3::UNIT_X;
			if((1.0f + src.dotProduct(mDirection)) < 0.0001f)
				mSceneNode->yaw(Ogre::Degree(180));
			else
			{
				Ogre::Quaternion quat = src.getRotationTo(mDirection);
				mSceneNode->rotate(quat);
			}
		}
		else
		{
			Ogre::Real speed = (sin(mWalked*Ogre::Math::PI/mDistance)*45.0f) + 230.0f;
			mWalked += speed * evt.timeSinceLastFrame;
			if(mWalked >= mDistance)
			{
				mSceneNode->setPosition(mDestination);
				mDirection = Ogre::Vector3::ZERO;
				nextLocation();
				if(!mWalking)
				{
					Ogre::AxisAlignedBox box(-120.0f,0.0f,-20.0f,-30.0f,15.0f,20.0f);
					mEntity->getMesh()->_setBounds(box);
					mAnimationState = mEntity->getAnimationState("Walk");
					mAnimationState->setEnabled(false);
					mAnimationState->setTimePosition(1.0f);
					m_bNeedWalk = false;
				}
				else
				{				
					Ogre::Vector3 src = mSceneNode->getOrientation() * Ogre::Vector3::UNIT_X;
					if((1.0f + src.dotProduct(mDirection)) < 0.0001f)
						mSceneNode->yaw(Ogre::Degree(180));
					else
					{
						Ogre::Quaternion quat = src.getRotationTo(mDirection);
						mSceneNode->rotate(quat);
					}
				}
			}
			else
			{
				mSceneNode->translate(mDirection * (speed * evt.timeSinceLastFrame));
			}
		}
	}
	else
	{
		mAnimationState = mEntity->getAnimationState("Die");
		mAnimationState->setLoop(false);
		mAnimationState->setEnabled(true);
	}
	mAnimationState->addTime(evt.timeSinceLastFrame);
	return true;
}

void FirstApp::createScene()
{
	//1...
	mSceneMgr->setAmbientLight(Ogre::ColourValue::White);
	
	//2...
	Ogre::Light * light = mSceneMgr->createLight("light1");
	light->setType(Ogre::Light::LT_DIRECTIONAL);
	light->setSpecularColour(Ogre::ColourValue::White);
	light->setDiffuseColour(Ogre::ColourValue::White);
	light->setDirection(-1,-1,0);

	//3...
	/*Ogre::Entity* ent1 = mSceneMgr->createEntity("ent1","ninja.mesh");
	Ogre::SceneNode* sn1 = mSceneMgr->getRootSceneNode()->createChildSceneNode("penguin");
	sn1->attachObject(ent1);
	sn1->setPosition(10,10,10);*/

	//4...
	Ogre::Plane plane1(Ogre::Vector3::UNIT_Y,0);
	Ogre::MeshManager::getSingleton().createPlane("plane1",Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		plane1,6000,5000,30,30,true,1,5,5,Ogre::Vector3::UNIT_Z);
	Ogre::Entity * ent2=mSceneMgr->createEntity("GroundEntity","plane1",Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(ent2);
	ent2->setCastShadows(false);
	ent2->setMaterialName("Examples/Rockwall");

	//5...
	mSceneMgr->setSkyDome(true,"Examples/CloudySky",5,8,1500);

	//6.create walklist...
	mWalkList.push_back(Ogre::Vector3(550.0f,  0.0f,  50.0f));
	mWalkList.push_back(Ogre::Vector3(-100.0f,  0.0f, -200.0f));

	//7.Create objects so we can see movement
	using namespace Ogre;
	Entity *ent;
	SceneNode *node;

	/*ent = mSceneMgr->createEntity("robot","robot.mesh");
	node = mSceneMgr->getRootSceneNode( )->createChildSceneNode("robot",Ogre::Vector3(0.0f,0.0f,25.0f));
	node->attachObject(ent);*/

	ent = mSceneMgr->createEntity( "Knot1", "knot.mesh" );
	node = mSceneMgr->getRootSceneNode( )->createChildSceneNode( "Knot1Node",
		Vector3(  0.0f, -10.0f,  25.0f ) );
	node->attachObject( ent );
	node->setScale( 0.1f, 0.1f, 0.1f );

	ent = mSceneMgr->createEntity( "Knot2", "knot.mesh" );
	node = mSceneMgr->getRootSceneNode( )->createChildSceneNode( "Knot2Node",
		Vector3( 550.0f, -10.0f,  50.0f ) );
	node->attachObject( ent );
	node->setScale( 0.1f, 0.1f, 0.1f );

	ent = mSceneMgr->createEntity( "Knot3", "knot.mesh" );
	node = mSceneMgr->getRootSceneNode( )->createChildSceneNode( "Knot3Node",
		Vector3(-100.0f, -10.0f,-200.0f ) );
	node->attachObject( ent );
	node->setScale( 0.1f, 0.1f, 0.1f );

	//8.chage the state of camera
	//mCameraMan->getCamera()->setPosition( 90.0f, 280.0f, 535.0f );
}

