#ifndef _NXOGRESAMPLE_CLOTH_
#define _NXOGRESAMPLE_CLOTH_

#include "../CEGUIBase/CEGUIBaseApp.h"

class NxOgreSample_ClothApp : public CEGUIBaseApp
{
public:
	void createScene();
	void setupPhysX();
	void makeCloth(const NxOgre::Vec3& barPosition);
	Critter::Body * makeBox(const NxOgre::Matrix44& globalPose,const NxOgre::Vec3& initV = NxOgre::Vec3::ZERO);
	void destroyScene();
	void frameRenderingQueued(const Ogre::FrameEvent& evt);
	void createFrameListener();
	bool mousePressed(const OIS::MouseEvent &arg,OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &arg,OIS::MouseButtonID id);
	enum VertexMethod
	{
		VM_SETPOSITION,
		VM_ATTACHVERTEX,
		VM_ADDFORCE
	};

protected:
	NxOgre::World * mWorld;
	NxOgre::Scene * mScene;
	float mLastTimeStep;
	NxOgre::MeshManager * mMeshManager;
	NxOgre::Material * mDefaultMaterial;
	Critter::RenderSystem * mRenderSystem;
	NxOgre::Cloth * mCloth;
	NxOgre::Mesh * mClothMesh;
	int mClothSelectedVertex;
	Ogre::SceneNode * mClothSelectedVertexMarker;
	VertexMethod mClothSelectedMethod;
};

class NxOgreSample_ClothListener : public CEGUIBaseListener
{
public:
	NxOgreSample_ClothListener(NxOgreSample_ClothApp * app,Ogre::Root *root,Ogre::SceneManager* sm,Ogre::RenderWindow* rw, OgreBites::SdkCameraMan *cm,
		CEGUI::OgreRenderer *or) : CEGUIBaseListener(app,root,sm,rw,cm,or)
	{}
	bool mousePressed(const OIS::MouseEvent &arg,OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &arg,OIS::MouseButtonID id);
};

#endif