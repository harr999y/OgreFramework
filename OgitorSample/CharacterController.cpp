#include "CharacterController.h"

String gAnimationNames[] = {"Stand","Attack1H","Whirlwind","Run","JumpStart","Jump","JumpEnd","JumpLandRun","HandsClosed"};

//set the lower and upper bones
unsigned gUpperBodyBones[] = {1 ,2 ,3 ,4 ,10 ,11 ,12 ,18 ,19 ,20 ,21 ,22 ,23 ,24 ,30 ,31 ,32 ,33 ,34 ,35 ,36 ,37 ,38 ,39 ,40 ,41 ,42 ,43 ,44 ,48 ,49 ,50 ,51 ,52 ,53 ,54 ,55 ,56 ,57 ,58 ,59 ,60 ,61 ,62 ,63 ,64 ,65 ,66 ,67 ,69 ,70 ,88 ,89 ,90 ,91 ,92 ,93 ,94 ,95 ,96 ,97 ,98 ,99 ,100 ,101 ,102 ,103 ,104 ,105 ,108 ,109 ,110 ,111 ,112 ,113 ,114 ,115 ,116 ,117 ,118 ,119 ,120 ,121 ,122 ,123 ,124 ,125 ,126 ,127 ,128 ,129 ,130 ,131 ,134 ,135 ,136 ,137 ,138 ,139 ,141 ,142 ,143 ,144,-1};
unsigned gLowerBodyBones[] = {0 ,5 ,6 ,7 ,8 ,9 ,13 ,14 ,15 ,16 ,17 ,25 ,26 ,27 ,28 ,29 ,45 ,46 ,47 ,68 ,71 ,72 ,73 ,74 ,75 ,76 ,77 ,78 ,79 ,80 ,81 ,82 ,83 ,84 ,85 ,86 ,87 ,106 ,107 ,132 ,133 ,140,-1};

static const int ShiftMulti = 2;

void CharacterController::setup()
{
	setupConfigFile();
	setupBody();	
	setupCameraController();
	setupAnimations();
}

void CharacterController::setupBody()
{
	SceneManager * sceneMgr;
	sceneMgr = mCamera->getSceneManager();
	mBodyNode = sceneMgr->getRootSceneNode()->createChildSceneNode(
		Vector3(0, mTerrainGroup->getHeightAtWorldPosition(0,0,0) + 10.5, 0));

	mBodyEntity = sceneMgr->createEntity("SinbadBody","Å£Í·ÈË.mesh");
	mBodyNode->attachObject(mBodyEntity);

	//attach weapon and shield.
	assert((mWeaponName != String("")) && "Ã°µÄÎäÆ÷Ãû×ÖÄØ£¡");
	mWeapon = mCamera->getSceneManager()->createEntity(mWeaponName);
	assert((mShieldName != String("")) && "Ã°µÄ¶ÜÅÆÃû×ÖÄØ£¡");
	mShield = mCamera->getSceneManager()->createEntity(mShieldName);

	FontPtr font = FontManager::getSingleton().getByName("SdkTrays/Chinese");
	font->load();

	MovableText * msg = new MovableText("xx",L"ò§çïÖ®Òí~","SdkTrays/Chinese",.6f,ColourValue::Red);
	msg->setTextAlignment(MovableText::H_CENTER,MovableText::V_ABOVE);
	msg->setCharacterHeight(.5f);
	msg->setLocalTranslation(Vector3(0.5f,2.8f,-3.0f));
	msg->showOnTop(true);
	mBodyNode->attachObject(msg);
	mBodyNode->setFixedYawAxis(true);

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

	mSwordTrail->setVisible(true);

	UserAllocator * mAllocator = new UserAllocator;
	mCCManager = NxCreateControllerManager(mAllocator);

	mKeyDirection = Vector3::ZERO;

	Ogre::AxisAlignedBox box = mBodyEntity->getBoundingBox();
	Ogre::Vector3 size = box.getSize();

	mCharacterController = new MyCharacterController(
		mCCManager,mNxScene,
		NxVec3(-100, mTerrainGroup->getHeightAtWorldPosition(-100,0,-150) + 1.4f,-150),
		size.x / 4.7,
		size.y * 0.41f);

	mCharacterController->GetCharacterActor()->getShapes()[0]->setFlag(NX_SF_DISABLE_RAYCASTING, true);
	mCharacterController->GetCharacterActor()->getShapes()[0]->setFlag(NX_SF_DISABLE_SCENE_QUERIES, true);
}

void CharacterController::setupCameraController()
{
	mCamera->setNearClipDistance(0.1f);
	mCamera->setFarClipDistance(0);
	mCamera->detachFromParent();

	mCameraController = new MyCameraController(mCamera,mBodyNode,mNxScene,mTerrainGroup);
}

void CharacterController::setupAnimations()
{
	mBodyEntity->getSkeleton()->setBlendMode(ANIMBLEND_CUMULATIVE);

	mAnimationStateSet = mBodyEntity->getAllAnimationStates();
	AnimationStateIterator it = mAnimationStateSet->getAnimationStateIterator();

	for(unsigned i = 0;it.hasMoreElements();i++)
	{
		AnimationState * as = it.getNext();
		mAnimationNameIDs[as->getAnimationName()] = i;
		mAnimationNames.push_back(as->getAnimationName()); 
	}

	for(unsigned i=0;i<mAnimationNames.size();i++)
	{
		mAnimations.push_back(mBodyEntity->getAnimationState(mAnimationNames[i]));
		mAnimations[i]->setLoop(false);
		mFadingIn.push_back(false);
		mFadingOut.push_back(false);
	}

	for (int i = 0;i<mBodyEntity->getSkeleton()->getNumAnimations();i++)
	{
		mUpperBodyInvalid.push_back(false);
		mLowerBodyInvalid.push_back(false);
	}

	mActiveAnimationID = mAnimationNameIDs[gAnimationNames[STAND]];//Helpler::findStringVector(mAnimationNames,gAnimationNames[STAND]);
	mAnimations[mActiveAnimationID]->setEnabled(true);
}

void CharacterController::setupConfigFile()
{
	ConfigFile cf;
	cf.load("../config/WeaponAndShield.cfg");

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
			if (typeName == "WeaponName")
				mWeaponName = archName;
			else if (typeName == "ShieldName")
				mShieldName = archName;
		}
	}
}

void CharacterController::changeAnimation(unsigned id,bool loop,bool notStopPrevious)
{
	if (id == -1)
		id = mAnimationNameIDs[gAnimationNames[STAND]];

	if (!notStopPrevious && mActiveAnimationID >= 0)
	{
		// if we have an old animation, fade it out
		mFadingIn[mActiveAnimationID] = false;
		mFadingOut[mActiveAnimationID] = true;
	}
	
	mAnimations[id]->setEnabled(true);
	mAnimations[id]->setLoop(loop);
	mAnimations[id]->setWeight(0);
	mFadingOut[id] = false;
	mFadingIn[id] = true;
	mAnimations[id]->setTimePosition(0);
	mActiveAnimationID = id;
	mTimer = 0.0f;
}

void CharacterController::setLowerBonesState(unsigned id,bool valid)
{
	if (!mAnimations[id]->hasBlendMask())
		mAnimations[id]->createBlendMask(mBodyEntity->getSkeleton()->getNumBones());

	if (!valid && !mLowerBodyInvalid[id])
	{
		for (int i = 0;gLowerBodyBones[i] != -1;i++)
		{
			Bone * bone = mBodyEntity->getSkeleton()->getBone(gLowerBodyBones[i]);
			mAnimations[id]->setBlendMaskEntry(bone->getHandle(),0);
		}
		mLowerBodyInvalid[id] = true;
	}
	else if (valid && mLowerBodyInvalid[id])
	{
		for (int i = 0;gLowerBodyBones[i] != -1;i++)
		{
			Bone * bone = mBodyEntity->getSkeleton()->getBone(gLowerBodyBones[i]);
			mAnimations[id]->setBlendMaskEntry(bone->getHandle(),1);
		}
		mLowerBodyInvalid[id] = false;
	}
}

void CharacterController::setUpperBonesState(unsigned id,bool valid)
{
	if (!mAnimations[id]->hasBlendMask())
		mAnimations[id]->createBlendMask(mBodyEntity->getSkeleton()->getNumBones());

	if (!valid && !mUpperBodyInvalid[id])
	{
		for (int i = 0;gUpperBodyBones[i] != -1;i++)
		{
			Bone * bone = mBodyEntity->getSkeleton()->getBone(gUpperBodyBones[i]);
			mAnimations[id]->setBlendMaskEntry(bone->getHandle(),0);
		}
		mUpperBodyInvalid[id] = true;
	}
	else if (valid && mUpperBodyInvalid[id])
	{
		for (int i = 0;gUpperBodyBones[i] != -1;i++)
		{
			Bone * bone = mBodyEntity->getSkeleton()->getBone(gUpperBodyBones[i]);
			mAnimations[id]->setBlendMaskEntry(bone->getHandle(),1);
		}
		mUpperBodyInvalid[id] = false;
	}
}

void CharacterController::update(float deltaTime)
{
	updateBody(deltaTime);
	updateAnimations(deltaTime);
	mCameraController->update(deltaTime);
}

void CharacterController::updateBody(float deltaTime)
{
	mGoalDirection = Vector3::ZERO;

	if(mKeyDirection != Vector3::ZERO && !mAnimations[mAnimationNameIDs[gAnimationNames[DANCE]]]->getEnabled())
	{
		mGoalDirection += mKeyDirection.z * mCameraController->getCameraPivot()->_getDerivedOrientation().zAxis();
		mGoalDirection += mKeyDirection.x * mCameraController->getCameraPivot()->_getDerivedOrientation().xAxis();
		mGoalDirection.y = 0;
		mGoalDirection.normalise();

		Quaternion toGoal = mBodyNode->getOrientation().zAxis().getRotationTo(mGoalDirection);

		Real yawToGoal = toGoal.getYaw().valueDegrees();
		Real yawAtSpeed = yawToGoal / Math::Abs(yawToGoal) * deltaTime * TURN_SPEED;
		if(mJump.isJumping())
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

		mGoalDirection = Vector3(0,0,deltaTime * RUN_SPEED * mAnimations[mActiveAnimationID]->getWeight());		
		mBodyNode->yaw(Degree(yawToGoal));
	}

	mGoalDirection.y -= 40 * deltaTime;

	if(mJump.isJumping())
	{
		mGoalDirection.y = mJump.getHeight(deltaTime) + 40 * deltaTime; 
	}

	NxU32 collisionFlags;
	Ogre::Vector3 vt = mBodyNode->getOrientation() * mGoalDirection;
	vt *= mIsShifting ? ShiftMulti : 1;

	mCharacterController->Move(NxVec3(vt.x,vt.y,vt.z), collisionFlags);
	NxVec3 v3 = mCharacterController->GetCharacterActor()->getGlobalPosition();
	mBodyNode->setPosition(Ogre::Vector3(v3.x,v3.y - 0.6,v3.z));

	if(mJump.isJumping())
	{
		if(collisionFlags == NXCC_COLLISION_DOWN)
		{
			if (mKeyDirection.z || mKeyDirection.x)
			{
				if (mAnimations[mAnimationNameIDs[gAnimationNames[ATTACK]]]->getEnabled() && mAnimations[mAnimationNameIDs[gAnimationNames[ATTACK]]]->hasEnded())
				{
					setUpperBonesState(mAnimationNameIDs[gAnimationNames[JUMPENDRUN]],false);
					setLowerBonesState(mAnimationNameIDs[gAnimationNames[ATTACK]],false);
					changeAnimation(mAnimationNameIDs[gAnimationNames[JUMPENDRUN]],false);
					mFadingOut[mAnimationNameIDs[gAnimationNames[ATTACK]]] = false;
				}
				else
					changeAnimation(mAnimationNameIDs[gAnimationNames[JUMPENDRUN]]);
			}
			else if (mAnimations[mAnimationNameIDs[gAnimationNames[ATTACK]]]->getEnabled() && !mAnimations[mAnimationNameIDs[gAnimationNames[ATTACK]]]->hasEnded())
			{
				changeAnimation(mAnimationNameIDs[gAnimationNames[STAND]]);
			}
			else
				changeAnimation(mAnimationNameIDs[gAnimationNames[JUMPEND]]);
			mJump.stopJump();
		}
	}
}

void CharacterController::updateAnimations(float deltaTime)
{
	Real animSpeed = 1;

	mTimer += deltaTime;
	if (mAnimations[mAnimationNameIDs[gAnimationNames[ATTACK]]]->getEnabled())
		mAttackTimer += deltaTime;

	if(mAttackTimer >= mAnimations[mAnimationNameIDs[gAnimationNames[ATTACK]]]->getLength())//mAnimations[mAnimationNameIDs[gAnimationNames[ATTACK]]]->getEnabled() && mAnimations[mAnimationNameIDs[gAnimationNames[ATTACK]]]->hasEnded())
	{
		mAttackTimer = 0.0f;
		for (int i = 0;i < ANIMS_NUM;i++)
		{
			setUpperBonesState(mAnimationNameIDs[gAnimationNames[i]],true);
			setLowerBonesState(mAnimationNameIDs[gAnimationNames[i]],true);
		}
		if (!mAnimations[mAnimationNameIDs[gAnimationNames[RUN]]]->getEnabled() && !mJump.isJumping())
		{
			if (mAnimationNameIDs[gAnimationNames[STAND]] != mActiveAnimationID)
			{
				changeAnimation(mAnimationNameIDs[gAnimationNames[STAND]],true);
			}
		}
		else if (mAnimations[mAnimationNameIDs[gAnimationNames[JUMPENDRUN]]]->getEnabled())
		{
			if (mKeyDirection.isZeroLength())
			{
				//changeAnimation(mAnimationNameIDs[gAnimationNames[JUMPSTART]]);
			}
			else
			{
				changeAnimation(mAnimationNameIDs[gAnimationNames[RUN]],true);
			}
		}
		mFadingIn[mAnimationNameIDs[gAnimationNames[ATTACK]]] = false;
		mFadingOut[mAnimationNameIDs[gAnimationNames[ATTACK]]] = true;

/*
		if (mKeyDirection != Vector3::ZERO)
		{
			setLowerBonesState(mAnimationNameIDs[gAnimationNames[ATTACK]],false);
			setUpperBonesState(mAnimationNameIDs[gAnimationNames[RUN]],false);
			changeAnimation(mAnimationNameIDs[gAnimationNames[RUN]],true,true);
		}
		if (mTimer >= mAnimations[mAnimationNameIDs[gAnimationNames[ATTACK]]]->getLength() - 0.1)
		{
		}*/
	}
	if(mAnimations[mAnimationNameIDs[gAnimationNames[JUMPSTART]]]->getEnabled() && mAnimations[mAnimationNameIDs[gAnimationNames[JUMPSTART]]]->hasEnded())
	{
			//changeAnimation(mAnimationNameIDs[gAnimationNames[JUMPING]]);
	}
	else if((mAnimations[mAnimationNameIDs[gAnimationNames[JUMPEND]]]->getEnabled() && mAnimations[mAnimationNameIDs[gAnimationNames[JUMPEND]]]->hasEnded()) || (mAnimations[mAnimationNameIDs[gAnimationNames[JUMPENDRUN]]]->getEnabled() && mTimer >= mAnimations[mAnimationNameIDs[gAnimationNames[JUMPENDRUN]]]->getLength() - 0.65f))
	{
		if(mKeyDirection != Vector3::ZERO)
		{
			changeAnimation(mAnimationNameIDs[gAnimationNames[RUN]],true);
			//mAnimations[mAnimationNameIDs[gAnimationNames[RUN]]]->setWeight(1);
			//mAnimations[mAnimationNameIDs[gAnimationNames[RUN]]]->setTimePosition(0.3f);
			//mAnimations[mAnimationNameIDs[gAnimationNames[RUN]]]->setWeight(.8f);
			mAnimations[mAnimationNameIDs[gAnimationNames[JUMPENDRUN]]]->setEnabled(false);
		}
		else
		{
			changeAnimation(mAnimationNameIDs[gAnimationNames[STAND]]);
		}
	}

	for (int i = 0;i < ANIMS_NUM;i++)
	{
		if (mAnimations[mAnimationNameIDs[gAnimationNames[i]]]->getEnabled())
		{
			mAnimations[mAnimationNameIDs[gAnimationNames[i]]]->addTime(deltaTime);
		}
	}
	
	fadeAnimations(deltaTime);

	if (mShield && mShield->getSkeleton()->getNumAnimations())
	{
		mShield->getAnimationState(gAnimationNames[STAND])->addTime(deltaTime);
	}
}

void CharacterController::fadeAnimations(float deltaTime)
{
	for(unsigned i=0;i<mAnimationNames.size();i++)
	{
		if(mFadingIn[i])
		{
			Real newWeight = mAnimations[i]->getWeight() + deltaTime * ANIM_FADE_SPEED;
			mAnimations[i]->setWeight(Math::Clamp<Real>(newWeight,0,1));
			if(newWeight >= 1)
				mFadingIn[i] = false;
		}
		else if(mFadingOut[i])
		{
			Real newWeight = mAnimations[i]->getWeight() - deltaTime * ANIM_FADE_SPEED;
			mAnimations[i]->setWeight(Math::Clamp<Real>(newWeight,0,1));
			if(newWeight <= 0)
			{
				mAnimations[i]->setEnabled(false);
				mFadingOut[i] = false;
			}
		}
	}
}

void CharacterController::injectKeyPressed(const OIS::KeyEvent &evt)
{
	if (evt.key == OIS::KC_LSHIFT)
	{
		mIsShifting = true;

		CompositorManager::getSingleton().setCompositorEnabled(mCamera->getViewport(), "Radial Blur", mIsShifting);
	}

	if(evt.key == OIS::KC_Q && ((mAnimations[mAnimationNameIDs[gAnimationNames[STAND]]]->getEnabled() && !mAnimations[mAnimationNameIDs[gAnimationNames[STAND]]]->hasEnded()) || (mAnimations[mAnimationNameIDs[gAnimationNames[RUN]]]->getEnabled() && !mAnimations[mAnimationNameIDs[gAnimationNames[RUN]]]->hasEnded())))
	{
		handleWeapon();
	}
	else if (evt.key == OIS::KC_Z && ((mAnimations[mAnimationNameIDs[gAnimationNames[STAND]]]->getEnabled() && !mAnimations[mAnimationNameIDs[gAnimationNames[STAND]]]->hasEnded()) || (mAnimations[mAnimationNameIDs[gAnimationNames[RUN]]]->getEnabled() && !mAnimations[mAnimationNameIDs[gAnimationNames[RUN]]]->hasEnded())))
	{
		handleShield();
	}
	else if(evt.key == OIS::KC_E)
	{
		if((mAnimations[mAnimationNameIDs[gAnimationNames[STAND]]]->getEnabled() && !mAnimations[mAnimationNameIDs[gAnimationNames[STAND]]]->hasEnded()) || (mAnimations[mAnimationNameIDs[gAnimationNames[RUN]]]->getEnabled() && !mAnimations[mAnimationNameIDs[gAnimationNames[RUN]]]->hasEnded()))
		{
			changeAnimation(mAnimationNameIDs[gAnimationNames[DANCE]],true);
		}
		else if(mAnimations[mAnimationNameIDs[gAnimationNames[DANCE]]]->getEnabled() && !mAnimations[mAnimationNameIDs[gAnimationNames[DANCE]]]->hasEnded())
		{
			changeAnimation(mAnimationNameIDs[gAnimationNames[STAND]]);
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
	else if(evt.key == OIS::KC_SPACE && !mAnimations[mAnimationNameIDs[gAnimationNames[DANCE]]]->getEnabled() && !mJump.isJumping())
	{
		if (mAnimations[mAnimationNameIDs[gAnimationNames[ATTACK]]]->getEnabled())
		{
			changeAnimation(mAnimationNameIDs[gAnimationNames[JUMPSTART]],false,true);
		}
		else
			changeAnimation(mAnimationNameIDs[gAnimationNames[JUMPSTART]]);
		mJump.startJump(0.6);
	}

	if(!mKeyDirection.isZeroLength() && !mJump.isJumping())
	{
		if (mAnimations[mAnimationNameIDs[gAnimationNames[ATTACK]]]->getEnabled() && !mFadingOut[mAnimationNameIDs[gAnimationNames[ATTACK]]])//!mAnimations[mAnimationNameIDs[gAnimationNames[ATTACK]]]->hasEnded())
		{
			setLowerBonesState(mAnimationNameIDs[gAnimationNames[ATTACK]],false);
			setUpperBonesState(mAnimationNameIDs[gAnimationNames[RUN]],false);
			changeAnimation(mAnimationNameIDs[gAnimationNames[RUN]],true);
		}
		else
		{
			changeAnimation(mAnimationNameIDs[gAnimationNames[RUN]],true);
		}
	}
}

void CharacterController::handleWeapon()
{
	static bool handling = false;
	if (!handling)
	{
		Ogre::SkeletonInstance * si = mBodyEntity->getSkeleton();
		Bone * bone = si->getBone(138);

		if (!mWeapon)
		{
			mWeapon = mCamera->getSceneManager()->createEntity("axe_1h_alliancecovenant_d_01.mesh");
		}

		mBodyEntity->attachObjectToBone(bone->getName(),mWeapon);

		unsigned ID = mAnimationNameIDs[gAnimationNames[HANDSCLOSED]];
		mAnimations[ID]->setEnabled(true);
		mFadingIn[ID] = true;
		mFadingOut[ID] = false;
		handling = true;

		mSwordTrail->addNode(mWeapon->getParentNode());
	}
	else
	{
		mSwordTrail->removeNode(mWeapon->getParentNode());

		if (mWeapon)
			mBodyEntity->detachObjectFromBone(mWeapon);

		mFadingIn[mAnimationNameIDs[gAnimationNames[HANDSCLOSED]]] = false;
		mFadingOut[mAnimationNameIDs[gAnimationNames[HANDSCLOSED]]] = true;
		handling = false;
	}
}

void CharacterController::handleShield()
{
	static bool handling = false;
	if (!handling)
	{
		Ogre::SkeletonInstance * si = mBodyEntity->getSkeleton();

		Bone * bone = si->getBone(58);

		if (!mShield)
		{
			mShield = mCamera->getSceneManager()->createEntity("shield_zulaman_d_01.mesh");
		}

		mBodyEntity->attachObjectToBone(bone->getName(),mShield);
		if (mShield->hasSkeleton() && mShield->getSkeleton()->getNumAnimations())
		{
			mShield->getAnimationState(gAnimationNames[STAND])->setEnabled(true);
		}
		unsigned ID = mAnimationNameIDs[gAnimationNames[HANDSCLOSED]];
		mAnimations[ID]->setEnabled(true);
		mFadingIn[ID] = true;
		mFadingOut[ID] = false;
		handling = true;

		mSwordTrail->addNode(mShield->getParentNode());
	}
	else
	{
		mSwordTrail->removeNode(mShield->getParentNode());

		if (mShield)
			mBodyEntity->detachObjectFromBone(mShield);

		unsigned ID = mAnimationNameIDs[gAnimationNames[HANDSCLOSED]];
		mFadingIn[ID] = false;
		mFadingOut[ID] = true;
		handling = false;
	}
}

void CharacterController::injectKeyReleased(const OIS::KeyEvent &evt)
{
	if(evt.key == OIS::KC_W && mKeyDirection.z == 1)
		mKeyDirection.z = 0.0f;
	else if (evt.key == OIS::KC_A && mKeyDirection.x == 1)
		mKeyDirection.x = 0.0f;
	else if (evt.key == OIS::KC_S && mKeyDirection.z == -1) 
		mKeyDirection.z = 0.0f;
	else if (evt.key == OIS::KC_D && mKeyDirection.x == -1) 
		mKeyDirection.x = 0.0f;

	if (evt.key == OIS::KC_LSHIFT)
	{
		mIsShifting = false;

		CompositorManager::getSingleton().setCompositorEnabled(mCamera->getViewport(), "Radial Blur", mIsShifting);
	}

	if(mKeyDirection.isZeroLength())
	{
		setLowerBonesState(mAnimationNameIDs[gAnimationNames[ATTACK]],true);
		if (!mJump.isJumping())
		{
			if(mAnimations[mAnimationNameIDs[gAnimationNames[RUN]]]->getEnabled() || mAnimations[mAnimationNameIDs[gAnimationNames[JUMPENDRUN]]]->getEnabled()) 
			{
				mFadingIn[mAnimationNameIDs[gAnimationNames[RUN]]] = false;
				mFadingOut[mAnimationNameIDs[gAnimationNames[RUN]]] = true;
				changeAnimation(mAnimationNameIDs[gAnimationNames[STAND]],true);
			}
		}
	}
}

void CharacterController::injectMouseMove(const OIS::MouseEvent &evt)
{
	mCameraController->updateGoal(0.05f * evt.state.Y.rel, -0.10f * evt.state.X.rel, -0.001f * evt.state.Z.rel);
}

void CharacterController::injectMousePressed(const OIS::MouseEvent &evt,OIS::MouseButtonID id)
{
	if (id == OIS::MB_Left && !mAnimations[mAnimationNameIDs[gAnimationNames[DANCE]]]->getEnabled() && !mAnimations[mAnimationNameIDs[gAnimationNames[ATTACK]]]->getEnabled() && !mAnimations[mAnimationNameIDs[gAnimationNames[JUMPENDRUN]]]->getEnabled())
	{
		if (mAnimations[mAnimationNameIDs[gAnimationNames[RUN]]]->getEnabled() && !mFadingOut[mAnimationNameIDs[gAnimationNames[RUN]]])
		{
			setLowerBonesState(mAnimationNameIDs[gAnimationNames[ATTACK]],false);
			setUpperBonesState(mAnimationNameIDs[gAnimationNames[RUN]],false);
			changeAnimation(mAnimationNameIDs[gAnimationNames[ATTACK]],false,true);
			mActiveAnimationID = mAnimationNameIDs[gAnimationNames[RUN]];
		}
		else if (mJump.isJumping())
		{
			//setLowerBonesState(mAnimationNameIDs[gAnimationNames[ATTACK]],false);
			//setUpperBonesState(mActiveAnimationID,false);

			unsigned id = mActiveAnimationID;
			setUpperBonesState(mAnimationNameIDs[gAnimationNames[JUMPSTART]],false);
			changeAnimation(mAnimationNameIDs[gAnimationNames[ATTACK]],false,true);
			mActiveAnimationID = id;
		}
		else
		{
			changeAnimation(mAnimationNameIDs[gAnimationNames[ATTACK]]);
			mActiveAnimationID = mAnimationNameIDs[gAnimationNames[STAND]];
		}
	}

}

void CharacterController::injectMouseReleased(const OIS::MouseEvent &evt,OIS::MouseButtonID id)
{}
