#include "OgitorSample.h"
#include "MovableManager.h"

//static AnimationState * as;
void OgitorSampleApp::createScene()
{
	mLoader = new DotSceneLoader();
	mLoader->parseDotScene("final.scene", "General", mSceneMgr);

	mCameraMan->setStyle(OgreBites::CS_MANUAL);

	mChara = new CharacterController(
		mCameraMan->getCamera(),
		mLoader->getPhysXScene()->getScene(),
		mLoader->getTerrainGroup());

	playEvironSound();

	MovableManager::getSingleton().initialize(mSceneMgr, mLoader->getTerrainGroup());

	regCompositors();
}

void OgitorSampleApp::regCompositors()
{
	try 
	{
		Ogre::Viewport * vp = mCameraMan->getCamera()->getViewport();
		Ogre::CompositorManager::getSingleton().addCompositor(vp, "Radial Blur", -1);

		Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp, "Radial Blur", false);
	} catch (...) {}
}

void OgitorSampleApp::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
	if (!mLoader->getTerrainGroup()->isDerivedDataUpdateInProgress())
	{
		if (mTerrainImported)
		{
			mLoader->getTerrainGroup()->saveAllTerrains(true);
			mTerrainImported = false;
		}
	}

	mChara->update(evt.timeSinceLastFrame);

	mLoader->update(evt.timeSinceLastFrame);

	mSoundManager->FrameStarted(
		mSceneMgr->getSceneNode("EnvironSoundNode"),
		evt.timeSinceLastFrame);

	MovableManager::getSingleton().update(evt.timeSinceLastFrame);
}

void OgitorSampleApp::switchCamera(int idx)
{
	if (idx <= (int)mCamNames.size())
	{
		mSceneMgr->getEntity(mActiveCamera->getName() + Ogre::String("_debug"))->setVisible(true);
		mSceneMgr->getSceneNode(mActiveCamera->getName())->setPosition(mActiveCamera->getPosition());
		mSceneMgr->getSceneNode(mActiveCamera->getName())->setOrientation(mActiveCamera->getOrientation());
		mActiveCamera = mSceneMgr->getCamera(mCamNames[idx-1]);
		mWindow->getViewport(0)->setCamera(mActiveCamera);
		mCameraMan->setCamera(mActiveCamera);
		mSceneMgr->getEntity(mActiveCamera->getName() + Ogre::String("_debug"))->setVisible(false);
	}
}

bool OgitorSampleApp::keyPressed(const OIS::KeyEvent &arg)
{
	//mChara->injectKeyDown(arg);
	mChara->injectKeyPressed(arg);

	if (arg.key == OIS::KC_0)   // toggle scenenode debug renderables
	{
		mSceneMgr->setDisplaySceneNodes(!mSceneMgr->getDisplaySceneNodes());
	}
	if (arg.key == OIS::KC_1)   // switch to camera 1
	{
		switchCamera(1);
	}
	if (arg.key == OIS::KC_2)   // switch to camera 2
	{
		switchCamera(2);
	}
	if (arg.key == OIS::KC_3)   // switch to camera 3
	{
		switchCamera(3);
	}
	if (arg.key == OIS::KC_4)   // switch to camera 4
	{
		switchCamera(4);
	}
	if (arg.key == OIS::KC_5)   // switch to camera 5
	{
		switchCamera(5);
	}
	if (arg.key == OIS::KC_6)   // switch to camera 6
	{
		switchCamera(6);
	}
	if (arg.key == OIS::KC_7)   // switch to camera 7
	{
		switchCamera(7);
	}
	if (arg.key == OIS::KC_8)   // switch to camera 8
	{
		switchCamera(8);
	}
	if (arg.key == OIS::KC_9)   // switch to camera 9
	{
		switchCamera(9);
	}
	if (arg.key == OIS::KC_RETURN)   // toggle fly/walk
	{
		mFly = !mFly;
	}

	return true;
}

bool OgitorSampleApp::keyReleased(const OIS::KeyEvent &arg)
{
	mChara->injectKeyReleased(arg);
	return true;
}

bool OgitorSampleApp::mouseMoved(const OIS::MouseEvent &arg)
{
	//mChara->injectMouseMove(arg);
	return true;
}

bool OgitorSampleApp::mousePressed(const OIS::MouseEvent &arg,OIS::MouseButtonID id)
{
	mChara->injectMousePressed(arg, id);
	return true;
}

void OgitorSampleApp::playEvironSound()
{
	mSoundManager = new SoundManager();
	mSoundManager->Initialize();

	Ogre::SceneNode * environSoundNode = mSceneMgr
		->getRootSceneNode()->createChildSceneNode("EnvironSoundNode");

	int sound = 0;
	sound = mSoundManager->CreateSound(Ogre::String("GameDemo.mp3"));
	int channal = 0;
	mSoundManager->PlaySound(
		sound, 
		environSoundNode,
		&channal);

	FMOD::Channel * soundChannel = mSoundManager->GetSoundChannel(channal);
	soundChannel->setVolume(0.5f);
	soundChannel->setLoopCount(-1);
}


void SinbadCharacterController::injectKeyPressed(const OIS::KeyEvent &evt)
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
		mJump.startJump(.4);//,mTerrainGroup->getHeightAtWorldPosition(Vector3(mBodyNode->getPosition().x,0,mBodyNode->getPosition().z)));
	}

	if(!mKeyDirection.isZeroLength() && mBaseAnimID == ANIM_IDLE_BASE)
	{
		setBaseAnimation(ANIM_RUN_BASE,true);
		if(mTopAnimID == ANIM_IDLE_TOP)
			setTopAnimation(ANIM_RUN_TOP,true);
	}
}

void SinbadCharacterController::injectKeyReleased(const OIS::KeyEvent &evt)
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
	mBodyNode = sceneMgr->getRootSceneNode()->createChildSceneNode(Vector3(0,mTerrainGroup->getHeightAtWorldPosition(0,0,0) + 6.5,0));
	mBodyEnt = sceneMgr->createEntity("SinbadBody","Sinbad.mesh");
	mBodyNode->attachObject(mBodyEnt);

	FontPtr font = FontManager::getSingleton().getByName("SdkTrays/Chinese");
	font->load();

	MovableText * msg = new MovableText("xx",L"颛顼之翼~","SdkTrays/Chinese",.6f,ColourValue::Red);
	msg->setTextAlignment(MovableText::H_CENTER,MovableText::V_ABOVE);
	msg->setCharacterHeight(.5f);
	msg->setLocalTranslation(Vector3(0.5f,0.2f,-3.0f));
	msg->showOnTop(true);
	mBodyNode->attachObject(msg);
	mBodyNode->setFixedYawAxis(true);

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

	UserAllocator * mAllocator = new UserAllocator;
	mCCManager = NxCreateControllerManager(mAllocator);

	Ogre::AxisAlignedBox box = mBodyEnt->getBoundingBox();
	Ogre::Vector3 size = box.getSize();
	//size *= 0.4f;
	mSinbadController = new MyCharacterController(mCCManager,mScene,NxVec3(-100,mTerrainGroup->getHeightAtWorldPosition(-100,0,-150) + 1.5,-150),size.x / 4.7,size.y * 0.41f);
	mSinbadController->GetCharacterActor()->getShapes()[0]->setFlag(NX_SF_DISABLE_RAYCASTING,true);
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
/*
	mCameraNode = cam->getSceneManager()->getRootSceneNode()->createChildSceneNode();
	mCameraPivot = cam->getSceneManager()->getRootSceneNode()->createChildSceneNode();

	mCameraOriginal = mCameraPivot->createChildSceneNode(Vector3(0,1,-20));

	mCameraPivot->setFixedYawAxis(true);
	mCameraOriginal->setFixedYawAxis(true);
	mCameraNode->setFixedYawAxis(true);*/

	cam->setNearClipDistance(0.1f);
	cam->setFarClipDistance(200);
	cam->detachFromParent();
	//mCameraNode->attachObject(cam);

	mCameraController = new MyCameraController(cam,mBodyNode,mScene,mTerrainGroup);
	//mCameraController->setup();
	//mPivotPitch = 0;
}

void SinbadCharacterController::updateBody(Ogre::Real deltaTime)
{
	mGoalDirection = Vector3::ZERO;

	if(mKeyDirection != Vector3::ZERO && mBaseAnimID != ANIM_DANCE)
	{
		mGoalDirection += mKeyDirection.z * mCameraController->getCameraPivot()->_getDerivedOrientation().zAxis();
		mGoalDirection += mKeyDirection.x * mCameraController->getCameraPivot()->_getDerivedOrientation().xAxis();
		mGoalDirection.y = 0;
		mGoalDirection.normalise();

		Quaternion toGoal = mBodyNode->getOrientation().zAxis().getRotationTo(mGoalDirection);

		Real yawToGoal = toGoal.getYaw().valueDegrees();
		Real yawAtSpeed = yawToGoal / Math::Abs(yawToGoal) * deltaTime * TURN_SPEED;
		if(mBaseAnimID == ANIM_JUMP_LOOP)
			yawAtSpeed *= 0.2f;

		if(yawToGoal < 0)
		{
			if (yawToGoal >= -75)
				yawToGoal = std::min<Real>(0,std::max<Real>(yawToGoal,yawAtSpeed));
		}
		else if(yawToGoal > 0)
		{
			if (yawToGoal <= 75)
				yawToGoal = std::max<Real>(0, std::min<Real>(yawToGoal,yawAtSpeed));
		}

		mGoalDirection = Vector3(0,0,deltaTime * RUN_SPEED * mAnims[mBaseAnimID]->getWeight());		
		mBodyNode->yaw(Degree(yawToGoal));
	}

	mGoalDirection.y -= 40 * deltaTime;

	if(mBaseAnimID == ANIM_JUMP_LOOP)
	{
		mGoalDirection.y = mJump.getHeight(deltaTime) + 40 * deltaTime; 
	}

	NxU32 collisionFlags;
	Ogre::Vector3 vt = mBodyNode->getOrientation() * mGoalDirection;

	mSinbadController->Move(NxVec3(vt.x,vt.y,vt.z), collisionFlags);
	NxVec3 v3 = mSinbadController->GetCharacterActor()->getGlobalPosition();
	mBodyNode->setPosition(Ogre::Vector3(v3.x,v3.y + .16,v3.z));

	static float preHeight = 0;
	if(mBaseAnimID == ANIM_JUMP_LOOP)
	{
		if(collisionFlags == NXCC_COLLISION_DOWN)
		{
			setBaseAnimation(ANIM_JUMP_END,true);
			mJump.stopJump();
		}
		preHeight = v3.y;
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
/*
	static bool bViewRestore = false;

	mCameraPivot->setPosition(mBodyNode->getPosition() + Vector3(0,0.2f,0));
	mCameraOriginal->_update(true,true);
	mCameraOriginal->lookAt(mCameraPivot->getPosition(),Node::TS_WORLD);

	Vector3 vo = mCameraOriginal->_getDerivedPosition();
	Vector3 vb = mCameraPivot->_getDerivedPosition();
	Ogre::Ray ray;
	NxVec3 vc(vb.x,vb.y,vb.z);
	NxVec3 vecDir = NxVec3(vo.x - vb.x,vo.y - vb.y,vo.z - vb.z);
	float length = vecDir.normalize();
	NxRay nxRay(vc,vecDir);
	NxRaycastHit hit;
	NxShape * shape = mScene->raycastClosestShape(nxRay,NX_ALL_SHAPES,hit);
	
	if (shape && hit.distance < length)
	{
		Vector3 hitWorld(hit.worldImpact.x,hit.worldImpact.y,hit.worldImpact.z);

		//为了防止穿透
		float transZ = -0.8f;
		//float transY = (-0.8f) * vo.y / (NxMath::sqrt(vo.x * vo.x + vo.z * vo.z));  //这个还是算了，不加上去。。。
		hitWorld += mCameraOriginal->_getDerivedOrientation() * Vector3(0,0,transZ);
		Vector3 hitLocal = -mCameraPivot->convertWorldToLocalPosition(hitWorld);
		mCameraNode->setPosition(hitWorld);
		mCameraNode->lookAt(mCameraPivot->getPosition(),Node::TS_WORLD);

		//为了防止在角落里时摄像机反向
		if (hitLocal.z < 0)
		{
			mCameraNode->setPosition(mCameraPivot->getPosition());  //这个效果和魔兽世界还有暗黑血统相似，但他们是不是这么写的就不得而知了~ 呵呵
			Vector3 v = mCameraPivot->getOrientation() * Vector3(0,0,3);
			mCameraNode->lookAt(mCameraPivot->getPosition() + v,Node::TS_WORLD);
		}
		bViewRestore = true;
	}
	else
	{
		if (bViewRestore)
		{
			Vector3 transPos = vo - mCameraNode->getPosition();
			Vector3 localPos = mCameraPivot->convertWorldToLocalPosition(mCameraNode->getPosition());
			Vector3 originalPos = mCameraOriginal->getPosition();
			if (localPos.z  > originalPos.z)
			{
				localPos.z -= 25 * deltaTime;
			}
			if (localPos.z <= originalPos.z)
			{
				localPos.z = originalPos.z;
				bViewRestore = false;
			}
			else
				originalPos.z = localPos.z;

			mCameraNode->setPosition(mCameraPivot->convertLocalToWorldPosition(originalPos));
		}
		else
			mCameraNode->setPosition(vo);

		mCameraNode->lookAt(mCameraPivot->getPosition(),Node::TS_WORLD);
	}*/

}

void SinbadCharacterController::updateCameraGoal(Ogre::Real deltaYaw, Ogre::Real deltaPitch, Ogre::Real deltaZoom)
{
/*
	mCameraPivot->yaw(Degree(deltaYaw),Node::TS_LOCAL);
	//mBodyNode->yaw(Degree(deltaYaw),Node::TS_LOCAL);

	if(mPivotPitch + deltaPitch <= 76 || mPivotPitch + deltaPitch >= -76)
	{
		mCameraPivot->pitch(Degree(deltaPitch), Node::TS_LOCAL);
		mPivotPitch += deltaPitch;
	}

	Real dist = mBodyNode->_getDerivedPosition().distance(mCameraOriginal->_getDerivedPosition());
	Real distChange = deltaZoom * dist;

	// bound the zoom
	if (!mTransLeft && dist + distChange >= 6 && dist + distChange <= 28)
	{
		mCameraOriginal->translate(0, 0, distChange, Node::TS_LOCAL);
	}*/

}

void SinbadCharacterController::injectMouseMove(const OIS::MouseEvent& evt)
{
	// update camera goal based on mouse movement
	//updateCameraGoal(-0.10f * evt.state.X.rel, 0.05f * evt.state.Y.rel, -0.001f * evt.state.Z.rel);
	mCameraController->updateGoal( 0.05f * evt.state.Y.rel,-0.10f * evt.state.X.rel, -0.001f * evt.state.Z.rel);
}

void SinbadCharacterController::injectMousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
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




//////////////////////////////////////////////////////////////////////////
//无用，废弃~
/*
float SinbadCharacterController::cameraPreviousTest(float translate,float yawAngle,float pitchAngle)
{
	Vector3 vecPivot = mCameraPivot->getPosition();
	Vector3 cameraPoint = mCamera->getCameraToViewportRay(.5f,.5f).getOrigin();

	if (translate)
	{
		cameraPoint += translate;
		vecPivot += translate;
	}
	if (yawAngle || pitchAngle)
	{
		Quaternion q;
		Matrix3 matrix33 = mCameraPivot->getLocalAxes();
		if (yawAngle)
		{
			Vector3 vUp = Vector3(matrix33[1]);
			q.FromAngleAxis( Radian(Degree(yawAngle)), vUp);
			cameraPoint = q * cameraPoint;
		}
		if (pitchAngle)
		{
			Vector3 vLR = Vector3(matrix33[0]);
			q.FromAngleAxis( Radian(Degree(pitchAngle)), vLR);
			cameraPoint = q * cameraPoint;
		}
	}
	Ogre::Ray ray;
	NxVec3 vecCC(vecPivot.x,vecPivot.y,vecPivot.z);
	NxVec3 vecDir = NxVec3(cameraPoint.x - vecPivot.x,cameraPoint.y - vecPivot.y,cameraPoint.z - vecPivot.z);
	float length = vecDir.normalize();
	NxRay nxRay(vecCC,vecDir);
	NxRaycastHit hit;
	NxShape * shape = mScene->raycastClosestShape(nxRay,NX_ALL_SHAPES,hit);

	if (shape && hit.distance < length)
	{
		Vector3 vecNode = mCameraNode->getPosition();
		if (length - hit.distance + vecNode.z >= 3 && length - hit.distance + vecNode.z <= 30)
			return length - hit.distance;
		else
			return 0;
	}
	else if(shape && hit.distance > length)
		return length - hit.distance;
	else
		return 0;
}*/
//////////////////////////////////////////////////////////////////////////

bool OgitorSampleListener::mouseMoved(const OIS::MouseEvent &arg)
{
	CEGUI::System& cegui = CEGUI::System::getSingleton();

	if (!mRMouseDown)
	{
		cegui.injectMouseMove(arg.state.X.rel * mMouseSpeed, arg.state.Y.rel * mMouseSpeed);
		cegui.injectMouseWheelChange(arg.state.Z.rel / 120.0f);
	}
	else
	{
		((OgitorSampleApp*)mApp)->mChara->injectMouseMove(arg);

		::ShowCursor(FALSE);
// 		LPRECT mouseClipRect;
// 
// 		if (!::GetClipCursor(mouseClipRect))
// 			return false;
// 
// 		LPRECT newClipRect;
// 		newClipRect->left = newClipRect->right = newClipRect->bottom = newClipRect->top = 
	}

	return mApp->mouseMoved(arg);
}

void OgitorSampleApp::createFrameListener()
{
	OgitorSampleListener * listener = new OgitorSampleListener(this,mRoot,mSceneMgr,mWindow,mCameraMan,mOgreRenderer);
	listener->initialise();
	mListener = listener;
}


