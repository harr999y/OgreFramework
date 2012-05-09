#include "FiveApp.h"

using namespace Ogre;

void FiveApp::createScene()
{
	mSceneMgr->setAmbientLight(ColourValue(0.2f,0.2f,0.2f));

	Light * l = mSceneMgr->createLight("MainLight");
	l->setPosition(20.f,80.f,50.f);

	mCameraMan->getCamera()->setPosition(60, 200, 70);
	mCameraMan->getCamera()->lookAt(0,0,0);

	Entity * ent;
	
	for(int i=0;i<6;i++)
	{
		SceneNode* headNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		ent = mSceneMgr->createEntity("head" + StringConverter::toString(i),"ogrehead.mesh");
		headNode->attachObject(ent);
		Radian angle(i*Math::TWO_PI/6);
		headNode->setPosition(75*Math::Cos(angle),0,75*Math::Sin(angle));
	}

	for(unsigned int i = 0;i<ent->getNumSubEntities();i++)
		makeMaterialReceiveDecal(ent->getSubEntity(i)->getMaterialName());
}

void FiveApp::createProjector()
{
	mDecalFrustum = new Frustum();
	mProjectorNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("DecalProjectorNode");
	mProjectorNode->attachObject(mDecalFrustum);
	mProjectorNode->setPosition(0.f,5.f,0.f);

	mFilterFrustum = new Frustum();
	mFilterFrustum->setProjectionType(PT_ORTHOGRAPHIC);
	SceneNode *filterNode = mProjectorNode->createChildSceneNode("DecalFilterNode");
	filterNode->attachObject(mFilterFrustum);
	filterNode->setOrientation(Quaternion(Degree(90),Vector3::UNIT_Y));
}

void FiveApp::makeMaterialReceiveDecal(const String & matName)
{
	MaterialPtr mat = (MaterialPtr)MaterialManager::getSingleton().getByName(matName);
	Pass *pass = mat->getTechnique(0)->createPass();

	pass->setSceneBlending(SBT_TRANSPARENT_ALPHA);
	pass->setDepthBias(1);
	pass->setLightingEnabled(true);

	TextureUnitState *texState = pass->createTextureUnitState("decal.png");
	texState->setProjectiveTexturing(true,mDecalFrustum);
	texState->setTextureAddressingMode(TextureUnitState::TAM_CLAMP);
	texState->setTextureFiltering(FO_LINEAR,FO_LINEAR,FO_NONE);

	texState = pass->createTextureUnitState("decal_filter.png");
	texState->setProjectiveTexturing(true,mFilterFrustum);
	texState->setTextureAddressingMode(TextureUnitState::TAM_CLAMP);
	texState->setTextureFiltering(TFO_NONE);
}

void FiveApp::createFrameListener()
{
	createProjector();
	FiveListener * listener = new FiveListener(mRoot,mSceneMgr,mWindow,mCameraMan,mProjectorNode,mDecalFrustum);
	listener->initialise();
}

bool FiveListener::frameEnded(const Ogre::FrameEvent& evt)
{
	mAnim += evt.timeSinceLastFrame / 2 + 0.001f;
	if(mAnim >= 1)
		mAnim -= 1;
	Ogre::Radian r = Degree(15 + Math::Sin(mAnim * Math::TWO_PI) * 10);
	mDecalFrustum->setFOVy(r);
	mProjectorNode->rotate(Vector3::UNIT_Y, Degree((evt.timeSinceLastFrame + 0.0001f)* 10));
	return BaseListener::frameEnded(evt);
}