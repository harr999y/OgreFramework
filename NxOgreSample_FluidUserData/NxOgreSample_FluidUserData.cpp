#include "NxOgreSample_FluidUserData.h"

void NxOgreSample_FluidUserDataApp::createScene()
{
	setupPhysX();
	createFluids();
	mCameraMan->setTopSpeed(50);
	mCameraMan->getCamera()->setPosition(0,5,30);
}

void NxOgreSample_FluidUserDataApp::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	mWorld->advance(evt.timeSinceLastFrame);
}

void NxOgreSample_FluidUserDataApp::setupPhysX()
{
	mWorld = NxOgre::World::createWorld();
	NxOgre::ResourceSystem::getSingleton()->openProtocol(new Critter::OgreResourceProtocol());
	NxOgre::SceneDescription sceneDes;
	sceneDes.mGravity = NxOgre::Constants::MEAN_EARTH_GRAVITY;
	sceneDes.mUseHardware = false;
	sceneDes.mWorkerThreadsCount = 2;
	sceneDes.mBackgroundThreadsCount = 2;
	sceneDes.mFlags |= NxOgre::SceneFlags::EnableMultithread | NxOgre::SceneFlags::FluidPerformanceHint | NxOgre::SceneFlags::AlternativeFluidTriangleCollision | NxOgre::SceneFlags::MultithreadedForcefield;
	mScene = mWorld->createScene(sceneDes);
	mScene->getMaterial(0)->setAll(0.1f,0.9f,0.5f);
	mScene->createSceneGeometry(NxOgre::PlaneGeometryDescription());
	mRenderSystem = new Critter::RenderSystem(mScene,mSceneMgr);
	//mRenderSystem->setVisualisationMode(NxOgre::Enums::VisualDebugger_Fluid_Bounds);
}

void NxOgreSample_FluidUserDataApp::createParticleSphere(NxParticleData &pd, unsigned maxParticles, bool append, const NxVec3 &pos, const NxVec3 vel, float lifetime, float distance, unsigned int sideNum)
{
	float rad = sideNum * distance * 0.5f;
	char * bufferPos = reinterpret_cast<char *>(pd.bufferPos);
	char * bufferVel = reinterpret_cast<char *>(pd.bufferVel);
	char * bufferLife = reinterpret_cast<char *>(pd.bufferLife);


	if (bufferPos == NULL && bufferVel == NULL && bufferLife == NULL)
		return;

	if (!append)
	{
		(*pd.numParticlesPtr) = 0;
	}
	else
	{
		bufferPos += pd.bufferPosByteStride * (*pd.numParticlesPtr);
		bufferVel += pd.bufferVelByteStride * (*pd.numParticlesPtr);
		bufferLife += pd.bufferLifeByteStride * (*pd.numParticlesPtr);
	}

	for (unsigned i=0;i<sideNum;i++)
	{
		for (unsigned j=0;j<sideNum;j++)
		{
			for (unsigned k=0;k<sideNum;k++)
			{
				if (*pd.numParticlesPtr >= maxParticles)
				{
					break;
				}

				NxVec3 p = NxVec3(i * distance,j * distance,k * distance);
				if (p.distance(NxVec3(rad,rad,rad)) < rad)
				{
					p += pos;
					if (bufferPos)
					{
						NxVec3 & position = *reinterpret_cast<NxVec3 *>(bufferPos);
						position = p;
						//CEGUI::Logger::getSingleton().logEvent(CEGUI::PropertyHelper::vector3ToString(CEGUI::Vector3(p.x,p.y,p.z)));
						bufferPos += pd.bufferPosByteStride;
					}

					if(bufferVel)
					{
						NxVec3& velocity = *reinterpret_cast<NxVec3*>(bufferVel);
						velocity = vel;
						//CEGUI::Logger::getSingleton().logEvent(CEGUI::PropertyHelper::vector3ToString(CEGUI::Vector3(vel.x,vel.y,vel.z)));
						bufferVel += pd.bufferVelByteStride;
					}

					if(bufferLife)
					{
						NxReal& life = *reinterpret_cast<NxReal*>(bufferLife);
						life = lifetime;
						bufferLife += pd.bufferLifeByteStride;
					}

					(*pd.numParticlesPtr)++;
				}
			}
		}
	}
}

void NxOgreSample_FluidUserDataApp::createFluids()
{
	mParticleParams = new ParticleParams[PARTICLESMAX];
	mCreatedIdsBuffer = new NxU32[PARTICLESMAX];
	mDeletedIdsBuffer = new NxU32[PARTICLESMAX];
	mParticleColorBuffer = new NxVec3[PARTICLESMAX];

	mParticleData.numParticlesPtr = &mParticlesNum;

	mParticleData.bufferPos = & mParticleParams[0].position.x;
	mParticleData.bufferPosByteStride = sizeof(ParticleParams);
	mParticleData.bufferVel = & mParticleParams[0].velocity.x;
	mParticleData.bufferVelByteStride = sizeof(ParticleParams);
	mParticleData.bufferLife = & mParticleParams[0].lifetime;
	mParticleData.bufferLifeByteStride = sizeof(ParticleParams);
	mParticleData.bufferId = & mParticleParams[0].id;
	mParticleData.bufferIdByteStride = sizeof(ParticleParams);
	//mParticleData.bufferDensityByteStride = sizeof(float);
	//mParticleData.bufferIdByteStride = sizeof(unsigned int);
	//mParticleData.bufferFlagByteStride = sizeof(unsigned int);
	//mParticleData.bufferCollisionNormalByteStride = sizeof(float) * 3;

	mCreatedIds.numIdsPtr = & mCreatedIdsBufferNum;
	mCreatedIds.bufferId = & mCreatedIdsBuffer[0];
	mCreatedIds.bufferIdByteStride = sizeof(NxU32);

	mDeletedIds.numIdsPtr = & mDeletedIdsBufferNum;
	mDeletedIds.bufferId = & mDeletedIdsBuffer[0];
	mDeletedIds.bufferIdByteStride = sizeof(NxU32);
	
	NxOgre::FluidDescription fluidDesc;
	fluidDesc.mMaxParticles = 32767;
	fluidDesc.mKernelRadiusMultiplier = 2.0f;
	fluidDesc.mRestParticlesPerMetre = 10.0f;
	fluidDesc.mMotionLimitMultiplier = 3.0f;
	fluidDesc.mPacketSizeMultiplier = 8;
	fluidDesc.mCollisionDistanceMultiplier = 0.1f;
	fluidDesc.mStiffness = 50.0f;
	fluidDesc.mViscosity = 40.0f;
	fluidDesc.mRestDensity = 1000.0f;
	fluidDesc.mDamping = 0.0f;
	fluidDesc.mRestitutionForStaticShapes = 0.162f;
	fluidDesc.mDynamicFrictionForStaticShapes = 0.146f;
	fluidDesc.mRestitutionForDynamicShapes = 0.5f;
	fluidDesc.mDynamicFrictionForDynamicShapes = 0.5f;
	fluidDesc.mSimulationMethod = NxOgre::Enums::FluidSimulationMethod_SPH;
	fluidDesc.mFlags |= NxOgre::Enums::FluidFlags_PriorityMode;// | NxOgre::Enums::FluidFlags_Hardware;
	fluidDesc.mNbReserveParticles = PARTICLESMAX;

	/*NxFluidDesc fluidDesc;
	fluidDesc.maxParticles                  = 32767;
	fluidDesc.kernelRadiusMultiplier		= 2.0f;
	fluidDesc.restParticlesPerMeter			= 10.0f;
	fluidDesc.motionLimitMultiplier			= 3.0f;
	fluidDesc.packetSizeMultiplier			= 8;
	fluidDesc.collisionDistanceMultiplier   = 0.1;
	fluidDesc.stiffness						= 50.0f;
	fluidDesc.viscosity						= 40.0f;
	fluidDesc.restDensity					= 1000.0f;
	fluidDesc.damping						= 0.0f;
	fluidDesc.restitutionForStaticShapes = 0.162f;
	fluidDesc.dynamicFrictionForStaticShapes  = 0.146f;
	fluidDesc.restitutionForDynamicShapes = 0.5f;
	fluidDesc.dynamicFrictionForDynamicShapes = 0.5f;
	fluidDesc.simulationMethod				= NX_F_SPH;
	fluidDesc.flags |= NX_FF_PRIORITY_MODE;
	fluidDesc.numReserveParticles = PARTICLESMAX;*/

	NxVec3 fluidPos(0,12,0);
	NxVec3 fluidVel(0,0,0);

	createParticleSphere(mParticleData,PARTICLESMAX,false,fluidPos,fluidVel,LIFETIME,DISTANCE,SIDENUM);

	//*fluidDesc.mParticleData = mParticleData;
	
	fluidDesc.mInitialParticleData = mParticleData;
	fluidDesc.mParticlesWriteData  = mParticleData;

	fluidDesc.mParticleCreationIdWriteData	= mCreatedIds;
	fluidDesc.mParticleDeletionIdWriteData	= mDeletedIds;

	fluidDesc.mHasUserData = true;

	/*fluidDesc.initialParticleData = mParticleData;
	fluidDesc.particlesWriteData  = mParticleData;

	fluidDesc.particleCreationIdWriteData = mCreatedIds;
	fluidDesc.particleDeletionIdWriteData = mDeletedIds;

	NxFluid * fluid = mScene->getScene()->createFluid(fluidDesc);*/

	mFluids = mRenderSystem->createFluid(fluidDesc,"CowFluid",Critter::Enums::FluidType_OgreParticle);

	//NxFluid * fluid = mFluids->getFluid();

	/*NxFluidEmitterDesc feDesc;
	feDesc.relPose.t = NxVec3(0, 0, 0);
	NxMat33 m1, m2;
	m1.rotY(0.5f*NxPi);
	feDesc.relPose.M = m1;
	feDesc.frameShape = NULL;
	feDesc.type = NX_FE_CONSTANT_PRESSURE;
	feDesc.rate = 100;
	feDesc.maxParticles = 300;
	feDesc.shape = NX_FE_RECTANGULAR;
	feDesc.dimensionX = 0.25;
	feDesc.dimensionY = 0.25;
	feDesc.randomPos = NxVec3(0.02,0.02,0.01);
	feDesc.randomAngle = 0;
	feDesc.particleLifetime = 10;
	feDesc.fluidVelocityMagnitude = 3;
	fluid->createEmitter(feDesc);*/

	/*NxOgre::FluidEmitterDescription edesc;
	edesc.mPose.set(0, 3, 0);
	edesc.mShape = NxOgre::Enums::FluidEmitterShape_Ellipse;
	edesc.mParticleLifetime = 1.0;
	edesc.mRate = 50;
	edesc.mFluidSpeed = 0.01f;
	edesc.mType = NxOgre::Enums::FluidEmitterType_FlowRate;
	edesc.mReplusionCoefficient = 0.8f;
	edesc.mDimensionX = 0.1f;
	edesc.mDimensionY = 0.1f;

	mFluids->createEmitter(edesc);	

	for (unsigned i=0;i<mCreatedIdsBufferNum;i++)
	{
		unsigned int id =mCreatedIdsBuffer[i];
		mParticleColorBuffer[id] = NxVec3(0.8f,0.1f,0.1f);
	}*/

	fluidPos.set(-6,16,0);
	fluidVel.set(12,0,0);
	createParticleSphere(mParticleData,PARTICLESMAX,false,fluidPos,fluidVel,LIFETIME,DISTANCE,SIDENUM);
	mFluids->getFluid()->addParticles(mParticleData);

	fluidPos.set(6,16,0);
	fluidVel.set(-12,0,0);
	createParticleSphere(mParticleData,PARTICLESMAX,false,fluidPos,fluidVel,LIFETIME,DISTANCE,SIDENUM);
	mFluids->getFluid()->addParticles(mParticleData);
}
