#ifndef _OGREPAGEDGEOMETRYSAMPLE_
#define _OGREPAGEDGEOMETRYSAMPLE_
#include "../CEGUIBase/CEGUIBaseApp.h"

using namespace Forests;
using namespace Ogre;

class MyRaySceneListener : public RaySceneQueryListener
{
	public:
		inline bool queryResult(SceneQuery::WorldFragment* fragment, Real distance)
		{
			resultDistance = distance;
			return false;
		}

		inline bool queryResult(MovableObject * obj,Real distance)
		{
			resultDistance = distance;
			return false;
		}

		float resultDistance;
};

class OgrePagedGeometrySampleApp : public CEGUIBaseApp
{
protected:
	PagedGeometry * mTreesMgr;
	PagedGeometry * mGrassMgr;
	PagedGeometry * mBushMgr;
	Ogre::TerrainGroup * mTerrainGroup;
	Ogre::TerrainGlobalOptions * mTerrainGlobals;

public:
	void createScene();
	void createTerrain();
	void createPagedGeometry();
	void frameRenderingQueued(const Ogre::FrameEvent& evt);
	void destroyScene();
};

inline float getTerrainHeight(float x,float z,void * userData = NULL); 

#endif