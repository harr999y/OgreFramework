#include "OgreSample4.h"
#include "../OgreChineseWithBillBoard/MovableText.h"

using namespace Ogre;

void SinbadCharacterController::injectKeyDown(const OIS::KeyEvent &evt)
{
	if(evt.key == OIS::KC_Q && (mTopAnimID == ANIM_IDLE_TOP || mTopAnimID == ANIM_RUN_TOP))
	{
		setTopAnimation(ANIM_DRAW_SWORDS,true);
		mTimer = 0;
	}
	else if(evt.key == OIS::KC_E && !mSwordsDrawn)
	{
		if(mTopAnimID == ANIM_IDLE_TOP || mTopAnimID == ANIM_RUN_TOP)
		{
			setBaseAnimation(ANIM_DANCE,true);
			setTopAnimation(ANIM_NONE);
			mAnims[ANIM_HANDS_RELAXED]->setEnabled(true);
		}
		else if(mBaseAnimID == ANIM_DANCE)
		{
			setBaseAnimation(ANIM_IDLE_BASE);
			setTopAnimation(ANIM_IDLE_TOP);
			mAnims[ANIM_HANDS_RELAXED]->setEnabled(true);
		}
	}
	else if(evt.key == OIS::KC_W)
		mKeyDirection.z = 1;
	else if(evt.key == OIS::KC_A)
		mKeyDirection.x = 1;
	else if(evt.key == OIS::KC_S)
		mKeyDirection.z = -1;
	else if(evt.key == OIS::KC_D)
		mKeyDirection.x = -1;

	else if(evt.key == OIS::KC_SPACE && (mTopAnimID == ANIM_IDLE_TOP || mTopAnimID == ANIM_RUN_TOP))
	{
		setBaseAnimation(ANIM_JUMP_START,true);
		setTopAnimation(ANIM_NONE);
		mTimer = 0.0f;
	}

	if(!mKeyDirection.isZeroLength() && mBaseAnimID == ANIM_IDLE_BASE)
	{
		setBaseAnimation(ANIM_RUN_BASE,true);
		if(mTopAnimID == ANIM_IDLE_TOP)
			setTopAnimation(ANIM_RUN_TOP,true);
	}
}

void SinbadCharacterController::injectKeyUp(const OIS::KeyEvent &evt)
{
	if(evt.key == OIS::KC_W && mKeyDirection.z == 1)
		mKeyDirection.z = 0.0f;
	else if (evt.key == OIS::KC_A && mKeyDirection.x == 1)
		mKeyDirection.x = 0.0f;
	else if (evt.key == OIS::KC_S && mKeyDirection.z == -1) 
		mKeyDirection.z = 0.0f;
	else if (evt.key == OIS::KC_D && mKeyDirection.x == -1) 
		mKeyDirection.x = 0.0f;

	if(mKeyDirection.isZeroLength() && mBaseAnimID == ANIM_RUN_BASE)
	{
		setBaseAnimation(ANIM_IDLE_BASE);
		if(mTopAnimID == ANIM_RUN_TOP)
			setTopAnimation(ANIM_IDLE_TOP);
	}
}

void SinbadCharacterController::setupBody(Ogre::SceneManager *sceneMgr)
{
	mBodyNode = sceneMgr->getRootSceneNode()->createChildSceneNode(Vector3::UNIT_Y * CHAR_HEIGHT);
	mBodyEnt = sceneMgr->createEntity("SinbadBody","Sinbad.mesh");
	mBodyNode->attachObject(mBodyEnt);

	FontPtr font = FontManager::getSingleton().getByName("SdkTrays/Chinese");
	font->load();

	MovableText * msg = new MovableText("xx",L"ò§çïÖ®Òí~","SdkTrays/Chinese",3.f,ColourValue::Red);
	msg->setTextAlignment(MovableText::H_CENTER,MovableText::V_ABOVE);
	msg->setCharacterHeight(1.0f);
	msg->setLocalTranslation(Vector3(0.5f,5.0f,-3.0f));
	mBodyNode->attachObject(msg);

	mSword1 = sceneMgr->createEntity("SinbadSword1","Sword.mesh");
	mSword2 = sceneMgr->createEntity("SinbadSword2","Sword.mesh");
	mBodyEnt->attachObjectToBone("Sheath.L",mSword1);
	mBodyEnt->attachObjectToBone("Sheath.R",mSword2);

	NameValuePairList params;
	params["numberOfChains"] = "2";
	params["maxElements"] = "80";
	mSwordTrail = (RibbonTrail *)sceneMgr->createMovableObject("RibbonTrail",&params);
	mSwordTrail->setMaterialName("Examples/LightRibbonTrail");
	mSwordTrail->setTrailLength(20.0f);
	mSwordTrail->setVisible(false);
	sceneMgr->getRootSceneNode()->attachObject(mSwordTrail);

	for(int i=0;i<2;i++)
	{
		mSwordTrail->setInitialColour(i,1,0.8f,0);
		mSwordTrail->setColourChange(i,0.75f,1.25f,1.25f,1.25f);
		mSwordTrail->setWidthChange(i,1);
		mSwordTrail->setInitialWidth(i,0.5f);
	}

	mKeyDirection = Vector3::ZERO;
	mVerticalVelocity = 0;
}

void SinbadCharacterController::setupAnimations()
{
	mBodyEnt->getSkeleton()->setBlendMode(ANIMBLEND_CUMULATIVE);

	String animNames[] = {"IdleBase","IdleTop","RunBase","RunTop","HandsClosed","HandsRelaxed","DrawSwords","SliceVertical","SliceHorizontal","Dance","JumpStart","JumpLoop","JumpEnd"};

	for(int i=0;i<NUM_ANIMS;i++)
	{
		mAnims[i] = mBodyEnt->getAnimationState(animNames[i]);
		mAnims[i]->setLoop(true);
		mFadingIn[i] = false;
		mFadingOut[i] = false;
	}

	setBaseAnimation(ANIM_IDLE_BASE);
	setTopAnimation(ANIM_IDLE_TOP);

	mAnims[ANIM_HANDS_RELAXED]->setEnabled(true);

	mSwordsDrawn = false;
}

void SinbadCharacterController::setupCamera(Ogre::Camera *cam)
{
	CEGUI::MouseCursor::getSingleton().hide();

	mCameraPivot = cam->getSceneManager()->getRootSceneNode()->createChildSceneNode();
	mCameraGoal = mCameraPivot->createChildSceneNode(Vector3(0,0,15));
	mCameraNode = cam->getSceneManager()->getRootSceneNode()->createChildSceneNode();
	mCameraNode->setPosition(mCameraPivot->getPosition() + mCameraGoal->getPosition());

	mCameraPivot->setFixedYawAxis(true);
	mCameraGoal->setFixedYawAxis(true);
	mCameraNode->setFixedYawAxis(true);

	cam->setNearClipDistance(0.1f);
	cam->setFarClipDistance(100.0f);
	cam->detachFromParent();
	mCameraNode->attachObject(cam);

	mPivotPitch = 0;
}

void SinbadCharacterController::updateBody(Ogre::Real deltaTime)
{
	mGoalDirection = Vector3::ZERO;

	if(mKeyDirection != Vector3::ZERO && mBaseAnimID != ANIM_DANCE)
	{
		mGoalDirection += mKeyDirection.z * mCameraNode->getOrientation().zAxis();
		mGoalDirection += mKeyDirection.x * mCameraNode->getOrientation().xAxis();
		mGoalDirection.y = 0;
		mGoalDirection.normalise();

		Quaternion toGoal = mBodyNode->getOrientation().zAxis().getRotationTo(mGoalDirection);

		Real yawToGoal = toGoal.getYaw().valueDegrees();
		Real yawAtSpeed = yawToGoal / Math::Abs(yawToGoal) * deltaTime * TURN_SPEED;
		if(mBaseAnimID == ANIM_JUMP_LOOP)
			yawAtSpeed *= 0.2f;
		if(yawToGoal < 0)
			yawToGoal = std::min<Real>(0,std::max<Real>(yawToGoal,yawAtSpeed));
		else if(yawToGoal > 0)
			yawToGoal = std::max<Real>(0, std::min<Real>(yawToGoal,yawAtSpeed));

		mBodyNode->yaw(Degree(yawToGoal));
		mBodyNode->translate(0,0,deltaTime * RUN_SPEED * mAnims[mBaseAnimID]->getWeight(),Node::TS_LOCAL);
	}

	if(mBaseAnimID == ANIM_JUMP_LOOP)
	{
		mBodyNode->translate(0,mVerticalVelocity * deltaTime,0,Node::TS_LOCAL);
		mVerticalVelocity -= GRAVITY * deltaTime;
		
		Vector3 pos = mBodyNode->getPosition();
		if(pos.y <= CHAR_HEIGHT)
		{
			pos.y = CHAR_HEIGHT;
			mBodyNode->setPosition(pos);
			setBaseAnimation(ANIM_JUMP_END,true);
			mTimer = 0;
		}
	}
}

void SinbadCharacterController::updateAnimations(Ogre::Real deltaTime)
{
	Real baseAnimSpeed = 1;
	Real topAnimSpeed = 1;
	mTimer += deltaTime;

	if(mTopAnimID == ANIM_DRAW_SWORDS)
	{
		topAnimSpeed = mSwordsDrawn ? -1 : 1;

		if(mTimer >= mAnims[mTopAnimID]->getLength() / 2 && mTimer - deltaTime < mAnims[mTopAnimID]->getLength() / 2)
		{
			mBodyEnt->detachAllObjectsFromBone();
			mBodyEnt->attachObjectToBone(mSwordsDrawn ? "Sheath.L" : "Handle.L",mSword1);
			mBodyEnt->attachObjectToBone(mSwordsDrawn ? "Sheath.R" : "Handle.R",mSword2);
			mAnims[ANIM_HANDS_CLOSED]->setEnabled(!mSwordsDrawn);
			mAnims[ANIM_HANDS_RELAXED]->setEnabled(mSwordsDrawn);
			if(mSwordsDrawn)
			{
				mSwordTrail->setVisible(false);
				mSwordTrail->removeNode(mSword1->getParentNode());
				mSwordTrail->removeNode(mSword2->getParentNode());
			}
			else
			{
				mSwordTrail->setVisible(true);
				mSwordTrail->addNode(mSword1->getParentNode());
				mSwordTrail->addNode(mSword2->getParentNode());
			}
		}

		if(mTimer >= mAnims[mTopAnimID]->getLength())
		{
			if(mBaseAnimID == ANIM_IDLE_BASE)
				setTopAnimation(ANIM_IDLE_TOP);
			else
			{
				setTopAnimation(ANIM_RUN_TOP);
				mAnims[ANIM_RUN_TOP]->setTimePosition(mAnims[ANIM_RUN_BASE]->getTimePosition());
			}
			mSwordsDrawn = !mSwordsDrawn;
		}
	}
	else if(mTopAnimID == ANIM_SLICE_VERTICAL || mTopAnimID == ANIM_SLICE_HORIZONTAL)
	{
		if(mTimer >= mAnims[mTopAnimID]->getLength())
		{
			if(mBaseAnimID == ANIM_IDLE_BASE)
				setTopAnimation(ANIM_IDLE_TOP);
			else
			{
				setTopAnimation(ANIM_RUN_TOP);
				mAnims[ANIM_RUN_TOP]->setTimePosition(mAnims[ANIM_RUN_BASE]->getTimePosition());
			}
		}
		if(mBaseAnimID == ANIM_IDLE_BASE)
			baseAnimSpeed = 0;
	}
	else if(mBaseAnimID == ANIM_JUMP_START)
	{
		if(mTimer >= mAnims[mBaseAnimID]->getLength())
		{
			setBaseAnimation(ANIM_JUMP_LOOP,true);
			mVerticalVelocity = JUMP_ACCEL;
		}
	}
	else if(mBaseAnimID == ANIM_JUMP_END)
	{
		if(mTimer >= mAnims[mBaseAnimID]->getLength())
		{
			if(mKeyDirection == Vector3::ZERO)
			{
				setBaseAnimation(ANIM_IDLE_BASE);
				setTopAnimation(ANIM_IDLE_TOP);
			}
			else
			{
				setBaseAnimation(ANIM_RUN_BASE,true);
				setTopAnimation(ANIM_RUN_TOP,true);
			}
		}
	}

	if(mBaseAnimID != ANIM_NONE)
		mAnims[mBaseAnimID]->addTime(deltaTime * baseAnimSpeed);
	if(mTopAnimID != ANIM_NONE)
		mAnims[mTopAnimID]->addTime(deltaTime * topAnimSpeed);

	fadeAnimations(deltaTime);
}

void SinbadCharacterController::fadeAnimations(Ogre::Real deltaTime)
{
	for(int i=0;i<NUM_ANIMS;i++)
	{
		if(mFadingIn[i])
		{
			Real newWeight = mAnims[i]->getWeight() + deltaTime * ANIM_FADE_SPEED;
			mAnims[i]->setWeight(Math::Clamp<Real>(newWeight,0,1));
			if(newWeight >= 1)
				mFadingIn[i] = false;
		}
		else if(mFadingOut[i])
		{
			Real newWeight = mAnims[i]->getWeight() - deltaTime * ANIM_FADE_SPEED;
			mAnims[i]->setWeight(Math::Clamp<Real>(newWeight,0,1));
			if(newWeight <= 0)
			{
				mAnims[i]->setEnabled(false);
				mFadingOut[i] = false;
			}
		}
	}
}

void SinbadCharacterController::updateCamera(Ogre::Real deltaTime)
{
	mCameraPivot->setPosition(mBodyNode->_getDerivedPosition() + Vector3::UNIT_Y * CAM_HEIGHT);
	//mCameraPivot->_update(true,true);
	/*CEGUI::Logger::getSingleton().logEvent("CameraPivot:");
	CEGUI::Logger::getSingleton().logEvent(CEGUI::PropertyHelper::floatToString(mCameraPivot->_getDerivedPosition().x));
	CEGUI::Logger::getSingleton().logEvent(CEGUI::PropertyHelper::floatToString(mCameraPivot->_getDerivedPosition().y));
	CEGUI::Logger::getSingleton().logEvent(CEGUI::PropertyHelper::floatToString(mCameraPivot->_getDerivedPosition().z));
	CEGUI::Logger::getSingleton().logEvent("mCameraGoal Derived:");
	CEGUI::Logger::getSingleton().logEvent(CEGUI::PropertyHelper::floatToString(mCameraGoal->getPosition().x));
	CEGUI::Logger::getSingleton().logEvent(CEGUI::PropertyHelper::floatToString(mCameraGoal->getPosition().y));
	CEGUI::Logger::getSingleton().logEvent(CEGUI::PropertyHelper::floatToString(mCameraGoal->getPosition().z));
	/*CEGUI::Logger::getSingleton().logEvent("mCameraGoal World:");
	CEGUI::Logger::getSingleton().logEvent(CEGUI::PropertyHelper::floatToString(mCameraGoal->_getDerivedPosition().x));
	CEGUI::Logger::getSingleton().logEvent(CEGUI::PropertyHelper::floatToString(mCameraGoal->_getDerivedPosition().y));
	CEGUI::Logger::getSingleton().logEvent(CEGUI::PropertyHelper::floatToString(mCameraGoal->_getDerivedPosition().z));*/
	Vector3 goalOffset = mCameraGoal->_getDerivedPosition() - mCameraNode->getPosition();
/*
	CEGUI::Logger::getSingleton().logEvent("goalOffset:");
	CEGUI::Logger::getSingleton().logEvent(CEGUI::PropertyHelper::floatToString(goalOffset.x));
	CEGUI::Logger::getSingleton().logEvent(CEGUI::PropertyHelper::floatToString(goalOffset.y));
	CEGUI::Logger::getSingleton().logEvent(CEGUI::PropertyHelper::floatToString(goalOffset.z));
	CEGUI::Logger::getSingleton().logEvent("deltaTime:");
	CEGUI::Logger::getSingleton().logEvent(CEGUI::PropertyHelper::floatToString(deltaTime));*/

	mCameraNode->translate(goalOffset * deltaTime * 9.0f);
	/*CEGUI::Logger::getSingleton().logEvent("mCameraNode:");
	CEGUI::Logger::getSingleton().logEvent(CEGUI::PropertyHelper::floatToString(mCameraNode->_getDerivedPosition().x));
	CEGUI::Logger::getSingleton().logEvent(CEGUI::PropertyHelper::floatToString(mCameraNode->_getDerivedPosition().y));
	CEGUI::Logger::getSingleton().logEvent(CEGUI::PropertyHelper::floatToString(mCameraNode->_getDerivedPosition().z));*/
	mCameraNode->lookAt(mCameraPivot->_getDerivedPosition(),Node::TS_WORLD);
}

void SinbadCharacterController::updateCameraGoal(Ogre::Real deltaYaw, Ogre::Real deltaPitch, Ogre::Real deltaZoom)
{
	mCameraPivot->yaw(Degree(deltaYaw),Node::TS_WORLD);
	if(!(mPivotPitch + deltaPitch > 85 && deltaPitch > 0) && !(mPivotPitch + deltaPitch < -90 && deltaPitch < 0))
	{
		mCameraPivot->pitch(Degree(deltaPitch), Node::TS_LOCAL);
		mPivotPitch += deltaPitch;
	}

	Real dist = mCameraGoal->_getDerivedPosition().distance(mCameraPivot->_getDerivedPosition());
	Real distChange = deltaZoom * dist;

	// bound the zoom
	if (!(dist + distChange < 8 && distChange < 0) &&
		!(dist + distChange > 25 && distChange > 0))
	{
		mCameraGoal->translate(0, 0, distChange, Node::TS_LOCAL);
	}
}

void SinbadCharacterController::injectMouseMove(const OIS::MouseEvent& evt)
{
	// update camera goal based on mouse movement
	updateCameraGoal(-0.10f * evt.state.X.rel, 0.05f * evt.state.Y.rel, -0.001f * evt.state.Z.rel);
}

void SinbadCharacterController::injectMouseDown(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
{
	if (mSwordsDrawn && (mTopAnimID == ANIM_IDLE_TOP || mTopAnimID == ANIM_RUN_TOP))
	{
		// if swords are out, and character's not doing something weird, then SLICE!
		if (id == OIS::MB_Left) setTopAnimation(ANIM_SLICE_VERTICAL, true);
		else if (id == OIS::MB_Right) setTopAnimation(ANIM_SLICE_HORIZONTAL, true);
		mTimer = 0;
	}
}

void SinbadCharacterController::setBaseAnimation(AnimID id, bool reset)
{
	if (mBaseAnimID >= 0 && mBaseAnimID < NUM_ANIMS)
	{
		// if we have an old animation, fade it out
		mFadingIn[mBaseAnimID] = false;
		mFadingOut[mBaseAnimID] = true;
	}

	mBaseAnimID = id;

	if (id != ANIM_NONE)
	{
		// if we have a new animation, enable it and fade it in
		mAnims[id]->setEnabled(true);
		mAnims[id]->setWeight(0);
		mFadingOut[id] = false;
		mFadingIn[id] = true;
		if (reset) mAnims[id]->setTimePosition(0);
	}
}

void SinbadCharacterController::setTopAnimation(AnimID id, bool reset)
{
	if (mTopAnimID >= 0 && mTopAnimID < NUM_ANIMS)
	{
		// if we have an old animation, fade it out
		mFadingIn[mTopAnimID] = false;
		mFadingOut[mTopAnimID] = true;
	}

	mTopAnimID = id;

	if (id != ANIM_NONE)
	{
		// if we have a new animation, enable it and fade it in
		mAnims[id]->setEnabled(true);
		mAnims[id]->setWeight(0);
		mFadingOut[id] = false;
		mFadingIn[id] = true;
		if (reset) mAnims[id]->setTimePosition(0);
	}
}

bool OgreSample4Listener::frameStarted(const FrameEvent& evt)
{
	// let character update animations and camera
	mChara->addTime(evt.timeSinceLastFrame);
	//mAnimationState->addTime(evt.timeSinceLastFrame);
	return true;
}

bool OgreSample4Listener::keyPressed(const OIS::KeyEvent& arg)
{
	if(arg.key == OIS::KC_ESCAPE)
		mShutDown = true;
	// relay input events to character controller
	mChara->injectKeyDown(arg);
	return true;
}

bool OgreSample4Listener::keyReleased(const OIS::KeyEvent& arg)
{
	// relay input events to character controller
	mChara->injectKeyUp(arg);
	return true;
}

void OgreSample4App::createScene()
{
	// set background and some fog
	mCameraMan->getCamera()->getViewport()->setBackgroundColour(ColourValue(1.0f, 1.0f, 0.8f));
	mSceneMgr->setFog(Ogre::FOG_LINEAR, ColourValue(1.0f, 1.0f, 0.8f), 0, 15, 100);

	// set shadow properties
	mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);
	mSceneMgr->setShadowColour(ColourValue(0.5, 0.5, 0.5));
	mSceneMgr->setShadowTextureSize(1024);
	mSceneMgr->setShadowTextureCount(1);

	// disable default camera control so the character can do its own
	mCameraMan->setStyle(OgreBites::CS_MANUAL);

	// use a small amount of ambient lighting
	mSceneMgr->setAmbientLight(ColourValue(0.9f, 0.9f, 0.9f));

	// add a bright light above the scene
	Light* light = mSceneMgr->createLight();
	light->setType(Light::LT_POINT);
	light->setPosition(-10, 40, 20);
	light->setSpecularColour(ColourValue::White);

	// create a floor mesh resource
	MeshManager::getSingleton().createPlane("floor", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Plane(Vector3::UNIT_Y, 0), 100, 100, 10, 10, true, 1, 10, 10, Vector3::UNIT_Z);

	// create a floor entity, give it a material, and place it at the origin
	Entity* floor = mSceneMgr->createEntity("Floor", "floor");
	floor->setMaterialName("Examples/Rockwall");
	floor->setCastShadows(false);
	mSceneMgr->getRootSceneNode()->attachObject(floor);
}

void OgreSample4App::createFrameListener()
{
	OgreSample4Listener * listener = new OgreSample4Listener(this,mRoot,mSceneMgr,mWindow,mCameraMan,mOgreRenderer);
	listener->initialise();
	mListener = listener;
}

bool OgreSample4Listener::mouseMoved(const OIS::MouseEvent& arg)
{
	mChara->injectMouseMove(arg);
	return true;
}

bool OgreSample4Listener::mousePressed(const OIS::MouseEvent& arg, OIS::MouseButtonID id)
{
	mChara->injectMouseDown(arg, id);
	return true;
}