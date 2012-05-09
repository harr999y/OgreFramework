#ifndef _OGRESAMPLE15_
#define _OGRESAMPLE15_

#include "../CEGUIBase/CEGUIBaseApp.h"

enum ShaderValType
{
	GPU_VERTEX,
	GPU_FRAGMENT,
	MAT_SPECULAR,
	MAT_DIFFUSE,
	MAT_AMBIENT,
	MAT_SHININESS,
	MAT_EMISSIVE
};

using namespace Ogre;

struct ShaderControl
{
	Ogre::String Name;
	Ogre::String ParamName;
	ShaderValType ValType;
	float MinVal;
	float MaxVal;
	size_t ElementIndex;
	mutable size_t PhysicalcalIndex;
	
	float getRange() const 
	{
		return MaxVal - MinVal;
	}
};

typedef Ogre::vector<ShaderControl>::type ShaderControlsContainer;
typedef ShaderControlsContainer::iterator ShaderControlIterator;

class MaterialControls
{
public:
	MaterialControls(const Ogre::String& displayName,const Ogre::String & materialName) : mDisplayName(displayName),mMaterialName(materialName)
	{}

	const Ogre::String & getDisplayName() const
	{
		return mDisplayName;
	}
	const Ogre::String & getMaterialName() const
	{
		return mMaterialName;
	}
	size_t getShaderControlCount() const
	{
		return mShaderControlsContainer.size();
	}
	const ShaderControl & getShaderControl(const size_t idx) const
	{
		assert(idx < mShaderControlsContainer.size());
		return mShaderControlsContainer[idx];
	}
	void addControl(const Ogre::String & params);

protected:
	Ogre::String mDisplayName;
	Ogre::String mMaterialName;

	ShaderControlsContainer mShaderControlsContainer;
};

typedef Ogre::vector<MaterialControls>::type MaterialControlsContainer;
typedef MaterialControlsContainer::iterator MaterialControlsIterator;

#define CONTROLS_PER_PAGE 5

enum OceanMaterial
{
	OCEAN1_CG,
	OCEAN1_NATIVE,
	OCEAN2_CG,
	OCEAN2_NATIVE
};

class OgreSample15App : public CEGUIBaseApp
{
public:
	void loadMaterialControlsFile(MaterialControlsContainer & controlsContainer,const Ogre::String & fileName);
	void loadAllMaterialControlFiles(MaterialControlsContainer & controlsContainer);

protected:
	Ogre::Vector3 mTranslateVector;
	int mSceneDetailIndex;
	float mUpdateFreq;
	SceneNode * mMainNode;
	Entity * mOceanSurfaceEnt;
	size_t mCurrentMaterial;
	size_t mCurrentPage;
	size_t mNumPages;
	Ogre::MaterialPtr mActiveMaterial;
	Ogre::Pass * mActivePass;
	Ogre::GpuProgramPtr mActiveFragmentProgram;
	Ogre::GpuProgramPtr mActiveVertexProgram;
	Ogre::GpuProgramParametersSharedPtr mActiveFragmentParameters;
	Ogre::GpuProgramParametersSharedPtr mActiveVertexParameters;
	Real mRotationSpeed;

	MaterialControlsContainer mMaterialControlsContainer;

	void createScene();
	void createGUI();
	void changePage();
	void createPages();
	void initMaterial();

	CEGUI::Listbox * mList;
	CEGUI::TabControl * mTab;

	bool handleListSelect(const CEGUI::EventArgs & args);
	bool handleSlider(const CEGUI::EventArgs & args);
	//bool handleChangePage(const CEGUI::EventArgs & args);
};

#endif