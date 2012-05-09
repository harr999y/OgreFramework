#ifndef _FIVE_
#define _FIVE_

#include "../BaseAPP/BaseClasses.h"

using namespace Ogre;

class FiveListener:public BaseListener
{
public:
	FiveListener(Ogre::Root *root,Ogre::SceneManager* sm,Ogre::RenderWindow* rw,OgreBites::SdkCameraMan* cm,SceneNode *proj, Frustum *decal)
		: BaseListener(root,sm,rw,cm),mProjectorNode(proj),mDecalFrustum(decal),mAnim(0.0f)
	{}

	bool frameEnded(const Ogre::FrameEvent& evt);

protected:
	SceneNode * mProjectorNode;
	Frustum * mDecalFrustum;
	float mAnim;
};

class FiveApp: public BaseApp
{
public:
	void createScene();
	void createProjector();
	void makeMaterialReceiveDecal(const String &matName);
	void createFrameListener();

protected:
	SceneNode * mProjectorNode;
	Frustum *mDecalFrustum;
	Frustum *mFilterFrustum;
};

#endif