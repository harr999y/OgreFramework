#ifndef _OGRESAMPLE18_
#define _OGRESAMPLE18_

#include "../CEGUIBase/CEGUIBaseApp.h"

using namespace Ogre;

class LightWibbler : public ControllerValue<Real>
{
protected:
	Light * mLight;
	Billboard * mBillboard;
	ColourValue mColourRange;
	ColourValue mMinColour;
	Real mMinSize;
	Real mSizeRange;
	Real intensity;

public:
	LightWibbler(Light* light,Billboard* billboard,const ColourValue& minColour,const ColourValue& maxColour,Real minSize,Real maxSize)
	{
		mLight = light;
		mBillboard = billboard;
		mMinColour = minColour;
		mColourRange.r = maxColour.r - minColour.r;
		mColourRange.g = maxColour.g - minColour.g;
		mColourRange.b = maxColour.b - minColour.b;
		mMinSize = minSize;
		mSizeRange = maxSize - minSize;
	}

	virtual Real getValue() const 
	{
		return intensity;
	}

	virtual void setValue(Real value)
	{
		intensity = value;
		ColourValue newColour;
		newColour.r = mMinColour.r + (mColourRange.r * intensity);
		newColour.g = mMinColour.g + (mColourRange.g * intensity);
		newColour.b = mMinColour.b + (mColourRange.b * intensity);
		mLight->setDiffuseColour(newColour);
		mBillboard->setColour(newColour);
		Real newSize = mMinSize + (intensity * mSizeRange);
		mBillboard->setDimensions(newSize,newSize);
	}
};

class OgreSample18App : public CEGUIBaseApp
{
protected:
	Entity * mAthene;
	AnimationState * mAnimState;
	Entity * pPlaneEnt;
	vector<Entity*>::type pColumns;
	Light * mLight;
	Light * mSunLight;
	SceneNode * mLightNode;
	AnimationState * mLightAnimationState;
	ColourValue mMinLightColour;
	ColourValue mMaxLightColour;
	Real mMinFlareSize;
	Real mMaxFlareSize;
	Controller<Real> * mController;

	enum ShadowProjection
	{
		UNIFORM,
		UNIFORM_FOCUSED,
		LISPSM,
		PLANE_OPTIMAL
	};

	enum ShadowMaterial
	{
		MAT_STANDARD,
		MAT_DEPTH_FLOAT,
		MAT_DEPTH_FLOAT_PCF
	};

	ShadowTechnique mCurrentShadowTechnique;
	ShadowProjection mCurrentProjection;
	ShadowMaterial mCurrentMaterial;
	GpuProgramParametersSharedPtr mCustomRockwallVparams;
	GpuProgramParametersSharedPtr mCustomRockwallFparams;
	GpuProgramParametersSharedPtr mCustomAtheneVparams;
	GpuProgramParametersSharedPtr mCustomAtheneFparams;
	ShadowCameraSetupPtr mCurrentShadowCameraSetup;
	MovablePlane * mPlane;
	LiSPSMShadowCameraSetup * mLiSPSMSetup;

public:
	OgreSample18App() 		
		: mAnimState(0)
		, mLightNode(0)
		, mLightAnimationState(0)
		, mMinLightColour(0.2f, 0.1f, 0.0f)
		, mMaxLightColour(0.5f, 0.3f, 0.1f)
		, mMinFlareSize(40)
		, mMaxFlareSize(80)
		, mController(0)
		, mPlane(0)
	{}
	void frameRenderingQueued(const Ogre::FrameEvent& evt);
	void createScene();
	void createGUI();
	void destroyScene();
	void changeShadowTechnique(ShadowTechnique newTech);
	void configureLights(ShadowTechnique newTech);
	bool handleItemSelected(const CEGUI::EventArgs& args);
	//bool handleSliderMoved(const CEGUI::EventArgs& args);
	void resetMaterials();
	void materialChanged(int index);
};

#endif