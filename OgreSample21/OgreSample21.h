#ifndef _OgreSample21_
#define _OgreSample21_

#include "../CEGUIBase/CEGUIBaseApp.h"

#include <OgreTerrain.h>
#include <OgreTerrainGroup.h>
#include <OgreTerrainQuadTreeNode.h>
#include <OgreTerrainMaterialGeneratorA.h>
#include <OgreTerrainPaging.h>
#include <OgrePageManager.h>

using namespace Ogre;

#define TERRAIN_FILE_PREFIX String("testTerrain")
#define TERRAIN_FILE_SUFFIX String("dat")
#define TERRAIN_WORLD_SIZE 12000.0f
#define TERRAIN_SIZE 513
#define TERRAIN_PAGE_MIN_X 0
#define TERRAIN_PAGE_MIN_Y 0
#define TERRAIN_PAGE_MAX_X 1
#define TERRAIN_PAGE_MAX_Y 1

class DummyPageProvider : public PageProvider
{
public:
	bool prepareProceduralPage(Page* page, PagedWorldSection* section)
	{
		section->_prepareProceduralPage(page);
		return true;
	}
	bool loadProceduralPage(Page* page, PagedWorldSection* section)
	{
		section->_loadProceduralPage(page);
		return true;
	}
	bool unloadProceduralPage(Page* page, PagedWorldSection* section)
	{
		section->_unloadProceduralPage(page);
		return true;
	}
	bool unprepareProceduralPage(Page* page, PagedWorldSection* section)
	{
		section->_unprepareProceduralPage(page);
		return true;
	}
};

class OgreSample21App : public CEGUIBaseApp
{
public:
	OgreSample21App(): 
		mTerrainsImported(false)
		, mPaging(true)
		, mTerrainGroup(0)
		, mTerrainPaging(0)
		, mPageManager(0)
		, mLayerEdit(1)
		, mBrushSizeTerrainSpace(0.02)
		, mHeightUpdateCountDown(0)
		, mTerrainPos(1000,0,5000)
	{
		mHeightUpdateRate = 1.0f / 20.f;
	}
	void createScene();
	void frameRenderingQueued(const Ogre::FrameEvent& evt);
	void saveTerrains(bool onlyIfModified);
	void createFrameListener();
	void defineTerrain(long x,long y,bool flat = false);
	void getTerrainImage(bool flipX,bool flipY,Image & img);
	void initBlendmaps(Terrain * terrain);
	void configureTerrainDefaults(Light * l);
	MaterialPtr buildDepthShadowMaterial(const String & textureName);
	void changeShadows(bool enabled,bool depthShadows);
	void destroyScene();

	//About the terrain;
	TerrainGlobalOptions * mTerrainGlobals;
	TerrainGroup * mTerrainGroup;
	bool mPaging;
	TerrainPaging * mTerrainPaging;
	PageManager * mPageManager;
	TerrainPagedWorldSection * mTerrainPagedSection;

	Ogre::uint8 mLayerEdit;
	Real mBrushSizeTerrainSpace;
	SceneNode * mEditNode;
	Entity * mEditMarker;
	Real mHeightUpdateCountDown;
	Real mHeightUpdateRate;
	Vector3 mTerrainPos;
	bool mTerrainsImported;
	ShadowCameraSetupPtr mPSSMSetup;

	typedef std::list<Entity *> EntityList;
	EntityList mHouseList;

	DummyPageProvider mDummyPageProvider;
};

class OgreSample21Listener : public CEGUIBaseListener
{
public:
	OgreSample21Listener(CEGUIBaseApp * app,Ogre::Root *root,Ogre::SceneManager* sm,Ogre::RenderWindow* rw, OgreBites::SdkCameraMan *cm,
		CEGUI::OgreRenderer *or) : CEGUIBaseListener(app,root,sm,rw,cm,or),mMode(0)
	{}
	bool keyPressed(const OIS::KeyEvent &arg);
	void modifyTerrain(Terrain* terrain,const Vector3& centrePos,Real timeElapsed);
	int mMode;
};


#endif