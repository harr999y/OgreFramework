#ifndef _FIRST_
#define _FIRST_

#include "../BaseApp/BaseClasses.h"
#include <deque>

using namespace std;

class FirstApp:public BaseApp
{
public:
	virtual void createScene();
	void createFrameListener();

protected:
	deque<Ogre::Vector3> mWalkList;
};

class FirstListener:public	BaseListener
{
public:
	FirstListener(Ogre::Root *root,Ogre::SceneManager* sm,Ogre::RenderWindow* rw,OgreBites::SdkCameraMan* cm,
		Ogre::SceneNode* sn,Ogre::Entity* ent,deque<Ogre::Vector3> &walk):BaseListener(root,sm,rw,cm),mSceneNode(sn),
		mEntity(ent),mWalkList(walk),mDistance(0),mWalked(0.0f),mWalking(false),m_bNeedWalk(false)
	{}
	virtual void nextLocation();
	void doSome();
	bool frameStarted(const Ogre::FrameEvent &evt);
	bool mousePressed(const OIS::MouseEvent &arg,OIS::MouseButtonID id);
	//bool mouseMoved(const OIS::MouseEvent &arg);
	
protected:
	Ogre::Real mDistance;
	Ogre::Vector3 mDirection;
	Ogre::Vector3 mDestination;
	Ogre::Real mWalked;
	bool mWalking;
	bool m_bNeedWalk;

	Ogre::AnimationState *mAnimationState;

	Ogre::Entity *mEntity;
	Ogre::SceneNode *mSceneNode;
	deque<Ogre::Vector3> mWalkList;
	
	Ogre::Real mWalkSpeed;
};

#endif