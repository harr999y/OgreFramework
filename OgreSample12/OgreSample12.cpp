#include "OgreSample12.h"

void OgreSample12App::createScene()
{
	mSceneMgr->setSkyBox(true, "Examples/SpaceSkyBox");

	// create a mesh for our ground
	MeshManager::getSingleton().createPlane("ground", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Plane(Vector3::UNIT_Y, 0), 1000, 1000, 20, 20, true, 1, 6, 6, Vector3::UNIT_Z);

	// create a ground entity from our mesh and attach it to the origin
	Entity* ground = mSceneMgr->createEntity("Ground", "ground");
	ground->setMaterialName("Examples/GrassFloor");
	ground->setCastShadows(false);
	mSceneMgr->getRootSceneNode()->attachObject(ground);

	//create GrassMesh
	MeshPtr mesh = MeshManager::getSingleton().createManual("grass",ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	SubMesh * sm = mesh->createSubMesh();
	sm->setMaterialName("Examples/GrassBlades");
	sm->useSharedVertices = false;
	sm->vertexData = OGRE_NEW VertexData();
	sm->vertexData->vertexStart = 0;
	sm->vertexData->vertexCount = 12;
	sm->indexData->indexCount = 18;
	
	VertexDeclaration * decl = sm->vertexData->vertexDeclaration;
	decl->addElement(0,0,VET_FLOAT3,VES_POSITION);
	decl->addElement(0,sizeof(float)*3,VET_FLOAT3,VES_NORMAL);
	decl->addElement(0,sizeof(float)*6,VET_FLOAT2,VES_TEXTURE_COORDINATES,0);
	HardwareVertexBufferSharedPtr vb = HardwareBufferManager::getSingleton().createVertexBuffer(decl->getVertexSize(0),sm->vertexData->vertexCount,HardwareBuffer::HBU_STATIC_WRITE_ONLY);
	GrassVertex * verts = (GrassVertex *)vb->lock(HardwareBuffer::HBL_DISCARD);

	for (unsigned int i=0;i<3;i++)
	{
		Real x = Math::Cos(Degree(i * 60)) * GRASS_WIDTH / 2;
		Real z = Math::Sin(Degree(i * 60)) * GRASS_WIDTH / 2;

		for (unsigned int j =0;j<4;j++)
		{
			GrassVertex & vert = verts[i * 4 + j];
			vert.x = j < 2 ? -x : x;
			vert.y = j % 2 ? 0 : GRASS_HEIGHT;
			vert.z = j < 2 ? -z : z;

			vert.nx = 0;
			vert.ny = 1;
			vert.nz = 0;

			vert.u = j < 2 ? 0 : 1;
			vert.v = j % 2;
		}
	}

	vb->unlock();

	sm->vertexData->vertexBufferBinding->setBinding(0,vb);
	
	sm->indexData->indexBuffer = HardwareBufferManager::getSingleton().createIndexBuffer(HardwareIndexBuffer::IT_16BIT,sm->indexData->indexCount,HardwareBuffer::HBU_STATIC_WRITE_ONLY);

	Ogre::uint16 * indices = (Ogre::uint16 *)sm->indexData->indexBuffer->lock(HardwareBuffer::HBL_DISCARD);

	for (unsigned int i=0;i<3;i++)
	{
		unsigned int off = i * 4;
		*indices++ = 0 + off;
		*indices++ = 3 + off;
		*indices++ = 1 + off;

		*indices++ = 0 + off;
		*indices++ = 2 + off;
		*indices++ = 3 + off;
	}
	sm->indexData->indexBuffer->unlock(); 

	mSceneMgr->setAmbientLight(ColourValue::White);

	Entity * grass = mSceneMgr->createEntity("Grass","grass");
	mField = mSceneMgr->createStaticGeometry("Field");
	mField->setRegionDimensions(Vector3(140,140,140));
	mField->setOrigin(Vector3(70,70,70));

	for (int x=-280;x<280;x+=20)
	{
		for (int z=-280;z<280;z+=20)
		{
			Vector3 pos(x + Math::RangeRandom(-7,7),0,z + Math::RangeRandom(-7 , 7));
			Quaternion ori(Degree(Math::RangeRandom(0,359)),Vector3::UNIT_Y);
			Vector3 scale(1,Math::RangeRandom(0.85f,1.15f),1);
			mField->addEntity(grass,pos,ori,scale);
		}
	}
	mField->build();

	MeshPtr headMesh = MeshManager::getSingleton().load("ogrehead.mesh",ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	unsigned short src,dest;
	if (!headMesh->suggestTangentVectorBuildParams(VES_TANGENT,src,dest))
	{
		headMesh->buildTangentVectors(VES_TANGENT,src,dest);
	}

	// put an ogre head in the middle of the field
	Entity* head = mSceneMgr->createEntity("Head", "ogrehead.mesh");
	head->setMaterialName("Examples/OffsetMapping/Specular");
	mSceneMgr->getRootSceneNode()->createChildSceneNode(Vector3(0, 30, 0))->attachObject(head);

	//Light
	mSceneMgr->setAmbientLight(ColourValue::Black);

	ColourValue lightColour(1,1,0.3f);
	Light * light = mSceneMgr->createLight();
	light->setDiffuseColour(lightColour);
	light->setSpecularColour(1, 1, 0.3f);
	light->setAttenuation(1500, 1, 0.0005f, 0);
	BillboardSet * bbs = mSceneMgr->createBillboardSet(1);
	bbs->setMaterialName("Examples/Flare");
	Billboard * bb = bbs->createBillboard(0,0,0,lightColour);

	//Õðº³£¡£¡£¡
	ControllerFunctionRealPtr func(OGRE_NEW WaveformControllerFunction(Ogre::WFT_SINE,0.5f,0.5f,0,0.5f));
	ControllerValueRealPtr dest2(OGRE_NEW LightPulse(light,bb,lightColour,15));
	ControllerManager & cm = ControllerManager::getSingleton();
	mLightController = cm.createController(cm.getFrameTimeSource(),dest2,func);
	SceneNode * lightNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	lightNode->attachObject(light);
	lightNode->attachObject(bbs);

	// set up a 20 second animation for our light, using spline interpolation for nice curves
	Animation* anim = mSceneMgr->createAnimation("LightTrack", 20);
	anim->setInterpolationMode(Animation::IM_SPLINE);

	// create a track to animate the camera's node
	NodeAnimationTrack* track = anim->createNodeTrack(0, lightNode);

	// create keyframes for our track
	track->createNodeKeyFrame(0)->setTranslate(Vector3(42, 77, -42));
	track->createNodeKeyFrame(2)->setTranslate(Vector3(21, 84, -35));
	track->createNodeKeyFrame(4)->setTranslate(Vector3(-21, 91, -14));
	track->createNodeKeyFrame(6)->setTranslate(Vector3(-56, 70, -28));
	track->createNodeKeyFrame(8)->setTranslate(Vector3(-28, 70, -56));
	track->createNodeKeyFrame(10)->setTranslate(Vector3(-14, 63, -28));
	track->createNodeKeyFrame(12)->setTranslate(Vector3(-14, 56, 28));
	track->createNodeKeyFrame(14)->setTranslate(Vector3(0, 35, 84));
	track->createNodeKeyFrame(16)->setTranslate(Vector3(14, 35, 14));
	track->createNodeKeyFrame(18)->setTranslate(Vector3(35, 84, 0));
	track->createNodeKeyFrame(20)->setTranslate(Vector3(42, 77, -42));

	lightNode->setPosition(track->getNodeKeyFrame(0)->getTranslate());

	// create a new animation state to track this
	mLightAnimState = mSceneMgr->createAnimationState("LightTrack");
	mLightAnimState->setEnabled(false);

	mCameraMan->getCamera()->setPosition(0,80,0);
}

void OgreSample12App::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
	mLightAnimState->addTime(evt.timeSinceLastFrame);
	waveGrass(evt.timeSinceLastFrame);
}

void OgreSample12App::waveGrass(Real timeElapsed)
{
	static Real xinc = Math::PI * 0.3f;
	static Real zinc = Math::PI * 0.44f;
	static Real xpos = Math::RangeRandom(-Math::PI,Math::PI);
	static Real zpos = Math::RangeRandom(-Math::PI,Math::PI);
	static Vector4 offset(0, 0, 0, 0);

	xpos += xinc * timeElapsed;
	zpos += zinc * timeElapsed;

	// update vertex program parameters by binding a value to each renderable
	StaticGeometry::RegionIterator regs = mField->getRegionIterator();
	while(regs.hasMoreElements())
	{
		StaticGeometry::Region * reg = regs.getNext();

		xpos += reg->getCentre().x * 0.001f;
		zpos += reg->getCentre().z * 0.001f;
		offset.x = Math::Sin(xpos) * 4;
		offset.z = Math::Sin(zpos) * 4;
		
		StaticGeometry::Region::LODIterator lods = reg->getLODIterator();
		while (lods.hasMoreElements())
		{
			StaticGeometry::LODBucket::MaterialIterator mats = lods.getNext()->getMaterialIterator();
			while (mats.hasMoreElements())
			{
				StaticGeometry::MaterialBucket::GeometryIterator geoms = mats.getNext()->getGeometryIterator();
				while (geoms.hasMoreElements())
				{
					geoms.getNext()->setCustomParameter(999,offset);
				}
			}
		}
	}
}