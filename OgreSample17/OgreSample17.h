#ifndef _OGRESAMPLE17_
#define _OGRESAMPLE17_

#include "../CEGUIBase/CEGUIBaseApp.h"
#include "OgreRTShaderSystem.h"

using namespace Ogre;
using namespace Ogre::RTShader;

class ShaderExReflectionMap : public SubRenderState
{
public:
	ShaderExReflectionMap();
	virtual const String& getType() const;
	virtual int getExecutionOrder() const;
	virtual void copyFrom(const SubRenderState& rhs);
	virtual bool preAddToRenderState(RenderState * renderState,Pass* srcPass,Pass* dstPass);
	virtual void updateGpuProgramParams(Renderable* rend,Pass* pass,const AutoParamDataSource* source,const LightList* pLightList);
	void setReflectionMapType(TextureType type);
	TextureType getReflectionMapType() const
	{
		return mReflectionMapType;
	} 
	void setReflectionPower(const Real reflectionPower);
	Real getReflectionPower() const 
	{
		return mReflectionPowerValue;
	}
	void setReflectionMapTextureName(const String& textureName)
	{
		mReflectionMapTextureName = textureName;
	}
	const String& getReflectionMapTextrueName() const
	{
		return mReflectionMapTextureName;
	}
	void setMaskMapTextureName(const String& textureName)
	{
		mMaskMapTextureName = textureName;
	}
	const String& getMaskMapTextureName() const 
	{
		return mMaskMapTextureName;
	}

	static String Type;

protected:
	virtual bool resolveParameters(ProgramSet* programSet);
	virtual bool resolveDependencies(ProgramSet* programSet);
	virtual bool addFunctionInvocations(ProgramSet* programSet);
	bool addVSInvocations(Function* vsMain,const int groupOrder);
	bool addPSInvocations(Function* psMain,const int groupOrder);

	String mReflectionMapTextureName;
	String mMaskMapTextureName;
	unsigned short mMaskMapSamplerIndex;
	unsigned short mReflectionMapSamplerIndex;
	Real mReflectionPowerValue;
	bool mReflectionPowerChanged;
	TextureType mReflectionMapType;
	UniformParameterPtr mMaskMapSampler;
	UniformParameterPtr mReflectionMapSampler;
	UniformParameterPtr mReflectionPower;
	ParameterPtr mVSInMaskTexcoord;
	ParameterPtr mVSOutMaskTexcoord;
	ParameterPtr mVSOutReflectionTexcoord;
	ParameterPtr mPSInReflectionTexcoord;
	ParameterPtr mPSInMaskTexCoord;
	UniformParameterPtr mWorldMatrix;
	UniformParameterPtr mWorldITMatrix;
	UniformParameterPtr mViewMatrix;
	ParameterPtr mVSInputNormal;
	ParameterPtr mVSInputPos;
	ParameterPtr mPSOutDiffuse;
};

class ShaderExReflectionMapFactory : public SubRenderStateFactory
{
public:
	virtual const String& getType() const;
	virtual SubRenderState* createInstance(ScriptCompiler* compiler,PropertyAbstractNode* prop,Pass* pass);
	virtual void writeInstance(MaterialSerializer* ser,SubRenderState* subRenderState,Pass* stcPass,Pass* dstPass);
protected:
	virtual SubRenderState* createInstanceImpl();
};

using namespace CEGUI;

enum ShaderSystemLightingModel
{
	SSLM_PerVertexLighting,
	SSLM_PerPixelLighting,
	SSLM_NormalMapLightingTangentSpace,
	SSLM_NormalMapLightingObjectSpace
};

class OgreSample17App : public CEGUIBaseApp
{
protected:
	typedef Ogre::vector<Entity*>::type EntityList;
	typedef EntityList::iterator EntityListIterator;
	typedef map<Ogre::String,bool>::type StringMap;
	typedef StringMap::iterator StringMapIterator;

	EntityList mTargetEntities;
	ShaderSystemLightingModel mCurLightingModel;
	bool mPerPixelFogEnable;
	bool mSpecularEnable;
	RTShader::SubRenderStateFactory* mReflectionMapFactory;
	RTShader::SubRenderState* mReflectionMapSubRS;
	bool mReflectionMapEnable;
	SceneNode* mPointLightNode;
	SceneNode* mDirectionalLightNode;
	RaySceneQuery* mRayQuery;
	MovableObject* mTargetObj;
	Ogre::String mRTShaderLibPath;
	Ogre::RTShader::ShaderGenerator* mShaderGenerator;

public:
	//bool handles();
	int getExecutionOrder() const;
	void frameRenderingQueued(const Ogre::FrameEvent& evt);
	//void updateTargetObjInfo();
	void loadResources();
	void setCurrentLightingModel(ShaderSystemLightingModel lightModel);
	ShaderSystemLightingModel getCurrentLightingMode() const 
	{
		return mCurLightingModel;
	}
	void setSpecularEnable(bool enable);
	bool getSpecularEnable() const 
	{
		return mSpecularEnable;
	}
	void setReflectionMapEnable(bool enable);
	bool getReflectionMapEnable() const 
	{
		return mReflectionMapEnable;
	}
	void setPerPixelFogEnable(bool enable);
	void createDirectionalLight();
	void createPointLight();
	void createSpotLight();
	void setLightVisible(const Ogre::String& lightName,bool visible);
	void updateSystemShaders();
	void exportRTShaderSystemMaterial(const Ogre::String& fileName,const Ogre::String& materialName);
	void generateShaders(Entity* entity);
	void createScene();
	void destroyScene();
	void createPrivateResourceGroup();
	void destroyPrivateResourceGroup();
	//void pickTargetobject();
	void applyShadowType(int menuIndex);
};

#endif