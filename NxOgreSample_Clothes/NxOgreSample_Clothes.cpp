#include "NxOgreSample_Clothes.h"

void NxOgreSample_ClothesApp::setupPhysX()
{
	mWorld = NxOgre::World::createWorld();
	NxOgre::ResourceSystem::getSingleton()->openProtocol(new Critter::OgreResourceProtocol());
	NxOgre::SceneDescription sceneDes;
	sceneDes.mGravity = NxOgre::Constants::MEAN_EARTH_GRAVITY;
	sceneDes.mUseHardware = false;
	mScene = mWorld->createScene(sceneDes);
	mScene->getMaterial(0)->setAll(0.1f,0.9f,0.5f);
	mScene->createSceneGeometry(NxOgre::PlaneGeometryDescription());
	mRenderSystem = new Critter::RenderSystem(mScene,mSceneMgr);

	//mWorld->getPhysXSDK()->setParameter(NX_DYN_FRICT_SCALING,2);
	//mWorld->getPhysXSDK()->setParameter(NX_STA_FRICT_SCALING,2);
	mWorld->getPhysXSDK()->setParameter(NX_BOUNCE_THRESHOLD,-0.1);

	//mRenderSystem->setVisualisationMode(NxOgre::Enums::VisualDebugger_ShowAll);
	setupPipeScene();
}

void NxOgreSample_ClothesApp::setupPipeScene()
{
	using namespace NxOgre;
	using namespace Critter;
	SceneGeometry * body = mScene->createSceneGeometry(BoxDescription(2,2,2),NxOgre::Vec3(0,10,0));
	Ogre::SceneNode* node = mRenderSystem->createSceneNodeEntityPair("cube.1m.mesh",NxOgre::Vec3(0,10,0));
	node->setScale(2,2,2);
	NxOgre::ClothDescription clothDesc;
	clothDesc.mGlobalPose = Vec3(0,10,0);
	clothDesc.mThickness = 0.1;	
	//clothDesc.mDensity = 0.8;
	//clothDesc.mBendingStiffness = 0.5;
	//clothDesc.mStretchingStiffness = 0.5;
	//clothDesc.mDampingCoefficient = 0.7;
	clothDesc.mPressure = 1.5;
	clothDesc.mFriction = 0.9;
	//clothDesc.mFriction = 0.2;
	//clothDesc.mCollisionResponseCoefficient = 0.2;
	//clothDesc.mCollisionResponseCoefficient = 1.8;
	//clothDesc.mAttachmentTearFactor = 2;

	clothDesc.mFlags |= NxOgre::Enums::ClothFlags_Hardware |NxOgre::Enums::ClothFlags_Pressure|NxOgre::Enums::ClothFlags_TwoWayCollisions|NxOgre::Enums::ClothFlags_FluidCollision | NxOgre::Enums::ClothFlags_BendingResistance | NxOgre::Enums::ClothFlags_Damping;//| NxOgre::Enums::ClothFlags_OrthogonalBendingResistance |NxOgre::Enums::ClothFlags_Damping|NxOgre::Enums::ClothFlags_COMDamping|NxOgre::Enums::ClothFlags_DisableDynamicCCD|NxOgre::Enums::ClothFlags_Adhere|(1<<11);//| NxOgre::Enums::ClothFlags_Pressure;// | NxOgre::Enums::ClothFlags_Tearable;//NxOgre::Enums::ClothFlags_BendingResistance | NxOgre::Enums::ClothFlags_Damping | NxOgre::Enums::ClothFlags_TwoWayCollisions | NxOgre::Enums::ClothFlags_DebugVisualisation | NxOgre::Enums::ClothFlags_Hardware;// 

	/*NxClothMeshDesc desc;
	
	WavefrontObj obj;
	obj.loadObj("pipe.obj");

	if (obj.mVertexCount == 0)
	{
		return;
	}

	desc.numVertices = obj.mVertexCount;
	desc.numTriangles = obj.mTriCount;
	desc.pointStrideBytes = sizeof(NxVec3);
	desc.triangleStrideBytes = 3 * sizeof(NxU32);
	desc.vertexMassStrideBytes = sizeof(NxReal);
	desc.vertexFlagStrideBytes = sizeof(NxU32);
	desc.points = (NxVec3*)malloc(sizeof(NxVec3) * desc.numVertices);
	desc.triangles = (NxU32*)malloc(sizeof(NxU32) * desc.numTriangles * 3);
	desc.vertexMasses = 0;   //?????
	desc.vertexFlags = 0; 
	desc.flags = 0;

	NxReal scale = 1;

	NxVec3 * vSrc = (NxVec3 *)obj.mVertices;
	NxVec3 * vDest = (NxVec3 *)desc.points;
	for (int i=0;i<obj.mVertexCount;i++,vSrc++,vDest++)
	{
		*vDest = (*vSrc) * scale;
	}
	memcpy((NxU32*)desc.triangles,obj.mIndices,desc.numTriangles * 3 * sizeof(NxU32));
	//desc.flags |= NX_CLOTH_MESH_TEARABLE;

	NxInitCooking();
	MemoryWriteBuffer wb;
	if (!NxCookClothMesh(desc,wb))
	{
		return;
	}
	MemoryReadBuffer rb(wb.data);
	NxClothMesh * clothMesh = mScene->getScene()->getPhysicsSDK().createClothMesh(rb);
	NxCloseCooking();

	//free
	NxVec3* p = (NxVec3*)desc.points;
	NxU32* t = (NxU32*)desc.triangles;
	NxReal* m = (NxReal*)desc.vertexMasses;
	NxU32* f = (NxU32*)desc.vertexFlags;
	free(p);
	free(t);
	free(m);
	free(f);

	//alocateReceiveBuffers
	int numVertices = desc.numVertices;
	int numTriangles = desc.numTriangles;
	/*NxMeshData meshData;

	NxU32 maxVertices = 2 * numVertices;
	meshData.verticesPosBegin = (NxVec3*)malloc(sizeof(NxVec3) * maxVertices);
	meshData.verticesNormalBegin = (NxVec3*)malloc(sizeof(NxVec3) * maxVertices);
	meshData.verticesPosByteStride = sizeof(NxVec3);
	meshData.verticesNormalByteStride = sizeof(NxVec3);
	meshData.maxVertices = maxVertices;
	meshData.numVerticesPtr = (NxU32*)malloc(sizeof(NxU32));

	NxU32 maxIndices = 3 * numTriangles;
	meshData.indicesBegin = (NxU32*)malloc(sizeof(NxU32) * maxIndices);
	meshData.indicesByteStride = sizeof(NxU32);
	meshData.maxIndices = maxIndices;
	meshData.numIndicesPtr = (NxU32*)malloc(sizeof(NxU32));

	NxU32 maxParentIndices = maxVertices;
	meshData.parentIndicesBegin = (NxU32*)malloc(sizeof(NxU32) * maxIndices);
	meshData.parentIndicesByteStride = sizeof(NxU32);
	meshData.maxParentIndices = maxParentIndices;
	meshData.numParentIndicesPtr = (NxU32*)malloc(sizeof(NxU32));

	*meshData.numVerticesPtr = 0;
	*meshData.numIndicesPtr = 0;
	
	//Mesh * me = NxOgre::MeshGenerator::makePlane(Vec2(10,10),2,NxOgre::Enums::MeshType_Cloth,MEMORY_PATH,"file://4.xcl");
	//NxClothMesh * cloth = me->getAsCloth();
	//*cloth = *clothMesh;
	
	//MeshStats ms(numVertices,3 * numTriangles,2 * numVertices,numVertices,numTriangles / 2);

	//其MeshData由MeshRenderable自动填充
	//me->mMeshStats = ms;

	//Mesh * me = MeshGenerator::makeBox(Vec3(2,2,2),NxOgre::Enums::MeshType_Cloth);

	Mesh * me = MeshManager::getSingleton()->load("ogre://castle.xcl");
	clothDesc.mMesh = me;

	//Mesh * me = NxOgre::MeshGenerator::makePlane(Vec2(10,10),1,NxOgre::Enums::MeshType_Cloth,Path("file://we.xcl"));	
	
	//Mesh * me = NxOgre::MeshManager::getSingleton()->load("ogre://castle.xcl");

	//Here can't use OgreReasource.
	Mesh * me = MeshGenerator::makeClothFromFlower(Path("file://../media/PhysX/3.xcl"),Path("file://../media/PhysX/1.flower"));

	/*Critter::OgreResource * res = static_cast<Critter::OgreResource * >(ResourceSystem::getSingleton()->open(Path("ogre://Popular/1.flower"), NxOgre::Enums::ResourceAccess_ReadOnly));
	NxOgre::MeshData * mesh = new MeshData;
	//ManualMesh mesh;
	//	md->clear();
	//NxOgre::Serialisation::Flower::load(md,res);

	/************************************************************************/
	/*					Comes from NxOgreFlower.cpp						    */
	/************************************************************************/
	/*buffer<char> lineBuffer;

	while(res->atEnd() == false)
	{
	NxOgre::Strings::getLine(res, lineBuffer);

	if (lineBuffer.size() == 1) // (0 characters + 1 byte null)
	continue;  // Skip empty lines 

	if (lineBuffer[0] == '#')
	continue;  // Skip full comment lines.

	std::string line(lineBuffer.first());

	Strings::slice_to_first_of(line, '#');
	Strings::trim(line);

	buffer<float> buf;
	// vertices
	if (Strings::starts_insensitive(line, "Vertices"))
	{
	Strings::slice(line, 8);
	//Strings::split<float>(line, mesh, ",");
	Strings::split<float>(line,buf,",");
	mesh->mVertices.push_back(buf[0]);
	mesh->mVertices.push_back(buf[1]);
	mesh->mVertices.push_back(buf[2]);
	continue;
	}

	// indices
	if (Strings::starts_insensitive(line, "indices"))
	{
	Strings::slice(line,7);
	//Strings::split<unsigned int>(line, mesh->mIndexes, ",");
	Strings::split<float>(line,buf,",");
	mesh->mIndexes.push_back(buf[0]);
	mesh->mIndexes.push_back(buf[1]);
	mesh->mIndexes.push_back(buf[2]);
	continue;
	}

	// normals
	if (Strings::starts_insensitive(line, "normals"))
	{
	Strings::slice(line, 7);
	//Strings::split<float>(line, mesh->mNormals, ",");
	Strings::split<float>(line,buf,",");
	mesh->mNormals.push_back(buf[0]);
	mesh->mNormals.push_back(buf[0]);
	mesh->mNormals.push_back(buf[0]);
	continue;
	}

	// texture coordinates
	if (Strings::starts_insensitive(line, "texturecoords"))
	{
	Strings::slice(line, 14);
	//Strings::split<float>(line, mesh->mTextureCoordinates, ",");
	Strings::split<float>(line,buf,",");
	mesh->mTextureCoordinates.push_back(buf[0]);
	mesh->mTextureCoordinates.push_back(buf[1]);
	continue;
	}
	}
	mesh->mType = NxOgre::Enums::MeshType_Cloth;
	Critter::OgreResource * resource = static_cast<Critter::OgreResource * >(ResourceSystem::getSingleton()->open("file://../media/PhysX/2.xcl",NxOgre::Enums::ResourceAccess_ReadAndWrite));
	mesh->cook(resource);
	Mesh * me = MeshManager::getSingleton()->load(resource);*/

	//Here can't use OgreReasource.
	Mesh * me = MeshGenerator::makeClothFromFlower(Path("file://../media/PhysX/cow.xcl"),Path("file://../media/PhysX/cow.flower"));

	//clothDesc.mMesh = MeshManager::getSingleton()->load("file://../media/PhysX/cow.xcl");
	//clothDesc.mGlobalPose.set(0.95,NxOgre::Enums::Z);  

	clothDesc.mMesh = me;
	mCloth = mRenderSystem->createCloth(clothDesc,"Cow");

	//mCloth->attachToShape(*body->getShapes().begin(),NxOgre::Enums::ClothAttachmentFlags_Twoway);

	//SceneGeometry * body2 = mScene->createSceneGeometry(BoxDescription(2,0.2,2),NxOgre::Vec3(8,5,0));
	//mCloth->getPhysXCloth()->attachToCollidingShapes(0);
	//Ogre::SceneNode* node = mRenderSystem->createSceneNodeEntityPair("cube.1m.mesh",NxOgre::Vec3(0,10,0));

	dragIndex = -1;

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
	desc.mFlags |= NxOgre::Enums::FluidFlags_PriorityMode | NxOgre::Enums::FluidFlags_CollisionTwoWay | NxOgre::Enums::FluidFlags_ForceStrictCookingFormat;
	NxOgre::Fluid * fluid = mRenderSystem->createFluid(desc,"CowFluid",Critter::Enums::FluidType_OgreParticle);

	NxOgre::FluidEmitterDescription edesc;
	edesc.mPose.set(0, 13, 0);
	edesc.mShape = NxOgre::Enums::FluidEmitterShape_Ellipse;
	edesc.mParticleLifetime = 2.0;
	edesc.mRate = 50;
	edesc.mFluidSpeed = 0;
	edesc.mType = NxOgre::Enums::FluidEmitterType_FlowRate;
	edesc.mReplusionCoefficient = 0.8f;
	edesc.mDimensionX = 10;
	edesc.mDimensionY = 10;
	edesc.mMaxParticles = 2000;

	fluid->createEmitter(edesc);
}

void NxOgreSample_ClothesApp::createScene()
{
	mSceneMgr->setAmbientLight(Ogre::ColourValue::White);
	mCameraMan->setTopSpeed(50);
	setupPhysX();
	setupScene();
	mCameraMan->getCamera()->setPosition(0,30,-42);
	mCameraMan->getCamera()->setDirection(0,-0.6,1);	
}

int ti = 0;

void NxOgreSample_ClothesApp::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	using namespace NxOgre;
	ti += evt.timeSinceLastFrame * 1000;
	mWorld->advance(evt.timeSinceLastFrame);
	if (ti % 2 == 0 && GetAsyncKeyState(VK_SPACE) && 0xff)
	{
		ti = 0;
		Critter::BodyDescription bodyDescription;
		bodyDescription.mMass = 1;
		//bodyDescription.mDensity = 0.8;
		bodyDescription.mLinearVelocity = NxOgre::Vec3(mCameraMan->getCamera()->getDirection().x * 10,mCameraMan->getCamera()->getDirection().y * 10,mCameraMan->getCamera()->getDirection().z * 10);//Vec3(0,-30,0);
		//bodyDescription.mRigidbodyFlags |= NxOgre::RigidbodyFlags::ForceConeFriction | NxOgre::RigidbodyFlags::UserActorPairFiltering; 
		NxOgre::SphereDescription sphere;
		sphere.mRadius = 0.3;
		sphere.mFlags |= Enums::ShapeFlags_TwoWayCloth | Enums::ShapeFlags::ShapeFlags_TwoWayFluid | Enums::ShapeFlags::ShapeFlags_TwoWaySoftBody;
		
		//sphere.mFlags |= NxOgre::ShapeFlags::NoCloth;
		//Critter::Body* body = mRenderSystem->createBody(box, Vec3::random(-228,228,500,1228,-228,228), "cube.1m.mesh", bodyDescription);
		Critter::Body* body = mRenderSystem->createBody(sphere, NxOgre::Vec3(mCameraMan->getCamera()->getPosition().x,mCameraMan->getCamera()->getPosition().y,mCameraMan->getCamera()->getPosition().z), "sphere.mesh", bodyDescription);
		//mScene->createActor(sphere,NxOgre::Vec3(mCameraMan->getCamera()->getPosition().x,mCameraMan->getCamera()->getPosition().y,mCameraMan->getCamera()->getPosition().z),bodyDescription);
		body->getSceneNode()->setScale(0.003,0.003,0.003);

		/*NxBodyDesc bd;
		bd.mass = 0.001;
		bd.massSpaceInertia = NxVec3(10,10,10);
		//bd.linearVelocity = NxVec3(mCameraMan->getCamera()->getDirection().x * 32,0,mCameraMan->getCamera()->getDirection().z * 32);
		NxActorDesc ad;
		ad.body = &bd;
		NxSphereShapeDesc s;
		s.radius = 0.35;
		ad.shapes.pushBack(&s);
		ad.globalPose.t = NxVec3(mCameraMan->getCamera()->getPosition().x,mCameraMan->getCamera()->getPosition().y,mCameraMan->getCamera()->getPosition().z);
		mScene->getScene()->createActor(ad);*/

		/*NxActorDesc t;
		NxSphereShapeDesc ss;
		//s.radius = 0.3;
		NxBodyDesc b;
		b.mass = 30;  //1.不能setToDefault() 2.不能没有Mass。
		//b.massSpaceInertia = NxVec3(10,10,10);
		b.linearVelocity = NxVec3(mCameraMan->getCamera()->getDirection().x,mCameraMan->getCamera()->getDirection().y,mCameraMan->getCamera()->getDirection().z);
		t.body = &b;
		t.shapes.pushBack(&ss);
		t.globalPose.t = NxVec3(mCameraMan->getCamera()->getPosition().x,mCameraMan->getCamera()->getPosition().y,mCameraMan->getCamera()->getPosition().z);
		mScene->getScene()->createActor(t);  */
	}

	if (dragIndex != -1 || mMouseActor != 0)
	{
		Ogre::Vector2 point(CEGUI::MouseCursor::getSingleton().getPosition().d_x/mWindow->getWidth(),CEGUI::MouseCursor::getSingleton().getPosition().d_y/mWindow->getHeight());
		Ogre::Ray ogreRay = mCameraMan->getCamera()->getCameraToViewportRay(point.x,point.y);
		NxOgre::Ray nxRay;
		nxRay.mDirection.from(ogreRay.getDirection());
		nxRay.mOrigin.from(ogreRay.getOrigin());
		NxOgre::RaycastHit result = mScene->raycastClosestShape(nxRay,NxOgre::Enums::ShapesType_All);

		if (result.mDistance > 0 && result.mDistance <= 200)
		{
			if (dragIndex != -1)
			{
				mCloth->attachVertexToGlobalPosition(dragIndex,result.mWorldImpact);
			}	
		}	
		if(mMouseActor != 0)
		{
			mMouseActor->setGlobalPose(nxRay.mOrigin + 45 * nxRay.mDirection);
		}	

	}

}

void NxOgreSample_ClothesApp::createFrameListener()
{
	NxOgreSample_ClothesListener * listener = new NxOgreSample_ClothesListener(this,mRoot,mSceneMgr,mWindow,mCameraMan,mOgreRenderer);
	mListener = listener;
	mListener->initialise();
}

bool NxOgreSample_ClothesApp::mousePressed(const OIS::MouseEvent &arg,OIS::MouseButtonID id)
{
	if (arg.state.buttonDown(OIS::MB_Left))
	{
		if (dragIndex != -1)
		{
			mCloth->freeVertex(dragIndex);
			dragIndex = -1;
		}

		Ogre::Vector2 point(arg.state.X.abs,arg.state.Y.abs);
		Ogre::Ray ogreRay = mCameraMan->getCamera()->getCameraToViewportRay(point.x/arg.state.width,point.y/arg.state.height);
		NxOgre::Ray nxRay;
		nxRay.mDirection.from(ogreRay.getDirection());
		nxRay.mOrigin.from(ogreRay.getOrigin());
		NxOgre::ClothRaycastHit result = mCloth->raycast(nxRay);

		if (result.mDidHit)
		{
			dragIndex = result.mVertexId;
		}
		else
			dragIndex = -1;

		if (mMouseActor == 0)
		{
			result = mCloth2->raycast(nxRay);
			if (result.mDidHit)
			{
				mMouseActor = mRenderSystem->createBody(NxOgre::SphereDescription(0.1),result.mWorldPosition,"sphere.mesh");
				mMouseActor->getSceneNode()->setScale(0.001,0.001,0.001);

				NxDistanceJointDesc desc;
				desc.actor[0] = mMouseActor->getNxActor();
				desc.actor[1] = mHitActor2;

				mMouseActor->getNxActor()->getGlobalPose().multiplyByInverseRT(NxVec3(result.mWorldPosition.x,result.mWorldPosition.y,result.mWorldPosition.z), desc.localAnchor[0]);
				mHitActor2->getGlobalPose().multiplyByInverseRT(NxVec3(result.mWorldPosition.x,result.mWorldPosition.y,result.mWorldPosition.z), desc.localAnchor[1]);
				//mMouseActor->get
				desc.spring.damper = 1;
				desc.spring.spring = 200;
				desc.flags |= NX_DJF_MAX_DISTANCE_ENABLED | NX_DJF_SPRING_ENABLED;
				mJoint = static_cast<NxDistanceJoint*>(mScene->getScene()->createJoint(desc));
			}
		}	
	}
	return true;
}

bool NxOgreSample_ClothesListener::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	mApp->mousePressed(arg,id);
	return CEGUIBaseListener::mousePressed(arg,id);
}

bool NxOgreSample_ClothesListener::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	mApp->mouseReleased(arg,id);
	return CEGUIBaseListener::mouseReleased(arg,id);
}

bool  NxOgreSample_ClothesApp::mouseReleased(const OIS::MouseEvent &arg,OIS::MouseButtonID id)
{		
	if (id == OIS::MB_Left)
	{
		if (dragIndex != -1)
		{
			mCloth->freeVertex(dragIndex);
			dragIndex = -1;
		}
		if (mMouseActor != 0)
		{
			mRenderSystem->destroyBody(mMouseActor);
			mMouseActor = 0; 
			mScene->getScene()->releaseJoint(*mJoint);
			mJoint = 0;
		}
	}
	return true;
}

void NxOgreSample_ClothesApp::setupScene()
{
	using namespace NxOgre;
	mScene->createSceneGeometry(BoxDescription(.3,5,20),NxOgre::Vec3(10,2.5,0));
	Ogre::SceneNode* node = mRenderSystem->createSceneNodeEntityPair("cube.1m.mesh",NxOgre::Vec3(10,2.5,0));
	node->setScale(.3,5,20);

	mScene->createSceneGeometry(BoxDescription(20,5,.3),NxOgre::Vec3(0,2.5,10));
	node = mRenderSystem->createSceneNodeEntityPair("cube.1m.mesh",NxOgre::Vec3(0,2.5,10));
	node->setScale(20,5,.3);

	mScene->createSceneGeometry(BoxDescription(.3,5,20),NxOgre::Vec3(-10,2.5,0));
	node = mRenderSystem->createSceneNodeEntityPair("cube.1m.mesh",NxOgre::Vec3(-10,2.5,0));
	node->setScale(.3,5,20);

	Critter::BodyDescription bd;
	bd.mMass = 10;
	//bd.mDensity = 0.1f;
	bd.mLinearDamping = 0.3f;
	bd.mAngularDamping = 0.2f;

	//mHitActor = mRenderSystem->createBody(NxOgre::BoxDescription(1,1,1),NxOgre::Vec3(0,5,0),"barrel.mesh",bd);
	mHitActor = mScene->createActor(NxOgre::BoxDescription(),Vec3(0,5,0),bd);

	NxBodyDesc  coreBodyDesc;
	coreBodyDesc.linearDamping = 0.2f;
	coreBodyDesc.angularDamping = 0.2f;

	NxActorDesc coreActorDesc;
	coreActorDesc.density = 0.1f;
	coreActorDesc.body = &coreBodyDesc;

	const NxU32 numSpheres = 10;
	for (NxU32 i = 0; i < numSpheres; i++) 
		coreActorDesc.shapes.pushBack(new NxSphereShapeDesc());

	coreActorDesc.body = &coreBodyDesc;
	coreActorDesc.density = 0.1f;
	mHitActor2 = mScene->getScene()->createActor(coreActorDesc);

	NxOgre::ClothDescription clothDesc;
	clothDesc.mGlobalPose = NxOgre::Vec3(0,5,0);
	clothDesc.mDensity = .1;
	clothDesc.mMesh = MeshGenerator::makeClothFromFlower("file://../Media/PhysX/barrel.xcl","file://../Media/PhysX/barrel.flower");
	mCloth2 = mRenderSystem->createCloth(clothDesc,"Cow");
	mCloth2->getPhysXCloth()->attachToCore(mHitActor2,50,0.5,0.5);
	mMouseActor = NULL;
}