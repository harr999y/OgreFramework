#ifndef _OGRESAMPLE5_
#define _OGRESAMPLE5_

#define COMPOSITORS_PER_PAGE 8

#include "../CEGUIBase/CEGUIBaseApp.h"

using namespace Ogre;

class ItemSelectorInterface
{
public :
	virtual ~ItemSelectorInterface()
	{}
	virtual void itemStateChanged(const size_t index,const bool state) = 0;
};

class ItemSelectorViewManager
{
public:
	ItemSelectorViewManager(const Ogre::String& parentWindowName);
	~ItemSelectorViewManager()
	{}

	size_t getSelectorCount()const
	{
		return mItemSelectorContainer.size();
	}
	void initItemSelector(CEGUI::String* names);
	Ogre::String getItemSelectorText(const size_t index) const
	{
		return Ogre::String(mItemSelectorContainer.at(index).CheckBoxWidget->getText().c_str());
	}
	void setItemSelectorController(ItemSelectorInterface* controller);

protected:
	struct ItemSelector
	{
		CEGUI::Checkbox* CheckBoxWidget;
		ItemSelector() : CheckBoxWidget(0)
		{}
	};
	typedef Ogre::vector<ItemSelector>::type ItemSelectorContainer;
	typedef ItemSelectorContainer::iterator ItemSelectorIterator;

	float mVerticalScrollPosition;
	CEGUI::Window* mParentWindow;
	CEGUI::ItemListbox* mScrollablePane;
	ItemSelectorInterface* mItemSelectorController;

	ItemSelectorContainer mItemSelectorContainer;

private:
	bool handleCheckStateChanged(const CEGUI::EventArgs& e);
};

class OgreSample5Listener:public CEGUIBaseListener,public ItemSelectorInterface
{	
	#define MINSPEED 0.150f
	#define MOVESPEED 30
	#define MANSPEED 1.800f
public:
	OgreSample5Listener(CEGUIBaseApp * app,Ogre::Root *root,Ogre::SceneManager* sm,Ogre::RenderWindow* rw, OgreBites::SdkCameraMan *cm,
		CEGUI::OgreRenderer *or) ;
	~OgreSample5Listener();

protected :
	Ogre::String mDebugText;

	Ogre::Vector3 mTranslateVector;
	bool mStateOn;
	size_t mNumScreenShots;
	bool mWriteToFile;
	float mSkipCount;
	float mSelfRotateSpeed;
	float mUpdateFreq;
	int mSceneDetailIndex;

	int mAniso;
	bool mQuit;

	CEGUI::Point mLastMousePosition;
	bool mLastMousePositionSet;
	Ogre::Real mTimeUnitNextToggle;
	float mRotX,mRotY;
	bool mProcessMovement;
	bool mUpdateMovement;
	std::vector<CEGUI::String > uniqueTextureNames;

	ItemSelectorViewManager* mCompositorSelectorViewManager;

	CEGUI::Window* mGuiDbg;
	CEGUI::Listbox* mDebugRTTListbox;
	CEGUI::Window* mDebugRTTStaticImage;
	typedef Ogre::vector<CEGUI::Imageset>::type ImageSetList;
	ImageSetList mDebugRTTImageSets;
	CEGUI::String mNames[21];
	Ogre::TextureUnitState * mDebugTextureTUS;
	SceneNode * mSceneNode;

	void updateStates();
	void registerCompositors();
	void initDebugRTTWindow();
	void updateDebugRTTWindow(int id);
	void createGUI();

public:
	virtual bool keyPressed(const OIS::KeyEvent &evt);

	bool frameRenderingQueued(const Ogre::FrameEvent& evt);
	bool handleRttSelection(const CEGUI::EventArgs& evt);
	void itemStateChanged(const size_t index,const bool state);
};

class OgreSample5App:public CEGUIBaseApp
{
public:
	void createScene();
	void cleanupScene();
	void doSome();

private:
	void setupViewport();
	void setupControls();
	void setupScene();
	void createEffects();
	void createTextures();
	void createFrameListener();
	void frameRenderingQueued(const Ogre::FrameEvent& evt);

	SceneNode* mSpinny;
	StringVector mCompositorNames;
	CEGUI::String mDebugCompositorName;
	TextureUnitState* mDebugTextureTUS;

};

class ListenerFactoryLogic : public Ogre::CompositorLogic
{
public:
	virtual void compositorInstanceCreated(Ogre::CompositorInstance* newInstance)
	{
		Ogre::CompositorInstance::Listener* listener = createListener(newInstance);
		newInstance->addListener(listener);
		mListeners[newInstance] = listener;
	}
	virtual void compositorInstanceDestroyed(Ogre::CompositorInstance* destroyInstance)
	{
		delete mListeners[destroyInstance];
		mListeners.erase(destroyInstance);
	}

protected:
	virtual Ogre::CompositorInstance::Listener* createListener(Ogre::CompositorInstance* instance) = 0;

private:
	typedef std::map<Ogre::CompositorInstance*,Ogre::CompositorInstance::Listener*> ListenerMap;
	ListenerMap mListeners;
};

class HeatVisionLogic : public ListenerFactoryLogic
{
protected:
	virtual Ogre::CompositorInstance::Listener * createListener(Ogre::CompositorInstance* instance);
};

class HDRLogic : public ListenerFactoryLogic
{
protected:
	virtual Ogre::CompositorInstance::Listener* createListener(Ogre::CompositorInstance* instance);
};

class GaussianBlurLogic : public ListenerFactoryLogic
{
protected:
	virtual Ogre::CompositorInstance::Listener* createListener(Ogre::CompositorInstance* instance);
};

class HeatVisionListener : public Ogre::CompositorInstance::Listener
{
public:
	HeatVisionListener();
	~HeatVisionListener();
	virtual void notifyMaterialSetup(Ogre::uint32 pass_id,Ogre::MaterialPtr& mat);
	virtual void notifyMaterialRender(Ogre::uint32 pass_id,Ogre::MaterialPtr& mat);

protected:
	Ogre::GpuProgramParametersSharedPtr fpParams;
	float start,end,curr;
	Ogre::Timer* timer;
};

class HDRListener : public Ogre::CompositorInstance::Listener
{
protected:
	int mVpWidth,mVpHeight;
	int mBloomSize;
	float mBloomTexOffsetsHorz[15][4];
	float mBloomTexWeights[15][4];
	float mBloomTexOffsetsVert[15][4];

public:
	HDRListener();
	virtual ~HDRListener();
	void notifyViewportSize(int width,int height);
	void notifyCompositor(Ogre::CompositorInstance* instance);
	virtual void notifyMaterialSetup(Ogre::uint32 pss_id,Ogre::MaterialPtr & mat);
	virtual void notifyMaterialRender(Ogre::uint32 pass_id,Ogre::MaterialPtr & mat);
};

class GaussianListener : public Ogre::CompositorInstance::Listener
{
protected:
	int mVpWidth,mVpHeight;
	float mBloomTexOffsetsHorz[15][4];
	float mBloomTexWeights[15][4];
	float mBloomTexOffsetsVert[15][4];

public:
	GaussianListener();
	~GaussianListener();
	void notifyViewportSize(int width,int height);
	virtual void notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);
	virtual void notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat);
};

#endif	