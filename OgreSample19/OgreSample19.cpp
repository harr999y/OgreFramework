#include "OgreSample19.h"

void OgreSample19App::createScene()
{
	mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);
	mSceneMgr->setShadowTextureSize(512);
	mSceneMgr->setShadowColour(ColourValue(0.6f, 0.6f, 0.6f));
	mSceneMgr->setShadowTextureCount(2);

	// add a little ambient lighting
	mSceneMgr->setAmbientLight(ColourValue(0.5f, 0.5f, 0.5f));

	SceneNode* lightsBbsNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	BillboardSet* bbs;

	// Create billboard set for lights .
	bbs = mSceneMgr->createBillboardSet();
	bbs->setMaterialName("Examples/Flare");
	lightsBbsNode->attachObject(bbs);

	// add a blue spotlight
	Light* l = mSceneMgr->createLight();
	Vector3 dir;
	l->setType(Light::LT_SPOTLIGHT);
	l->setPosition(-40, 180, -10);
	dir = -l->getPosition();
	dir.normalise();
	l->setDirection(dir);
	l->setDiffuseColour(0.0f, 0.0f, 0.5f);
	bbs->createBillboard(l->getPosition())->setColour(l->getDiffuseColour());

	// add a green spotlight.
	l = mSceneMgr->createLight();
	l->setType(Light::LT_SPOTLIGHT);
	l->setPosition(0, 150, -100);
	dir = -l->getPosition();
	dir.normalise();
	l->setDirection(dir);
	l->setDiffuseColour(0.0f, 0.5f, 0.0f);		
	bbs->createBillboard(l->getPosition())->setColour(l->getDiffuseColour());

	// create a floor mesh resource
	MeshManager::getSingleton().createPlane("floor", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		Plane(Vector3::UNIT_Y, -1), 250, 250, 25, 25, true, 1, 15, 15, Vector3::UNIT_Z);

	// add a floor to our scene using the floor mesh we created
	Entity* floor = mSceneMgr->createEntity("Floor", "floor");
	floor->setMaterialName("Examples/Rockwall");
	floor->setCastShadows(false);
	mSceneMgr->getRootSceneNode()->attachObject(floor);

	tweakSneakAnim();

	SceneNode* sn = NULL;
	Entity * ent = NULL;
	AnimationState* as = NULL;

	for (unsigned int i = 0; i < NUM_MODELS; i++)
	{
		// create scene nodes for the models at regular angular intervals
		sn = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		sn->yaw(Radian(Math::TWO_PI * (float)i / (float)NUM_MODELS));
		sn->translate(0, 0, -20, Node::TS_LOCAL);
		mModelNodes.push_back(sn);

		// create and attach a jaiqua entity
		ent = mSceneMgr->createEntity("Jaiqua" + StringConverter::toString(i + 1), "jaiqua.mesh");
		sn->attachObject(ent);

		// enable the entity's sneaking animation at a random speed and loop it manually since translation is involved
		as = ent->getAnimationState("Sneak");
		as->setEnabled(true);
		as->setLoop(false);
		mAnimSpeeds.push_back(Math::RangeRandom(0.5, 1.5));
		mAnimStates.push_back(as);
	}

	StringVector names;
	names.push_back("Skinning");
	String value = "Software";

	Pass * pass = ent->getSubEntity(0)->getMaterial()->getBestTechnique()->getPass(0);
	if (pass->hasVertexProgram() && pass->getVertexProgram()->isSkeletalAnimationIncluded())
	{
		value = "Hardware";
	}
}

void OgreSample19App::tweakSneakAnim()
{
	SkeletonPtr skel = SkeletonManager::getSingleton().load("jaiqua.skeleton",ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	Animation * anim = skel->getAnimation("Sneak");
	Animation::NodeTrackIterator tracks = anim->getNodeTrackIterator();
	
	while(tracks.hasMoreElements())
	{
		NodeAnimationTrack * track = tracks.getNext();
		TransformKeyFrame oldKf(0,0);
		track->getInterpolatedKeyFrame(ANIM_CHOP,&oldKf);
		while (track->getKeyFrame(track->getNumKeyFrames()-1)->getTime() >= ANIM_CHOP - 0.3f)
		{
			track->removeKeyFrame(track->getNumKeyFrames()-1);
		}

		TransformKeyFrame * newKf = track->createNodeKeyFrame(ANIM_CHOP);
		TransformKeyFrame * startKf = track->getNodeKeyFrame(0);

		Bone * bone = skel->getBone(track->getHandle());

		if (bone->getName() == "Spineroot")
		{
			mSneakStartPos = startKf->getTranslate() + bone->getInitialPosition();
			mSneakEndPos = oldKf.getTranslate() + bone->getInitialPosition();
			mSneakStartPos.y = mSneakEndPos.y;
			newKf->setTranslate(oldKf.getTranslate());
			newKf->setRotation(oldKf.getRotation());
			newKf->setScale(oldKf.getScale());
		}
		else
		{
			newKf->setTranslate(startKf->getTranslate());
			newKf->setRotation(startKf->getRotation());
			newKf->setScale(startKf->getScale());
		}
	}
}

void OgreSample19App::destroyScene()
{
	mModelNodes.clear();
	mAnimStates.clear();
	mAnimSpeeds.clear();
	MeshManager::getSingleton().remove("floor");
}

void OgreSample19App::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	for(unsigned int i=0;i<NUM_MODELS;i++)
	{
		mAnimStates[i]->addTime(mAnimSpeeds[i] * evt.timeSinceLastFrame);
		if(mAnimStates[i]->getTimePosition() >= ANIM_CHOP)
		{
			Quaternion rot(Degree(-60),Ogre::Vector3::UNIT_Y);
			Vector3 currEnd = mModelNodes[i]->getOrientation() * mSneakEndPos + mModelNodes[i]->getPosition();
			Vector3 offset = rot * mModelNodes[i]->getOrientation() * -mSneakStartPos;

			mModelNodes[i]->setPosition(currEnd + offset);
			mModelNodes[i]->rotate(rot);
			mAnimStates[i]->setTimePosition(0);
		}
	}
}