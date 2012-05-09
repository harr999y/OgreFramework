#include "NxOgreSampleCar.h"

void NxOgreSampleCarApp::createScene()
{
	setupPhysX();
	createTerrain();
	createCar();

	mCameraMan->setTopSpeed(50);
}

static float gPressTime = 0;

void NxOgreSampleCarApp::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	gPressTime += evt.timeSinceLastFrame;
	if (gPressTime > 0.13f)
	{
		//if ((GetAsyncKeyState('T') && 0xff) || (GetAsyncKeyState('G') && 0xff) || (GetAsyncKeyState('F') && 0xff) || (GetAsyncKeyState('H') && 0xff))
		//{
		
		if (wheelLeftFront->getAxleSpeed() && (mSteerAngle > 0 || mSteerAngle < 0))
		{
			if (mSteerAngle > 0)
			{
				mSteerAngle -= NxPi * 0.006;
				if (mSteerAngle < NxPi * 0.006)
				{
					mSteerAngle = 0;
				}				
			}
			else if(mSteerAngle < 0)
			{
				mSteerAngle += NxPi * 0.006;
				if (mSteerAngle > -NxPi * 0.006)
				{
					mSteerAngle = 0;
				}				
			}
			wheelLeftFront->setSteerAngle(mSteerAngle);
			wheelRightFront->setSteerAngle(mSteerAngle);
			wheelLeftFront->setSteerAngle(mSteerAngle);
			wheelRightFront->setSteerAngle(mSteerAngle);

		}
		if (GetAsyncKeyState('T') && 0xff)
		{
			NxQuat quat;
			NxMat33 mat33 = wheelLeftFront->getGlobalOrientation();
			NxVec3 src(0,0,1),dest;
			mat33.multiply(src,dest);
			//quat.normalise();
			mCar->addForce(NxOgre::Vec3(dest.x,dest.y,dest.z) * 50000);
		}
		if (GetAsyncKeyState('G') && 0xff)
		{
			NxQuat quat;
			NxMat33 mat33 = wheelLeftFront->getGlobalOrientation();
			NxVec3 src(0,0,1),dest;
			mat33.multiply(src,dest);
			mCar->addForce(NxOgre::Vec3(dest.x,dest.y,dest.z) * -50000);
		}
		if (GetAsyncKeyState('F') && 0xff)
		{
			if (mSteerAngle <= 0.75f)
			{
				mSteerAngle += NxPi * 0.015;
				wheelLeftFront->setSteerAngle(mSteerAngle);
				wheelRightFront->setSteerAngle(mSteerAngle);
			}

			//wheelLeftRear->setSteerAngle(mSteerAngle);
			//wheelRightRear->setSteerAngle(mSteerAngle);
			//mCar->addForce(NxOgre::Vec3(100000,0,0));
		}
		if (GetAsyncKeyState('H') && 0xff)
		{
			if (mSteerAngle >= -0.75f)
			{
				mSteerAngle -= NxPi * 0.015;
				wheelLeftFront->setSteerAngle(mSteerAngle);
				wheelRightFront->setSteerAngle(mSteerAngle);
			}
			//mCar->addForce(NxOgre::Vec3(-100000,0,0));
		}

		gPressTime = 0;

		//}	
	}
	mWorld->advance(evt.timeSinceLastFrame);
}
void NxOgreSampleCarApp::setupPhysX()
{
	mWorld = NxOgre::World::createWorld();
	NxOgre::ResourceSystem::getSingleton()->openProtocol(new Critter::OgreResourceProtocol());
	NxOgre::SceneDescription sceneDes;
	sceneDes.mGravity = NxOgre::Constants::MEAN_EARTH_GRAVITY;
	//sceneDes.mUseHardware = true;
	sceneDes.mUseHardware = false;
	sceneDes.mWorkerThreadsCount = 2;
	sceneDes.mBackgroundThreadsCount = 2;
	sceneDes.mFlags |= NxOgre::SceneFlags::EnableMultithread | NxOgre::SceneFlags::FluidPerformanceHint | NxOgre::SceneFlags::AlternativeFluidTriangleCollision | NxOgre::SceneFlags::MultithreadedForcefield;
	mScene = mWorld->createScene(sceneDes);
	mScene->getMaterial(0)->setAll(0.1f,0.9f,0.5f);
	mScene->createSceneGeometry(NxOgre::PlaneGeometryDescription());
	mRenderSystem = new Critter::RenderSystem(mScene,mSceneMgr);

	mWorld->getRemoteDebugger()->connect();
	//mRenderSystem->setVisualisationMode(NxOgre::Enums::VisualDebugger_ShowAll);
}

void NxOgreSampleCarApp::createTerrain()
{
	using namespace Ogre;
	mTerrainGlobals = OGRE_NEW TerrainGlobalOptions();
	mTerrainGroup = OGRE_NEW TerrainGroup(mSceneMgr, Terrain::ALIGN_X_Z, 65, 200);
	mTerrainGroup->setOrigin(Ogre::Vector3(0,0,0));

	// Configure global
	mTerrainGlobals->setMaxPixelError(8);
	// testing composite map
	mTerrainGlobals->setCompositeMapDistance(3000);

	//matProfile->setLightmapEnabled(false);
	// Important to set these so that the terrain knows what to use for derived (non-realtime) data
	//mTerrainGlobals->setLightMapDirection(mLight->getDerivedDirection());
	mTerrainGlobals->setCompositeMapAmbient(mSceneMgr->getAmbientLight());
	//mTerrainGlobals->setCompositeMapAmbient(ColourValue::Red);
	mTerrainGlobals->setCompositeMapDiffuse(ColourValue::White);

	// Configure default import settings for if we use imported image
	Terrain::ImportData& defaultimp = mTerrainGroup->getDefaultImportSettings();
	defaultimp.terrainSize = 65;
	defaultimp.worldSize = 200;
	defaultimp.inputScale = 1;
	defaultimp.minBatchSize = 33;
	defaultimp.maxBatchSize = 65;
	// textures
	defaultimp.layerList.resize(3);
	defaultimp.layerList[0].worldSize = 100;
	defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_diffusespecular.dds");
	defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_normalheight.dds");
	defaultimp.layerList[1].worldSize = 30;
	defaultimp.layerList[1].textureNames.push_back("grass_green-01_diffusespecular.dds");
	defaultimp.layerList[1].textureNames.push_back("grass_green-01_normalheight.dds");
	defaultimp.layerList[2].worldSize = 200;
	defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_diffusespecular.dds");
	defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_normalheight.dds");

	Image img;
	img.load("terrain.png", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	mTerrainGroup->defineTerrain(0, 0,&img);

	mTerrainGroup->loadAllTerrains(true);
	TerrainGroup::TerrainIterator ti = mTerrainGroup->getTerrainIterator();
	while(ti.hasMoreElements())
	{
		Terrain* terrain = ti.getNext()->instance;
		TerrainLayerBlendMap* blendMap0 = terrain->getLayerBlendMap(1);
		TerrainLayerBlendMap* blendMap1 = terrain->getLayerBlendMap(2);
		Ogre::Real minHeight0 = 70;
		Ogre::Real fadeDist0 = 40;
		Ogre::Real minHeight1 = 70;
		Ogre::Real fadeDist1 = 15;
		float * pBlend0 = blendMap0->getBlendPointer();
		float* pBlend1 = blendMap1->getBlendPointer();
		for (Ogre::uint16 y = 0; y < terrain->getLayerBlendMapSize(); ++y)
		{
			for (Ogre::uint16 x = 0; x < terrain->getLayerBlendMapSize(); ++x)
			{
				Ogre::Real tx, ty;

				blendMap0->convertImageToTerrainSpace(x, y, &tx, &ty);
				Ogre::Real height = terrain->getHeightAtTerrainPosition(tx, ty);
				Ogre::Real val = (height - minHeight0) / fadeDist0;
				val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
				*pBlend0++ = val;

				val = (height - minHeight1) / fadeDist1;
				val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
				*pBlend1++ = val;
			}
		}
		blendMap0->dirty();
		blendMap1->dirty();
		blendMap0->update();
		blendMap1->update();
	}
	mTerrainGroup->freeTemporaryResources();	

	NxOgre::SceneGeometry * sg;

	ti = mTerrainGroup->getTerrainIterator();
	while (ti.hasMoreElements())
	{
		Terrain * t = ti.getNext()->instance;
		sg = mRenderSystem->createTerrain(t);
	}
}

NxWheelShape * NxOgreSampleCarApp::attachCarWheel(NxWheelDesc & wheelDesc,bool left)
{
	NxWheelShapeDesc wheelShapeDesc;
	if (!mMat)
	{
		NxMaterialDesc m;
		m.flags |= NX_MF_DISABLE_FRICTION;
		mMat = mScene->getScene()->createMaterial(m);
	}

	wheelShapeDesc.materialIndex = mMat->getMaterialIndex();

	wheelShapeDesc.localPose.t = wheelDesc.position;
	NxQuat q;
	q.fromAngleAxis(90,NxVec3(0,1,0));
	//wheelShapeDesc.localPose.M.fromQuat(q);
	NxReal heightModifier = (wheelDesc.wheelSuspension + wheelDesc.wheelRadius) / wheelDesc.wheelSuspension;
	wheelShapeDesc.suspension.spring = wheelDesc.springRestitution * heightModifier;
	wheelShapeDesc.suspension.damper = 0;
	wheelShapeDesc.suspension.targetValue = wheelDesc.springBias * heightModifier;
	wheelShapeDesc.radius = wheelDesc.wheelRadius;
	wheelShapeDesc.suspensionTravel = wheelDesc.wheelSuspension;
	wheelShapeDesc.inverseWheelMass = 0.1f;
	wheelShapeDesc.lateralTireForceFunction.stiffnessFactor *= wheelDesc.frictionToSide;
	wheelShapeDesc.longitudalTireForceFunction.stiffnessFactor *= wheelDesc.frictionToFront;

	NxWheelShape * wheelShape = NULL;
	Ogre::SceneNode * node;

	if (left)
	{
		//wheelShapeDesc.name = "left";
		node = mRenderSystem->createSceneNodeEntityPair(Ogre::String("wheelLeft.mesh"),NxOgre::Vec3(0,0,0),NxOgre::Quat());
		//node->setOrientation(Ogre::Quaternion(Ogre::Radian(55),Ogre::Vector3::UNIT_Y));
		//node->yaw(Ogre::Radian(55));
	}
	else
	{
		//wheelShapeDesc.name = "right";		
		node = mRenderSystem->createSceneNodeEntityPair(Ogre::String("wheelRight.mesh"),NxOgre::Vec3(0,0,0),NxOgre::Quat());
		//node->setOrientation(Ogre::Quaternion(Ogre::Radian(-55),Ogre::Vector3::UNIT_Y));
		//node->yaw(Ogre::Radian(-55));
	}

	node->setScale(0.55,0.55,0.55);

	wheelShape = static_cast<NxWheelShape *>(mCar->addBodyShape(wheelShapeDesc,node));
	return wheelShape;
}

void NxOgreSampleCarApp::createCar()
{
	using namespace Critter;
	using namespace NxOgre;
	BodyDescription desc;
	desc.mMass = 300;
	desc.mDynamicRigidbodyFlags |= DynamicRigidbodyFlags::FreezeRotationZ | DynamicRigidbodyFlags::FreezeRotationX;// | DynamicRigidbodyFlags::FreezeRotationY;
	BoxDescription bd;
	bd.mSize = Vec3(5,4,10);

	CompartmentDescription cd;
	cd.mType = NxOgre::Enums::CompartmentType_RigidBody;
	cd.mDeviceCode = NxOgre::Enums::DeviceCode_PPU0;
	Compartment * compartment = mScene->createCompartment(cd);

	desc.mCompartment = compartment;

	mCar = mRenderSystem->createBody(bd,Vec3(0,6,0),"cube.1m.mesh",desc);
	mCar->getSceneNode()->setScale(5,4,10);

	NxTireFunctionDesc lngTFD;
	lngTFD.extremumSlip = 1.0f;
	lngTFD.extremumValue = 0.02f;
	lngTFD.asymptoteSlip = 2.0f;
	lngTFD.asymptoteValue = 0.01f;
	lngTFD.stiffnessFactor = 1000000.0f;

	NxTireFunctionDesc latTFD;
	latTFD.extremumSlip = 1.0f;
	latTFD.extremumValue = 0.02f;
	latTFD.asymptoteSlip = 2.0f;
	latTFD.asymptoteValue = 0.01f;	
	latTFD.stiffnessFactor = 1000000.0f;

	NxTireFunctionDesc slipTFD;
	slipTFD.extremumSlip = 1.0f;
	slipTFD.extremumValue = 0.02f;
	slipTFD.asymptoteSlip = 2.0f;
	slipTFD.asymptoteValue = 0.01f;	
	slipTFD.stiffnessFactor = 100.0f;  

	NxTireFunctionDesc medTFD;
	medTFD.extremumSlip = 1.0f;
	medTFD.extremumValue = 0.02f;
	medTFD.asymptoteSlip = 2.0f;
	medTFD.asymptoteValue = 0.01f;	
	medTFD.stiffnessFactor = 10000.0f;  

	NxWheelDesc wheelDesc;
	wheelDesc.wheelApproximation = 10;
	wheelDesc.wheelRadius = 1.5;
	wheelDesc.wheelWidth = 0.3;
	wheelDesc.wheelSuspension = 1.0;
	wheelDesc.springRestitution = 100;
	wheelDesc.springDamping = .5f;
	wheelDesc.springBias = 0;
	wheelDesc.maxBrakeForce = 1;
	wheelDesc.position = NxVec3(2.8,-1.2,5);
	wheelDesc.wheelFlags = NX_WF_USE_WHEELSHAPE | NX_WF_BUILD_LOWER_HALF | NX_WF_ACCELERATED | NX_WF_AFFECTED_BY_HANDBRAKE | NX_WF_STEERABLE_INPUT;
	wheelLeftFront = attachCarWheel(wheelDesc,true);

	// Front right wheel
	NxWheelDesc wheelDesc2;
	wheelDesc2.wheelApproximation = 10;
	wheelDesc2.wheelRadius = 1.5;
	wheelDesc2.wheelWidth = 0.3;  // 0.1
	wheelDesc2.wheelSuspension = 1.0;  
	wheelDesc2.springRestitution = 100;
	wheelDesc2.springDamping = 0.5;
	wheelDesc2.springBias = 0;  
	wheelDesc2.maxBrakeForce = 1;
	wheelDesc2.position = NxVec3(-2.8,-1.2,5);
	wheelDesc2.wheelFlags = NX_WF_USE_WHEELSHAPE | NX_WF_BUILD_LOWER_HALF | NX_WF_ACCELERATED | 
		NX_WF_AFFECTED_BY_HANDBRAKE | NX_WF_STEERABLE_INPUT; 
	wheelRightFront = attachCarWheel(wheelDesc2,false);

	// Rear left wheel
	NxWheelDesc wheelDesc3;
	wheelDesc3.wheelApproximation = 10;
	wheelDesc3.wheelRadius = 1.5;
	wheelDesc3.wheelWidth = 0.3;  // 0.1
	wheelDesc3.wheelSuspension = 1.0;  
	wheelDesc3.springRestitution = 100;
	wheelDesc3.springDamping = 0.5;
	wheelDesc3.springBias = 0;  
	wheelDesc3.maxBrakeForce = 1;
	wheelDesc3.position = NxVec3(2.8,-1.2,-5);
	wheelDesc3.wheelFlags = NX_WF_USE_WHEELSHAPE | NX_WF_BUILD_LOWER_HALF | NX_WF_ACCELERATED | 
		NX_WF_AFFECTED_BY_HANDBRAKE | NX_WF_STEERABLE_INPUT; 
	wheelLeftRear = attachCarWheel(wheelDesc3,true);

	// Rear right wheel
	NxWheelDesc wheelDesc4;
	wheelDesc4.wheelApproximation = 10;
	wheelDesc4.wheelRadius = 1.5;
	wheelDesc4.wheelWidth = 0.3;  // 0.1
	wheelDesc4.wheelSuspension = 1.0;  
	wheelDesc4.springRestitution = 100;
	wheelDesc4.springDamping = 0.5;
	wheelDesc4.springBias = 0;  
	wheelDesc4.maxBrakeForce = 1;
	wheelDesc4.position = NxVec3(-2.8,-1.2,-5);
	wheelDesc4.wheelFlags = NX_WF_USE_WHEELSHAPE | NX_WF_BUILD_LOWER_HALF | NX_WF_ACCELERATED | 
		NX_WF_AFFECTED_BY_HANDBRAKE | NX_WF_STEERABLE_INPUT;
	wheelRightRear = attachCarWheel(wheelDesc4,false);

	Vec3 massPos = mCar->getCMassLocalPosition();
	massPos.y -= 1;
	mCar->setCMassOffsetLocalPosition(massPos.x,massPos.y,massPos.z);

	wheelLeftRear->setBrakeTorque(20);
	wheelRightRear->setBrakeTorque(20);
	wheelLeftFront->setBrakeTorque(20);
	wheelRightFront->setBrakeTorque(20);

	wheelLeftRear->setLateralTireForceFunction(latTFD);
	wheelLeftRear->setLongitudalTireForceFunction(lngTFD);
	wheelRightRear->setLateralTireForceFunction(latTFD);
	wheelLeftRear->setLongitudalTireForceFunction(lngTFD);
	
}