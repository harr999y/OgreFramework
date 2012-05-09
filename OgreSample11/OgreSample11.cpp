#include "OgreSample11.h"

void OgreSample11App::createScene()
{
	mSceneMgr->setAmbientLight(ColourValue(0.5, 0.5, 0.5));  // set ambient light

	mSceneMgr->setSkyBox(true, "Examples/CloudyNoonSkyBox");  // set a skybox

	// make the scene's main light come from above
	Light* l = mSceneMgr->createLight();
	l->setType(Light::LT_DIRECTIONAL);
	l->setDirection(Vector3::NEGATIVE_UNIT_Y);	

	for (unsigned int i=0;i<2;i++)
	{
		TexturePtr tex = TextureManager::getSingleton().createManual(i==0 ? "refraction" : "reflection",ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,TEX_TYPE_2D,512,512,0,PF_R8G8B8,TU_RENDERTARGET);
		RenderTarget * rt = tex->getBuffer()->getRenderTarget();
		rt->addViewport(mCameraMan->getCamera())->setOverlaysEnabled(false);
		rt->addListener(this);

		if (i == 0)
		{
			mRefractionTarget = rt;
		}
		else
		{
			mReflectionTarget = rt;
		}
	}

	mWaterPlane = Plane(Vector3::UNIT_Y,0);
	MeshManager::getSingleton().createPlane("water",ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,mWaterPlane,700,1300,10,10,true,1,3,5,Vector3::UNIT_Z);
	
	MaterialPtr mat = (MaterialPtr)MaterialManager::getSingleton().load("Examples/FresnelReflectionRefraction","Popular");
	mWater = mSceneMgr->createEntity("Water","water");
	//mWater->setMaterialName("Examples/FresnelReflectionRefraction");
	mWater->setMaterial(mat);
	mSceneMgr->getRootSceneNode()->attachObject(mWater);
	//mat->setCullingMode(CULL_NONE);//Ogre::CullingMode::CULL_NONE);

	Entity * ent;
	ent = mSceneMgr->createEntity("UpperBath", "RomanBathUpper.mesh" );
	mSceneMgr->getRootSceneNode()->attachObject(ent);        
	mSurfaceEnts.push_back(ent);
	ent = mSceneMgr->createEntity("Columns", "columns.mesh");
	mSceneMgr->getRootSceneNode()->attachObject(ent);        
	mSurfaceEnts.push_back(ent);
	ent = mSceneMgr->createEntity("Head", "ogrehead.mesh");
	ent->setMaterialName("RomanBath/OgreStone");
	mSurfaceEnts.push_back(ent);
	SceneNode* headNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	headNode->setPosition(-350, 55, 130);
	headNode->yaw(Degree(90));
	headNode->attachObject(ent);
	ent = mSceneMgr->createEntity("LowerBath", "RomanBathLower.mesh");
	mSceneMgr->getRootSceneNode()->attachObject(ent);
	mSubmergedEnts.push_back(ent);

	mFishNodes.resize(NUM_FISH);
	mFishAnimStates.resize(NUM_FISH);
	mFishSplines.resize(NUM_FISH);

	for (unsigned int i=0;i<NUM_FISH;i++)
	{
		Entity * ent = mSceneMgr->createEntity("Fish" + StringConverter::toString(i + 1),"fish.mesh");
		mSubmergedEnts.push_back(ent);
		mFishNodes[i] = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		mFishNodes[i]->setScale(Vector3::UNIT_SCALE * FISH_SCALE);
		mFishNodes[i]->attachObject(ent);

		// enable and save the swim animation state
		mFishAnimStates[i] = ent->getAnimationState("swim");
		mFishAnimStates[i]->setEnabled(true);

		mFishSplines[i].setAutoCalculate(false);

		for (unsigned int j=0;j<NUM_FISH_WAYPOINTS;j++)
		{
			Vector3 pos(Math::SymmetricRandom() * 270,-10,Math::SymmetricRandom() * 700);

			if (j > 0)
			{
				const  Vector3 & lastPos = mFishSplines[i].getPoint(j - 1);
				Vector3 delta = pos - lastPos;
				if (delta.length() > 750)
				{
					pos = lastPos + delta.normalisedCopy() * 750;
				}
			}
			mFishSplines[i].addPoint(pos);
		}
		mFishSplines[i].addPoint(mFishSplines[i].getPoint(0));
		mFishSplines[i].recalcTangents();
	}
	mFishAnimTime = 0;
}

void OgreSample11App::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
	mFishAnimTime += evt.timeSinceLastFrame;
	while (mFishAnimTime >= FISH_PATH_LENGTH)
	{
		mFishAnimTime = FISH_PATH_LENGTH;
	}

	for (unsigned int i=0;i<NUM_FISH;i++)
	{
		mFishAnimStates[i]->addTime(evt.timeSinceLastFrame * 2);

		Vector3 lastPos = mFishNodes[i]->getPosition();
		mFishNodes[i]->setPosition(mFishSplines[i].interpolate(mFishAnimTime / FISH_PATH_LENGTH));
		mFishNodes[i]->setDirection(mFishNodes[i]->getPosition()- lastPos,Node::TS_PARENT,Vector3::NEGATIVE_UNIT_X);
		mFishNodes[i]->setFixedYawAxis(true);
	}
}

void OgreSample11App::preRenderTargetUpdate(const Ogre::RenderTargetEvent &evt)
{
	mWater->setVisible(false);

	if (evt.source == mReflectionTarget)
	{
		mCameraMan->getCamera()->enableReflection(mWaterPlane);
		for (std::vector<Entity *>::iterator i = mSubmergedEnts.begin();i != mSubmergedEnts.end();i++)
		{
			(*i)->setVisible(false);
		}
	}
	else
	{
		for (std::vector<Entity *>::iterator i = mSurfaceEnts.begin();i != mSurfaceEnts.end();i++)
		{
			(*i)->setVisible(false);
		}
	}
}

void OgreSample11App::postRenderTargetUpdate(const RenderTargetEvent& evt)
{
	mWater->setVisible(true);  // unhide the water

	if (evt.source == mReflectionTarget)  // for reflection, turn off camera reflection and unhide submerged entities
	{
		mCameraMan->getCamera()->disableReflection();
		for (std::vector<Entity*>::iterator i = mSubmergedEnts.begin(); i != mSubmergedEnts.end(); i++)
			(*i)->setVisible(true);
	}
	else  // for refraction, unhide surface entities
	{
		for (std::vector<Entity*>::iterator i = mSurfaceEnts.begin(); i != mSurfaceEnts.end(); i++)
			(*i)->setVisible(true);
	}
}
