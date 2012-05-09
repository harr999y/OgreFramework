#include "OgreSample16.h"

void OgreSample16App::createScene()
{
	using namespace Ogre;
	mSceneMgr->setAmbientLight(ColourValue(0.3f, 0.3f, 0.3f));
	mSceneMgr->createLight()->setPosition(20, 80, 50);
	Entity* ent = mSceneMgr->createEntity("Head", "ogrehead.mesh");
	mSceneMgr->getRootSceneNode()->attachObject(ent);

	ParticleSystem::setDefaultNonVisibleUpdateTimeout(NULL);
	ParticleSystem * ps;
	ps = mSceneMgr->createParticleSystem("Fireworks","Examples/Fireworks");
	mSceneMgr->getRootSceneNode()->attachObject(ps);
	ps = mSceneMgr->createParticleSystem("Nimbus","Examples/GreenyNimbus");
	mSceneMgr->getRootSceneNode()->attachObject(ps);
	ps = mSceneMgr->createParticleSystem("Rain", "Examples/Rain"); 
	ps->fastForward(5);
	mSceneMgr->getRootSceneNode()->createChildSceneNode(Vector3(0, 1000, 0))->attachObject(ps);

	// create aureola around ogre head perpendicular to the ground
	ps = mSceneMgr->createParticleSystem("Aureola", "Examples/Aureola");
	mSceneMgr->getRootSceneNode()->attachObject(ps);

	mNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	ps = mSceneMgr->createParticleSystem("Fountain1","Examples/PurpleFountain");
	mNode->createChildSceneNode(Vector3(200,-100,0),Quaternion(Degree(20),Vector3::UNIT_Z))->attachObject(ps);

	ps = mSceneMgr->createParticleSystem("Fountain2", "Examples/PurpleFountain");  // create fountain 2
	// attach the fountain to a child node of the pivot at a distance and angle
	mNode->createChildSceneNode(Vector3(-200, -100, 0), Quaternion(Degree(-20), Vector3::UNIT_Z))->attachObject(ps);
	mCameraMan->getCamera()->setPosition(0,0,40);
}

void OgreSample16App::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
	mNode->yaw(Ogre::Degree(evt.timeSinceLastFrame * 30));
}