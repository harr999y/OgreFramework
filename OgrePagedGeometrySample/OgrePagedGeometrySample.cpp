#include "OgrePagedGeometrySample.h"

void OgrePagedGeometrySampleApp::createTerrain()
{
	using namespace Ogre;
	mTerrainGlobals = OGRE_NEW TerrainGlobalOptions();
	mTerrainGroup = OGRE_NEW TerrainGroup(mSceneMgr, Terrain::ALIGN_X_Z, 513,3200);
	mTerrainGroup->setOrigin(Ogre::Vector3(mTerrainGroup->getTerrainWorldSize() / 2,0,mTerrainGroup->getTerrainWorldSize() / 2));

	// Configure global
	mTerrainGlobals->setMaxPixelError(8);
	// testing composite map
	mTerrainGlobals->setCompositeMapDistance(3000);

	//matProfile->setLightmapEnabled(false);
	// Important to set these so that the terrain knows what to use for derived (non-realtime) data
	//mTerrainGlobals->setLightMapDirection(mLight->getDerivedDirection());

	Ogre::Light * light = mSceneMgr->getLight("Sun");
	mTerrainGlobals->setCompositeMapAmbient(mSceneMgr->getAmbientLight());
	//mTerrainGlobals->setCompositeMapAmbient(ColourValue::Red);
	mTerrainGlobals->setCompositeMapDiffuse(light->getDiffuseColour());
	mTerrainGlobals->setLightMapDirection(light->getDerivedDirection());

	// Configure default import settings for if we use imported image
	Terrain::ImportData& defaultimp = mTerrainGroup->getDefaultImportSettings();
	defaultimp.terrainSize = 513;
	defaultimp.worldSize = 3200;
	defaultimp.inputScale = 80;
	defaultimp.minBatchSize = 33;
	defaultimp.maxBatchSize = 65;
	// textures
	defaultimp.layerList.resize(3);
	defaultimp.layerList[0].worldSize = 513;
	defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_diffusespecular.dds");
	defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_normalheight.dds");

	defaultimp.layerList[1].worldSize = 30;
	defaultimp.layerList[1].textureNames.push_back("grass_green-01_diffusespecular.dds");
	defaultimp.layerList[1].textureNames.push_back("grass_green-01_normalheight.dds");
	defaultimp.layerList[2].worldSize = 200;
	defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_diffusespecular.dds");
	defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_normalheight.dds");


	Image img;
	img.load("densitymap.png", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
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
}


inline float getTerrainHeight(float x, float z, void *userData)
{
	return ((Ogre::TerrainGroup *)userData)->getHeightAtWorldPosition(x,0,z) + 1.5;
}

void OgrePagedGeometrySampleApp::createScene()
{
	mCameraMan->setTopSpeed(200);
	mWindow->getViewport(0)->setBackgroundColour(ColourValue(0.47f, 0.67f, 0.96f));
	mSceneMgr->setFog(FOG_LINEAR, mWindow->getViewport(0)->getBackgroundColour(), 0, 100, 700);
	mSceneMgr->setSkyBox(true, "3D-Diggers/SkyBox", 2000);

	Light *light = mSceneMgr->createLight("Sun");
	light->setType(Light::LT_DIRECTIONAL);
	light->setDirection(Vector3(0.0f, -0.5f, 1.0f));
	mSceneMgr->setAmbientLight(ColourValue(1, 1, 1));
	createTerrain();
	createPagedGeometry();
}

void OgrePagedGeometrySampleApp::createPagedGeometry()
{
	using namespace Forests;
	using namespace Ogre;

	//Trees
	mTreesMgr = new PagedGeometry(mCameraMan->getCamera(),150);
	mTreesMgr->addDetailLevel<WindBatchPage>(150,50);
	mTreesMgr->addDetailLevel<ImpostorPage>(1000,100);
	TreeLoader2D * treeLoader = new TreeLoader2D(mTreesMgr,TBounds(0,0,mTerrainGroup->getTerrainWorldSize(),mTerrainGroup->getTerrainWorldSize()));
	mTreesMgr->setPageLoader(treeLoader);
	treeLoader->setHeightFunction(&getTerrainHeight,(void *)mTerrainGroup);
	treeLoader->setColorMap("terrain_lightmap.jpg");
	Entity * tree1 = mSceneMgr->createEntity("tree1","tree2.mesh");
	Entity * tree2 = mSceneMgr->createEntity("tree2","fir14_25.mesh");

	mTreesMgr->setCustomParam(tree1->getName(), "windFactorX", 15);
	mTreesMgr->setCustomParam(tree1->getName(), "windFactorY", 0.01f);
	mTreesMgr->setCustomParam(tree2->getName(), "windFactorX", 32);
	mTreesMgr->setCustomParam(tree2->getName(), "windFactorY", 0.01f);

	Ogre::Degree yaw;
	Ogre::Vector3 pos;

	for (int i=0;i<2400;i++)
	{
		pos.x = Math::RangeRandom(0, mTerrainGroup->getTerrainWorldSize());
		pos.z = Math::RangeRandom(0, mTerrainGroup->getTerrainWorldSize());
		yaw = Degree(Math::RangeRandom(0,360));

		if (i % 3 == 0)
		{
			treeLoader->addTree(tree1,pos,yaw,Math::RangeRandom(0.6f,0.9f));
		}
		else
		{
			treeLoader->addTree(tree1,pos,yaw);
		}
	}

	//Bushes
	mBushMgr = new PagedGeometry(mCameraMan->getCamera(),150);
	mBushMgr->addDetailLevel<WindBatchPage>(100,50);
	mBushMgr->addDetailLevel<ImpostorPage>(160,50);

	TreeLoader2D * bushLoader = new TreeLoader2D(mBushMgr,TBounds(0,0,mTerrainGroup->getTerrainWorldSize(),mTerrainGroup->getTerrainWorldSize()));
	mBushMgr->setPageLoader(bushLoader);
	bushLoader->setColorMap("terrain_lightmap.jpg");
	bushLoader->setHeightFunction(&getTerrainHeight,(void *)mTerrainGroup);

	Entity *fern = mSceneMgr->createEntity("Fern", "farn1.mesh");
	Entity *plant = mSceneMgr->createEntity("Plant", "plant2.mesh");
	Entity *mushroom = mSceneMgr->createEntity("Mushroom", "shroom1_3.mesh");
	mBushMgr->setCustomParam(fern->getName(), "factorX", 1);
	mBushMgr->setCustomParam(fern->getName(), "factorY", 0.01);
	mBushMgr->setCustomParam(plant->getName(), "factorX", 0.6);
	mBushMgr->setCustomParam(plant->getName(), "factorY", 0.02);

	for (int i = 0; i < 2000; i++)
	{
		yaw = Degree(Math::RangeRandom(0, 360));
		pos.x = Math::RangeRandom(0, mTerrainGroup->getTerrainWorldSize());
		pos.z = Math::RangeRandom(0, mTerrainGroup->getTerrainWorldSize());

		if (i % 3 == 0)
		{
			bushLoader->addTree(fern,pos,yaw,Math::RangeRandom(0.6f,0.9f));
		}
		else if (i % 3 == 1)
		{
			bushLoader->addTree(plant,pos,yaw,Math::RangeRandom(0.6f,0.9f));
		}
		else
		{
			bushLoader->addTree(mushroom,pos,yaw,Math::RangeRandom(0.6f,0.9f));
		}

	}

	//Grass
	mGrassMgr = new PagedGeometry(mCameraMan->getCamera(),70);
	mGrassMgr->addDetailLevel<GrassPage>(100,300);
	GrassLoader * grassLoader = new GrassLoader(mGrassMgr);
	mGrassMgr->setPageLoader(grassLoader);
	grassLoader->setHeightFunction(&getTerrainHeight,(void *)mTerrainGroup);

	GrassLayer * grassLayer = grassLoader->addLayer("grass");
	grassLayer->setMinimumSize(2.0f, 2.0f);
	grassLayer->setMaximumSize(2.5f, 2.5f);
	grassLayer->setAnimationEnabled(true);
	grassLayer->setSwayDistribution(7.0f);
	grassLayer->setSwayLength(0.23f);
	grassLayer->setSwaySpeed(0.5f);
	grassLayer->setDensity(2.0f);
	grassLayer->setRenderTechnique(GRASSTECH_QUAD);
	grassLayer->setFadeTechnique(FADETECH_GROW);
	grassLayer->setColorMap("terrain_texture2.jpg");
	grassLayer->setDensityMap("densitymap.png");
	grassLayer->setMapBounds(TBounds(0,0,mTerrainGroup->getTerrainWorldSize(),mTerrainGroup->getTerrainWorldSize()));

}

void OgrePagedGeometrySampleApp::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
	Ogre::Vector3 pos = mCameraMan->getCamera()->getPosition();
	if (pos.y <= mTerrainGroup->getHeightAtWorldPosition(pos) + 4)
	{
		pos.y = mTerrainGroup->getHeightAtWorldPosition(pos) + 4;
	}
	//pos.y = mTerrainGroup->getHeightAtWorldPosition(pos) + 60;
	mCameraMan->getCamera()->setPosition(pos);

	mTreesMgr->update();
	mBushMgr->update();
	mGrassMgr->update();
}

void OgrePagedGeometrySampleApp::destroyScene()
{
	delete mTreesMgr->getPageLoader();
	delete mBushMgr->getPageLoader();
	delete mGrassMgr->getPageLoader();

	delete mTreesMgr;
	delete mBushMgr;
	delete mGrassMgr;

	mSceneMgr->destroyAllEntities();
}