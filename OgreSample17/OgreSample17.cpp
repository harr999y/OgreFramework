#include "OgreSample17.h"

#include <OgreShaderFFPRenderState.h>
#include <OgreShaderProgram.h>
#include <OgreShaderParameter.h>
#include <OgreShaderProgramSet.h>
#include <OgreScriptCompiler.h>

Ogre::String ShaderExReflectionMap::Type = "SGX_ReflectionMap";

#define SGX_LIB_REFLECTIONMAP "SampleLib_ReflectionMap"
#define SGX_FUNC_APPLY_REFLECTION_MAP "SGX_ApplyReflectionMap"

ShaderExReflectionMap::ShaderExReflectionMap()
{
	mMaskMapSamplerIndex = 0;
	mReflectionMapType = TEX_TYPE_2D;
	mReflectionMapSamplerIndex = 0;
	mReflectionPowerChanged = true;
	mReflectionPowerValue = 0.5f;
}

const Ogre::String& ShaderExReflectionMap::getType()const
{
	return Type;
}

int ShaderExReflectionMap::getExecutionOrder() const 
{
	return FFP_TEXTURING + 1;
}

void ShaderExReflectionMap::copyFrom(const Ogre::RTShader::SubRenderState &rhs)
{
	const ShaderExReflectionMap& rhsReflectionMap = static_cast<const ShaderExReflectionMap&>(rhs);
	mMaskMapSamplerIndex = rhsReflectionMap.mMaskMapSamplerIndex;
	mMaskMapTextureName = rhsReflectionMap.mMaskMapTextureName;
	mReflectionMapSamplerIndex = rhsReflectionMap.mReflectionMapSamplerIndex;
	mReflectionMapType = rhsReflectionMap.mReflectionMapType;
	mReflectionPowerChanged = rhsReflectionMap.mReflectionPowerChanged;
	mReflectionPowerValue = rhsReflectionMap.mReflectionPowerValue;
	mReflectionMapTextureName = rhsReflectionMap.mReflectionMapTextureName;
}

bool ShaderExReflectionMap::preAddToRenderState(Ogre::RTShader::RenderState *renderState, Ogre::Pass *srcPass, Ogre::Pass *dstPass)
{
	TextureUnitState* textureUnit;

	textureUnit = dstPass->createTextureUnitState();
	textureUnit->setTextureName(mMaskMapTextureName);
	mMaskMapSamplerIndex = dstPass->getNumTextureUnitStates() - 1;
	textureUnit = dstPass->createTextureUnitState();
	if (mReflectionMapType == TEX_TYPE_2D)
	{
		textureUnit->setTextureName(mReflectionMapTextureName);
	}
	else
	{
		textureUnit->setCubicTextureName(mReflectionMapTextureName,true);
	}
	mReflectionMapSamplerIndex = dstPass->getNumTextureUnitStates() - 1;
	return true;
}

bool ShaderExReflectionMap::resolveParameters(Ogre::RTShader::ProgramSet *programSet)
{
	Program* vsProgram = programSet->getCpuVertexProgram();
	Program* psProgram = programSet->getCpuFragmentProgram();
	Function* vsMain = vsProgram->getEntryPointFunction();
	Function* psMain = psProgram->getEntryPointFunction();

	mVSInMaskTexcoord = vsMain->resolveInputParameter(Parameter::SPS_TEXTURE_COORDINATES,0,Parameter::SPC_TEXTURE_COORDINATE0,GCT_FLOAT2);
	if (mVSInMaskTexcoord.get() == 0)
	{
		return false;
	}

	mVSOutMaskTexcoord = vsMain->resolveOutputParameter(Parameter::SPS_TEXTURE_COORDINATES,-1,mVSInMaskTexcoord->getContent(),GCT_FLOAT2);
	if (mVSOutMaskTexcoord.get() == 0)
	{
		return false;
	}
	mPSInMaskTexCoord = psMain->resolveInputParameter(Parameter::SPS_TEXTURE_COORDINATES,mVSOutMaskTexcoord->getIndex(),mVSOutMaskTexcoord->getContent(),GCT_FLOAT2);
	if (mPSInMaskTexCoord.get() == 0)
	{
		return false;
	}
	mVSOutReflectionTexcoord = vsMain->resolveOutputParameter(Parameter::SPS_TEXTURE_COORDINATES,-1,Parameter::SPC_UNKNOWN,mReflectionMapType == TEX_TYPE_2D ? GCT_FLOAT2 : GCT_FLOAT3);
	if (mVSOutReflectionTexcoord.get() == 0)
	{
		return false;
	}
	mPSInReflectionTexcoord= psMain->resolveInputParameter(Parameter::SPS_TEXTURE_COORDINATES,mVSOutReflectionTexcoord->getIndex(),mVSOutReflectionTexcoord->getContent(),mVSOutReflectionTexcoord->getType());

	mWorldMatrix = vsProgram->resolveAutoParameterInt(GpuProgramParameters::ACT_WORLD_MATRIX,0);
	mWorldITMatrix = vsProgram->resolveAutoParameterInt(GpuProgramParameters::ACT_INVERSE_TRANSPOSE_WORLD_MATRIX,0);
	mViewMatrix = vsProgram->resolveAutoParameterInt(GpuProgramParameters::ACT_VIEW_MATRIX,0);
	mVSInputPos = vsMain->resolveInputParameter(Parameter::SPS_POSITION,0,Parameter::SPC_POSITION_OBJECT_SPACE,GCT_FLOAT4);
	mVSInputNormal = vsMain->resolveInputParameter(Parameter::SPS_NORMAL,0,Parameter::SPC_NORMAL_OBJECT_SPACE,GCT_FLOAT3);
	mMaskMapSampler = psProgram->resolveParameter(GCT_SAMPLER2D,mMaskMapSamplerIndex,(Ogre::uint16)GPV_GLOBAL,"mask_sampler");
	mReflectionMapSampler = psProgram->resolveParameter(mReflectionMapType == TEX_TYPE_2D ? GCT_SAMPLER2D : GCT_SAMPLERCUBE,mReflectionMapSamplerIndex,(Ogre::uint16)GPV_GLOBAL,"reflection_texture");
	mReflectionPower = psProgram->resolveParameter(GCT_FLOAT1,-1,(Ogre::uint16)GPV_GLOBAL,"reflection_power");
	mPSOutDiffuse = psMain->resolveOutputParameter(Parameter::SPS_COLOR,0,Parameter::SPC_COLOR_DIFFUSE,GCT_FLOAT4);
	return true;
}

bool ShaderExReflectionMap::resolveDependencies(Ogre::RTShader::ProgramSet *programSet)
{
	Program* vsProgram = programSet->getCpuVertexProgram();
	Program* psProgram = programSet->getCpuFragmentProgram();

	vsProgram->addDependency(FFP_LIB_COMMON);
	vsProgram->addDependency(FFP_LIB_TEXTURING);

	psProgram->addDependency(FFP_LIB_TEXTURING);
	psProgram->addDependency(FFP_LIB_COMMON);
	psProgram->addDependency(SGX_LIB_REFLECTIONMAP);
	return true;
}

bool ShaderExReflectionMap::addFunctionInvocations(Ogre::RTShader::ProgramSet *programSet)
{
	Program* vsProgram = programSet->getCpuVertexProgram();
	Function* vsMain = vsProgram->getEntryPointFunction();
	Program* psProgram = programSet->getCpuFragmentProgram();
	Function* psMain = psProgram->getEntryPointFunction();

	if (false == addVSInvocations(vsMain,FFP_VS_TEXTURING + 1))
	{
		return false;
	}

	if (false == addPSInvocations(psMain,FFP_PS_TEXTURING + 1))
	{
		return false;
	}
	return true;
}

bool ShaderExReflectionMap::addVSInvocations(Function* vsMain,const int groupOrder)
{
	FunctionInvocation* funcInvocation = NULL;
	int internalCounter = 0;
	funcInvocation = OGRE_NEW FunctionInvocation(FFP_FUNC_ASSIGN,groupOrder,internalCounter++);
	funcInvocation->pushOperand(mVSInMaskTexcoord,Operand::OPS_IN);
	funcInvocation->pushOperand(mVSOutMaskTexcoord,Operand::OPS_OUT);
	vsMain->addAtomInstace(funcInvocation);

	if (mReflectionMapType == TEX_TYPE_2D)
	{
		funcInvocation = OGRE_NEW FunctionInvocation(FFP_FUNC_GENERATE_TEXCOORD_ENV_SPHERE,groupOrder,internalCounter++);
		funcInvocation->pushOperand(mWorldITMatrix,Operand::OPS_IN);
		funcInvocation->pushOperand(mViewMatrix,Operand::OPS_IN);
		funcInvocation->pushOperand(mVSInputNormal,Operand::OPS_IN);
		funcInvocation->pushOperand(mVSOutReflectionTexcoord,Operand::OPS_OUT);
		vsMain->addAtomInstace(funcInvocation);
	}
	else
	{
		funcInvocation = OGRE_NEW FunctionInvocation(FFP_FUNC_GENERATE_TEXCOORD_ENV_REFLECT,groupOrder,internalCounter++);
		funcInvocation->pushOperand(mWorldMatrix, Operand::OPS_IN);
		funcInvocation->pushOperand(mWorldITMatrix, Operand::OPS_IN);
		funcInvocation->pushOperand(mViewMatrix, Operand::OPS_IN);					
		funcInvocation->pushOperand(mVSInputNormal, Operand::OPS_IN);	
		funcInvocation->pushOperand(mVSInputPos, Operand::OPS_IN);				
		funcInvocation->pushOperand(mVSOutReflectionTexcoord, Operand::OPS_OUT);
		vsMain->addAtomInstace(funcInvocation);
	}
	return true;
}

bool ShaderExReflectionMap::addPSInvocations(Function* psMain,const int groupOrder)
{
	FunctionInvocation * funcInvocation = NULL;
	int internalCounter = 0;

	funcInvocation = OGRE_NEW FunctionInvocation(SGX_FUNC_APPLY_REFLECTION_MAP,groupOrder,internalCounter++);
	funcInvocation->pushOperand(mMaskMapSampler,Operand::OPS_IN);
	funcInvocation->pushOperand(mPSInMaskTexCoord,Operand::OPS_IN);
	funcInvocation->pushOperand(mReflectionMapSampler,Operand::OPS_IN);
	funcInvocation->pushOperand(mPSInReflectionTexcoord,Operand::OPS_IN);
	funcInvocation->pushOperand(mPSOutDiffuse,Operand::OPS_IN,(Operand::OPM_X | Operand::OPM_Y | Operand::OPM_Z));
	funcInvocation->pushOperand(mReflectionPower,Operand::OPS_IN);
	funcInvocation->pushOperand(mPSOutDiffuse,Operand::OPS_OUT,(Operand::OPM_X | Operand::OPM_Y | Operand::OPM_Z));
	psMain->addAtomInstace(funcInvocation);
	return true;
}

void ShaderExReflectionMap::setReflectionMapType(Ogre::TextureType type)
{
	if (type != TEX_TYPE_2D && type != TEX_TYPE_CUBE_MAP)
	{
		OGRE_EXCEPT(Ogre::Exception::ERR_ITEM_NOT_FOUND,"Invalid texture type set - only 2D or Cube supported","ShaderExReflectionMap::setReflectionMapType");
	}
	mReflectionMapType = type;
}

void ShaderExReflectionMap::setReflectionPower(const Real reflectionPower)
{
	mReflectionPowerValue = reflectionPower;
	mReflectionPowerChanged = true;
}

void ShaderExReflectionMap::updateGpuProgramParams(Ogre::Renderable *rend, Ogre::Pass *pass, const Ogre::AutoParamDataSource *source, const Ogre::LightList *pLightList)
{
	if (mReflectionPowerChanged)
	{
		GpuProgramParametersSharedPtr fsParams = pass->getFragmentProgramParameters();
		mReflectionPower->setGpuParameter(mReflectionPowerValue);
		mReflectionPowerChanged = false;
	}
}

SubRenderState* ShaderExReflectionMapFactory::createInstance(Ogre::ScriptCompiler *compiler, PropertyAbstractNode *prop, Ogre::Pass *pass)
{
	if (prop->name == "rtss_ext_reflection_map")
	{
		if (prop->values.size() >= 2)
		{
			Ogre::String strValue;
			AbstractNodeList::const_iterator it = prop->values.begin();

			if (false == SGScriptTranslator::getString(*it,&strValue))
			{
				compiler->addError(ScriptCompiler::CE_INVALIDPARAMETERS,prop->file,prop->line);
				return NULL;
			}
			++it;

			SubRenderState* subRenderState = SubRenderStateFactory::createInstance();
			ShaderExReflectionMap * refelectionMapSubRenderState = static_cast<ShaderExReflectionMap*>(subRenderState);

			if (strValue == "cube_map")
			{
				refelectionMapSubRenderState->setReflectionMapType(TEX_TYPE_CUBE_MAP);
			}
			else if (strValue == "2d_map")
			{
				refelectionMapSubRenderState->setReflectionMapType(TEX_TYPE_2D);
			}

			if (false == SGScriptTranslator::getString(*it,&strValue))
			{
				compiler->addError(ScriptCompiler::CE_STRINGEXPECTED,prop->file,prop->line);
			}
			refelectionMapSubRenderState->setMaskMapTextureName(strValue);
			++it;

			if (false == SGScriptTranslator::getString(*it,&strValue))
			{
				compiler->addError(ScriptCompiler::CE_STRINGEXPECTED,prop->file,prop->line);
				return NULL;
			}
			refelectionMapSubRenderState->setReflectionMapTextureName(strValue);
			++it;

			Real reflectionPower = 0.5f;
			if (false == SGScriptTranslator::getReal(*it,&reflectionPower))
			{
				compiler->addError(ScriptCompiler::CE_STRINGEXPECTED,prop->file,prop->line);
				return NULL;
			}
			refelectionMapSubRenderState->setReflectionPower(reflectionPower);
			return subRenderState;

		}
	}
	return NULL;
}

void ShaderExReflectionMapFactory::writeInstance(Ogre::MaterialSerializer *ser, Ogre::RTShader::SubRenderState *subRenderState, Ogre::Pass *stcPass, Ogre::Pass *dstPass)
{
	ser->writeAttribute(4,"rtss_ext_reflection_map");

	ShaderExReflectionMap* reflectionMapSubRenderState = static_cast<ShaderExReflectionMap*>(subRenderState);
	if (reflectionMapSubRenderState->getReflectionMapType() == TEX_TYPE_CUBE_MAP)
	{
		ser->writeValue("cube_map");
	}
	else if (reflectionMapSubRenderState->getReflectionMapType() == TEX_TYPE_2D)
	{
		ser->writeValue("2d_map");
	}

	ser->writeValue(reflectionMapSubRenderState->getMaskMapTextureName());
	ser->writeValue(reflectionMapSubRenderState->getReflectionMapTextrueName());
	ser->writeValue(StringConverter::toString(reflectionMapSubRenderState->getReflectionPower()));
}

const Ogre::String& ShaderExReflectionMapFactory::getType() const
{
	return ShaderExReflectionMap::Type;
}

SubRenderState* ShaderExReflectionMapFactory::createInstanceImpl()
{
	return OGRE_NEW ShaderExReflectionMap;
}

const Ogre::String DIRECTIONAL_LIGHT_NAME		= "DirectionalLight";
const Ogre::String POINT_LIGHT_NAME             = "PointLight";
const Ogre::String SPOT_LIGHT_NAME			    = "SpotLight";
const Ogre::String PER_PIXEL_FOG_BOX			= "PerPixelFog";
const Ogre::String MAIN_ENTITY_MESH				= "ShaderSystem.mesh";
const Ogre::String SPECULAR_BOX					= "SpecularBox";
const Ogre::String REFLECTIONMAP_BOX			= "ReflectionMapBox";
const Ogre::String REFLECTIONMAP_POWER_SLIDER	= "ReflectionPowerSlider";
const Ogre::String MAIN_ENTITY_NAME				= "MainEntity";
const Ogre::String EXPORT_BUTTON_NAME			= "ExportMaterial";
const Ogre::String FLUSH_BUTTON_NAME			= "FlushShaderCache";
const Ogre::String SAMPLE_MATERIAL_GROUP		= "RTShaderSystemMaterialsGroup";

const int MESH_ARRAY_SIZE = 2;
const Ogre::String MESH_ARRAY[MESH_ARRAY_SIZE] = 
{
	MAIN_ENTITY_MESH,
	"knot.mesh"
};

void OgreSample17App::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
	if (mSceneMgr->hasLight(SPOT_LIGHT_NAME))
	{
		Light * light = mSceneMgr->getLight(SPOT_LIGHT_NAME);
		light->setPosition(mCameraMan->getCamera()->getDerivedPosition() + mCameraMan->getCamera()->getDerivedUp() * 20.0);
		light->setDirection(mCameraMan->getCamera()->getDerivedDirection());
	}

	if (mPointLightNode != NULL)
	{
		static Real sToatalTime = 0.0;

		sToatalTime += evt.timeSinceLastFrame;
		mPointLightNode->yaw(Degree(evt.timeSinceLastFrame * 15));
		mPointLightNode->setPosition(0.0, Math::Sin(sToatalTime) * 30.0, 0.0);
	}

	//updateTargetObjInfo();
}

void OgreSample17App::createScene()
{
	mCurLightingModel 		= SSLM_NormalMapLightingTangentSpace;
	mPerPixelFogEnable		= false;
	mSpecularEnable   		= true;
	mReflectionMapEnable	= false;
	mReflectionMapSubRS  = NULL;

	mTargetObj = NULL;


	// Set ambient lighting.
	mSceneMgr->setAmbientLight(ColourValue(0.2f, 0.2f, 0.2f));

	// Setup the sky box,
	mSceneMgr->setSkyBox(true, "Examples/SceneCubeMap2");

	Plane plane;
	plane.normal = Ogre::Vector3::UNIT_Y;
	plane.d = 0;
	MeshManager::getSingleton().createPlane("Myplane",
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,
		1500,1500,25,25,true,1,60,60,Ogre::Vector3::UNIT_Z);

	Entity* pPlaneEnt = mSceneMgr->createEntity( "plane", "Myplane" );
	pPlaneEnt->setMaterialName("Examples/Rockwall");
	pPlaneEnt->setCastShadows(false);
	mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(0,0,0))->attachObject(pPlaneEnt);
	mTargetEntities.push_back(pPlaneEnt);

	// Load sample meshes and generate tangent vectors.
	for (int i=0; i < MESH_ARRAY_SIZE; ++i)
	{
		const Ogre::String& curMeshName = MESH_ARRAY[i];

		MeshPtr pMesh = MeshManager::getSingleton().load(curMeshName,
			ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,    
			HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY, 
			HardwareBuffer::HBU_STATIC_WRITE_ONLY, 
			true, true); //so we can still read it

		// Build tangent vectors, all our meshes use only 1 texture coordset 
		// Note we can build into VES_TANGENT now (SM2+)
		unsigned short src, dest;
		if (!pMesh->suggestTangentVectorBuildParams(VES_TANGENT, src, dest))
		{
			pMesh->buildTangentVectors(VES_TANGENT, src, dest);		
		}
	}

	Entity* entity;
	SceneNode* childNode;

	// Create the main entity and mark it as the current target object.
	entity = mSceneMgr->createEntity(MAIN_ENTITY_NAME, MAIN_ENTITY_MESH);
	mTargetEntities.push_back(entity);
	childNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();		
	childNode->attachObject(entity);
	mTargetObj = entity;
	childNode->showBoundingBox(true);

	// Create reflection entity that will show the exported material.
	const Ogre::String& mainExportedMaterial = mSceneMgr->getEntity(MAIN_ENTITY_NAME)->getSubEntity(0)->getMaterialName() + "_RTSS";
	MaterialPtr matMainEnt        = MaterialManager::getSingleton().getByName(mainExportedMaterial, SAMPLE_MATERIAL_GROUP);

	entity = mSceneMgr->createEntity("ExportedMaterialEntity", MAIN_ENTITY_MESH);
	entity->setMaterial(matMainEnt);
	childNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	childNode->setPosition(0.0, 200.0, -200.0);
	childNode->attachObject(entity);

	// Create secondary entities that will be using custom RT Shader materials.
	entity = mSceneMgr->createEntity("PerPixelEntity", "knot.mesh");
	entity->setMaterialName("RTSS/PerPixel_SinglePass");
	childNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	childNode->setPosition(300.0f, 100.0f, -100.0f);
	childNode->attachObject(entity);

	// Create secondary entities that will be using custom RT Shader materials.
	entity = mSceneMgr->createEntity("NormalMapEntity", "knot.mesh");
	entity->setMaterialName("RTSS/NormalMapping_SinglePass");
	childNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	childNode->setPosition(-300.0, 100.0, -100.0);
	childNode->attachObject(entity);

	createDirectionalLight();
	//createPointLight();
	//createSpotLight();
	mPointLightNode = 0;

	mCameraMan->getCamera()->setPosition(0.0, 300.0, 450.0);
	mCameraMan->getCamera()->lookAt(0.0, 150.0, 0.0);

	mCameraMan->getCamera()->getViewport()->setMaterialScheme(RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);

	setCurrentLightingModel(SSLM_NormalMapLightingTangentSpace);

	updateSystemShaders();
	applyShadowType(1);
}

int OgreSample17App::getExecutionOrder() const
{
	return FFP_TEXTURING + 1;
}

void OgreSample17App::destroyScene()
{
	for (int i=0;i<MESH_ARRAY_SIZE;++i)
	{
		const Ogre::String& curMeshName = MESH_ARRAY[i];
		MeshManager::getSingleton().unload(curMeshName);
	}

	if (mReflectionMapFactory != NULL)
	{
		OGRE_DELETE mReflectionMapFactory;
		mReflectionMapFactory = NULL;
	}

	MeshManager::getSingleton().remove(MAIN_ENTITY_MESH);
	mTargetEntities.clear();

	//mSceneMgr->destroyQuery(mRayQuery);
	destroyPrivateResourceGroup();
}

void OgreSample17App::setCurrentLightingModel(ShaderSystemLightingModel lightModel)
{
	if (mCurLightingModel != lightModel)
	{
		mCurLightingModel = lightModel;
		EntityListIterator it = mTargetEntities.begin();
		EntityListIterator itEnd = mTargetEntities.end();

		for (;it != itEnd;++it)
		{
			generateShaders(*it);
		}
	}
}

void OgreSample17App::setSpecularEnable(bool enable)
{
	if (mSpecularEnable != enable)
	{
		mSpecularEnable = enable;
		updateSystemShaders();
	}
}

void OgreSample17App::setReflectionMapEnable(bool enable)
{
	if (mReflectionMapEnable != enable)
	{
		mReflectionMapEnable = enable;
		updateSystemShaders();
	}
}

void OgreSample17App::setPerPixelFogEnable(bool enable)
{
	if (mPerPixelFogEnable != enable)
	{
		mPerPixelFogEnable = enable;

		RenderState* schemeRenderState = mShaderGenerator->getRenderState(RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
		const SubRenderStateList& SubRenderStateList = schemeRenderState->getTemplateSubRenderStateList();
		SubRenderStateListConstIterator it = SubRenderStateList.begin();
		SubRenderStateListConstIterator itEnd = SubRenderStateList.end();
		FFPFog * fogSubRenderState = NULL;

		for (;it != itEnd;++it)
		{
			SubRenderState * curSubRenderState = *it;

			if (curSubRenderState->getType() == FFPFog::Type)
			{
				fogSubRenderState = static_cast<FFPFog*>(curSubRenderState);
				break;
			}
		}
		
		if (fogSubRenderState == NULL)
		{
			SubRenderState * subRenderState = mShaderGenerator->createSubRenderState(FFPFog::Type);

			fogSubRenderState = static_cast<FFPFog*>(subRenderState);
			schemeRenderState->addTemplateSubRenderState(fogSubRenderState);
		}

		if (mPerPixelFogEnable)
		{
			fogSubRenderState->setCalcMode(FFPFog::CM_PER_PIXEL);
		}
		else
		{
			fogSubRenderState->setCalcMode(FFPFog::CM_PER_VERTEX);
		}
		mShaderGenerator->invalidateScheme(RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
	}
}

void OgreSample17App::updateSystemShaders()
{
	EntityListIterator it = mTargetEntities.begin();
	EntityListIterator itEnd = mTargetEntities.end();

	for (;it!=itEnd;++it)
	{
		generateShaders(*it);
	}
}

void OgreSample17App::generateShaders(Ogre::Entity *entity)
{
	for (unsigned int i=0;i<entity->getNumSubEntities();++i)
	{
		SubEntity* curSubEntity = entity->getSubEntity(i);
		const Ogre::String& curMaterialName = curSubEntity->getMaterialName();
		bool success;

		success = mShaderGenerator->createShaderBasedTechnique(curMaterialName,MaterialManager::DEFAULT_SCHEME_NAME,RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
		
		if (success)
		{
			MaterialPtr curMaterial = MaterialManager::getSingleton().getByName(curMaterialName);
			Pass* curPass = curMaterial->getTechnique(0)->getPass(0);

			if (mSpecularEnable)
			{
				curPass->setSpecular(ColourValue::White);
				curPass->setShininess(32.0f);
			}
			else
			{
				curPass->setSpecular(ColourValue::Black);
				curPass->setShininess(0.0f);
			}

			RTShader::RenderState* renderState = mShaderGenerator->getRenderState(RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME,curMaterialName,0);
			
			renderState->reset();

			if (mCurLightingModel == SSLM_PerVertexLighting)
			{
				RTShader::SubRenderState * perPerVertexLightModel = mShaderGenerator->createSubRenderState(RTShader::FFPLighting::Type);
				renderState->addTemplateSubRenderState(perPerVertexLightModel);
			}
			else if (mCurLightingModel == SSLM_PerPixelLighting)
			{
				RTShader::SubRenderState * perPixelLightModel = mShaderGenerator->createSubRenderState(RTShader::PerPixelLighting::Type);
				renderState->addTemplateSubRenderState(perPixelLightModel);
			}
			else if (mCurLightingModel == SSLM_NormalMapLightingTangentSpace)
			{
				if (entity->getName() == MAIN_ENTITY_NAME)
				{
					RTShader::SubRenderState * subRenderState = mShaderGenerator->createSubRenderState(RTShader::NormalMapLighting::Type);
					RTShader::NormalMapLighting * normalMapSubRS = static_cast<RTShader::NormalMapLighting*>(subRenderState);
					normalMapSubRS->setNormalMapSpace(RTShader::NormalMapLighting::NMS_TANGENT);
					normalMapSubRS->setNormalMapTextureName("Panels_Normal_Tangent.png");
					renderState->addTemplateSubRenderState(normalMapSubRS);
				}
				else
				{
					RTShader::SubRenderState * perPixelLightModel = mShaderGenerator->createSubRenderState(RTShader::PerPixelLighting::Type);
					renderState->addTemplateSubRenderState(perPixelLightModel);
				}
			}
			else if (mCurLightingModel == SSLM_NormalMapLightingObjectSpace)
			{
				if (entity->getName() == MAIN_ENTITY_NAME)
				{
					RTShader::SubRenderState * subRenderState = mShaderGenerator->createSubRenderState(RTShader::NormalMapLighting::Type);
					RTShader::NormalMapLighting * normalMapSubRS = static_cast<RTShader::NormalMapLighting*>(subRenderState);
					normalMapSubRS->setNormalMapSpace(RTShader::NormalMapLighting::NMS_OBJECT);
					normalMapSubRS->setNormalMapTextureName("Panels_Normal_Obj.png");
					renderState->addTemplateSubRenderState(normalMapSubRS);
				}
				else
				{
					RTShader::SubRenderState * perPixelLightModel = mShaderGenerator->createSubRenderState(RTShader::PerPixelLighting::Type);
					renderState->addTemplateSubRenderState(perPixelLightModel);
				}
			}

			if (mReflectionMapEnable)
			{
				RTShader::SubRenderState * subRenderState = mShaderGenerator->createSubRenderState(ShaderExReflectionMap::Type);
				ShaderExReflectionMap * reflectionMapSubRS = static_cast<ShaderExReflectionMap*>(subRenderState);

				reflectionMapSubRS->setReflectionMapType(TEX_TYPE_CUBE_MAP);
				reflectionMapSubRS->setReflectionPower(0.35f);
				reflectionMapSubRS->setMaskMapTextureName("Panels_refmask.png");
				reflectionMapSubRS->setReflectionMapTextureName("cubescene.jpg");

				renderState->addTemplateSubRenderState(subRenderState);
				mReflectionMapSubRS = subRenderState;
			}
			else
			{
				mReflectionMapSubRS = NULL;
			}

			mShaderGenerator->invalidateMaterial(RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME,curMaterialName);
		}
	}
}

void OgreSample17App::createDirectionalLight()
{
	Light*  light;
	Ogre::Vector3 dir;

	light = mSceneMgr->createLight(DIRECTIONAL_LIGHT_NAME);
	light->setType(Light::LT_DIRECTIONAL);
	light->setCastShadows(true);
	dir.x = 0.5f;
	dir.y = -1.0f;
	dir.z = 0.3f;
	dir.normalise();
	light->setDirection(dir);
	light->setDiffuseColour(.5, .5, .5);
	light->setSpecularColour(.5, .5, .5);

	// create pivot node
	mDirectionalLightNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	BillboardSet* bbs;

	// Create billboard set.
	bbs = mSceneMgr->createBillboardSet();
	bbs->setMaterialName("Examples/Flare3");
	bbs->createBillboard(-dir * 500.0)->setColour(light->getDiffuseColour());
	bbs->setCastShadows(false);

	mDirectionalLightNode->attachObject(bbs);
	mDirectionalLightNode->attachObject(light);
}

void OgreSample17App::createPointLight()
{
	Light*  light;
	Ogre::Vector3 dir;

	light = mSceneMgr->createLight(POINT_LIGHT_NAME);
	light->setType(Light::LT_POINT);
	light->setCastShadows(true);
	dir.x = 0.5;
	dir.y = 0.0;
	dir.z = 0.0f;
	dir.normalise();
	light->setDirection(dir);
	light->setDiffuseColour(0.15f, 0.65f, 0.15f);
	light->setSpecularColour(0.5f, 0.5f, 0.5f);	
	light->setAttenuation(200.0f, 1.0f, 0.0005f, 0.0f);

	// create pivot node
	mPointLightNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();

	BillboardSet* bbs;

	// Create billboard set.
	bbs = mSceneMgr->createBillboardSet();
	bbs->setMaterialName("Examples/Flare3");
	bbs->createBillboard(200, 100, 0)->setColour(light->getDiffuseColour());
	bbs->setCastShadows(false);

	mPointLightNode->attachObject(bbs);
	mPointLightNode->createChildSceneNode(Ogre::Vector3(200, 100, 0))->attachObject(light);
}

void OgreSample17App::createSpotLight()
{
	Light*  light;
	Ogre::Vector3 dir;

	light = mSceneMgr->createLight(SPOT_LIGHT_NAME);
	light->setType(Light::LT_SPOTLIGHT);
	light->setCastShadows(false);
	dir.x = 0.0;
	dir.y = 0.0;
	dir.z = -1.0f;
	dir.normalise();	
	light->setSpotlightRange(Degree(20.0f), Degree(25.0f), 0.95f);
	light->setDirection(dir);
	light->setDiffuseColour(0.15f, 0.15f, 0.65f);
	light->setSpecularColour(0.5f, 0.5f, 0.5f);	
	light->setAttenuation(1000.0f, 1.0f, 0.0005f, 0.0f);
}

void OgreSample17App::setLightVisible(const Ogre::String &lightName, bool visible)
{
	if (mSceneMgr->hasLight(lightName))
	{
		if (lightName == POINT_LIGHT_NAME)
		{
			if (visible)
			{
				if (mPointLightNode->isInSceneGraph() == false)
				{
					mSceneMgr->getRootSceneNode()->addChild(mPointLightNode);
				}
			}
			else
			{
				if (mPointLightNode->isInSceneGraph() == true)
				{
					mSceneMgr->getRootSceneNode()->removeChild(mPointLightNode);
				}
			}
			mSceneMgr->getLight(lightName)->setVisible(visible);
		}
		else if (lightName == DIRECTIONAL_LIGHT_NAME)
		{
			SceneNode::ObjectIterator it = mDirectionalLightNode->getAttachedObjectIterator();
			while (it.hasMoreElements())
			{
				MovableObject * o = it.getNext();
				o->setVisible(visible);
			}
		}
		else
		{
			mSceneMgr->getLight(lightName)->setVisible(visible);
		}
	}
}

void OgreSample17App::applyShadowType(int menuIndex)
{
	Ogre::RTShader::RenderState* schemeRenderState = mShaderGenerator->getRenderState(RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);

	if (menuIndex == 0)
	{
		mSceneMgr->setShadowTechnique(SHADOWTYPE_NONE);
		const RTShader::SubRenderStateList & subRenderStateList = schemeRenderState->getTemplateSubRenderStateList();
		RTShader::SubRenderStateListConstIterator it = subRenderStateList.begin();
		RTShader::SubRenderStateListConstIterator itEnd = subRenderStateList.end();

		for (; it!=itEnd;++it)
		{
			RTShader::SubRenderState * curSubRenderState = *it;
			if (curSubRenderState->getType() == RTShader::IntegratedPSSM3::Type)
			{
				schemeRenderState->removeTemplateSubRenderState(*it);
				break;
			}
		}
	}
	else if (menuIndex == 1)
	{
		mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED);

		mSceneMgr->setShadowTextureCountPerLightType(Light::LT_DIRECTIONAL,3);
		mSceneMgr->setShadowTextureSettings(512,3,PF_FLOAT32_R);
		mSceneMgr->setShadowTextureSelfShadow(true);
		mSceneMgr->setShadowTextureCasterMaterial("PSSM/shadow_caster");
		
		MaterialPtr passCasterMaterial = MaterialManager::getSingleton().getByName("PSSM/shadow_caster");
		Pass * pssmCasterPass = passCasterMaterial->getTechnique(0)->getPass(0);
		pssmCasterPass->setFog(false);
		PSSMShadowCameraSetup * pssmSetup = new PSSMShadowCameraSetup();
		pssmSetup->calculateSplitPoints(3,3,10000);
		pssmSetup->setSplitPadding(10);
		pssmSetup->setOptimalAdjustFactor(0,2);
		pssmSetup->setOptimalAdjustFactor(1,1);
		pssmSetup->setOptimalAdjustFactor(2,0.5f);

		mSceneMgr->setShadowCameraSetup(ShadowCameraSetupPtr(pssmSetup));

		Ogre::RTShader::SubRenderState* subRenderState = mShaderGenerator->createSubRenderState(RTShader::IntegratedPSSM3::Type);
		RTShader::IntegratedPSSM3* pssm3SubRenderState = static_cast<RTShader::IntegratedPSSM3*>(subRenderState); 
		const PSSMShadowCameraSetup::SplitPointList& srcSplitPoints = pssmSetup->getSplitPoints();
		RTShader::IntegratedPSSM3::SplitPointList dstSplitPoints;

		for (unsigned int i=0;i<srcSplitPoints.size();++i)
		{
			dstSplitPoints.push_back(srcSplitPoints[i]);
		}

		pssm3SubRenderState->setSplitPoints(dstSplitPoints);
		schemeRenderState->addTemplateSubRenderState(subRenderState);
	}

	mShaderGenerator->invalidateScheme(RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
}

void OgreSample17App::exportRTShaderSystemMaterial(const Ogre::String &fileName, const Ogre::String &materialName)
{
	MaterialPtr materialPtr = MaterialManager::getSingleton().getByName(materialName);

	bool success = mShaderGenerator->createShaderBasedTechnique(materialName,MaterialManager::DEFAULT_SCHEME_NAME,RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);

	if (success)
	{
		RTShader::ShaderGenerator::getSingleton().validateMaterial(RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME,materialName);
		MaterialSerializer::Listener * matRTSSListener = RTShader::ShaderGenerator::getSingleton().getMaterialSerializerListener();
		MaterialSerializer matSer;

		matSer.addListener(matRTSSListener);
		matSer.exportMaterial(materialPtr,fileName,false,false,"",materialPtr->getName() + "_RTSS");
	}
}

void OgreSample17App::loadResources()
{
	BaseApp::loadResources();
	if (Ogre::RTShader::ShaderGenerator::initialize())
	{
		mShaderGenerator = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
		mShaderGenerator->addSceneManager(mSceneMgr);
	}		
	mReflectionMapFactory = OGRE_NEW ShaderExReflectionMapFactory;
	mShaderGenerator->addSubRenderStateFactory(mReflectionMapFactory);

	createPrivateResourceGroup();
}

void OgreSample17App::createPrivateResourceGroup()
{
	ResourceGroupManager & rgm = ResourceGroupManager::getSingleton();
	StringVector groupVector = ResourceGroupManager::getSingleton().getResourceGroups();
	StringVector::iterator itGroup = groupVector.begin();
	StringVector::iterator itGroupEnd = groupVector.end();
	Ogre::String shaderCoreLibsPath;

	for (;itGroup != itGroupEnd;++itGroup)
	{
		ResourceGroupManager::LocationList resLocationsList = Ogre::ResourceGroupManager::getSingleton().getResourceLocationList(*itGroup);
		ResourceGroupManager::LocationList::iterator it = resLocationsList.begin();
		ResourceGroupManager::LocationList::iterator itEnd = resLocationsList.end();
		bool coreLibsfound = false;

		for (;it != itEnd;++it)
		{
			if ((*it)->archive->getName().find("RTShaderLib") != Ogre::String::npos)
			{
				shaderCoreLibsPath = (*it)->archive->getName() + "/";
				coreLibsfound = true;
				break;
			}
		}

		if (coreLibsfound)
		{
			break;
		}
	}

	mRTShaderLibPath = shaderCoreLibsPath;

	rgm.createResourceGroup(SAMPLE_MATERIAL_GROUP,false);
	rgm.addResourceLocation(shaderCoreLibsPath + "materials", "FileSystem",SAMPLE_MATERIAL_GROUP);
	rgm.initialiseResourceGroup(SAMPLE_MATERIAL_GROUP);
	rgm.loadResourceGroup(SAMPLE_MATERIAL_GROUP,true);
}

void OgreSample17App::destroyPrivateResourceGroup()
{
	ResourceGroupManager & rgm = ResourceGroupManager::getSingleton();
	rgm.destroyResourceGroup(SAMPLE_MATERIAL_GROUP);
}

