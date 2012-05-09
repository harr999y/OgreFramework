#ifndef _NXOGRESAMPLE_FLUIDUSERDATA_
#define _NXOGRESAMPLE_FLUIDUSERDATA_

#include "../CEGUIBase/CEGUIBaseApp.h"

using namespace NxOgre;

struct ParticleParams
{
	NxVec3 position;
	NxVec3 velocity;
	NxReal density;
	NxReal lifetime;
	NxU32 id;
	NxVec3 collisionNormal;
};

#define PARTICLESMAX 7000
#define DISTANCE 0.08
#define SIDENUM 12
#define LIFETIME 100

class NxOgreSample_FluidUserDataApp : public CEGUIBaseApp
{
public:
	NxOgreSample_FluidUserDataApp() : mParticleBufferNum(0), mCreatedIdsBufferNum(0), mDeletedIdsBufferNum(0), mParticlesNum(0)
	{}

	void createScene();
	void setupPhysX();
	void frameRenderingQueued(const Ogre::FrameEvent& evt);
	void createParticleSphere(NxParticleData &pd, unsigned maxParticles, bool append, const NxVec3 &pos, const NxVec3 vel, float lifetime, float distance, unsigned int sideNum);
	void createFluids();

protected:
	NxOgre::World * mWorld;
	Scene * mScene;
	Critter::RenderSystem * mRenderSystem;
	NxOgre::Fluid * mFluids;
	ParticleParams * mParticleParams;
	NxParticleData mParticleData;
	NxVec3 mParticleBuffer[10000];
	NxU32 mParticleBufferCap;
	NxU32 mParticleBufferNum;
	NxU32 * mCreatedIdsBuffer;
	NxU32 mCreatedIdsBufferNum;
	NxU32 * mDeletedIdsBuffer;
	NxU32 mDeletedIdsBufferNum;
	NxVec3 * mParticleColorBuffer;
	NxParticleIdData mCreatedIds;
	NxParticleIdData mDeletedIds;
	unsigned mParticlesNum;
};

#endif