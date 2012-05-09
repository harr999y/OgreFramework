#include "HydraxSample1.h"

Ogre::String mSkyBoxes[SKYNUM] = {"Sky/ClubTropicana","Sky/EarlyMorning","Sky/Clouds"};
Ogre::Vector3 mSunPosition[SKYNUM] = {Ogre::Vector3(0,10000,0),Ogre::Vector3(0,10000,90000),Ogre::Vector3(0,10000,0)};
Ogre::Vector3 mSunColor[SKYNUM] = {Ogre::Vector3(1, 0.9, 0.6),Ogre::Vector3(1,0.6,0.4),Ogre::Vector3(0.45,0.45,0.45)};

Ogre::String entityNames[3] = {"tudorhouse.mesh","ninja.mesh","Palm.mesh"};
Ogre::String materialNames[3] = {"Examples/TudorHouse","Examples/Ninja","Palm"};

void HydraxSample1App::createScene()
{
	using namespace Ogre;
	mSceneMgr->setAmbientLight(ColourValue(1, 1, 1));

	mRaySceneQuery = mSceneMgr->createRayQuery(Ogre::Ray());

	// Create the SkyBox
	//mSceneMgr->setSkyBox(true, mSkyBoxes[mCurrentSkyBox], 99999 * 6, true);

	// Set some camera params

	mCameraMan->getCamera()->setFarClipDistance(99999 * 6);
	//mCameraMan->getCamera()->setNearClipDistance(10);

	mCameraMan->getCamera()->setPosition(312.902,206.419,1524.02);
	mCameraMan->getCamera()->setOrientation(Ogre::Quaternion(0.998, -0.0121, -0.0608, -0.00074));

/*
	Ogre::Light *mLight = mSceneMgr->createLight("Light0");
	mLight->setPosition(mSunPosition[mCurrentSkyBox]);
	mLight->setDiffuseColour(1, 1, 1);
	mLight->setSpecularColour(mSunColor[mCurrentSkyBox].x,
		mSunColor[mCurrentSkyBox].y,
		mSunColor[mCurrentSkyBox].z);
*/

	// Light
	Ogre::Light *mLight0 = mSceneMgr->createLight("Light0");
	//mLight0->setType(Ogre::Light::LT_DIRECTIONAL);
	mLight0->setDiffuseColour(1, 1, 1);
	mLight0->setCastShadows(false);

	// Shadow caster
	Ogre::Light *mLight1 = mSceneMgr->createLight("Light1");
	mLight1->setType(Ogre::Light::LT_DIRECTIONAL);
	//mLight1->setDiffuseColour(1, 1, 1);
	//mLight1->setCastShadows(false);

	createHydraX();
	createSkyX();
	//createTerrain();

	//Ogre::CompositorManager::getSingleton().addCompositor(mWindow->getViewport(0),"BloomSkyX")->addListener(new BloomListener(mSkyX));
	//Ogre::CompositorManager::getSingleton().setCompositorEnabled(mWindow->getViewport(0),"BloomSkyX",true);

	mSceneMgr->setShadowCameraSetup(Ogre::ShadowCameraSetupPtr(new Ogre::FocusedShadowCameraSetup()));
	mSceneMgr->setShadowTextureCasterMaterial("HydraXShadowCaster");
	mSceneMgr->getLight("Light1")->setShadowFarDistance(1750);

	mForceDisableShadows = false;

	Ogre::MaterialPtr islandMat = static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName("Island"));
	mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED);
	mSceneMgr->setShadowTextureConfig(0,2048,2048,Ogre::PF_X8R8G8B8);
	islandMat->getTechnique(0)->setSchemeName("Default");
	islandMat->getTechnique(1)->setSchemeName("NoDefault");

	mHydrax->getRttManager()->addRttListener(new HydraxRttListener(mSkyX,mHydrax));

	/*
	Ogre::SceneNode * node2 = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		Ogre::Entity * ent2 = mSceneMgr->createEntity("ninja.mesh");
		ent2->setMaterial(Ogre::MaterialManager::getSingleton().getByName("Examples/Ninja"));
		node2->attachObject(ent2);
		node2->setPosition(Ogre::Vector3(0,0,0));*/
	

	mRaySceneQuery = mSceneMgr->createRayQuery(Ogre::Ray());
}

void HydraxSample1App::createHydraX()
{
	mHydrax = new Hydrax::Hydrax(mSceneMgr,mCameraMan->getCamera(),mWindow->getViewport(0));

	Hydrax::Module::ProjectedGrid * module = new Hydrax::Module::ProjectedGrid(mHydrax,new Hydrax::Noise::Perlin(/*Hydrax::Noise::Perlin::Options(4,0.0425,0.809,1.54,1.67,8,Ogre::Vector3(0.5,50,150000))Hydrax::Noise::Perlin::Options(18,0.032f,0.24,1.8f,1.32f)*/),Ogre::Plane(Ogre::Vector3(0,1,0),Ogre::Vector3(0,0,0)),Hydrax::MaterialManager::NM_VERTEX,Hydrax::Module::ProjectedGrid::Options(/*106,45,50,true*/));

	mHydrax->setModule(static_cast<Hydrax::Module::Module *>(module));

	mHydrax->loadCfg("VertexTest.hdx");

	mHydrax->create();

	mSceneMgr->setWorldGeometry("Island.cfg");
	mHydrax->getMaterialManager()->addDepthTechnique(static_cast<Ogre::MaterialPtr>(Ogre::MaterialManager::getSingleton().getByName("Island"))->createTechnique());
}

void HydraxSample1App::createSkyX()
{
	mLastPositionLength = (Ogre::Vector3(1500, 100, 1500) - mCameraMan->getCamera()->getDerivedPosition()).length();
	
	mWaterGradient = SkyX::ColorGradient();
	mWaterGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.058209,0.535822,0.779105)*0.4, 1));
	mWaterGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.058209,0.535822,0.729105)*0.3, 0.8));
	mWaterGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.058209,0.535822,0.679105)*0.25, 0.6));
	mWaterGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.058209,0.535822,0.679105)*0.2, 0.5));
	mWaterGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.058209,0.535822,0.679105)*0.1, 0.45));
	mWaterGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.058209,0.535822,0.679105)*0.025, 0));
	// Sun
	mSunGradient = SkyX::ColorGradient();
	mSunGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.8,0.75,0.55)*1.5, 1.0f));
	mSunGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.8,0.75,0.55)*1.4, 0.75f));
	mSunGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.8,0.75,0.55)*1.3, 0.5625f));
	mSunGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.6,0.5,0.2)*1.5, 0.5f));
	mSunGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.5,0.5,0.5)*0.25, 0.45f));
	mSunGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0.5,0.5,0.5)*0.01, 0.0f));
	//mSunGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(0,0,0), 0.02f));
	// Ambient
	mAmbientGradient = SkyX::ColorGradient();
	mAmbientGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(1,1,1)*1, 1.0f));
	mAmbientGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(1,1,1)*1, 0.6f));
	mAmbientGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(1,1,1)*0.7, 0.5f));
	mAmbientGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(1,1,1)*0.6, 0.45f));
	mAmbientGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(1,1,1)*0.5, 0.35f));
	mAmbientGradient.addCFrame(SkyX::ColorGradient::ColorFrame(Ogre::Vector3(1,1,1)*0.35, 0.0f));


	mSkyX = new SkyX::SkyX(mSceneMgr, mCameraMan->getCamera());
	mSkyX->getMeshManager()->setSkydomeFadingParameters(false);
	SkyX::AtmosphereManager::Options atOpt = mSkyX->getAtmosphereManager()->getOptions();
	atOpt.RayleighMultiplier = 0.003075f;
	atOpt.MieMultiplier = 0.00125f;
	atOpt.InnerRadius = 9.92f;
	atOpt.OuterRadius = 10.3311f;
	mSkyX->getAtmosphereManager()->setOptions(atOpt);
mSkyX->getMeshManager()->setMaterialName("SkyX.material");
	mSkyX->create();

	mSkyX->getCloudsManager()->add(SkyX::CloudLayer::Options());

	mSkyX->setTimeMultiplier(0.4f);
}

bool HydraxSample1App::keyPressed(const OIS::KeyEvent &arg)
{
/*
	if (arg.key == OIS::KC_Q)
	{
		mCurrentSkyBox++;
		if (mCurrentSkyBox == 3)
		{
			mCurrentSkyBox = 0;
		}
	}
	mSceneMgr->setSkyBox(true,mSkyBoxes[mCurrentSkyBox],1000,true);

	mHydrax->setSunPosition(mSunPosition[mCurrentSkyBox]);
	mHydrax->setSunColor(mSunColor[mCurrentSkyBox]);

	mSceneMgr->getLight("Light0")->setPosition(mSunPosition[mCurrentSkyBox]);
	mSceneMgr->getLight("Light0")->setSpecularColour(mSunColor[mCurrentSkyBox].x,mSunColor[mCurrentSkyBox].y,mSunColor[mCurrentSkyBox].z);*/

	if (arg.key == OIS::KC_1 || arg.key == OIS::KC_2 || arg.key == OIS::KC_3)
	{
		CEGUI::Point point = CEGUI::MouseCursor::getSingleton().getPosition();
		Ogre::Ray ray = mCameraMan->getCamera()->getCameraToViewportRay(point.d_x/mWindow->getWidth(),point.d_y/mWindow->getHeight());
		mRaySceneQuery->setRay(ray);
		Ogre::RaySceneQueryResult * mRayQueryResult = &(mRaySceneQuery->execute());

		Ogre::RaySceneQueryResult::iterator itr = mRayQueryResult->begin();
		while(itr != mRayQueryResult->end())
		{
			if(itr->worldFragment)
			{
				Ogre::Entity * ent;
				Ogre::SceneNode * node;
				if (arg.key == OIS::KC_1)
				{			
					ent = mSceneMgr->createEntity(entityNames[0]);
					ent->setMaterialName(materialNames[0]);
					node = mSceneMgr->getRootSceneNode()->createChildSceneNode(itr->worldFragment->singleIntersection + Ogre::Vector3(0,200,0));
				}
				else if (arg.key == OIS::KC_2)
				{
					ent = mSceneMgr->createEntity(entityNames[1]);
					ent->setMaterialName(materialNames[1]);
					node = mSceneMgr->getRootSceneNode()->createChildSceneNode(itr->worldFragment->singleIntersection);
				}
				else if (arg.key == OIS::KC_3)
				{
					ent = mSceneMgr->createEntity(entityNames[2]);
					ent->setMaterialName(materialNames[2]);
					node = mSceneMgr->getRootSceneNode()->createChildSceneNode(itr->worldFragment->singleIntersection);
				}
				node->attachObject(ent);
				break;
			}
			itr++;
		}
	}
	return true;
}

void HydraxSample1App::frameStarted(const Ogre::FrameEvent &evt)
{

/*
		Ogre::Vector3 camPos = mCameraMan->getCamera()->getPosition();
		Ogre::Ray ray(Ogre::Vector3(camPos.x,6000.0f,camPos.z),Ogre::Vector3::NEGATIVE_UNIT_Y);
		mRaySceneQuery->setRay(ray);
		mRaySceneQuery->setQueryMask(0);
		//mRaySceneQuery->setQueryMask(ACTIVITIES_MASK);
		mRaySceneQuery->setSortByDistance(false);
		Ogre::RaySceneQueryResult &result = mRaySceneQuery->execute();
		Ogre::RaySceneQueryResult::iterator itr = result.begin();
		if(itr != result.end())
		{
			if(itr->worldFragment)
			{
				Ogre::Real height = itr->worldFragment->singleIntersection.y;
				if((height + 120.0f) != camPos.y)
					mCameraMan->getCamera()->setPosition(camPos.x,height+70.0f,camPos.z);
			}
		}*/
	

	updateEnvironmentLighting();
	updateShadowFarDistance();
	mHydrax->update(evt.timeSinceLastFrame);
	mSkyX->update(evt.timeSinceLastFrame);
}

void BloomListener::notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{
	if (pass_id == 3)
	{
		float bloomStrenth = 0.75 + Ogre::Math::Clamp<Ogre::Real>(-mSkyX->getAtmosphereManager()->getSunDirection().y,0,1) * 0.75f;
		mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant("uBloomStrength",bloomStrenth);
	}
}

void HydraxRttListener::preRenderTargetUpdate(const Hydrax::RttManager::RttType& Rtt)
{
	bool underWater = mHydrax->_isCurrentFrameUnderwater();

	if (underWater)
	{
		switch (Rtt)
		{
		case Hydrax::RttManager::RTT_REFLECTION:
			mSkyX->setStarfieldEnabled(false);
			break;

		case Hydrax::RttManager::RTT_REFRACTION:
			//
			break;

		case Hydrax::RttManager::RTT_DEPTH:
		case Hydrax::RttManager::RTT_DEPTH_REFLECTION:
			{
				mSkyX->getMeshManager()->getEntity()->setVisible(false);
				mSkyX->getMoonManager()->getMoonBillboard()->setVisible(false);
			}
			break;
		}
	}
}

void HydraxRttListener::postRenderTargetUpdate(const Hydrax::RttManager::RttType& Rtt)
{
	bool underWater = mHydrax->_isCurrentFrameUnderwater();

	if (underWater)
	{
		switch (Rtt)
		{
		case Hydrax::RttManager::RTT_REFLECTION:
			{
				mSkyX->setStarfieldEnabled(true);
			}
			break;

		case Hydrax::RttManager::RTT_REFRACTION:
			{
			}
			break;

		case Hydrax::RttManager::RTT_DEPTH: case Hydrax::RttManager::RTT_DEPTH_REFLECTION:
			{
				mSkyX->getMeshManager()->getEntity()->setVisible(true);
				mSkyX->getMoonManager()->getMoonBillboard()->setVisible(true);
			}
			break;
		}
	}
}

void HydraxSample1App::updateEnvironmentLighting()
{
	Ogre::Vector3 lightDir = mSkyX->getAtmosphereManager()->getSunDirection();
	
	bool disable = (lightDir.y > 0.05f) ? true : false;
	if ((disable == true) && (mForceDisableShadows == false))
	{
		//mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
		mSceneMgr->getLight("Light1")->setCastShadows(false);
		mForceDisableShadows = true;
	}

	if (disable == false && mForceDisableShadows == true)
	{
		//mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED);
		mSceneMgr->getLight("Light1")->setCastShadows(true);
		//mSceneMgr->setShadowTextureConfig(0,2048,2048,Ogre::PF_X8R8G8B8);
		mForceDisableShadows = false;
	}

	float point = (-lightDir.y + 1.0f) / 2.0f;
	mHydrax->setWaterColor(mWaterGradient.getColor(point));
	Ogre::Vector3 sunPos = mCameraMan->getCamera()->getDerivedPosition() - lightDir * mSkyX->getMeshManager()->getSkydomeRadius() * 0.1f;
	mHydrax->setSunPosition(sunPos);

	Ogre::Light * light0 = mSceneMgr->getLight("Light0");
	Ogre::Light * light1 = mSceneMgr->getLight("Light1");

	light0->setPosition(mCameraMan->getCamera()->getDerivedPosition() - lightDir * mSkyX->getMeshManager()->getSkydomeRadius() * 0.01f);
	//light1->setPosition(mCameraMan->getCamera()->getDerivedPosition() - lightDir * mSkyX->getMeshManager()->getSkydomeRadius() * 0.01f);
	//light0->setDirection( lightDir * mSkyX->getMeshManager()->getSkydomeRadius() * 0.1f);
	light1->setDirection(lightDir);
	//light0->setDirection(Ogre::Math::Clamp(Ogre::Vector3(1378,0,1630) - sunPos,Ogre::Vector3(-1,-1,-1),Ogre::Vector3(1,1,1)));//Ogre::Math::Clamp(lightDir - Ogre::Vector3(-0.2,0,0),Ogre::Vector3(-1,-1,-1),Ogre::Vector3(1,1,1)));
	Ogre::Vector3 sunCol = mSunGradient.getColor(point);
	light0->setSpecularColour(sunCol.x,sunCol.y,sunCol.z);
	light1->setSpecularColour(sunCol.x,sunCol.y,sunCol.z);
	Ogre::Vector3 ambientCol = mAmbientGradient.getColor(point);
	light0->setDiffuseColour(ambientCol.x,ambientCol.y,ambientCol.z);
	light1->setDiffuseColour(ambientCol.x,ambientCol.y,ambientCol.z);
	mHydrax->setSunColor(sunCol);
}

void HydraxSample1App::updateShadowFarDistance()
{
	Ogre::Light * light1 = mSceneMgr->getLight("Light1");
	float currentLength = (Ogre::Vector3(1500, 100, 1500) - mCameraMan->getCamera()->getDerivedPosition()).length();
	if (currentLength < 1000)
	{
		mLastPositionLength = currentLength;
		return;
	}

	if (currentLength - mLastPositionLength > 100)
	{
		mLastPositionLength += 100;
		light1->setShadowFarDistance(light1->getShadowFarDistance() + 100);
	}
	else if (currentLength - mLastPositionLength < -100)
	{
		mLastPositionLength -= 100;
		light1->setShadowFarDistance(light1->getShadowFarDistance() - 100);
	}
}

void HydraxSample1App::createTerrain()
{
	using namespace Ogre;
	mTerrainGlobals = OGRE_NEW TerrainGlobalOptions();
	mTerrainGroup = OGRE_NEW TerrainGroup(mSceneMgr, Terrain::ALIGN_X_Z, 65, 10200);
	mTerrainGroup->setOrigin(Ogre::Vector3(0,22,0));

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
	defaultimp.worldSize = 10200;
	defaultimp.inputScale = 400;
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
}