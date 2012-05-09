#ifndef _NXOGREEXAMPLE_
#define _NXOGREEXAMPLE_

#include "../CEGUIBase/CEGUIBaseApp.h"

class NxOgreExample1 : public CEGUIBaseApp,public NxOgre::Callback
{
public:
	void createScene();
	void frameRenderingQueued(const Ogre::FrameEvent& evt);
	void onVolumeEvent(NxOgre::Volume* volume, NxOgre::Shape* volumeShape, NxOgre::RigidBody* collision_body, NxOgre::Shape* rigidBodyShape, unsigned int collisionEvent);

protected:
	NxOgre::World * mWorld;
	NxOgre::Scene * mScene;
	OGRE3DRenderSystem * mRenderSystem;
	OGRE3DBody * mCube;
	NxOgre::VisualDebugger * mVisualDebugger;
	OGRE3DRenderable * mVisualDebuggerRenderable;
	Ogre::SceneNode * mVisualDebbugerNode;
	NxOgre::Volume * mVolume;

	OGRE3DKinematicBody * mKB;
	NxOgre::Real mYPos;
	bool mDown;
};

#endif