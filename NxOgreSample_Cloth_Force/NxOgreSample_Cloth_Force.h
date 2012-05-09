#ifndef _NXOGRESAMPLECLOTHFORCE_
#define _NXOGRESAMPLECLOTHFORCE_

#include "../CEGUIBase/CEGUIBaseApp.h"

class NxOgreSample_Cloth_ForceApp : public CEGUIBaseApp
{
public:
	void createScene();
	void setupPhysX();
	void frameRenderingQueued(const Ogre::FrameEvent& evt);
	NxOgre::Cloth * makeFlag(NxOgre::Vec3,const std::string& flag_image);
	void destroyScene();

protected:
	NxOgre::World * mWorld;
	NxOgre::Scene * mScene;
	float mLastTimeStep;
	NxOgre::MeshManager * mMeshManager;
	NxOgre::Material * mDefaultMaterial;
	Critter::RenderSystem * mRenderSystem;
	NxOgre::Mesh * mFlagMesh;
	NxOgre::Cloth * mCLoth;
	Critter::KinematicBody * mOrb;
	float mWind;
};

#endif