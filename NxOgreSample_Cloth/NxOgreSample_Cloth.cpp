#include "NxOgreSample_Cloth.h"

void NxOgreSample_ClothApp::setupPhysX()
{
	mWorld = NxOgre::World::createWorld();
	NxOgre::ResourceSystem::getSingleton()->openProtocol(new Critter::OgreResourceProtocol());
	mWorld->getRemoteDebugger()->connect();
	NxOgre::SceneDescription sceneDes;
	sceneDes.mGravity = NxOgre::Constants::MEAN_EARTH_GRAVITY;
	sceneDes.mUseHardware = false;
	mScene = mWorld->createScene(sceneDes);
	mScene->getMaterial(0)->setAll(0.1f,0.9f,0.5f);
	mMeshManager = NxOgre::MeshManager::getSingleton();
	mScene->createSceneGeometry(NxOgre::PlaneGeometryDescription());
	mRenderSystem = new Critter::RenderSystem(mScene,mSceneMgr);
	mRenderSystem->setVisualisationMode(NxOgre::Enums::VisualDebugger_ShowAll);

	makeBox(NxOgre::Vec3(0,0.5f,0.5f));
	makeCloth(NxOgre::Vec3(0,4,0));

	mClothSelectedVertexMarker = mRenderSystem->createSceneNodeEntityPair("cube.mesh",Ogre::Vector3(0,-10,0));
	mClothSelectedVertexMarker->setScale(0.001f,0.001f,0.001f);
	mClothSelectedVertexMarker->roll(Ogre::Degree(45));   //need test  //test Done!
	mClothSelectedVertexMarker->yaw(Ogre::Degree(45));
	mClothSelectedVertexMarker->pitch(Ogre::Degree(45));
}

void NxOgreSample_ClothApp::destroyScene()
{
	NxOgre::World::destroyWorld();
}

Critter::Body * NxOgreSample_ClothApp::makeBox(const NxOgre::Matrix44& globalPose,const NxOgre::Vec3& initV)
{
	Critter::BodyDescription bodyDescription;
	bodyDescription.mMass = 40.0f;
	bodyDescription.mLinearVelocity = initV;
	Critter::Body * box = mRenderSystem->createBody(NxOgre::BoxDescription(1,1,1),globalPose,Ogre::String("cube.1m.mesh"),bodyDescription);
	return box;
}

void NxOgreSample_ClothApp::makeCloth(const NxOgre::Vec3 &barPosition)
{
	using namespace NxOgre;
	Vec3 pos = barPosition;
	Vec2 clothSize(8,4);
	pos.x -= clothSize.x * 0.5f;  //need test	//test Done!
	mClothMesh = NULL;
	if (mClothMesh == 0)
	{
		mClothMesh = MeshGenerator::makePlane(clothSize,0.1f,NxOgre::Enums::MeshType_Cloth,"file://rug.xcl");
	}
	NxOgre::Vec3 holderPos = pos;
	holderPos.x += clothSize.x * 0.5f;  //need test		//test Done!
	holderPos.y += 0.05f;
	holderPos.z -= 0.05f;
	SceneGeometry * geom = mScene->createSceneGeometry(BoxDescription(10,0.1f,0.1f),holderPos);//need test 	//test Done!
	Ogre::SceneNode * geomNode = mRenderSystem->createSceneNodeEntityPair("cube.mesh",holderPos);//need test	//test Done!
	geomNode->scale(0.1f,0.001f,0.001f);

	ClothDescription desc;
	desc.mMesh = mClothMesh;
	desc.mThickness = 0.2f;
	desc.mFriction = 0.5f;
	//desc.mFlags |= Enums::ClothFlags_Tearable;//need test  		//test Done!  //How to make it Tearable?
	desc.mGlobalPose.set(pos);
	//desc.mFlags |= NxOgre::Enums::ClothFlags_Hardware;

	mCloth = mRenderSystem->createCloth(desc,"wales");
	mCloth->attachToShape((*geom->getShapes().begin()),Enums::ClothAttachmentFlags_Twoway); //need test  //test Done! 
}

bool NxOgreSample_ClothApp::mousePressed(const OIS::MouseEvent &arg,OIS::MouseButtonID id)
{
	if (arg.state.buttonDown(OIS::MB_Left))
	{
		if (mClothSelectedVertex != -1)
		{
			mCloth->freeVertex(mClothSelectedVertex);
			mClothSelectedVertex = -1;
			mClothSelectedVertexMarker->setPosition(0,-1,0); //need test	  //test Done! 
		}

		Ogre::Vector2 point(arg.state.X.abs,arg.state.Y.abs);
		Ogre::Ray ogreRay = mCameraMan->getCamera()->getCameraToViewportRay(point.x/arg.state.width,point.y/arg.state.height);
		NxOgre::Ray nxRay;
		nxRay.mDirection.from(ogreRay.getDirection());
		nxRay.mOrigin.from(ogreRay.getOrigin());
		NxOgre::ClothRaycastHit result = mCloth->raycast(nxRay);

		if (result.mDidHit)
		{
			mClothSelectedVertex = result.mVertexId;
		}
		else
			mClothSelectedVertex = -1;
	}

	if (arg.state.buttonDown(OIS::MB_Middle))
	{
		if (mClothSelectedMethod == VM_ADDFORCE)
		{
			mClothSelectedMethod = VM_ATTACHVERTEX;
		}
		else if (mClothSelectedMethod == VM_ATTACHVERTEX)
		{
			mClothSelectedMethod = VM_SETPOSITION;
		}
		else if (mClothSelectedMethod == VM_SETPOSITION)
		{
			mClothSelectedMethod = VM_ADDFORCE;
		}
	}
	return true;
}

bool NxOgreSample_ClothListener::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	mApp->mousePressed(arg,id);
	return CEGUIBaseListener::mousePressed(arg,id);
}

bool NxOgreSample_ClothApp::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (mClothSelectedVertex != -1)
	{
		mCloth->freeVertex(mClothSelectedVertex);
		mClothSelectedVertex = -1;
		mClothSelectedVertexMarker->setPosition(0,-1,0);
	}
	return true;
}

bool NxOgreSample_ClothListener::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	mApp->mouseReleased(arg,id);
	return CEGUIBaseListener::mouseReleased(arg,id);
}

void NxOgreSample_ClothApp::createFrameListener()
{
	NxOgreSample_ClothListener * listener = new NxOgreSample_ClothListener(this,mRoot,mSceneMgr,mWindow,mCameraMan,mOgreRenderer);
	mListener = listener;
	mListener->initialise();
}

void NxOgreSample_ClothApp::createScene()
{
	using namespace Ogre;
	Ogre::ColourValue background = Ogre::ColourValue(16.f/255.f, 16.f/255.f, 16.f/255.f);
	mCameraMan->getCamera()->getViewport()->setBackgroundColour(background);
	mSceneMgr->setFog(Ogre::FOG_EXP, background, 0.001f, 800, 1000);

	// set shadow properties
	mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE);
	mSceneMgr->setShadowColour(Ogre::ColourValue(0.5, 0.5, 0.5));
	mSceneMgr->setShadowTextureSize(1024);
	mSceneMgr->setShadowTextureCount(1);

	// create a floor mesh resource
	Ogre::MeshManager::getSingleton().createPlane("floor", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Ogre::Plane(Ogre::Vector3::UNIT_Y, 0), 1000, 1000, 1,1 , true, 1, 1, 1, Ogre::Vector3::UNIT_Z);

	// create a floor entity, give it a material, and place it at the origin
	Entity * floor = mSceneMgr->createEntity("Floor", "floor");
	floor->setMaterialName("ground-from-nxogre.org");
	floor->setCastShadows(false);
	mSceneMgr->getRootSceneNode()->attachObject(floor);

	// use a small amount of ambient lighting
	mSceneMgr->setAmbientLight(ColourValue(0.3f, 0.3f, 0.3f));

	// add a bright light above the scene
	Light* light = mSceneMgr->createLight();
	light->setType(Light::LT_POINT);
	light->setPosition(-10, 40, 20);
	light->setSpecularColour(ColourValue::White);

	mCameraMan->getCamera()->setPosition(10,10,10);
	mCameraMan->getCamera()->lookAt(0,0,0);
	mCameraMan->getCamera()->setNearClipDistance(0.02f);
	mCameraMan->getCamera()->setFarClipDistance(1000.0f);
	mCameraMan->setTopSpeed(107.5);

	setupPhysX();

	mClothSelectedMethod = VM_ADDFORCE;
}

void NxOgreSample_ClothApp::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
	mWorld->advance(evt.timeSinceLastFrame);

	Ogre::Vector2 point(CEGUI::MouseCursor::getSingleton().getPosition().d_x/mWindow->getWidth(),CEGUI::MouseCursor::getSingleton().getPosition().d_y/mWindow->getHeight());
	Ogre::Ray ogreRay = mCameraMan->getCamera()->getCameraToViewportRay(point.x,point.y);
	NxOgre::Ray nxRay;
	nxRay.mDirection.from(ogreRay.getDirection());
	nxRay.mOrigin.from(ogreRay.getOrigin());
	NxOgre::RaycastHit result = mScene->raycastClosestShape(nxRay,NxOgre::Enums::ShapesType_All);

	if (result.mDistance > 0 && result.mDistance < 100)
	{
		if (mClothSelectedMethod == VM_ATTACHVERTEX)
		{
			mCloth->attachVertexToGlobalPosition(mClothSelectedVertex,result.mWorldImpact);
		}
		else if (mClothSelectedMethod == VM_SETPOSITION)
		{
			mCloth->setPosition(result.mWorldImpact,mClothSelectedVertex);
		}
		else if (mClothSelectedMethod == VM_ADDFORCE)
		{
			NxOgre::Vec3 force = mCloth->getPosition(mClothSelectedVertex) - result.mWorldImpact;
			force.normalise();
			mCloth->addForceAtVertex(-force * 100,mClothSelectedVertex);
			//mCloth->tearVertex(mClothSelectedVertex,force);  //need test
		}

		mClothSelectedVertexMarker->setPosition(mCloth->getPosition(mClothSelectedVertex).as<Ogre::Vector3>());   //need test		  //test Done! 
	}
}