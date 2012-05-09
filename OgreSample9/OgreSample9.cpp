#include "OgreSample9.h"

void OgreSample9App::createScene()
{
	mSceneMgr->setSkyBox(true, "Examples/StormySkyBox");  // add a skybox

	// setup some basic lighting for our scene
	mSceneMgr->setAmbientLight(ColourValue(0.5, 0.5, 0.5));
	mSceneMgr->createLight()->setPosition(20, 80, 50);

	// create a penguin and attach him to our penguin node
	Entity* penguin = mSceneMgr->createEntity("Penguin", "penguin.mesh");
	mPenguinNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	mPenguinNode->attachObject(penguin);

	// get and enable the penguin idle animation
	mPenguinAnimState = penguin->getAnimationState("amuse");
	mPenguinAnimState->setEnabled(true);

	ParticleSystem * ps = mSceneMgr->createParticleSystem("Snow","Examples/Snow");
	mSceneMgr->getRootSceneNode()->attachObject(ps);
	ps->fastForward(30);
	mCameraMan->getCamera()->setPosition(Ogre::Vector3(0,0,230));	
	mCameraMan->getCamera()->setDirection(Ogre::Vector3(0,0,-1));
}

void OgreSample9Listener::doSome()
{
	TexturePtr tex = TextureManager::getSingleton().createManual("thaw",ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,TEX_TYPE_2D,TEXTURE_SIZE,TEXTURE_SIZE,0,PF_L8,TU_DYNAMIC_WRITE_ONLY);
	mTexBuf = tex->getBuffer();
	mTexBuf->lock(HardwareBuffer::HBL_DISCARD);
	memset(mTexBuf->getCurrentLock().data,0xff,mTexBuf->getSizeInBytes());
	mTexBuf->unlock();

	Ogre::Entity * ent = mSceneMgr->createEntity("Plane",SceneManager::PT_PLANE);
	ent->setMaterialName("Examples/Frost");
	SceneNode * node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	node->setPosition(0.0f,0.0f,50.0f);
	node->attachObject(ent);
	mPlaneSize = mSceneMgr->getEntity("Plane")->getBoundingBox().getSize().x;
	mCursorQuery = mSceneMgr->createRayQuery(Ray());
}

void OgreSample9App::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
		mPenguinAnimState->addTime(evt.timeSinceLastFrame); 
		mPenguinNode->yaw(Radian(evt.timeSinceLastFrame));
}

bool OgreSample9Listener::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	CEGUIBaseListener::mousePressed(arg,id);
	mWiping = true;
	return true;
}

bool OgreSample9Listener::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	CEGUIBaseListener::mouseReleased(arg,id);
	mWiping = false;
	return true;
}

bool OgreSample9Listener::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
	CEGUI::Point point = CEGUI::MouseCursor::getSingleton().getPosition();
	Ray ray = mCameraMan->getCamera()->getCameraToViewportRay(point.d_x / mWindow->getWidth(),point.d_y / mWindow->getHeight());
	mCursorQuery->setRay(ray);
	RaySceneQueryResult & result = mCursorQuery->execute();

	if (!result.empty())
	{
		Vector3 pt = ray.getPoint(result.back().distance);
		mBrushPos = (Vector2(pt.x,-pt.y) / mPlaneSize + Vector2(0.5f,0.5f)) * TEXTURE_SIZE;
	}

	Ogre::uint8 freezeAmount = 0;
	mTimeSinceLastFreeze += evt.timeSinceLastFrame;

	while (mTimeSinceLastFreeze >= 0.1f)
	{
		mTimeSinceLastFreeze -= 0.1f;
		freezeAmount += 0x04;
	}

	updateTexture(freezeAmount);
	return CEGUIBaseListener::frameRenderingQueued(evt);
}

void OgreSample9Listener::updateTexture(Ogre::uint8 freezeAmount)
{
	mTexBuf->lock(HardwareBuffer::HBL_NORMAL);

	Ogre::uint8 * data = (Ogre::uint8*)mTexBuf->getCurrentLock().data;
	Ogre::uint8 temperature;
	Real sqrDistToBrush;

	for (unsigned int y=0;y<TEXTURE_SIZE;y++)
	{
		for (unsigned int x=0;x<TEXTURE_SIZE;x++)
		{
			if (freezeAmount != 0)
			{
				temperature = 0xff - *data;
				if (temperature > freezeAmount)
				{
					*data += freezeAmount;
				}
				else
					*data = 0xff;
			}
			if (mWiping)
			{
				sqrDistToBrush = Math::Sqr(x - mBrushPos.x) + Math::Sqr(y - mBrushPos.y);
				if (sqrDistToBrush <= SQE_BRUSH_RADIUS)
				{
					*data = std::min<Ogre::uint8>(sqrDistToBrush / SQE_BRUSH_RADIUS * 0xff,*data);
				}
			}
			data++;
		}
	}
	mTexBuf->unlock();
}

void OgreSample9App::createFrameListener()
{
	OgreSample9Listener* listener = new OgreSample9Listener(this,mRoot,mSceneMgr,mWindow,mCameraMan,mOgreRenderer);
	listener->initialise();
	mListener = listener;
}