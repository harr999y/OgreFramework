#ifndef _OGRESAMPLE20_
#define _OGRESAMPLE20_

#include "../CEGUIBase/CEGUIBaseApp.h"

class OgreSample20App : public CEGUIBaseApp
{
public:
	void createScene()
	{
		using namespace Ogre;
		mSceneMgr->setAmbientLight(ColourValue(0.3f, 0.3f, 0.3f));
		mSceneMgr->createLight()->setPosition(20, 80, 50);

		mSceneMgr->setSkyBox(true, "Examples/SpaceSkyBox", 5000);  // set our skybox

		// create a spaceship model, and place it at the origin
		Entity * ent = mSceneMgr->createEntity("Razor", "razor.mesh");
		ent->setMaterialName("Examples/SphereMappedRustySteel");
		mSceneMgr->getRootSceneNode()->attachObject(ent);
		ParticleSystem* thrusters = mSceneMgr->createParticleSystem(25);
		thrusters->setMaterialName("Examples/Flare");
		thrusters->setDefaultDimensions(25,25);
		
		for (int i=0;i<2;i++)
		{
			ParticleEmitter* emitter = thrusters->addEmitter("Point");
			emitter->setAngle(Degree(3));
			emitter->setTimeToLive(0.3f);
			emitter->setEmissionRate(134);
			emitter->setParticleVelocity(135);
			emitter->setDirection(Vector3::NEGATIVE_UNIT_Z);
			emitter->setColour(ColourValue::White,ColourValue::Red);
			emitter->setPosition(Vector3(i == 0 ? 5.7 : -18, 0, 0));
		}
		mSceneMgr->getRootSceneNode()->createChildSceneNode(Vector3(0, 6.5, -67))->attachObject(thrusters);
		mSceneMgr->getRootSceneNode()->createChildSceneNode(Vector3(0, 6.5, -67))->attachObject(mSceneMgr->createParticleSystem("Smoke","Examples/Smoke"));
		mCameraMan->getCamera()->setPosition(0,0,-130);
	}
};
#endif