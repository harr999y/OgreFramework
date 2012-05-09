#include "OgreSample3.h"

void OgreSample3App::createScene()
{
	using namespace Ogre;
	mCameraMan->getCamera()->getViewport()->setBackgroundColour(ColourValue::White);

	// create a basic point light with an offset
	Light* light = mSceneMgr->createLight();
	light->setPosition(20, 40, 50);

	// attach the light to a pivot node
	mLightPivot = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	mLightPivot->attachObject(light);

	// create our model, give it the shader material, and place it at the origin
	Entity *ent = mSceneMgr->createEntity("Head", "ogrehead.mesh");
	ent->setMaterialName("Examples/CelShading");
	mSceneMgr->getRootSceneNode()->attachObject(ent);

	SubEntity * sub = ent->getSubEntity(0);
	sub->setCustomParameter(SP_SHININESS,Vector4(35.f,0,0,0));
	sub->setCustomParameter(SP_DIFFUSE,Vector4(1, 0.3f, 0.3f, 1));
	sub->setCustomParameter(SP_SPECULAR, Vector4(1, 0.6f, 0.6f, 1));

	sub = ent->getSubEntity(1);    // skin
	sub->setCustomParameter(SP_SHININESS, Vector4(10, 0, 0, 0));
	sub->setCustomParameter(SP_DIFFUSE, Vector4(0, 0.5f, 0, 1));
	sub->setCustomParameter(SP_SPECULAR, Vector4(0.3f, 0.5f, 0.3f, 1));

	sub = ent->getSubEntity(2);    // earring
	sub->setCustomParameter(SP_SHININESS, Vector4(25, 0, 0, 0));
	sub->setCustomParameter(SP_DIFFUSE, Vector4(1, 1, 0, 1));
	sub->setCustomParameter(SP_SPECULAR, Vector4(1, 1, 0.7f, 1));

	sub = ent->getSubEntity(3);    // teeth
	sub->setCustomParameter(SP_SHININESS, Vector4(20, 0, 0, 0));
	sub->setCustomParameter(SP_DIFFUSE, Vector4(1, 1, 0.7f, 1));
	sub->setCustomParameter(SP_SPECULAR, Vector4(1, 1, 1, 1));
}

void OgreSample3App::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
	mLightPivot->yaw(Ogre::Degree(evt.timeSinceLastFrame * 30));
}