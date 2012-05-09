#include "OgreSample18.h"

String CUSTOM_ROCKWALL_MATERIAL("Ogre/DepthShadowmap/Receiver/RockWall");
String CUSTOM_CASTER_MATERIAL("Ogre/DepthShadowmap/Caster/Float");
String CUSTOM_RECEIVER_MATERIAL("Ogre/DepthShadowmap/Receiver/Float");
String CUSTOM_ATHENE_MATERIAL("Ogre/DepthShadowmap/Receiver/Athene");

String BASIC_ROCKWALL_MATERIAL("Examples/Rockwall");
String BASIC_ATHENE_MATERIAL("Examples/Athene/NormalMapped");

void OgreSample18App::createScene()
{
	mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);
	mCurrentShadowTechnique = SHADOWTYPE_STENCIL_ADDITIVE;

	// Set ambient light off
	mSceneMgr->setAmbientLight(ColourValue(0.0, 0.0, 0.0));

	// Fixed light, dim
	mSunLight = mSceneMgr->createLight("SunLight");
	mSunLight->setType(Light::LT_SPOTLIGHT);
	mSunLight->setPosition(1500,1750,1300);
	mSunLight->setSpotlightRange(Degree(30), Degree(50));
	Ogre::Vector3 dir;
	dir = -mSunLight->getPosition();
	dir.normalise();
	mSunLight->setDirection(dir);
	mSunLight->setDiffuseColour(0.35, 0.35, 0.38);
	mSunLight->setSpecularColour(0.9, 0.9, 1);

	// Point light, movable, reddish
	mLight = mSceneMgr->createLight("Light2");
	mLight->setDiffuseColour(mMinLightColour);
	mLight->setSpecularColour(1, 1, 1);
	mLight->setAttenuation(8000,1,0.0005,0);

	// Create light node
	mLightNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(
		"MovingLightNode");
	mLightNode->attachObject(mLight);
	// create billboard set
	BillboardSet* bbs = mSceneMgr->createBillboardSet("lightbbs", 1);
	bbs->setMaterialName("Examples/Flare");
	Billboard* bb = bbs->createBillboard(0,0,0,mMinLightColour);
	// attach
	mLightNode->attachObject(bbs);

	ControllerFunctionRealPtr func = ControllerFunctionRealPtr(new WaveformControllerFunction(Ogre::WFT_SINE,0.75f,0.5f));
	ControllerManager& contMgr = ControllerManager::getSingleton();
	ControllerValueRealPtr val = ControllerValueRealPtr(new LightWibbler(mLight,bb,mMinLightColour,mMaxLightColour,mMinFlareSize,mMaxFlareSize));
	mController = contMgr.createController(contMgr.getFrameTimeSource(),val,func);
	mLightNode->setPosition(Vector3(300,1750,-700));
	Animation* anim = mSceneMgr->createAnimation("LightTrack", 20);
	// Spline it for nice curves
	anim->setInterpolationMode(Animation::IM_SPLINE);
	// Create a track to animate the camera's node
	NodeAnimationTrack* track = anim->createNodeTrack(0, mLightNode);
	// Setup keyframes
	TransformKeyFrame* key = track->createNodeKeyFrame(0); // A startposition
	key->setTranslate(Vector3(300,750,-700));
	key = track->createNodeKeyFrame(2);//B
	key->setTranslate(Vector3(150,800,-250));
	key = track->createNodeKeyFrame(4);//C
	key->setTranslate(Vector3(-150,850,-100));
	key = track->createNodeKeyFrame(6);//D
	key->setTranslate(Vector3(-400,700,-200));
	key = track->createNodeKeyFrame(8);//E
	key->setTranslate(Vector3(-200,700,-400));
	key = track->createNodeKeyFrame(10);//F
	key->setTranslate(Vector3(-100,850,-200));
	key = track->createNodeKeyFrame(12);//G
	key->setTranslate(Vector3(-100,575,180));
	key = track->createNodeKeyFrame(14);//H
	key->setTranslate(Vector3(0,750,300));
	key = track->createNodeKeyFrame(16);//I
	key->setTranslate(Vector3(100,850,100));
	key = track->createNodeKeyFrame(18);//J
	key->setTranslate(Vector3(250,800,0));
	key = track->createNodeKeyFrame(20);//K == A
	key->setTranslate(Vector3(300,750,-700));
	// Create a new animation state to track this
	mAnimState = mSceneMgr->createAnimationState("LightTrack");
	mAnimState->setEnabled(true);

	mLightNode->setAutoTracking(true,mSceneMgr->getRootSceneNode());

	MeshPtr pAthene = MeshManager::getSingleton().load("athene.mesh", 
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	unsigned short src, dest;
	if (!pAthene->suggestTangentVectorBuildParams(VES_TANGENT, src, dest))
	{
		pAthene->buildTangentVectors(VES_TANGENT, src, dest);
	}

	SceneNode* node;
	node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	mAthene = mSceneMgr->createEntity( "athene", "athene.mesh" );
	mAthene->setMaterialName(BASIC_ATHENE_MATERIAL);
	node->attachObject( mAthene );
	node->yaw(Degree(90));

	Entity* pEnt;
	// Columns
	for (int x = -2; x <= 2; ++x)
	{
		for (int z = -2; z <= 2; ++z)
		{
			if (x != 0 || z != 0)
			{
				StringUtil::StrStreamType str;
				str << "col" << x << "_" << z;
				node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
				pEnt = mSceneMgr->createEntity( str.str(), "column.mesh" );
				pEnt->setMaterialName(BASIC_ROCKWALL_MATERIAL);
				pColumns.push_back(pEnt);
				node->attachObject( pEnt );
				node->translate(x*300,0, z*300);
			}
		}
	}

	// Skybox
	mSceneMgr->setSkyBox(true, "Examples/StormySkyBox");

	// Floor plane (use POSM plane def)
	mPlane = new MovablePlane("*mPlane");
	mPlane->normal = Vector3::UNIT_Y;
	mPlane->d = 107;
	MeshManager::getSingleton().createPlane("Myplane",
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, *mPlane,
		1500,1500,50,50,true,1,5,5,Vector3::UNIT_Z);
	pPlaneEnt = mSceneMgr->createEntity( "plane", "Myplane" );
	pPlaneEnt->setMaterialName(BASIC_ROCKWALL_MATERIAL);
	pPlaneEnt->setCastShadows(false);
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(pPlaneEnt);

	if (mRoot->getRenderSystem()->getCapabilities()->hasCapability(RSC_HWRENDER_TO_TEXTURE))
	{
		mSceneMgr->setShadowTextureSettings(1024,2);
	}
	else
	{
		mSceneMgr->setShadowTextureSettings(512,2);
	}

	mSceneMgr->setShadowColour(ColourValue(0.5f,0.5f,0.5f));
	//handles;

	resetMaterials();
	materialChanged(0);
	configureLights(ShadowTechnique::SHADOWTYPE_STENCIL_MODULATIVE);
}

void OgreSample18App::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	if (mAnimState)
	{
		mAnimState->addTime(evt.timeSinceLastFrame);
	}
}

void OgreSample18App::destroyScene()
{
	ControllerManager::getSingleton().destroyController(mController);
}

void OgreSample18App::changeShadowTechnique(Ogre::ShadowTechnique newTech)
{
	mSceneMgr->setShadowTechnique(newTech);
	configureLights(newTech);
	mCurrentShadowTechnique = newTech;
	configureLights(newTech);
}

void OgreSample18App::configureLights(Ogre::ShadowTechnique newTech)
{
	Vector3 dir;
	switch (newTech)
	{
	case SHADOWTYPE_STENCIL_ADDITIVE:
		mSunLight->setCastShadows(true);
		mLight->setType(Light::LT_POINT);
		mLight->setCastShadows(true);
		mLight->setDiffuseColour(mMinLightColour);
		mLight->setSpecularColour(1,1,1);
		mLight->setAttenuation(8000,1,0.0005f,0);
		break;
	case SHADOWTYPE_STENCIL_MODULATIVE:
		mSunLight->setCastShadows(false);
		mLight->setType(Light::LT_POINT);
		mLight->setDiffuseColour(mMinLightColour);
		mLight->setSpecularColour(1,1,1);
		mLight->setAttenuation(8000,1,0.0005f,0);
		break;
	case SHADOWTYPE_TEXTURE_MODULATIVE:
	case SHADOWTYPE_TEXTURE_ADDITIVE:
		mSunLight->setCastShadows(true);
		mLight->setType(Light::LT_SPOTLIGHT);
		mLight->setDirection(Ogre::Vector3::NEGATIVE_UNIT_Z);
		mLight->setCastShadows(true);
		mLight->setDiffuseColour(mMinLightColour);
		mLight->setSpecularColour(1, 1, 1);
		mLight->setAttenuation(8000,1,0.0005,0);
		mLight->setSpotlightRange(Degree(80),Degree(90));
		break;
	default:
		break;		
	}
}

void OgreSample18App::resetMaterials()
{
	pPlaneEnt->setMaterialName(BASIC_ROCKWALL_MATERIAL);
	mAthene->setMaterialName(BASIC_ATHENE_MATERIAL);
	for (vector<Entity*>::type::iterator i = pColumns.begin();i != pColumns.end();++i)
	{
		(*i)->setMaterialName(BASIC_ROCKWALL_MATERIAL);
	}

	mCustomRockwallVparams.setNull();
	mCustomRockwallFparams.setNull();
	mCustomAtheneVparams.setNull();
	mCustomAtheneFparams.setNull();
}

bool OgreSample18App::handleItemSelected(const CEGUI::EventArgs& args)
{
	using namespace CEGUI;
	/*Combobox* select = static_cast<Combobox*>(static_cast<const CEGUI::WindowEventArgs&>(args).window);
	if (select->getName() == "OgreSample18/select1")
	{
		if (select->getSelectedItem()->getText() == "stencile")
		{}
	}*/
	return true;
}

void OgreSample18App::createGUI()
{
	using namespace CEGUI;
	/*mRootWindow->addChildWindow(WindowManager::getSingleton().loadWindowLayout("OgreSample18.layout"));

	Combobox* cb = WindowManager::getSingleton().getWindow("OgreSample18/select1");
	cb->addItem(new ListboxTextItem("Stencile"));
	cb->addItem(new ListboxTextItem("Texture"));*/
}

void OgreSample18App::materialChanged(int index)
{
	MaterialPtr themat;
	if ((ShadowMaterial)index != mCurrentMaterial)
	{
		switch((ShadowMaterial)index)
		{
		case MAT_STANDARD:
			mSceneMgr->setShadowTexturePixelFormat(PF_X8R8G8B8);
			mSceneMgr->setShadowTextureCasterMaterial(StringUtil::BLANK);
			mSceneMgr->setShadowTextureReceiverMaterial(StringUtil::BLANK);
			mSceneMgr->setShadowTextureSelfShadow(false);	

			resetMaterials();

			break;
		case MAT_DEPTH_FLOAT:
			mSceneMgr->setShadowTexturePixelFormat(PF_FLOAT32_R);
			mSceneMgr->setShadowTextureCasterMaterial(CUSTOM_CASTER_MATERIAL);
			mSceneMgr->setShadowTextureReceiverMaterial(CUSTOM_RECEIVER_MATERIAL);
			mSceneMgr->setShadowTextureSelfShadow(true);	
			// Sort out base materials
			pPlaneEnt->setMaterialName(CUSTOM_ROCKWALL_MATERIAL);
			mAthene->setMaterialName(CUSTOM_ATHENE_MATERIAL);
			for (vector<Entity*>::type::iterator i = pColumns.begin();
				i != pColumns.end(); ++i)
			{
				(*i)->setMaterialName(CUSTOM_ROCKWALL_MATERIAL);
			}

			themat = MaterialManager::getSingleton().getByName(CUSTOM_ROCKWALL_MATERIAL);
			mCustomRockwallVparams = themat->getTechnique(0)->getPass(1)->getShadowReceiverVertexProgramParameters();
			mCustomRockwallFparams = themat->getTechnique(0)->getPass(1)->getShadowReceiverFragmentProgramParameters();
			themat = MaterialManager::getSingleton().getByName(CUSTOM_ATHENE_MATERIAL);
			mCustomAtheneVparams = themat->getTechnique(0)->getPass(1)->getShadowReceiverVertexProgramParameters();
			mCustomAtheneFparams = themat->getTechnique(0)->getPass(1)->getShadowReceiverFragmentProgramParameters();

			break;
		case MAT_DEPTH_FLOAT_PCF:
			mSceneMgr->setShadowTexturePixelFormat(PF_FLOAT32_R);
			mSceneMgr->setShadowTextureCasterMaterial(CUSTOM_CASTER_MATERIAL);
			mSceneMgr->setShadowTextureReceiverMaterial(CUSTOM_RECEIVER_MATERIAL + "/PCF");
			mSceneMgr->setShadowTextureSelfShadow(true);	
			// Sort out base materials
			pPlaneEnt->setMaterialName(CUSTOM_ROCKWALL_MATERIAL + "/PCF");
			mAthene->setMaterialName(CUSTOM_ATHENE_MATERIAL + "/PCF");
			for (vector<Entity*>::type::iterator i = pColumns.begin();
				i != pColumns.end(); ++i)
			{
				(*i)->setMaterialName(CUSTOM_ROCKWALL_MATERIAL + "/PCF");
			}

			themat = MaterialManager::getSingleton().getByName(CUSTOM_ROCKWALL_MATERIAL + "/PCF");
			mCustomRockwallVparams = themat->getTechnique(0)->getPass(1)->getShadowReceiverVertexProgramParameters();
			mCustomRockwallFparams = themat->getTechnique(0)->getPass(1)->getShadowReceiverFragmentProgramParameters();
			themat = MaterialManager::getSingleton().getByName(CUSTOM_ATHENE_MATERIAL + "/PCF");
			mCustomAtheneVparams = themat->getTechnique(0)->getPass(1)->getShadowReceiverVertexProgramParameters();
			mCustomAtheneFparams = themat->getTechnique(0)->getPass(1)->getShadowReceiverFragmentProgramParameters();
			break;
		};
		mCurrentMaterial = (ShadowMaterial)index;
	}
}	
