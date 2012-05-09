#ifndef _OGRESAMPLE7_
#define _OGRESAMPLE7_
#include "../CEGUIBase/CEGUIBaseApp.h"

class AmbientLight : public Ogre::SimpleRenderable
{
public:
	AmbientLight();
	~AmbientLight();

	virtual Ogre::Real getBoundingRadius() const;
	virtual Ogre::Real getSquaredViewDepth(const Ogre::Camera*) const;
	virtual const Ogre::MaterialPtr& getMaterial() const;
	virtual void getWorldTransforms(Ogre::Matrix4 * xform) const;
	void updateFromCamera(Ogre::Camera *);

protected:
	Ogre::Real mRadius;
	Ogre::MaterialPtr mMatPtr;
};

class MaterialGenerator
{
public:
	typedef Ogre::uint32 Perm;
	virtual ~MaterialGenerator();
	const Ogre::MaterialPtr & getMaterial(Perm permutation);

	class Impl
	{
	public:
		virtual ~Impl();
		virtual Ogre::GpuProgramPtr generateVertexShader(Perm permutation) = 0;
		virtual Ogre::GpuProgramPtr generateFragmentShader(Perm permutation) = 0;
		virtual Ogre::MaterialPtr generateTemplateMaterial(Perm permutation) = 0;
	};

protected:
	MaterialGenerator();

	const Ogre::GpuProgramPtr & getVertexShader(Perm permutation);
	const Ogre::GpuProgramPtr & getFragmentShader(Perm permutation);
	const Ogre::MaterialPtr & getTemplateMaterial(Perm permutation);

	Ogre::String materialBaseName;
	Perm vsMask;
	Perm fsMask;
	Perm matMask;
	Impl * mImpl;

	typedef Ogre::map<Perm,Ogre::GpuProgramPtr>::type ProgramMap;
	typedef Ogre::map<Perm,Ogre::MaterialPtr>::type MaterialMap;

	ProgramMap mVs,mFs;
	MaterialMap mMaterials,mTemplateMat;
};

class LightMaterialGenerator : public MaterialGenerator
{
public:
	enum MaterialID
	{
		MI_POINT	= 0x01,
		MI_SPOTLIGHT = 0x02,
		MI_DIRECTIONAL = 0x04,
		MI_ATTENUATED  = 0x08,
		MI_SPECULAR    = 0x10,
		MI_SHADOW_CASTER = 0x20
	};

	LightMaterialGenerator();
	virtual ~LightMaterialGenerator()
	{}
};

class GBufferMaterialGenerator : public MaterialGenerator
{
public:
	GBufferMaterialGenerator();

	enum
	{
		//(Regular) Textures
		GBP_NO_TEXTURES =			0x00000000,
		GBP_ONE_TEXTURE =			0x00000001,
		GBP_TWO_TEXTURES =			0x00000002,
		GBP_THREE_TEXTURES =		0x00000003,
		GBP_TEXTURE_MASK =			0x0000000F,

		//Material properties
		GBP_HAS_DIFFUSE_COLOUR =     0x00000010,

		//The number of texture coordinate sets
		GBP_NO_TEXCOORDS =			0x00000000,
		GBP_ONE_TEXCOORD =			0x00000100,
		GBP_TWO_TEXCOORDS =			0x00000200,
		GBP_TEXCOORD_MASK =			0x00000700,

		//Do we have a normal map
		GBP_NORMAL_MAP =			0x00000800,

		//Are we skinned?
		GBP_SKINNED =				0x00010000
	};

	static const Ogre::uint32 FS_MASK = 0x0000FFFF;
	static const Ogre::uint32 VS_MASK = 0x00FFFF00;
	static const Ogre::uint32 MAT_MASK = 0xFF00FFFF;
};

class DLight : public Ogre::SimpleRenderable
{
public:
	DLight(MaterialGenerator * gen,Ogre::Light * parentLight);
	~DLight();

	void updateFromParent();
	void updateFromCamera(Ogre::Camera* camera);
	virtual bool getCastChadows() const;
	virtual Ogre::Real getBoundingRadius() const;
	virtual Ogre::Real getSquaredViewDepth(const Ogre::Camera*)const;
	virtual const Ogre::MaterialPtr & getMaterial() const;
	virtual void getWorldTransforms(Ogre::Matrix4 * xform) const;

	bool isCameraInsideLight(Ogre::Camera* camera);
	void rebuildGeometry(float radius);
	void createSphere(float radius,int nRings,int nSegments);
	void createRectangle2D();
	void createCone(float radius,float height,int nVerticesInBase);
	void setAttenuation(float c,float b,float a);
	void setSpecularColour(const Ogre::ColourValue & col);
	Ogre::Light * mParentLight;
	bool bIgnoreWorld;
	float mRadius;
	MaterialGenerator * mGenerator;
	Ogre::uint32 mPermutation;
};

class DeferredLightRenderOperation : public Ogre::CompositorInstance::RenderSystemOperation
{
public:
	DeferredLightRenderOperation(Ogre::CompositorInstance* instance,const Ogre::CompositionPass * pass);

	virtual void execute(Ogre::SceneManager * sm,Ogre::RenderSystem * rs);

	virtual ~DeferredLightRenderOperation();

private:
	DLight * createDLight(Ogre::Light * light);
	Ogre::String mTexName0;
	Ogre::String mTexName1;

	MaterialGenerator * mLightMaterialGenerator;
	typedef std::map<Ogre::Light* ,DLight*> LightsMap;
	LightsMap mLights;

	AmbientLight * mAmbientLight;

	Ogre::Viewport * mViewport;
};

class DeferredLightCompositionPass : public Ogre::CustomCompositionPass
{
public:
	virtual Ogre::CompositorInstance::RenderSystemOperation * createOperation(Ogre::CompositorInstance * instance,const Ogre::CompositionPass * pass)
	{
		return OGRE_NEW DeferredLightRenderOperation(instance,pass);
	}
	virtual ~DeferredLightCompositionPass()
	{}
};

class GeomUtils
{
public:
	static void createSphere(const Ogre::String& strName,float radius,int nRings,int nSegments,bool bNormals,bool bTexCoords);
	static void createSphere(Ogre::VertexData*& vertexData,Ogre::IndexData*& indexData,float radius,int nRings,int nSegments,bool bNormals,bool bTexCoords);
	static void createCone(const Ogre::String& strName,float radius,float height,int nVerticesInBase);
	static void createCone(Ogre::VertexData*& vertexData,Ogre::IndexData*& indexData,float radius,float height,int nVerticesInBase);

	static void createQuad(Ogre::VertexData*& vertexData);
};

class GBufferSchemeHandler : public Ogre::MaterialManager::Listener
{
public:
	virtual Ogre::Technique* handleSchemeNotFound(unsigned short schemeIndex,const Ogre::String& schemeName,Ogre::Material* originalMaterial,unsigned short lodIndex,const Ogre::Renderable* rend);

protected:
	GBufferMaterialGenerator mMaterialGenerator;
	static const Ogre::String NORMAL_MAP_PATTERN;
	
	struct PassProperties
	{
		PassProperties() : isDeferred(true),normalMap(0),isSkinned(false)
		{}
		bool isDeferred;
		Ogre::vector<Ogre::TextureUnitState*>::type regularTextures;
		Ogre::TextureUnitState* normalMap;
		bool isSkinned;
		bool hasDiffuseColour;
	};
	PassProperties inspectPass(Ogre::Pass * pass,unsigned short lodIndex,const Ogre::Renderable * rend);
	MaterialGenerator::Perm getPermutation(const PassProperties & props);
	void fillPass(Ogre::Pass * gBufferPass,Ogre::Pass* originalPass,const PassProperties& props);
	bool checkNormalMap(Ogre::TextureUnitState* tus,PassProperties& props);

};

class NullSchemeHandler : public Ogre::MaterialManager::Listener
{
	virtual Ogre::Technique* handleSchemeNotFound(unsigned short schemeIndex,const Ogre::String& schemeName,Ogre::Material* originalMaterial,unsigned short lodIndex,const Ogre::Renderable* rend)
	{
		Ogre::Technique * emptyTech = originalMaterial->createTechnique();
		emptyTech->removeAllPasses();
		emptyTech->setSchemeName(schemeName);
		return emptyTech;
	}
};

class DeferredShadingSystem : public Ogre::RenderTargetListener
{
public:
	DeferredShadingSystem(Ogre::Viewport * vp,Ogre::SceneManager * sm,Ogre::Camera* cam);
	~DeferredShadingSystem();

	enum DSMode
	{
		DSM_SHOWLIT = 0,
		DSM_SHOWCOLOUR = 1,
		DSM_SHOWNORMALS = 2,
		DSM_SHOWDSP = 3,
		DSM_COUNT = 4
	};

	static const Ogre::uint8 PRE_GBUFFER_RENDER_QUEUE;
	static const Ogre::uint8 POST_GBUFFER_RENDER_QUEUE;

	void initialize();
	void setMode(DSMode mode);
	DSMode getMode() const;
	void setSSAO(bool ssao);
	bool getSSAO()const;
	void setActive(bool active);

protected:
	Ogre::Viewport * mViewport;
	Ogre::SceneManager * mSceneMgr;
	Ogre::Camera * mCamera;
	Ogre::CompositorInstance * mGBufferInstance;
	Ogre::CompositorInstance * mInstance[DSM_COUNT];
	Ogre::CompositorInstance * mSSAOInstance;
	bool mActive;
	DSMode mCurrentMode;
	bool mSSAO;
	void createResources();
	void logCurrentMode();

};

class SharedData : public Ogre::Singleton<SharedData>
{
public:
	SharedData() : iRoot(0),iCamera(0),iWindow(0),mMLAnimState(0),iMainLight(0)
	{
		iActivate = false;
	}
	~SharedData()
	{}
	
	Ogre::Real iLastFrameTime;
	Ogre::Root * iRoot;
	Ogre::Camera * iCamera;
	Ogre::RenderWindow * iWindow;

	DeferredShadingSystem * iSystem;
	bool iActivate;
	bool iGlobalActivate;
	Ogre::AnimationState * mMLAnimState;
	Ogre::Light * iMainLight;
	Ogre::vector<Ogre::Node *>::type mLightNodes;
};

class ListenerFactoryLogic : public Ogre::CompositorLogic
{
public :
	virtual void compositorInstanceCreated(Ogre::CompositorInstance * newInstance)
	{
		Ogre::CompositorInstance::Listener * listener = createListener(newInstance);
		newInstance->addListener(listener);
		mListeners[newInstance] = listener;
	}

	virtual void compositorInstanceDestroyed(Ogre::CompositorInstance * destroyedInstance)
	{
		delete mListeners[destroyedInstance];
		mListeners.erase(destroyedInstance);
	}

protected:
	virtual Ogre::CompositorInstance::Listener * createListener(Ogre::CompositorInstance * instance) = 0;

private:
	typedef std::map<Ogre::CompositorInstance *,Ogre::CompositorInstance::Listener*> ListenerMap;
	ListenerMap mListeners;
};

class SSAOLogic : public ListenerFactoryLogic
{
protected:
	virtual Ogre::CompositorInstance::Listener* createListener(Ogre::CompositorInstance* instance);
};

const Ogre::ColourValue SAMPLE_COLORS[] = 
{   Ogre::ColourValue::Red, Ogre::ColourValue::Green, Ogre::ColourValue::Blue, 
Ogre::ColourValue::White, Ogre::ColourValue(1,1,0,1), Ogre::ColourValue(1,0,1,1) };
SharedData* Ogre::Singleton<SharedData>::ms_Singleton = 0;

class OgreSample7App : public CEGUIBaseApp,public Ogre::RenderTargetListener
{
public:
	DeferredShadingSystem * mSystem;
	void cleanupContent(void)
	{
		delete ( SharedData::getSingletonPtr() );

		delete mSystem;
	}

	void frameRenderingQueued(const Ogre::FrameEvent& evt);	
	void createScene();
	void setEntityHeight(Ogre::Entity* ent, Ogre::Real newHeight);
};

#endif