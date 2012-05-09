#include "NxOgreSample_Cloth_Force.h"

void NxOgreSample_Cloth_ForceApp::setupPhysX()
{
	using namespace NxOgre;
	using namespace Critter;
	mWorld = NxOgre::World::createWorld();
	NxOgre::ResourceSystem::getSingleton()->openProtocol(new Critter::OgreResourceProtocol());
	//mWorld->getRemoteDebugger()->connect();

	NxOgre::SceneDescription scene_description;
	scene_description.mGravity = NxOgre::Constants::MEAN_EARTH_GRAVITY;
	scene_description.mUseHardware = false;

	mScene = mWorld->createScene(scene_description);

	// Set default material properties
	mScene->getMaterial(0)->setAll(0.1f, 0.9f, 0.5f);

	// Get a copy of it's pointer
	mMeshManager = NxOgre::MeshManager::getSingleton();

	// Plane creation
	mScene->createSceneGeometry(NxOgre::PlaneGeometryDescription());

	// Create the rendersystem.
	mRenderSystem = new Critter::RenderSystem(mScene, mSceneMgr);
	//mRenderSystem->setVisualisationMode(NxOgre::Enums::VisualDebugger_ShowAll);

	mCLoth = makeFlag(NxOgre::Vec3(0,10,0),"wales");
	mWind = 22.0f;

	// Create a ShapeDescriptions container (NxOgre::vector<ShapeDescription*>).
	ShapeDescriptions shapes;

	// Each shape in the Actor/Body may have it's own seperate material, but the default one is good enough for now.
	NxOgre::MaterialIdentifier material = mScene->getMaterial(0)->getIdentifier();

	// Our container should be hollow inside, to let things through it. 
	// - Convex shapes cannot have holes.
	// - Spheres, Capsules do have holes either.
	// - Triangle Meshes can only be used in StaticGeometries, and StaticGeometries do not move.
	// - Instead we make up the container using box shapes repesenting each side of the container, there are four
	//   sides, so four BoxDescriptions.

	BoxDescription side1(Vec3(3.784, 1.75, 0.2),  material, Vec3(0, 0, -0.975));
	BoxDescription side2(Vec3(3.784, 1.75, 0.2), material, Vec3(0, 0,  0.975));
	BoxDescription side3(Vec3(3.784, 0.2, 2),  material, Vec3(0, -0.85, 0));
	BoxDescription side4(Vec3(3.784, 0.2, 2), material, Vec3(0, 0.85, 0));

	// Then add them to the ShapeDescriptions.
	// - You can use pointers and create the BoxDescriptions via new. But you need to delete them afterwards for leaks.
	// - Careful using references! As with C++ they will be deleted when they are out of scope.
	shapes.push_back(&side1);
	shapes.push_back(&side1);
	shapes.push_back(&side2);
	shapes.push_back(&side3);

	// Create the container BodyDescription.
	BodyDescription description;
	description.mMass = 1000; // Make it pretty heavy, 1000kg.

	// Create the container using the "shapes" ShapeDescriptions as our Shape argument.
	Body* container = mRenderSystem->createBody(shapes, Vec3(3.5, 5, 0), "container.mesh", description);


	//create Kinematic Body 
	KinematicBody * orb = mRenderSystem->createKinematicBody(SphereDescription(0.3f),Vec3(0,2.5f,2),"nxogre.orb.mesh");
	using namespace Ogre;
	Light* light = mSceneMgr->createLight();
	light->setType(Ogre::Light::LT_POINT);
	Ogre::ColourValue colour(65.0f / 255.0f, 105.0f / 255.0f, 225.0f / 255.0f);
	light->setDiffuseColour(colour);
	orb->getSceneNode()->attachObject(light);
	mOrb = orb;
}

NxOgre::Cloth * NxOgreSample_Cloth_ForceApp::makeFlag(NxOgre::Vec3 pos, const std::string &flag_image)
{
	using namespace NxOgre;
	using namespace Ogre;
	Vec3 polePos = pos;
	polePos.y *= 0.5f;
	SceneGeometry* geom = mScene->createSceneGeometry(CapsuleDescription(0.15, 10), polePos);
	SceneNode* geom_node = mRenderSystem->createSceneNodeEntityPair("cube.mesh", polePos);
	geom_node->scale(0.0015, 0.10, 0.0015);

	mFlagMesh = NxOgre::MeshGenerator::makePlane(Vec2(4,2),0.1f,NxOgre::Enums::MeshType_Cloth,"file://flag.xcl");

	NxOgre::ClothDescription desc;
	desc.mMesh = mFlagMesh;
	desc.mThickness = 0.2;
	desc.mFriction = 0.5;
	desc.mFlags |= NxOgre::Enums::ClothFlags_BendingResistance;   //Need Test!
	desc.mFlags |= NxOgre::Enums::ClothFlags_TwoWayCollisions;
	desc.mFlags |= NxOgre::Enums::ClothFlags_Damping;// | NxOgre::Enums::ClothFlags_Hardware;  
	//desc.mFlags |= NxOgre::Enums::ClothFlags_COMDamping;
	desc.mGlobalPose.set(pos);
	desc.mGlobalPose.set(-NxOgre::Math::HalfPi,NxOgre::Enums::X);  //Need Test!
	Cloth * cloth = mRenderSystem->createCloth(desc,flag_image);

	cloth->attachToShape((*geom->getShapes().begin()),0);
	return cloth;
}

void NxOgreSample_Cloth_ForceApp::createScene()
{
	using namespace Ogre;

	ColourValue background = ColourValue(16.f/255.f, 16.f/255.f, 16.f/255.f);
	mCameraMan->getCamera()->getViewport()->setBackgroundColour(background);
	mSceneMgr->setFog(Ogre::FOG_EXP, background, 0.001, 800, 1000);

	// set shadow properties
	mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);
	mSceneMgr->setShadowColour(ColourValue(0.5, 0.5, 0.5));
	mSceneMgr->setShadowTextureSize(1024);
	mSceneMgr->setShadowTextureCount(1);

	// create a floor mesh resource
	Ogre::MeshManager::getSingleton().createPlane("floor", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Plane(Vector3::UNIT_Y, 0), 1000, 1000, 1,1 , true, 1, 1, 1, Vector3::UNIT_Z);

	// create a floor entity, give it a material, and place it at the origin
	Entity* floor = mSceneMgr->createEntity("Floor", "floor");
	floor->setMaterialName("ground-from-nxogre.org");
	floor->setCastShadows(false);
	mSceneMgr->getRootSceneNode()->attachObject(floor);

	// use a small amount of ambient lighting
	mSceneMgr->setAmbientLight(ColourValue(0.3, 0.3, 0.3));

	// add a bright light above the scene
	Light* light = mSceneMgr->createLight();
	light->setType(Light::LT_POINT);
	light->setPosition(-10, 40, 20);
	light->setSpecularColour(ColourValue::White);

	mCameraMan->getCamera()->setPosition(10,10,10);
	mCameraMan->getCamera()->lookAt(0,0,0);
	mCameraMan->getCamera()->setNearClipDistance(0.02f);
	mCameraMan->getCamera()->setFarClipDistance(1000.0f);
	mCameraMan->setTopSpeed(57.5);
	setupPhysX();

	NxOgre::FluidDescription desc;
	desc.mMaxParticles = 2400;
	desc.mKernelRadiusMultiplier = 2.0f;
	desc.mRestParticlesPerMetre = 1;
	desc.mMotionLimitMultiplier = 3.0f;
	desc.mPacketSizeMultiplier = 64;   //2的根数(不含2)
	desc.mCollisionDistanceMultiplier = 0.1f;
	desc.mStiffness = 50.0f;
	desc.mViscosity = 15.0f;
	desc.mRestDensity = 1000.0f;
	desc.mSimulationMethod = NxOgre::Enums::FluidSimulationMethod_SPH;
	desc.mFadeInTime = 20.0f;
	desc.mFlags |= NxOgre::Enums::FluidFlags_DisableGravity | NxOgre::Enums::FluidFlags_PriorityMode | NxOgre::Enums::FluidFlags_ProjectToPlane | NxOgre::Enums::FluidFlags_CollisionTwoWay | NxOgre::Enums::FluidFlags_ForceStrictCookingFormat;
	NxOgre::Fluid * fluid = mRenderSystem->createFluid(desc,"CowFluid",Critter::Enums::FluidType_OgreParticle);

	NxOgre::FluidEmitterDescription edesc;
	edesc.mPose.set(0, 3, 0);
	edesc.mShape = NxOgre::Enums::FluidEmitterShape_Ellipse;
	edesc.mParticleLifetime = 1.0;
	edesc.mRate = 50;
	edesc.mFluidSpeed = 0.01f;
	edesc.mType = NxOgre::Enums::FluidEmitterType_FlowRate;
	edesc.mReplusionCoefficient = 0.8f;
	edesc.mDimensionX = 0.1f;
	edesc.mDimensionY = 0.1f;

	fluid->createEmitter(edesc);
}

void NxOgreSample_Cloth_ForceApp::destroyScene()
{
	NxOgre::World::destroyWorld();
}

void NxOgreSample_Cloth_ForceApp::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
	NxOgre::Vec3 wind = NxOgre::Vec3::random(-mWind,mWind,-mWind,mWind,-mWind,mWind);
	wind.x += 2.5f * mWind;
	mCLoth->setWindAcceleration(wind);

	mWorld->advance(evt.timeSinceLastFrame);

	mOrb->moveGlobalPosition(NxOgre::Vec3::random(-2,2,-2,2,-2,2));
}