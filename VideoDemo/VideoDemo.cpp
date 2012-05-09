#include "VideoDemo.h"

//#define VideoName "a.ogg"

void VideoDemo::createScene()
{
	using namespace Ogre;
	Entity * ent1 = mSceneMgr->createEntity("cube.mesh");
	SceneNode* node1 = mSceneMgr->getRootSceneNode()->createChildSceneNode(Vector3(-60,-60,0));
	node1->attachObject(ent1);

	Entity * ent2 = mSceneMgr->createEntity("cube.mesh");
	SceneNode* node2 = mSceneMgr->getRootSceneNode()->createChildSceneNode(Vector3(60,-60,0));
	node2->attachObject(ent2);

	Entity * ent3 = mSceneMgr->createEntity("cube.mesh");
	SceneNode* node3 = mSceneMgr->getRootSceneNode()->createChildSceneNode(Vector3(-60,60,0));
	node3->attachObject(ent3);

	Entity * ent4 = mSceneMgr->createEntity("cube.mesh");
	SceneNode* node4 = mSceneMgr->getRootSceneNode()->createChildSceneNode(Vector3(60,60,0));
	node4->attachObject(ent4);

	//play movie
	OgreVideoManager * mgr = (OgreVideoManager *)OgreVideoManager::getSingletonPtr();

	mgr->setNumWorkerThreads(1);
	mgr->setInputName("a.ogg");
	mgr->createDefinedTexture("video_material1");
	mgr->setInputName("b.ogg");
	mgr->createDefinedTexture("video_material2");
	mgr->setInputName("c.ogg");
	mgr->createDefinedTexture("video_material3");
	mgr->setInputName("d.ogg");
	mgr->createDefinedTexture("video_material4");

	TheoraVideoManager * tvmgr = TheoraVideoManager::getSingletonPtr();
	tvmgr->getVideoClipByName("a.ogg")->setAutoRestart(true);
	tvmgr->getVideoClipByName("b.ogg")->setAutoRestart(true);
	tvmgr->getVideoClipByName("c.ogg")->setAutoRestart(true);
	tvmgr->getVideoClipByName("d.ogg")->setAutoRestart(true);

	ent1->setMaterialName("video_material1");
	ent2->setMaterialName("video_material2");
	ent3->setMaterialName("video_material3");
	ent4->setMaterialName("video_material4");
}
