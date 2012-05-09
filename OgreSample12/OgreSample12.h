#ifndef _OGRESAMPLE12_
#define _OGRESAMPLE12_

#include "../CEGUIBase/CEGUIBaseApp.h"

using namespace Ogre;

class OgreSample12App : public CEGUIBaseApp
{
public:
	OgreSample12App():GRASS_WIDTH(40),GRASS_HEIGHT(40)
	{}
	struct GrassVertex
	{
		float x,y,z;
		float nx,ny,nz;
		float u,v;
	};
	void createScene();
	void frameRenderingQueued(const Ogre::FrameEvent& evt);
	void waveGrass(Real timeElapsed);

protected:
	const Real GRASS_WIDTH;
	const Real GRASS_HEIGHT;
	StaticGeometry * mField;
	AnimationState * mLightAnimState;
	Controller<Real> * mLightController;
};

class LightPulse : public ControllerValue<Real>
{
public:
	LightPulse(Light * light ,Billboard * billboard,const ColourValue& maxColour,Real maxSize)
	{
		mLight = light;
		mBillboard = billboard;
		mMaxColour = maxColour;
		mMaxSize = maxSize;
	}

	Real getValue () const
	{
		return mIntensity;
	}

	void setValue(Real value)
	{	
		mIntensity = value;
		ColourValue newColour = mMaxColour * mIntensity;
		mLight->setDiffuseColour(newColour);
		mBillboard->setColour(newColour);
		Real newSize = mMaxSize * mIntensity;
		mBillboard->setDimensions(newSize , newSize);
	}
protected:
	Light * mLight;
	Billboard * mBillboard;
	ColourValue mMaxColour;
	Real mMaxSize;
	Real mIntensity;
};

#endif