#ifndef _MOVABLE_MANAGER_H_
#define _MOVABLE_MANAGER_H_

#include "utility"
#include "vector"

#include "commonClasses-inl.h"
#include "Ogre.h"

namespace Util
{
	class XmlReader;
}

namespace Ogre
{
	class TerrainGroup;
}

class MovableManager
	 : public CommonClasses::Singleton<MovableManager>
{
	friend class CommonClasses::Singleton<MovableManager>;

public:
	void initialize(const Ogre::SceneManager *sceneMgr, Ogre::TerrainGroup *terrainGroup);
	void update(Ogre::Real floatTime);

private:
	MovableManager()
		: mSceneManager(NULL),
		  mTerrainGroup(NULL)
	{}

	DISALLOW_COPY_AND_ASSIGN(MovableManager);

	void init_impl();
	void initMovablesStates();

	void updateAnimation(Ogre::Real floatTime);
	void updateMovableTrack(Ogre::Real floatTime);

	struct Movable;
	void parseTracks(Movable &movable);

private:
	typedef std::vector<Ogre::Vector2> TrackVector;
	struct Movable
	{
		Ogre::String name;
		Ogre::String animName;
		TrackVector trackVec;
		Ogre::Real    speed;
	};

	typedef std::vector<Movable> MovableVetor;
	typedef std::vector<size_t> ActiveTrackVector;
	typedef std::vector<Ogre::Real> DistanceVector;

	const Ogre::SceneManager * mSceneManager;
	Ogre::TerrainGroup * mTerrainGroup;
	MovableVetor mMovableVec;
	ActiveTrackVector mActiveTrackVec;
	DistanceVector mDistanceVec;
	DistanceVector mWalkedDistanceVec;
	Util::XmlReader *mReader;
};

#endif
