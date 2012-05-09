#include "OgitorSample.h"

void OgitorSampleApp::createScene()
{
	mLoader = new DotSceneLoader();
	mLoader->parseDotScene("OgitorSampleScene2.scene", "General", mSceneMgr);
	/*
	mSceneMgr->destroyCamera(mCameraMan->getCamera());

	Ogre::SceneManager::CameraIterator cameraIt = mSceneMgr->getCameraIterator();
	while (cameraIt.hasMoreElements())
	{
	Camera * camera = cameraIt.getNext();
	mCamNames.push_back(camera->getName());
	Entity * debugEnt = mSceneMgr->createEntity(camera->getName() + Ogre::String("_debug"), "scbCamera.mesh");
	mSceneMgr->getSceneNode(camera->getName())->attachObject(debugEnt);
	mSceneMgr->getSceneNode(camera->getName())->scale(0.5, 0.5, 0.5);
	}
	String cameraName = mCamNames[0];
	mActiveCamera = mSceneMgr->getCamera(cameraName);
	mWindow->getViewport(0)->setCamera(mActiveCamera);
	mCameraMan->setCamera(mActiveCamera);
	mSceneMgr->getEntity(mActiveCamera->getName() + Ogre::String("_debug"))->setVisible(false);

	mCameraMan->setTopSpeed(10);*/

	mCameraMan->setStyle(OgreBites::CS_MANUAL);
	//((MovableObject *)(mCameraMan->getCamera()))->setUserAny(Any(mCameraMan));

	mChara = new SinbadCharacterController(mCameraMan->getCamera(),mLoader->getPhysXScene()->getScene(),mLoader->getTerrainGroup());
	//mCameraMan->setCamera(mActiveCamera);
}

void OgitorSampleApp::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
	/*
	if (!mFly)
	{
	Ogre::Vector3 camPos = mActiveCamera->getPosition();
	Ogre::Ray ray;
	ray.setOrigin(Ogre::Vector3(camPos.x, 10000, camPos.z));
	ray.setDirection(Ogre::Vector3::NEGATIVE_UNIT_Y);

	Ogre::TerrainGroup::RayResult rayResult = mLoader->getTerrainGroup()->rayIntersects(ray);
	if (rayResult.hit && camPos.y <= rayResult.position.y + 1.4)
	{
	mActiveCamera->setPosition(camPos.x, rayResult.position.y + 1.4, camPos.z);
	}
	}
	*/

	if (!mLoader->getTerrainGroup()->isDerivedDataUpdateInProgress())
	{
		if (mTerrainImported)
		{
			mLoader->getTerrainGroup()->saveAllTerrains(true);
			mTerrainImported = false;
		}
	}
	mChara->addTime(evt.timeSinceLastFrame);
	mLoader->update(evt.timeSinceLastFrame);
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
	mChara->injectKeyDown(arg);

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
	mChara->injectKeyUp(arg);
	return true;
}

bool OgitorSampleApp::mouseMoved(const OIS::MouseEvent &arg)
{
	//mChara->injectMouseMove(arg);
	return true;
}

bool OgitorSampleApp::mousePressed(const OIS::MouseEvent &arg,OIS::MouseButtonID id)
{
	mChara->injectMouseDown(arg, id);
	return true;
}

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
		mJump.startJump(.4);//,mTerrainGroup->getHeightAtWorldPosition(Vector3(mBodyNode->getPosition().x,0,mBodyNode->getPosition().z)));
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
	mBodyNode = sceneMgr->getRootSceneNode()->createChildSceneNode(Vector3(0,mTerrainGroup->getHeightAtWorldPosition(0,0,0) + 6.5,0));
	mBodyEnt = sceneMgr->createEntity("SinbadBody","Sinbad.mesh");
	mBodyNode->attachObject(mBodyEnt);

	FontPtr font = FontManager::getSingleton().getByName("SdkTrays/Chinese");
	font->load();

	MovableText * msg = new MovableText("xx",L"ò§çïÖ®Òí~","SdkTrays/Chinese",.6f,ColourValue::Red);
	msg->setTextAlignment(MovableText::H_CENTER,MovableText::V_ABOVE);
	msg->setCharacterHeight(.5f);
	msg->setLocalTranslation(Vector3(0.5f,0.2f,-3.0f));
	msg->showOnTop(true);
	//msg->setRenderQueueGroupAndPriority(RENDER_QUEUE_SKIES_LATE,OGRE_RENDERABLE_DEFAULT_PRIORITY);
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

	//mCCManager = NxCreateControllerManager(NxOgre::GC::safe_new0<NxOgre::PhysXUserAllocator>(NXOGRE_GC_THIS));
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
	//CEGUI::MouseCursor::getSingleton().hide();

	mCameraNode = cam->getSceneManager()->getRootSceneNode()->createChildSceneNode();
	mCameraPivot = cam->getSceneManager()->getRootSceneNode()->createChildSceneNode();
	//,Quaternion(Radian(Degree(180)),Vector3(0,0,1)));
	//mCameraPivot->setOrientation(Quaternion(Radian(Degree(180)),Vector3(0,1,0)));
	//mCameraNode = cam->getSceneManager()->getRootSceneNode()->createChildSceneNode();
	//mCameraNode->setPosition(mCameraPivot->_getDerivedPosition());// + mCameraGoal->getPosition());

/*
	NxBodyDesc bodyDesc;
	NxBoxShapeDesc boxDesc;
	boxDesc.dimensions = NxVec3(1.0.02,1);
	Ogre::Ray ray;
	ray = mCamera->getCameraToViewportRay(0.5f,0.5f);
	boxDesc.localPose.t = NxVec3(ray.getOrigin().x,ray.getOrigin().y,ray.getOrigin().z);

	bodyDesc.mass = 20;

	NxActorDesc actorDesc;
	actorDesc.shapes.pushBack(&boxDesc);

	mCameraCollision = */
	mCameraOriginal = mCameraPivot->createChildSceneNode(Vector3(0,1,-20));

	mCameraPivot->setFixedYawAxis(true);
	mCameraOriginal->setFixedYawAxis(true);
	mCameraNode->setFixedYawAxis(true);

	cam->setNearClipDistance(0.1f);
	cam->setFarClipDistance(0);
	cam->detachFromParent();
	mCameraNode->attachObject(cam);

	//OgreBites::SdkCameraMan *cm = any_cast<OgreBites::SdkCameraMan*>(((MovableObject *)cam)->getUserAny());
	//cm->setTarget(mBodyNode);

	mPivotPitch = 0;
}

void SinbadCharacterController::updateBody(Ogre::Real deltaTime)
{
	mGoalDirection = Vector3::ZERO;

	if(mKeyDirection != Vector3::ZERO && mBaseAnimID != ANIM_DANCE)
	{
		mGoalDirection -= mKeyDirection.z * mCameraOriginal->_getDerivedOrientation().zAxis();
		mGoalDirection -= mKeyDirection.x * mCameraOriginal->_getDerivedOrientation().xAxis();
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
		//NxQuat quat(yawToGoal,NxVec3(0,1,0));
		//mSinbadController->GetCharacterActor()->moveGlobalOrientationQuat(quat);

		//NxF32 Vt = mVerticalVelocity + GRAVITY*gJumpTime; // Vt = Vo + GT
		//gJumpTime += elapsedTime;
		// S = VtT + 1/2GT^2
		//mKeyDirection.y = mVerticalVelocity*deltaTime + 1/2*GRAVITY*deltaTime*deltaTime;
	}

	mGoalDirection.y -= 40 * deltaTime;

	if(mBaseAnimID == ANIM_JUMP_LOOP)
	{
		//mBodyNode->translate(0,mVerticalVelocity * deltaTime,0,Node::TS_LOCAL);
		mGoalDirection.y = mJump.getHeight(deltaTime) + 40 * deltaTime; 
	}

	NxU32 collisionFlags;
	Ogre::Vector3 vt = mBodyNode->getOrientation() * mGoalDirection;

	//NxQuat q = mSinbadController->GetCharacterActor()->getGlobalOrientationQuat();
	//NxVec3 vv;
	//vv = q.rot(v);
	//Ogre::Quaternion quat = mBodyNode->_getDerivedOrientation();
	//mSinbadController->GetCharacterActor()->setGlobalOrientationQuat(NxQuat(NxVec3(quat.x,quat.y,quat.z),quat.w));
	mSinbadController->Move(NxVec3(vt.x,vt.y,vt.z), collisionFlags);
	NxVec3 v3 = mSinbadController->GetCharacterActor()->getGlobalPosition();
	mBodyNode->setPosition(Ogre::Vector3(v3.x,v3.y + .16,v3.z));

	//mBodyNode->translate(0,0,deltaTime * RUN_SPEED * mAnims[mBaseAnimID]->getWeight(),Node::TS_LOCAL);
	//Ogre::Vector3 v3 = mBodyNode->getPosition();
	//mSinbadController->SetCharacterPos(NxVec3(v3.x,v3.y,v3.z));
	//mCCManager->updateControllers();

	static float preHeight = 0;
	if(mBaseAnimID == ANIM_JUMP_LOOP)
	{
		//if(pos.y <= CHAR_HEIGHT)
		//if(mJump.mJumpTime > 0.5f && mTerrainGroup->getHeightAtWorldPosition(Vector3(v3.x,0,v3.z)) >= (v3.y - 1.5))
		if(collisionFlags == NXCC_COLLISION_DOWN)//preHeight == v3.y && mJump.mJumpTime > 0.5f)
		{
			//pos.y = CHAR_HEIGHT;
			//mBodyNode->setPosition(pos);
			setBaseAnimation(ANIM_JUMP_END,true);
			mJump.stopJump();
			//mTimer = 0;
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
	static bool restore = false;
	static float trans = -0.2f;
	static bool noTrans = false;
	static NxShape * lastShape = NULL;
	NxShape * currentShape = NULL;*/
	mCameraPivot->setPosition(mBodyNode->getPosition() + Vector3::UNIT_Y * 0.3);
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
		mCameraNode->setPosition(Vector3(hit.worldImpact.x,hit.worldImpact.y,hit.worldImpact.z));
	}
	else if (shape && hit.distance < length + 6)
	{
		Ogre::Ray ray1,ray2,ray3,ray4,ray5;
		ray1 = mCamera->getCameraToViewportRay(0,0);
		ray2 = mCamera->getCameraToViewportRay(0,1);
		ray3 = mCamera->getCameraToViewportRay(1,0);
		ray4 = mCamera->getCameraToViewportRay(1,1);

		NxVec3 origins[4];
		origins[0] = NxVec3(ray1.getOrigin().x,ray1.getOrigin().y,ray1.getOrigin().z);
		origins[1] = NxVec3(ray2.getOrigin().x,ray2.getOrigin().y,ray2.getOrigin().z);
		origins[2] = NxVec3(ray3.getOrigin().x,ray3.getOrigin().y,ray3.getOrigin().z);
		origins[3] = NxVec3(ray4.getOrigin().x,ray4.getOrigin().y,ray4.getOrigin().z);

		NxVec3 vecDirs[4];
		for (int i = 0;i<4;i++)
			vecDirs[i] = origins[i] - NxVec3(vv.x,vv.y,vv.z);

		float lengths[4]; 
		for (int i = 0;i<4;i++)
			lengths[i] = vecDirs[i].normalize();

		NxRay nxRay[4];
		for (int i = 0;i<4;i++)
			nxRay[i] = NxRay(vecActor,vecDirs[i]);
		NxRaycastHit hits[4];
		NxShape * shapes[4];
		for (int i = 0;i<4;i++)
			shapes[i] = mScene->raycastClosestShape(nxRay[i],NX_ALL_SHAPES,hits[i],0xffffffff);

		int num = 0;
		for (int i = 1;i<4;i++)
		{
			if (!shapes[i-1] || !shapes[i])
			{
				if (!shapes[i-1])
				{
					num = i;
					continue;
				}
				if (!shapes[i])
				{
					num = i - 1;
					continue;
				}
			}
			else
			{
				if ((lengths[i] - hits[i].distance) >= (lengths[num] - hits[num].distance))
					num = i;		
			}
		}

		mCameraNode->setPosition(Vector3(hit.worldImpact.x,hit.worldImpact.y,hit.worldImpact.z));
		if (lengths[i] - hits[i].distance > hit.distance < length)
		{
			float dis = (lengths[i] - hits[i].distance) * 0.707f;
			mCameraNode->translate(Vector3(0,0,dis),Node::TS_LOCAL);	
		}
	}
	else
	{
		mCameraNode->setPosition(vo);
	}

	mCameraNode->lookAt(mCameraPivot->getPosition(),Node::TS_WORLD);
/*
	if (noTrans && mGoalDirection.z)
	{
		Vector3 previousPos = mCameraNode->_getDerivedPosition();
		mCameraNode->_update(true,true);
		mCameraNode->translate(previousPos - mCameraNode->_getDerivedPosition(),Node::TS_WORLD);
		mTransLeft = mCameraNode->getPosition().z - mLastPos;
	}*/

	//mCameraNode->_update(true,true);
	//mCameraNode->lookAt(mCameraNode->getPosition(),Node::TS_WORLD);

	//if mTransLeft
	/*
	if (!mTransLeft)
		{*/
	//}

	//ray test
/*
		Ogre::Ray ray;
		NxVec3 vecCC(vecPivot.x,vecPivot.y,vecPivot.z);
		NxVec3 vecDir = NxVec3(mCameraOriginal->getPosition().x - vecPivot.x,mCameraOriginal->getPosition().y - vecPivot.y,mCameraOriginal->getPosition().z - vecPivot.z);
		float length = vecDir.normalize();
		NxRay nxRay(vecCC,vecDir);
		NxRaycastHit hit;
		NxShape * shape = mScene->raycastClosestShape(nxRay,NX_ALL_SHAPES,hit);
	
		static float times = 0.0f;
		if (shape && hit.distance < length)
		{
			if (mCameraNode->getPosition().z >= mLastPos + (length - hit.distance))
			{
			mCameraNode->translate(Vector3(0,0,length - hit.distance),Node::TS_LOCAL);
			mTransLeft = length - hit.distance;
			}
		}
		else 
		{
			if (mTransLeft >= 0)
			{
				Vector3 cameraPoint = mCamera->getCameraToViewportRay(.5f,.5f).getOrigin();
				times += deltaTime; 
				float l = mCameraOriginal->getPosition().distance( cameraPoint);
				mTransLeft -= l * times;
				if (mTransLeft < 0)
				{
					mTransLeft = 0;
					return;
				}
				mCameraNode->translate(Vector3(0,0,l * times),Node::TS_LOCAL);
			}
		}*/
	
		
/*
				//Quaternion quat = mCameraPivot->_getDerivedOrientation();
				Vector3 trans =  (Vector3(0,0,-1));
				float deltaTrans;
				deltaTrans = cameraPreviousTest(-1,0,0);
				if (deltaTrans < 0)
				{	
					mTransLeft -= 1;
					if (mTransLeft < 0)
					{
						mTransLeft = 0;
						mNeedRestore = false;
					}	
					else
						mCameraNode->translate(trans,Node::TS_LOCAL);
				}*/

/*
	float deltaTrans = cameraPreviousTest((mBodyNode->getOrientation() * mGoalDirection).z);
	if (deltaTrans > 0)
	{
		mCameraNode->translate(Vector3(0,0,deltaTrans + 0.4f),Node::TS_LOCAL);
		mTransLeft = mCameraNode->getPosition().z - mLastPos;
		mNeedRestore = true;
	}*/


/*
	//mCameraNode->_update(true,true);
	//Vector3 goalOffset = mCameraGoal->_getDerivedPosition() - mCameraNode->_getDerivedPosition();
	//Vector3 trans = Vector3(goalOffset.x,goalOffset.y,goalOffset.z) * deltaTime * 9;
	//mCameraNode->translate(goalOffset * deltaTime * 9);
	//mCameraNode->setPosition(mCameraGoal->_getDerivedPosition());// + mCameraPivot->getPosition() - 4.5);

/ *
	Vector3 pos = mCameraNode->_getDerivedPosition();
	float height = mTerrainGroup->getHeightAtWorldPosition(pos.x,0,pos.z);
	mPitchLimit = false;
	if (pos.y >= height -1)
	{
		mPitchLimit = true;
		pos.y = height -1;
		mCameraNode->_setDerivedPosition(pos);
	}* /

	//Move the camera so that it's clothest to the Sinbad
	Vector3 vv = mCameraPivot->_getDerivedPosition();
/ *
	Vector3 v3 = mCameraNode->_getDerivedPosition();
	Vector3 vvv = mCameraNode->getPosition();* /

	NxVec3 vecActor = NxVec3(vv.x,vv.y,vv.z);//mSinbadController->GetCharacterActor()->getGlobalPosition();

/ *
	static Ogre::Entity * entityCurrent;
	static Ogre::Entity * entityBefore;
* /

	if (mTransLeft && restore)
	{
/ *
		Quaternion quat = mCameraPivot->_getDerivedOrientation();
		Vector3 trans = quat * (Vector3(0,0,-lastPos));* /
		mTransLeft -= 0.2f;
		mCameraNode->translate(Vector3(0,0,trans),Node::TS_LOCAL);
		if (mTransLeft < 0)
		{
			mTransLeft = 0;
			noTrans = false;
			lastShape = currentShape;
		}
/ *
		if (entityCurrent)
		{
			entityCurrent->setVisible(true);
		}
		* /

		restore = false;
	}

	/ *
	Ogre::Ray ray;
		ray = mCamera->getCameraToViewportRay(0.5f,0.5f);
		NxVec3 origin(ray.getOrigin().x,ray.getOrigin().y,ray.getOrigin().z);
	
		NxVec3 vecDir = origin - NxVec3(vv.x,vv.y,vv.z);
		float length = vecDir.normalize();
		NxRay nxRay(vecActor,vecDir);
		//mScene->raycastAllShapes(ray,*mSinbadController,NX_ALL_SHAPES,0xffffffff);
		NxRaycastHit hit;
		shape = mScene->raycastClosestShape(nxRay,NX_ALL_SHAPES,hit,0xffffffff);* /
/ *	Ogre::Ray ray1,ray2,ray3,ray4,ray5;
	ray1 = mCamera->getCameraToViewportRay(0,0);
	ray2 = mCamera->getCameraToViewportRay(0,1);
	ray3 = mCamera->getCameraToViewportRay(1,0);
	ray4 = mCamera->getCameraToViewportRay(1,1);
	ray5 = mCamera->getCameraToViewportRay(.5f,.5f);

	NxVec3 origin[5];
	origin[0] = NxVec3(ray1.getOrigin().x,ray1.getOrigin().y,ray1.getOrigin().z);
	origin[1] = NxVec3(ray2.getOrigin().x,ray2.getOrigin().y,ray2.getOrigin().z);
	origin[2] = NxVec3(ray3.getOrigin().x,ray3.getOrigin().y,ray3.getOrigin().z);
	origin[3] = NxVec3(ray4.getOrigin().x,ray4.getOrigin().y,ray4.getOrigin().z);
	origin[4] = NxVec3(ray5.getOrigin().x,ray5.getOrigin().y,ray5.getOrigin().z);

	NxVec3 vecDir[5];
	for (int i = 0;i<5;i++)
		vecDir[i] = origin[i] - NxVec3(vv.x,vv.y,vv.z);

	float length[5]; 
	for (int i = 0;i<5;i++)
		length[i] = vecDir[i].normalize();

	NxRay nxRay[5];
	for (int i = 0;i<5;i++)
		nxRay[i] = NxRay(vecActor,vecDir[i]);
	//mScene->raycastAllShapes(ray,*mSinbadController,NX_ALL_SHAPES,0xffffffff);
	NxRaycastHit hit[5];
	NxShape * shape[5];
	for (int i = 0;i<5;i++)
		shape[i] = mScene->raycastClosestShape(nxRay[i],NX_ALL_SHAPES,hit[i],0xffffffff);


	int num = 0;
	for (int i = 1;i<4;i++)
	{
		if (!shape[i-1] || !shape[i])
		{
			if (!shape[i-1])
			{
				num = i;
				continue;
			}
			if (!shape[i])
			{
				num = i - 1;
				continue;
			}
		}
		else
		{
			if ((length[i] - hit[i].distance) >= (length[num] - hit[num].distance))
			{
				num = i;
			}					
		}
	}* /


/ *
	if (shape[num] && length[num] - hit[num].distance >= 0 && shape[num]->userData !=  (void*)0x00000011)
	{
		restore = false;
		mLastPos += .2f;
		mCameraNode->translate(Vector3(0,0,.2f),Node::TS_LOCAL);
	}
* /

	if (shape[4] / *&& shape->userData * /&& shape[4]->userData !=  (void*)0x00000011)
	{
		currentShape = shape[4];
		if (lastShape && lastShape != currentShape)
			noTrans = false;
		if (length[4] - hit[4].distance > 0) 
		{
				restore = false;
/ *
				mLastPos += 1;
				float z = mCameraNode->getPosition().z;
				if (z < 28.7)
				{
					mCameraNode->translate(Vector3(0,0,1),Node::TS_LOCAL);
				}* /

				//Vector3 vLimit = mCameraNode->getPosition();
				//if (vLimit.z < 28)
				{
					Vector3 vHit = Vector3(hit->worldImpact.x,hit->worldImpact.y,hit->worldImpact.z);
					//Vector3 vn = mCameraNode->getPosition() - mCameraPivot->convertWorldToLocalPosition(Vector3(hit->worldImpact.x,hit->worldImpact.y,hit->worldImpact.z));
					//mLastPos = -vn.z;

					mCameraNode->translate(Vector3(0,0,length[4] - hit[4].distance),Node::TS_LOCAL);			
					mTransLeft = -mLastPos + mCameraNode->getPosition().z;
				}
				lastShape = currentShape;

/ *
			if (entityBefore && entityBefore != entityCurrent)
			{
				entityBefore->setVisible(true);
			}* /

/ *
			else
			{
				if(hit.distance - length < 5)
				{
					restore = false;
					lastPos += 1;
					mCameraNode->translate(Vector3(0,0,1),Node::TS_LOCAL);				
				}
			}* /

			//mCameraNode->_setDerivedPosition(Vector3(hit.worldImpact.x,hit.worldImpact.y,hit.worldImpact.z));
			//mCameraNode->translate(Vector3(hit.worldImpact.x - v3.x,hit.worldImpact.y - v3.y,hit.worldImpact.z - v3.z),Node::TS_LOCAL);
/ *
		    Quaternion quat = mCameraPivot->_getDerivedOrientation();
			NxVec3 nxVec = hit.worldImpact - origin; 
			Vector3 trans = quat * (Vector3(nxVec.x,nxVec.y,nxVec.z));* /
		}
		else
		{
			if (mTransLeft)
			{
				noTrans = true;
				//restore = true;
			}
			if (lastShape && lastShape != currentShape)
			{
				noTrans = false;
				restore = true;
			}
/ *
			entityCurrent = (Entity *)(shape->userData);
			if (entityCurrent->isVisible())
			{
				entityCurrent->setVisible(false);
			}* /

		}

/ *
		float newDistance = (mCameraNode->getOrientation() * Vector3(0,0,0.23f)).length();
		if (mLastPos && hit[4].distance >= length[4] + newDistance)
		{
			restore = true;
		}
* /


/ *
		float newDistance = NxMath::sqrt((hit.worldImpact.x - vv.x) * (hit.worldImpact.x - vv.y)+ 0.22 * 0.22 + (hit.worldImpact.y - vv.y) * (hit.worldImpact.y - vv.y));
		if (hit.distance <= length + newDistance)
		{
	//		restore = true;
		}* /

	}
	else 
	{
		restore = true;
		lastShape = currentShape;

/ *
		if (entityCurrent)
		{
			if (!entityCurrent->isVisible())
			{
				entityCurrent->setVisible(true);
			}
		}* /

	}

	/ *
	if (mSinbadController->stop == true && mSinbadController->mClosestPoint != NxVec3(0.0f,0.0f,0.0f))
		{
			//mSinbadController->mClosestPoint += .1 * vecDir;
			//mCameraNode->_setDerivedPosition(Vector3(mSinbadController->mClosestPoint.x,mSinbadController->mClosestPoint.y,mSinbadController->mClosestPoint.z));
			mCameraNode->translate(-Vector3(vecDir.x,vecDir.y,vecDir.z),Node::TS_LOCAL);
			mSinbadController->mClosestPoint.zero();
		}
		mSinbadController->stop = false;
		mCameraNode->_update(true,true);* /*/



/*
	if (shapes[0]->getActor() != mSinbadController->GetCharacterActor())
	{
		int i = 0;
		while (hit.shape[i++])
		{
			if (shapes[i]->getActor() == mSinbadController->GetCharacterActor())
			{
				i -= 2;
				break;
			}
		}
		//hit.worldImpact
		//hit.shape[i].*/
}

void SinbadCharacterController::updateCameraGoal(Ogre::Real deltaYaw, Ogre::Real deltaPitch, Ogre::Real deltaZoom)
{
/*
	float deltaTrans;
	if(!(mPivotPitch + deltaPitch > 75 && deltaPitch > 0) && !(mPivotPitch + deltaPitch < -75 && deltaPitch < 0))
		deltaTrans = cameraPreviousTest(0,deltaYaw,deltaPitch);
	else
		deltaTrans = cameraPreviousTest(0,0,deltaPitch);
	
	if (deltaTrans > 0)
	{
		mCameraNode->translate(Vector3(0,0,deltaTrans),Node::TS_LOCAL);
		mTransLeft = mCameraNode->getPosition().z - mLastPos;
		mCameraNode->_update(true,true);
		mNeedRestore = true;
	}*/

	mCameraPivot->yaw(Degree(deltaYaw),Node::TS_LOCAL);

	/*
	Quaternion q;
		Matrix3 matrix33 = mCameraPivot->getLocalAxes();
	
		if (mTransLeft)
		{
			Vector3 vUp = Vector3(matrix33[1]);
			q.FromAngleAxis( Radian(Degree(deltaYaw)), vUp);
			Vector3 v = mCameraOriginal->getPosition();
			mCameraOriginal->setPosition(q * v);
		}*/
	

/*
	if(!(mPivotPitch + deltaPitch > 75 && deltaPitch > 0) && !(mPivotPitch + deltaPitch < -75 && deltaPitch < 0))
	{*/

	mCameraPivot->pitch(Degree(deltaPitch), Node::TS_LOCAL);
	mPivotPitch += deltaPitch;

/*
	Vector3 vUp = Vector3(matrix33[0]);
	q.FromAngleAxis( Radian(Degree(deltaPitch)), vUp);
	Vector3 v = mCameraOriginal->getPosition();
	mCameraOriginal->setPosition(q * v);*/

	//}

	Real dist = mBodyNode->_getDerivedPosition().distance(mCameraOriginal->_getDerivedPosition());
	Real distChange = deltaZoom * dist;

	// bound the zoom
	if (!mTransLeft && dist + distChange >= 6 && dist + distChange <= 28)
	{
		//if (!mTransLeft)
		//{
			mCameraOriginal->translate(0, 0, distChange, Node::TS_LOCAL);
			//mLastPos = mCameraNode->getPosition().z;
/*
			if (!mNeedRestore)
			{
				mLastPos = mCameraNode->getPosition().z;
			}*/

			//mTransLeft = ;//mCameraNode->getPosition().z - mLastPos;
		//}
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
}

bool OgitorSampleListener::mouseMoved(const OIS::MouseEvent &arg)
{
	CEGUI::System& cegui = CEGUI::System::getSingleton();

	cegui.injectMouseMove(arg.state.X.rel * mMouseSpeed, arg.state.Y.rel * mMouseSpeed);
	cegui.injectMouseWheelChange(arg.state.Z.rel / 120.0f);

	if(mRMouseDown)
	{
		//mCameraMan->getCamera()->yaw(Ogre::Degree(-arg.state.X.rel * mRotateSpeed));
		//mCameraMan->getCamera()->pitch(Ogre::Degree(-arg.state.Y.rel * mRotateSpeed));
		((OgitorSampleApp*)mApp)->mChara->injectMouseMove(arg);
	}
	return mApp->mouseMoved(arg);
}

void OgitorSampleApp::createFrameListener()
{
	OgitorSampleListener * listener = new OgitorSampleListener(this,mRoot,mSceneMgr,mWindow,mCameraMan,mOgreRenderer);
	listener->initialise();
	mListener = listener;
}


