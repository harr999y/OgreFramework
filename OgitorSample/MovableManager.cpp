#include "MovableManager.h"
#include "xml_reader.h"
#include "OgreString.h"
#include "OgreStringConverter.h"
#include "OgreTerrainGroup.h"

//////////////////////////////////////////////////////////////////////////
static const Ogre::String ConfigFileName("../config/MovableTrackAndAnims.cfg");
static const Ogre::String NodeName("Movable");

void MovableManager::initialize(const Ogre::SceneManager *sceneMgr, Ogre::TerrainGroup *terrainGroup)
{
	mSceneManager = sceneMgr;
	mTerrainGroup = terrainGroup;

	mReader = new Util::XmlReader(ConfigFileName);
	
	init_impl();
}

void MovableManager::update(Ogre::Real floatTime)
{
	try
	{
		updateAnimation(floatTime);
		updateMovableTrack(floatTime);
	}
	catch (...)
	{}
}

void MovableManager::updateAnimation(Ogre::Real floatTime)
{
	Ogre::Entity * entity = NULL;
	for (MovableVetor::iterator itBeg = mMovableVec.begin();
		itBeg != mMovableVec.end();
		++itBeg)
	{
		entity = mSceneManager->getEntity((*itBeg).name);

		Ogre::AnimationState *animState = entity->getAnimationState((*itBeg).animName);
		animState->addTime(floatTime);
	}
}

void MovableManager::updateMovableTrack(Ogre::Real floatTime)
{
	if (!mTerrainGroup)
		return;

	Ogre::Entity * entity = NULL;
	for (size_t idx = 0; idx < mMovableVec.size(); ++idx)
	{
		if (mMovableVec[idx].trackVec.empty())
			continue;

		entity = mSceneManager->getEntity(mMovableVec[idx].name);

		const Ogre::Vector3 &pos = entity->getParentSceneNode()->getPosition();
		const Ogre::Real transDistance = mMovableVec[idx].speed * floatTime;
		Ogre::Real newZ = pos.z + transDistance;
		Ogre::Real newY = mTerrainGroup->getHeightAtWorldPosition(pos.x, 0, newZ);
		Ogre::Vector3 direction(0, newY - pos.y, transDistance);
		Ogre::Vector3 transVec = entity->getParentSceneNode()->getOrientation() * direction;
		entity->getParentSceneNode()->translate(transVec, Ogre::Node::TS_WORLD);

		mWalkedDistanceVec[idx] += transVec.normalise();

		if (mWalkedDistanceVec[idx] >= mDistanceVec[idx])
		{
			++mActiveTrackVec[idx];
			if (mActiveTrackVec[idx] == mMovableVec[idx].trackVec.size())
				mActiveTrackVec[idx] = 0;

			const Ogre::Vector3 &pos = entity->getParentSceneNode()->getPosition();
			const Ogre::Vector2 &nextTrackPos = (mMovableVec[idx]).trackVec[mActiveTrackVec[idx]];
			Ogre::Vector3 newLookAtPos(nextTrackPos.x, pos.y, nextTrackPos.y);
			entity->getParentSceneNode()->lookAt(newLookAtPos, Ogre::Node::TS_WORLD, Ogre::Vector3::UNIT_Z);

			Ogre::Real height = mTerrainGroup->getHeightAtWorldPosition(nextTrackPos.x, 0, nextTrackPos.y);
			Ogre::Vector3 newTargetPos(nextTrackPos.x, height, nextTrackPos.y);
			mDistanceVec[idx] = (newTargetPos - pos).normalise();

			mWalkedDistanceVec[idx] = 0.0f;
		}
	}
}

void MovableManager::init_impl()
{
	if (!mReader->advanceFirstChildNode(NodeName))
		return;

	do
	{
		Movable movable;
		movable.name = mReader->getAttribute("name");
		movable.animName = mReader->getAttribute("anim");
		movable.speed = Ogre::StringConverter::parseReal(mReader->getAttribute("speed"));

		parseTracks(movable);

		mMovableVec.push_back(movable);
	}while (mReader->advanceNextSiblingNode(NodeName));

	size_t size = mMovableVec.size();
	mWalkedDistanceVec.resize(size, 0);
	mActiveTrackVec.resize(size, 0);
	
	delete mReader;
	mReader = NULL;

	initMovablesStates();
}

void MovableManager::parseTracks(Movable &movable)
{
	const Ogre::String & str = mReader->getAttribute("track");
	if (str.empty())
		return;

	Ogre::StringVector strVec = Ogre::StringUtil::split(str, ";");

	Ogre::Vector2 trackCoord;
	for (Ogre::StringVector::iterator itBeg = strVec.begin();
		    itBeg != strVec.end();
			++itBeg)
	{
		trackCoord = Ogre::StringConverter::parseVector2(*itBeg);
		movable.trackVec.push_back(trackCoord);
	}
}

void MovableManager::initMovablesStates()
{
	try
	{
		Ogre::Entity * entity = NULL;
		for (MovableVetor::iterator itBeg = mMovableVec.begin();
			itBeg != mMovableVec.end();
			++itBeg)
		{
			entity = mSceneManager->getEntity((*itBeg).name);
			Ogre::AnimationState *animState = entity->getAnimationState((*itBeg).animName);
			animState->setEnabled(true);
			animState->setLoop(true);

			// ÉèÖÃ³¯Ïò
			if ((*itBeg).trackVec.empty())
			{
				mDistanceVec.push_back(0.0f);
				continue;
			}

			const Ogre::Vector3 &pos = entity->getParentSceneNode()->getPosition();
			const Ogre::Vector2 &nextTrackPos = (*itBeg).trackVec[0];
			Ogre::Vector3 newLookAtPos(nextTrackPos.x, pos.y, nextTrackPos.y);
			entity->getParentSceneNode()->lookAt(newLookAtPos, Ogre::Node::TS_WORLD, Ogre::Vector3::UNIT_Z);

			Ogre::Real height = mTerrainGroup->getHeightAtWorldPosition(nextTrackPos.x, 0, nextTrackPos.y);
			Ogre::Vector3 newTargetPos(nextTrackPos.x, height, nextTrackPos.y);
			mDistanceVec.push_back((newTargetPos - pos).normalise());
		}
	}
	catch (const Ogre::Exception &/*e*/)
	{}
}