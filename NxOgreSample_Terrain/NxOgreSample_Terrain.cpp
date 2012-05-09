#include "NxOgreSample_Terrain.h"
#include "NxTruck.h"

void NxOgreSample_TerrainApp::setupPhysX()
{
	using namespace NxOgre;
	mWorld = World::createWorld();
	NxOgre::ResourceSystem::getSingleton()->openProtocol(new Critter::OgreResourceProtocol());

	SceneDescription sd;
	sd.mGravity = Constants::MEAN_EARTH_GRAVITY;
	//sd.mUseHardware = true;
	mScene = mWorld->createScene(sd);
	mScene->getMaterial(0)->setAll(0.1f,0.9f,0.5f);
	mRenderSystem = new Critter::RenderSystem(mScene,mSceneMgr);

	//mWorld->getRemoteDebugger()->connect("localhost",5425);
	//mRenderSystem->setVisualisationMode(Enums::VisualDebugger_ShowAll);
	//mRenderSystem->setVisualisationMode(Enums::VisualDebugger_);
}

void NxOgreSample_TerrainApp::destroyScene()
{
	OGRE_DELETE mTerrainGroup;
	OGRE_DELETE mTerrainGlobals;
	NxOgre::World::destroyWorld();
}

void NxOgreSample_TerrainApp::createScene()
{
	using namespace Ogre;
	ColourValue background = ColourValue(135.0f/255.f, 206.f/255.f, 235.f/255.f);
	//mCameraMan->getCamera()->getViewport()->setBackgroundColour(background);

	// add a bright light above the scene
	mLight = mSceneMgr->createLight();
	mLight->setType(Light::LT_POINT);
	mLight->setPosition(-10, 40, 20);
	mLight->setSpecularColour(ColourValue::White);

	mCameraMan->getCamera()->setPosition(10,10,10);
	mCameraMan->getCamera()->lookAt(0,0,0);
	mCameraMan->getCamera()->setNearClipDistance(0.02f);
	mCameraMan->getCamera()->setFarClipDistance(1000);
	mCameraMan->setTopSpeed(200);

	mSceneMgr->setSkyBox(true, "Examples/CloudyNoonSkyBox");
	mSceneMgr->setAmbientLight(ColourValue::White);

	setupPhysX();

	mTerrainGlobals = OGRE_NEW TerrainGlobalOptions();
	mTerrainGroup = OGRE_NEW TerrainGroup(mSceneMgr, Terrain::ALIGN_X_Z, 65, 1000);
	mTerrainGroup->setOrigin(Ogre::Vector3(0,-40,0));

	// Configure global
	mTerrainGlobals->setMaxPixelError(8);
	// testing composite map
	mTerrainGlobals->setCompositeMapDistance(3000);
	//mTerrainGlobals->setUseRayBoxDistanceCalculation(true);
	//mTerrainGlobals->getDefaultMaterialGenerator()->setDebugLevel(1);
	//mTerrainGlobals->setLightMapSize(256);

	//matProfile->setLightmapEnabled(false);
	// Important to set these so that the terrain knows what to use for derived (non-realtime) data
	mTerrainGlobals->setLightMapDirection(mLight->getDerivedDirection());
	mTerrainGlobals->setCompositeMapAmbient(mSceneMgr->getAmbientLight());
	//mTerrainGlobals->setCompositeMapAmbient(ColourValue::Red);
	mTerrainGlobals->setCompositeMapDiffuse(ColourValue::White);

	// Configure default import settings for if we use imported image
	Terrain::ImportData& defaultimp = mTerrainGroup->getDefaultImportSettings();
	defaultimp.terrainSize = 65;
	defaultimp.worldSize = 1000;
	defaultimp.inputScale = 10;
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
	//mTerrainGroup->defineTerrain(-1, 0,&img);
	//mTerrainGroup->defineTerrain(0, -1,&img);
	//mTerrainGroup->defineTerrain(0, 0,&img);

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
		//blendMap0->loadImage("blendmap1.png", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		blendMap0->update();
		blendMap1->update();

		// set up a colour map
		
		/*if (!terrain->getGlobalColourMapEnabled())
		{
		terrain->setGlobalColourMapEnabled(true);
		Image colourMap;
		colourMap.load("RustedMetal.jpg", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		terrain->getGlobalColourMap()->loadImage(colourMap);
		}*/
		
	}
	mTerrainGroup->freeTemporaryResources();	

	NxOgre::SceneGeometry * sg;
	mTruck = new Truck(mRenderSystem);

	ti = mTerrainGroup->getTerrainIterator();
	while (ti.hasMoreElements())
	{
		Terrain * t = ti.getNext()->instance;
		sg = mRenderSystem->createTerrain(t);
	}

	//mRaySceneQuery = mSceneMgr->createRayQuery(Ogre::Ray());
	//sg->getNxActor()->setForceFieldMaterial(0);
	NxOgre::Material * mat = mScene->getMaterial(0);
	mat->setAll(0.0f,2.0f,1.0f);
	
	NxOgre::MaterialDescription m;
	m.mRestitution = 0;
	m.mStaticFriction = 1.2;
	m.mDynamicFriction = 1;
	NxOgre::MaterialIdentifier wheelMat = mScene->createMaterial(m)->getIdentifier();
	m.mStaticFriction = 2;
	m.mDynamicFriction = 1;
	sg->getNxActor()->setForceFieldMaterial(mScene->createMaterial(m)->getIdentifier());

	mTruck->create(*mScene,*(sg->getNxActor()),wheelMat);	

	//mCameraMan->setStyle(OgreBites::CS_ORBIT);
	//mCameraMan->setTarget(mTruck->tractor->getSceneNode());
	//mCameraMan->setYawPitchDist(Ogre::Radian(5),Ogre::Radian(5),2);

 }

void NxOgreSample_TerrainApp::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	using namespace NxOgre;
	//mCameraMan->getCamera()->setPosition(mTruck->tractor->getSceneNode()->_getDerivedPosition());
	mWorld->advance(evt.timeSinceLastFrame);
	if (GetAsyncKeyState(VK_SPACE) && 0xff)
	{
		Critter::BodyDescription bodyDescription;
		bodyDescription.mMass = 5340;
		bodyDescription.mLinearVelocity = NxOgre::Vec3(mCameraMan->getCamera()->getDirection().x * 10,mCameraMan->getCamera()->getDirection().y * 10,mCameraMan->getCamera()->getDirection().z * 10);//Vec3(0,-30,0);
		NxOgre::BoxDescription box;
		box.mSize.set(1,1,1);
		box.mSkinWidth = 0.025f;
		//Critter::Body* body = mRenderSystem->createBody(box, Vec3::random(-228,228,500,1228,-228,228), "cube.1m.mesh", bodyDescription);
		Critter::Body* body = mRenderSystem->createBody(box, NxOgre::Vec3(mCameraMan->getCamera()->getPosition().x,mCameraMan->getCamera()->getPosition().y,mCameraMan->getCamera()->getPosition().z), "cube.1m.mesh", bodyDescription);
		body->getSceneNode()->setScale(1,1,1);


		//OGRE3DBody * cube = mRenderSystem->createBody(new NxOgre::Box(1,1,1),NxOgre::Vec3(mCameraMan->getCamera()->getPosition().x,mCameraMan->getCamera()->getPosition().y,mCameraMan->getCamera()->getPosition().z),"fish.mesh");
		//cube->addForce(NxOgre::Vec3(mCameraMan->getCamera()->getDirection().x * 10000,mCameraMan->getCamera()->getDirection().y * 10000,mCameraMan->getCamera()->getDirection().z * 10000),NxOgre::Enums::ForceMode_Force,true);
		
	}
	if ((GetAsyncKeyState('T') && 0xff) || (GetAsyncKeyState('G') && 0xff) || (GetAsyncKeyState('F') && 0xff) || (GetAsyncKeyState('H') && 0xff))
	{
		bool l,r,u,d;
		l = u = r = d = false;
		if (GetAsyncKeyState('T') && 0xff)
		{
			u = true;
		}
		if (GetAsyncKeyState('G') && 0xff)
		{
			d = true;
		}
		if (GetAsyncKeyState('F') && 0xff)
		{
			l = true;
		}
		if (GetAsyncKeyState('H') && 0xff)
		{
			r= true;
		}
		mTruck->control(l,r,u,d,30);
		return;
	}
	//mTruck->control(false,false,false,false,0);
	
	//mTruck->control(false,false,false,false,5);
	//up the terrain
	/*Ogre::Vector3 camPos = mCameraMan->getCamera()->getDerivedPosition();
	//Ogre::Terrain::rayIntersects()
	Ogre::TerrainGroup::TerrainIterator ti = mTerrainGroup->getTerrainIterator();
	Ogre::Real camY; 
	while (ti.hasMoreElements())
	{
		Ogre::Terrain * t = ti.getNext()->instance;
		camY = mTerrainGroup->getHeightAtWorldPosition(camPos,&t);	
	}
	
	mCameraMan->getCamera()->setPosition(camPos.x,camY + 20,camPos.z);*/


}

bool NxOgreSample_TerrainApp::keyPressed(const OIS::KeyEvent &arg)
{
	if (arg.key == OIS::KC_RETURN)
	{
		if (mCameraMan->getTopSpeed() == 2)
		{
			mCameraMan->setTopSpeed(200);
		}
		else
			mCameraMan->setTopSpeed(2);
	}

	return true;
}

bool NxOgreSample_TerrainListener::keyPressed(const OIS::KeyEvent &arg)
{
	CEGUIBaseListener::keyPressed(arg);
	return mApp->keyPressed(arg);
}

void NxOgreSample_TerrainApp::createFrameListener()
{
	NxOgreSample_TerrainListener * listener = new NxOgreSample_TerrainListener(this,mRoot,mSceneMgr,mWindow,mCameraMan,mOgreRenderer);
	listener->initialise();
	mListener = listener;
}