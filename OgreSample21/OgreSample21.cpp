#include "OgreSample21.h"
 
void OgreSample21App::createScene()
{
	bool blankTerrain = false;
	mTerrainGlobals = OGRE_NEW TerrainGlobalOptions();

	mEditMarker = mSceneMgr->createEntity("editMarker", "sphere.mesh");
	mEditNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	mEditNode->attachObject(mEditMarker);
	mEditNode->setScale(0.05, 0.05, 0.05);

	MaterialManager::getSingleton().setDefaultTextureFiltering(TFO_ANISOTROPIC);
	MaterialManager::getSingleton().setDefaultAnisotropy(7);

	mSceneMgr->setFog(FOG_LINEAR,ColourValue(.7f,.7f,.8f),0,10000,25000);

	LogManager::getSingleton().setLogDetail(LL_BOREME);
	
	Vector3 lightdir(0.55, -0.3, 0.75);
	lightdir.normalise();

	Light* l = mSceneMgr->createLight("tstLight");
	l->setType(Light::LT_DIRECTIONAL);
	l->setDirection(lightdir);
	l->setDiffuseColour(ColourValue::White);
	l->setSpecularColour(ColourValue(0.4, 0.4, 0.4));

	MaterialManager::getSingleton().setDefaultTextureFiltering(TFO_ANISOTROPIC);
	MaterialManager::getSingleton().setDefaultAnisotropy(7);

	mSceneMgr->setAmbientLight(ColourValue(0.2, 0.2, 0.2));

	mDummyPageProvider = *(new DummyPageProvider());

	mTerrainGroup = OGRE_NEW TerrainGroup(mSceneMgr,Terrain::ALIGN_X_Z,TERRAIN_SIZE,TERRAIN_WORLD_SIZE);
	mTerrainGroup->setFilenameConvention(TERRAIN_FILE_PREFIX,TERRAIN_FILE_SUFFIX);
	mTerrainGroup->setOrigin(Ogre::Vector3(0,0,0));

	configureTerrainDefaults(l);

/*
	for(long x=0;x<=0;++x)
	{
		for (long y=0;y<=0;++y)
		{
			defineTerrain(x,y,blankTerrain);
		}
	}
	mTerrainGroup->loadAllTerrains(true);*/



	mPageManager = OGRE_NEW PageManager();
	mPageManager->setPageProvider(&mDummyPageProvider);
	mPageManager->addCamera(mCameraMan->getCamera());
	mTerrainPaging = OGRE_NEW TerrainPaging(mPageManager);
	PagedWorld * world = mPageManager->createWorld();//->loadWorld("x.world");//
	mTerrainPagedSection = mTerrainPaging->createWorldSection(world, mTerrainGroup, 2000, 3000, 0, 0, 6, 4);
	//mTerrainPagedSection->loadPage(1,true);
	
	//mTerrainGroup->update();

	//saveTerrains(true);
	
	//ws->getPage(0)->save("1.page");
/*
	if (1)
	{
		TerrainGroup::TerrainIterator ti = mTerrainGroup->getTerrainIterator();
		while (ti.hasMoreElements())
		{
			Terrain * t = ti.getNext()->instance;
			initBlendmaps(t);
		}
	}*/

	mTerrainGroup->freeTemporaryResources();

	//mTerrainGroup->saveAllTerrains(true);

	//mPageManager->saveWorld(world,"x.world");
//	mTerrainGroup->loadAllTerrains("testTerrain.dat");
	
	Entity * e = mSceneMgr->createEntity("tudorhouse.mesh");
	Vector3 entPos(mTerrainPos.x + 2043,0,mTerrainPos.z + 1715);
	Quaternion rot;
	entPos.y = mTerrainGroup->getHeightAtWorldPosition(entPos) + 65.5f + mTerrainPos.y;
	rot.FromAngleAxis(Degree(Math::RangeRandom(-180,180)),Vector3::UNIT_Y);
	SceneNode* sn = mSceneMgr->getRootSceneNode()->createChildSceneNode(entPos, rot);
	sn->setScale(Vector3(0.12, 0.12, 0.12));
	sn->attachObject(e);
	mHouseList.push_back(e);

	e = mSceneMgr->createEntity("tudorhouse.mesh");
	entPos = Vector3(mTerrainPos.x + 1850, 0, mTerrainPos.z + 1478);
	entPos.y = mTerrainGroup->getHeightAtWorldPosition(entPos) + 65.5 + mTerrainPos.y;
	rot.FromAngleAxis(Degree(Math::RangeRandom(-180, 180)), Vector3::UNIT_Y);
	sn = mSceneMgr->getRootSceneNode()->createChildSceneNode(entPos, rot);
	sn->setScale(Vector3(0.12, 0.12, 0.12));
	sn->attachObject(e);
	mHouseList.push_back(e);

	e = mSceneMgr->createEntity("tudorhouse.mesh");
	entPos = Vector3(mTerrainPos.x + 1970, 0, mTerrainPos.z + 2180);
	entPos.y = mTerrainGroup->getHeightAtWorldPosition(entPos) + 65.5 + mTerrainPos.y;
	rot.FromAngleAxis(Degree(Math::RangeRandom(-180, 180)), Vector3::UNIT_Y);
	sn = mSceneMgr->getRootSceneNode()->createChildSceneNode(entPos, rot);
	sn->setScale(Vector3(0.12, 0.12, 0.12));
	sn->attachObject(e);
	mHouseList.push_back(e);

	mSceneMgr->setSkyBox(true, "Examples/CloudyNoonSkyBox");

	mCameraMan->getCamera()->setFarClipDistance(0);

	//changeShadows(true,true);

	mCameraMan->getCamera()->setPosition(0,260,0);
}

void OgreSample21App::destroyScene()
{
	if (mTerrainPaging)
	{
		OGRE_DELETE mTerrainPaging;
		OGRE_DELETE mPageManager;
	}
	else
		OGRE_DELETE mTerrainGlobals;

	OGRE_DELETE mTerrainGroup;
}

void OgreSample21App::changeShadows(bool enabled,bool depthShadows)
{
	TerrainMaterialGeneratorA::SM2Profile* matProfile = static_cast<TerrainMaterialGeneratorA::SM2Profile *>(mTerrainGlobals->getDefaultMaterialGenerator()->getActiveProfile());
	matProfile->setReceiveDynamicShadowsEnabled(enabled);
	matProfile->setReceiveDynamicShadowsLowLod(true);

	for (EntityList::iterator i = mHouseList.begin(); i != mHouseList.end(); ++i)
	{
		(*i)->setMaterialName("Examples/TudorHouse");
	}

	if (enabled)
	{
		mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
		mSceneMgr->setShadowFarDistance(3000);
		mSceneMgr->setShadowTextureCountPerLightType(Ogre::Light::LT_DIRECTIONAL,3);

		if (mPSSMSetup.isNull())
		{
			PSSMShadowCameraSetup * pssmSetup = new PSSMShadowCameraSetup();
			pssmSetup->setSplitPadding(mCameraMan->getCamera()->getNearClipDistance());
			pssmSetup->calculateSplitPoints(3,mCameraMan->getCamera()->getNearClipDistance(),mSceneMgr->getShadowFarDistance());
			pssmSetup->setOptimalAdjustFactor(0,2);
			pssmSetup->setOptimalAdjustFactor(1,1);
			pssmSetup->setOptimalAdjustFactor(2,0.5f);

			mPSSMSetup.bind(pssmSetup);
		}
		mSceneMgr->setShadowCameraSetup(mPSSMSetup);

		if (depthShadows)
		{
			mSceneMgr->setShadowTextureCount(3);
			mSceneMgr->setShadowTextureConfig(0,2048,2048,PF_FLOAT32_R);
			mSceneMgr->setShadowTextureConfig(1,1024,1024,PF_FLOAT32_R);
			mSceneMgr->setShadowTextureConfig(2,1024,1024,PF_FLOAT32_R);
			mSceneMgr->setShadowTextureSelfShadow(true);
			mSceneMgr->setShadowCasterRenderBackFaces(true);
			mSceneMgr->setShadowTextureCasterMaterial("PSSM/shadow_caster");
			MaterialPtr houseMat = buildDepthShadowMaterial("fw12b.jpg");
			for (EntityList::iterator i = mHouseList.begin(); i != mHouseList.end(); ++i)
			{
				(*i)->setMaterial(houseMat);
			}
		}
		else
		{
			mSceneMgr->setShadowTextureCount(3);
			mSceneMgr->setShadowTextureConfig(0,2048,2048,PF_X8B8G8R8);
			mSceneMgr->setShadowTextureConfig(1, 1024, 1024, PF_X8B8G8R8);
			mSceneMgr->setShadowTextureConfig(2, 1024, 1024, PF_X8B8G8R8);
			mSceneMgr->setShadowTextureSelfShadow(false);
			mSceneMgr->setShadowCasterRenderBackFaces(false);
			mSceneMgr->setShadowTextureCasterMaterial(StringUtil::BLANK);
		}
		
		matProfile->setReceiveDynamicShadowsDepth(depthShadows);
		matProfile->setReceiveDynamicShadowsPSSM(static_cast<PSSMShadowCameraSetup *>(mPSSMSetup.get()));
	}
	else
		mSceneMgr->setShadowTechnique(SHADOWTYPE_NONE);
}

MaterialPtr OgreSample21App::buildDepthShadowMaterial(const String & textureName)
{
	String matName = "DepthShadows/" + textureName;

	MaterialPtr ret = MaterialManager::getSingleton().getByName(matName);
	if (ret.isNull())
	{
		MaterialPtr baseMat = MaterialManager::getSingleton().getByName("Ogre/shadow/depth/integrated/pssm");
		ret = baseMat->clone(matName);
		Pass * p = ret->getTechnique(0)->getPass(0);
		p->getTextureUnitState("diffuse")->setTextureName(textureName);
		Vector4 splitPoints;
		const PSSMShadowCameraSetup::SplitPointList& splitPointList = static_cast<PSSMShadowCameraSetup *>(mPSSMSetup.get())->getSplitPoints();
		for(int i=0;i<3;i++)
		{
			splitPoints[i] = splitPointList[i];
		}
		p->getFragmentProgramParameters()->setNamedConstant("pssmSplitPoints",splitPoints);
	}
	return ret;
}

void OgreSample21App::configureTerrainDefaults(Light * l)
{
	mTerrainGlobals->setMaxPixelError(8);
	mTerrainGlobals->setCompositeMapDistance(3000);
	mTerrainGlobals->setLightMapDirection(l->getDerivedDirection());
	mTerrainGlobals->setCompositeMapDiffuse(l->getDiffuseColour());
	mTerrainGlobals->setCompositeMapAmbient(mSceneMgr->getAmbientLight());

	Terrain::ImportData& defaultimp = mTerrainGroup->getDefaultImportSettings();
	defaultimp.terrainSize = TERRAIN_SIZE;
	defaultimp.worldSize = TERRAIN_WORLD_SIZE;
	defaultimp.inputScale = 600;
	defaultimp.minBatchSize = 33;
	defaultimp.maxBatchSize = 65;

	defaultimp.layerList.resize(3);
	defaultimp.layerList[0].worldSize = 100;
	defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_diffusespecular.dds");
	defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_normalheu=ight.dds");
	defaultimp.layerList[1].worldSize = 30;
	defaultimp.layerList[1].textureNames.push_back("grass_green-01_diffusespecular.dds");
	defaultimp.layerList[1].textureNames.push_back("grass_green-01_normalheight.dds");
	defaultimp.layerList[2].worldSize = 200;
	defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_diffusespecular.dds");
	defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_normalheight.dds");

}

void OgreSample21App::initBlendmaps(Ogre::Terrain *terrain)
{
	TerrainLayerBlendMap * blendMap0 = terrain->getLayerBlendMap(1);
	TerrainLayerBlendMap * blendMap1 = terrain->getLayerBlendMap(2);
	Real minHeight0 = 70;
	Real fadeDist0 = 40;
	Real minHeight1 = 70;
	Real fadeDist1 = 15;
	float * pBlend0 = blendMap0->getBlendPointer();
	float * pBlend1 = blendMap1->getBlendPointer();
	for (Ogre::uint16 y=0;y<terrain->getLayerBlendMapSize();++y)
	{
		for (uint16 x=0;x<terrain->getLayerBlendMapSize();++x)
		{
			Real tx,ty;
			blendMap0->convertImageToTerrainSpace(x,y,&tx,&ty);
			Real height = terrain->getHeightAtTerrainPosition(tx,ty);
			Real val = (height - minHeight0)/fadeDist0;
			val = Math::Clamp(val,(Real)0,(Real)1);
			*pBlend0++ = val;
			
			val = (height - minHeight1)/fadeDist1;
			val = Math::Clamp(val,(Real)0,(Real)1);
			*pBlend1++ = val;
		}
	}
	blendMap0->dirty();
	blendMap1->dirty();
	blendMap0->update();
	blendMap1->update();

	/*
	if (!terrain->getGlobalColourMapEnabled())
		{
			terrain->setGlobalColourMapEnabled(true);
			Image colourMap;
			colourMap.load("testcolourmap.jpg", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
			terrain->getGlobalColourMap()->loadImage(colourMap);
		}*/
	
}

void OgreSample21App::getTerrainImage(bool flipX, bool flipY, Ogre::Image &img)
{
	img.load("terrain.png",ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	if (flipX)
	{
		img.flipAroundY();
	}
	if (flipY)
	{
		img.flipAroundX();
	}
}

void OgreSample21App::defineTerrain(long x, long y, bool flat)
{
	if (flat)
	{
		mTerrainGroup->defineTerrain(x,y,0.0f);
	}
	else
	{
		String filename = mTerrainGroup->generateFilename(x,y);
		ResourceGroupManager& rm = ResourceGroupManager::getSingleton();
		if (rm.resourceExists(ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME,filename))
		{
			mTerrainGroup->defineTerrain(x,y);	
		}
		else
		{
			Image img;
			getTerrainImage(x % 2 != 0,y % 2 != 0,img);
			mTerrainGroup->defineTerrain(x,y,&img);
			mTerrainsImported = true;
		}
	}
}

void OgreSample21App::saveTerrains(bool onlyIfModified)
{
	mTerrainGroup->saveAllTerrains(onlyIfModified);
}

void OgreSample21App::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
	Ray ray;
	CEGUI::Point point = CEGUI::MouseCursor::getSingleton().getPosition();
	Viewport * vp = mCameraMan->getCamera()->getViewport();
	ray = mCameraMan->getCamera()->getCameraToViewportRay(Real(point.d_x / vp->getActualWidth()),Real(point.d_y / vp->getActualHeight()));
	TerrainGroup::RayResult rayResult = mTerrainGroup->rayIntersects(ray);
	if (rayResult.hit)
	{
		mEditMarker->setVisible(true);
		mEditNode->setPosition(rayResult.position);
		TerrainGroup::TerrainList terrainList;
		Real brushSizeWorldSpace = TERRAIN_WORLD_SIZE * mBrushSizeTerrainSpace;
		Sphere sphere(rayResult.position,brushSizeWorldSpace);
		mTerrainGroup->sphereIntersects(sphere,&terrainList);
		for (TerrainGroup::TerrainList::iterator ti = terrainList.begin();ti != terrainList.end();++ti)
		{
			static_cast<OgreSample21Listener*>(mListener)->modifyTerrain(*ti,rayResult.position,evt.timeSinceLastFrame);
		}
	}
	
	if (mHeightUpdateCountDown > 0)
	{
		mHeightUpdateCountDown -= evt.timeSinceLastFrame;
		if (mHeightUpdateCountDown <= 0)
		{
			mTerrainGroup->update();
			mHeightUpdateCountDown = 0;
		}
	}

	if (!mTerrainGroup->isDerivedDataUpdateInProgress())
	{
		if (mTerrainsImported)
		{
			saveTerrains(true);
			mTerrainsImported = false;
		}
	}
}

void OgreSample21Listener::modifyTerrain(Ogre::Terrain *terrain, const Ogre::Vector3 &centrePos, Ogre::Real timeElapsed)
{
	Vector3 tsPos;
	terrain->getTerrainPosition(centrePos,&tsPos);
	if (mKeyboard->isKeyDown(OIS::KC_Q) || mKeyboard->isKeyDown(OIS::KC_E))
	{
		//static_cast<OgreSample21App*>(mApp)->mTerrainsImported = true;
		Real size = static_cast<OgreSample21App*>(mApp)->mBrushSizeTerrainSpace;
		switch (mMode)
		{
		case 0:
			{
				Real terrainSize = (terrain->getSize() - 1);
				long startx = (tsPos.x - size) * terrainSize;
				long starty = (tsPos.y - size) * terrainSize;
				long endx = (tsPos.x + size) * terrainSize;
				long endy = (tsPos.y + size) * terrainSize;
				startx = std::max(startx,0L);
				starty = std::max(starty,0L);
				endx = std::min(endx,(long)terrainSize);
				endy = std::min(endy,(long)terrainSize);
				for (long y=starty;y<=endy;++y)
				{
					for (long x=startx;x<=endx;++x)
					{
						Real tsXdist = (x / terrainSize) - tsPos.x;
						Real tsYdist = (y / terrainSize) - tsPos.y;
						Real weight = std::min((Real)1.0f,Math::Sqrt(tsYdist * tsYdist + tsXdist * tsXdist) / Real(0.5f * size));
						weight = 1.0f - (weight * weight);

						float addedHeight = weight * 250.0f * timeElapsed;
						float newheight;
						if (mKeyboard->isKeyDown(OIS::KC_Q))
						{
							newheight = terrain->getHeightAtPoint(x,y) + addedHeight;
						}
						else
							newheight = terrain->getHeightAtPoint(x,y) - addedHeight;
						terrain->setHeightAtPoint(x,y,newheight);
					}
				}
				if (static_cast<OgreSample21App*>(mApp)->mHeightUpdateCountDown == 0)
				{
					static_cast<OgreSample21App*>(mApp)->mHeightUpdateCountDown = static_cast<OgreSample21App*>(mApp)->mHeightUpdateRate;
				}
			}
			break;
		case 1:
			{
				TerrainLayerBlendMap * layer = terrain->getLayerBlendMap(static_cast<OgreSample21App*>(mApp)->mLayerEdit);
				Real imgSize = terrain->getLayerBlendMapSize();
				long startx = (tsPos.x - size) * imgSize;
				long starty = (tsPos.y - size) * imgSize;
				long endx = (tsPos.x + size) * imgSize;
				long endy= (tsPos.y + size) * imgSize;
				startx = std::max(startx,0L);
				starty = std::max(starty,0L);
				endx = std::min(endx, (long)imgSize);
				endy = std::min(endy, (long)imgSize);
				for (long y = starty; y <= endy; ++y)
				{
					for (long x = startx; x <= endx; ++x)
					{
						Real tsXdist = (x / imgSize) - tsPos.x;
						Real tsYdist = (y / imgSize) - tsPos.y;

						Real weight = std::min((Real)1.0f,Math::Sqrt(tsYdist * tsYdist + tsXdist * tsXdist) / Real(0.5f * size));
						weight = 1.0f - (weight * weight);

						float paint = weight * timeElapsed;
						size_t imgY = imgSize - y;
						float val;
						if (mKeyboard->isKeyDown(OIS::KC_Q))
						{
							val = layer->getBlendValue(x,imgY) + paint;
						}
						else
							val = layer->getBlendValue(x,imgY) - paint;
						val = Math::Clamp(val,0.0f,1.0f);
						layer->setBlendValue(x,imgY,val);
					}
				}
				layer->update();
			}
			break;
		}
	}
}

void OgreSample21App::createFrameListener()
{
	OgreSample21Listener * listener = new OgreSample21Listener(this,mRoot,mSceneMgr,mWindow,mCameraMan,mOgreRenderer);
	listener->initialise();
	mListener = listener;
}

bool OgreSample21Listener::keyPressed(const OIS::KeyEvent &arg)
{
	switch (arg.key)
	{
	case OIS::KC_S:
		if (mKeyboard->isKeyDown(OIS::KC_LCONTROL) || mKeyboard->isKeyDown(OIS::KC_RCONTROL))
		{
			static_cast<OgreSample21App*>(mApp)->saveTerrains(true);
		}
		else
			return CEGUIBaseListener::keyPressed(arg);
		break;
	case OIS::KC_F10:
		{
			TerrainGroup::TerrainIterator ti = static_cast<OgreSample21App*>(mApp)->mTerrainGroup->getTerrainIterator();
			while (ti.hasMoreElements())
			{
				uint32 tkey = ti.peekNextKey();
				TerrainGroup::TerrainSlot * ts = ti.getNext();
				if (ts->instance && ts->instance->isLoaded())
				{
					ts->instance->_dumpTextures("terrain_" + StringConverter::toString(tkey),".png");
				}
			}
		}	
		break;
	default:
		return CEGUIBaseListener::keyPressed(arg);
	}
	return true;
}