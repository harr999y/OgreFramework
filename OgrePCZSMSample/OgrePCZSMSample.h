#include "../CEGUIBase/CEGUIBaseApp.h"

using namespace Ogre;

class OgrePCZSMSampleApp : public CEGUIBaseApp
{
protected:
	PCZSceneNode * mSceneNode;
	RaySceneQuery * mRaySceneQuery;
	MovableObject * mTargetMO;

public:
	enum RoomWalls
	{
		TOP_WALL,
		BOT_WALL,
		FRONT_WALL,
		BACK_WALL,
		LEFT_WALL,
		RIGHT_WALL
	};
	enum RoomDoors
	{
		DOOR_NONE	= 0x00,
		DOOR_TOP	= 0x01,
		DOOR_BOT	= 0x02,
		DOOR_FRONT	= 0x04,
		DOOR_BACK	= 0x08,
		DOOR_LEFT	= 0x10,
		DOOR_RIGHT	= 0x20,
		DOOR_ALL	= 0xFF
	};


	void chooseSceneManager();
	void createCamera();
	void frameStarted(const Ogre::FrameEvent& evt);
	Ogre::PCZSceneNode * createTestBuilding(Ogre::SceneManager *scene, const Ogre::String & name);
	void createScene();

	Ogre::ManualObject*createRoom(Ogre::SceneManager *scene,const Ogre::String &name, short doorFlags,bool isEnclosure,Ogre::Vector3 dimensions,Ogre::Vector3 doorDimensions);

	void addMaterial(const Ogre::String& mat,const Ogre::ColourValue &clr,Ogre::SceneBlendType sbt);
	void createPoints(Ogre::Vector3 dimensions,Ogre::Vector3 doorDimensions);
	void createWalls(Ogre::ManualObject* room,short doorFlags,bool isEnclosure);
	void createPortals(Ogre::SceneManager *scene,Ogre::ManualObject* room,Ogre::SceneNode * roomNode,Ogre::PCZone * zone,short doorFlags,bool isEnclosure);
	void createPortals(Ogre::SceneManager *scene,Ogre::Entity* room,Ogre::SceneNode * roomNode,Ogre::PCZone * zone,short doorFlags,bool isEnclosure);

	Ogre::Vector3 points[32];
	int mPortalCount;
};