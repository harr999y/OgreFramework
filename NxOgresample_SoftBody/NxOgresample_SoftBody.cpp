#include "NxOgresample_SoftBody.h"

void NxOgreSample_SoftBodyApp::createScene()
{
	setupPhysX();
	setupScene();
}

void NxOgreSample_SoftBodyApp::setupPhysX()
{
	using namespace NxOgre;
	mWorld = NxOgre::World::createWorld();
	NxOgre::ResourceSystem::getSingleton()->openProtocol(new Critter::OgreResourceProtocol());
	NxOgre::SceneDescription sceneDes;
	sceneDes.mGravity = Vec3(0,0,0);//NxOgre::Constants::MEAN_EARTH_GRAVITY;//
	//sceneDes.mUseHardware = true;
	sceneDes.mUseHardware = false;
	sceneDes.mWorkerThreadsCount = 2;
	sceneDes.mBackgroundThreadsCount = 2;
	sceneDes.mFlags |= NxOgre::SceneFlags::EnableMultithread | NxOgre::SceneFlags::FluidPerformanceHint | NxOgre::SceneFlags::AlternativeFluidTriangleCollision | NxOgre::SceneFlags::MultithreadedForcefield;
	mScene = mWorld->createScene(sceneDes);
	mScene->getMaterial(0)->setAll(0.1f,0.9f,0.5f);
	//mScene->createSceneGeometry(NxOgre::PlaneGeometryDescription());
	mRenderSystem = new Critter::RenderSystem(mScene,mSceneMgr);

	mRenderSystem->setVisualisationMode(NxOgre::Enums::VisualDebugger_ShowAll);

	mWorld->getRemoteDebugger()->connect();
}

void NxOgreSample_SoftBodyApp::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
	mWorld->advance(evt.timeSinceLastFrame);

	if (mSelectedVertex != -1)
	{
		Ogre::Vector2 point(CEGUI::MouseCursor::getSingleton().getPosition().d_x/mWindow->getWidth(),CEGUI::MouseCursor::getSingleton().getPosition().d_y/mWindow->getHeight());
		Ogre::Ray ogreRay = mCameraMan->getCamera()->getCameraToViewportRay(point.x,point.y);
		NxOgre::Ray nxRay;
		nxRay.mDirection.from(ogreRay.getDirection());
		nxRay.mOrigin.from(ogreRay.getOrigin());

		NxVec3 vec3 = (nxRay.mOrigin + 45 * nxRay.mDirection).as<NxVec3>();

		mSoftBody->getPhysXSoftBody()->attachVertexToGlobalPosition(mSelectedVertex,vec3);
	}	
}

using namespace NxOgre;

void NxOgreSample_SoftBodyApp::setupScene()
{
	SoftBodyDescription softDesc;
	softDesc.mParticleRadius = .2f;
	softDesc.mVolumeStiffness = 0.8f;
	softDesc.mStretchingStiffness = 1;
	softDesc.mFriction = 1;
	softDesc.mSolverIterations = 5;
	softDesc.mGlobalPose = Vec3(0,0,0);
	softDesc.mAttachmentResponseCoefficient = .1f;
	softDesc.mFlags |= Enums::SoftBodyFlags_Gravity | Enums::SoftBodyFlags_TwoWayCollisions | Enums::SoftBodyFlags_VolumeConservation;// | Enums::SoftBodyFlags_Tearable; Enums::SoftBodyFlags_Hardware |
	//softDesc.mMesh->mType = Enums::MeshType_SoftBody;
	//softDesc.mMesh = MeshGenerator::makeBox(Vec3(1,1,1),Enums::MeshType_SoftBody,"file://../Media/PhysX/softBody.nxs");//MeshManager::getSingleton()->load("file://../Media/PhysX/barrel.xcl");//MeshGenerator::makeClothFromFlower("")
	softDesc.mMesh = MeshGenerator::makeSoftBodyFromTet(softDesc.mValidBounds,"file://../Media/PhysX/softBody.nxs","file://../Media/PhysX/palm.tet");

	mSoftBody = mRenderSystem->createSoftBody(softDesc,"file://../Media/PhysX/palm.flower");

	mCameraMan->setTopSpeed(50);

	//Ogre::Entity * entity = mSceneMgr->createEntity("cow.mesh");
	//mSceneMgr->getRootSceneNode()->createChildSceneNode("",Ogre::Vector3(0,20,0))->attachObject(entity);
}

void NxOgreSample_SoftBodyApp::createFrameListener()
{
	NxOgreSample_SoftBodyListener * listener = new NxOgreSample_SoftBodyListener(this,mRoot,mSceneMgr,mWindow,mCameraMan,mOgreRenderer);
	mListener = listener;
	mListener->initialise();
}

bool NxOgreSample_SoftBodyListener::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	mApp->mousePressed(arg,id);
	return CEGUIBaseListener::mousePressed(arg,id);
}

bool NxOgreSample_SoftBodyListener::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	mApp->mouseReleased(arg,id);
	return CEGUIBaseListener::mouseReleased(arg,id);
}

bool NxOgreSample_SoftBodyListener::keyPressed(const OIS::KeyEvent &arg)
{
	mApp->keyPressed(arg);
	return CEGUIBaseListener::keyPressed(arg);
}

bool NxOgreSample_SoftBodyApp::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (arg.state.buttonDown(OIS::MB_Left))
	{
		if (mSelectedVertex != -1)
		{
			mSoftBody->getPhysXSoftBody()->freeVertex(mSelectedVertex);
			mSelectedVertex = -1;
		}

		Ogre::Vector2 point(arg.state.X.abs,arg.state.Y.abs);
		Ogre::Ray ogreRay = mCameraMan->getCamera()->getCameraToViewportRay(point.x/arg.state.width,point.y/arg.state.height);
		NxRay nxRay(NxVec3(ogreRay.getOrigin().x,ogreRay.getOrigin().y,ogreRay.getOrigin().z),NxVec3(ogreRay.getDirection().x,ogreRay.getDirection().y,ogreRay.getDirection().z));

		NxVec3 hit;
		
		if (!mSoftBody->getPhysXSoftBody()->raycast(nxRay,hit,mSelectedVertex))
			mSelectedVertex = -1;
	}

	return true;
}

bool NxOgreSample_SoftBodyApp::mouseReleased(const OIS::MouseEvent &arg, OIS::MouseButtonID id)
{
	if (mSelectedVertex != -1)
	{
		mSoftBody->getPhysXSoftBody()->freeVertex(mSelectedVertex);
		mSelectedVertex = -1;
	}

	return true;
}

bool NxOgreSample_SoftBodyApp::keyPressed(const OIS::KeyEvent &arg)
{
	return true;
}