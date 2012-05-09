#include "OgreSample5.h"

//ItemSelectorViewManager
ItemSelectorViewManager::ItemSelectorViewManager(const Ogre::String& parentWindowName) : mItemSelectorController(0)
{
	mScrollablePane = (CEGUI::ItemListbox*)CEGUI::WindowManager::getSingleton().getWindow("Root2/List");
}

void ItemSelectorViewManager::initItemSelector(CEGUI::String* names)
{
	assert(mScrollablePane);
	for(int i=0;names[i].size() != 0;i++)
	{
		ItemSelector& item = ItemSelector();
		item.CheckBoxWidget = (CEGUI::Checkbox*)CEGUI::WindowManager::getSingleton().getWindow("Root2/List/" + Ogre::StringConverter::toString(i));
		mItemSelectorContainer.push_back(item);
		item.CheckBoxWidget->setID(i);
		item.CheckBoxWidget->setText(names[i]);
		item.CheckBoxWidget->setFont("DejaVuSans-10");
		item.CheckBoxWidget->subscribeEvent(CEGUI::Checkbox::EventCheckStateChanged,CEGUI::Event::Subscriber(&ItemSelectorViewManager::handleCheckStateChanged,this));
	}
}

void ItemSelectorViewManager::setItemSelectorController(ItemSelectorInterface* controller)
{
	mItemSelectorController = controller;
}

bool ItemSelectorViewManager::handleCheckStateChanged(const CEGUI::EventArgs & e)
{
	assert(mItemSelectorController);
	CEGUI::Checkbox* checkbox = static_cast<CEGUI::Checkbox*>(static_cast<const CEGUI::WindowEventArgs&>(e).window);
	mItemSelectorController->itemStateChanged(checkbox->getID(),checkbox->isSelected());
	float selectColour = checkbox->isSelected() ? 0.0f : 1.0f;
	checkbox->setProperty("NormalTextColour",CEGUI::PropertyHelper::colourToString(CEGUI::colour(selectColour,1.0f,selectColour)));
	return true;
}

//createListener	
Ogre::CompositorInstance::Listener* HDRLogic::createListener(Ogre::CompositorInstance* instance)
{
	HDRListener* listener = new HDRListener;
	Ogre::Viewport* vp = instance->getChain()->getViewport();
	listener->notifyViewportSize(vp->getActualWidth(),vp->getActualHeight());
	listener->notifyCompositor(instance);
	return listener;
}

Ogre::CompositorInstance::Listener* HeatVisionLogic::createListener(Ogre::CompositorInstance* instance)
{
	return new HeatVisionListener;
}

Ogre::CompositorInstance::Listener* GaussianBlurLogic::createListener(Ogre::CompositorInstance* instance)
{
	GaussianListener* listener = new GaussianListener;
	Ogre::Viewport * vp =instance->getChain()->getViewport();
	listener->notifyViewportSize(vp->getActualWidth(),vp->getActualHeight());
	return listener;
}

//The Listeners
HeatVisionListener::HeatVisionListener()
{
	timer = new Ogre::Timer();
	start = end = curr = 0.0f;
}

HeatVisionListener::~HeatVisionListener()
{
	delete timer;
}

void HeatVisionListener::notifyMaterialSetup(Ogre::uint32 pass_id,Ogre::MaterialPtr& mat)
{
	if(pass_id == 0xDEADBABE)
	{
		timer->reset();
		fpParams = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
	}
}

void HeatVisionListener::notifyMaterialRender(Ogre::uint32 pass_id,Ogre::MaterialPtr& mat)
{
	if(pass_id == 0xDEADBABE)
	{
		fpParams->setNamedConstant("random_fractions",Ogre::Vector4(Ogre::Math::RangeRandom(0.0f,1.0f),Ogre::Math::RangeRandom(0.0f,1.0f),0,0));
		float inc = ((float)timer->getMilliseconds())/1000.0f;
		if((fabs(curr - end) <= 0.001))
		{
			end = Ogre::Math::RangeRandom(0.95f,1.0f);
			start = curr;
		}
		else
		{
			if(curr > end)
				curr -= inc;
			else
				curr += inc;
		}
		timer->reset();

		fpParams->setNamedConstant("depth_modulator",Ogre::Vector4(curr,0,0,0));
	}
}

HDRListener::HDRListener()
{}

HDRListener::~HDRListener()
{}

void HDRListener::notifyViewportSize(int width,int height)
{
	mVpWidth = width;
	mVpHeight = height;
}

void HDRListener::notifyCompositor(Ogre::CompositorInstance* instance)
{
	Ogre::CompositionTechnique::TextureDefinitionIterator defIter = instance->getTechnique()->getTextureDefinitionIterator();
	while(defIter.hasMoreElements())
	{
		Ogre::CompositionTechnique::TextureDefinition* def = defIter.getNext();
		if(def->name == "rt_bloom0")
		{
			mBloomSize = (int )def->width;
			float deviation = 3.0f;
			float texelSize = 1.0f/(float )mBloomSize;

			mBloomTexOffsetsHorz[0][0] = 0.0f;
			mBloomTexOffsetsHorz[0][1] = 0.0f;
			mBloomTexOffsetsVert[0][0] = 0.0f;
			mBloomTexOffsetsVert[0][1] = 0.0f;
			mBloomTexWeights[0][0] = mBloomTexWeights[0][1] = mBloomTexWeights[0][2] = Ogre::Math::gaussianDistribution(0,0,deviation);
			mBloomTexWeights[0][3] = 1.0f;

			for(int i=1;i<8;++i)
			{
				mBloomTexWeights[i][0] = mBloomTexWeights[i][1] = mBloomTexWeights[i][2] = 1.25f * Ogre::Math::gaussianDistribution(i,0,deviation);
				mBloomTexWeights[i][3] = 1.0f;
				mBloomTexOffsetsHorz[i][0] = i*texelSize;
				mBloomTexOffsetsHorz[i][1] = 0.0f;
				mBloomTexOffsetsVert[i][0] = 0.0f;
				mBloomTexOffsetsVert[i][1] = i * texelSize;
			}

			for(int i = 8;i < 15;++i)
			{
				mBloomTexWeights[i][0] = mBloomTexWeights[i][1] =
					mBloomTexWeights[i][2] = mBloomTexWeights[i - 7][0];
				mBloomTexWeights[i][3] = 1.0f;

				mBloomTexOffsetsHorz[i][0] = -mBloomTexOffsetsHorz[i - 7][0];
				mBloomTexOffsetsHorz[i][1] = 0.0f;
				mBloomTexOffsetsVert[i][0] = 0.0f;
				mBloomTexOffsetsVert[i][1] = -mBloomTexOffsetsVert[i - 7][1];
			}
		}
	}
}

void HDRListener::notifyMaterialSetup(Ogre::uint32 pass_id,Ogre::MaterialPtr& mat)
{
	switch(pass_id)
	{
	//case 994: // rt_lum4
	case 993: // rt_lum3
	case 992: // rt_lum2
	case 991: // rt_lum1
	case 990: // rt_lum0
		break;
	case 800: // rt_brightpass
		break;
	case 701:
		{
			mat->load();
			Ogre::GpuProgramParametersSharedPtr fparams = mat->getBestTechnique()->getPass(0)->getFragmentProgramParameters();
			fparams->setNamedConstant("sampleOffsets",mBloomTexOffsetsHorz[0],15);
			fparams->setNamedConstant("sampleWeights",mBloomTexWeights[0],15);
			break;
		}
	case 700:
		{
			mat->load();
			Ogre::GpuProgramParametersSharedPtr fparams = mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
			fparams->setNamedConstant("sampleOffsets",mBloomTexOffsetsVert[0],15);
			fparams->setNamedConstant("sampleWeights",mBloomTexWeights[0],15);
			break;
		}
	}
}

void HDRListener::notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{}

GaussianListener::GaussianListener()
{}

GaussianListener::~GaussianListener()
{}

void GaussianListener::notifyViewportSize(int width,int height)
{
	mVpWidth = width;
	mVpHeight = height;
	float deviation = 3.0f;
	float texelSize = 1.0f/(float)std::min(mVpWidth,mVpHeight);

	// central sample, no offset
	mBloomTexOffsetsHorz[0][0] = 0.0f;
	mBloomTexOffsetsHorz[0][1] = 0.0f;
	mBloomTexOffsetsVert[0][0] = 0.0f;
	mBloomTexOffsetsVert[0][1] = 0.0f;
	mBloomTexWeights[0][0] = mBloomTexWeights[0][1] =
		mBloomTexWeights[0][2] = Ogre::Math::gaussianDistribution(0, 0, deviation);
	mBloomTexWeights[0][3] = 1.0f;

		// 'pre' samples
	for(int i = 1; i < 8; ++i)
	{
		mBloomTexWeights[i][0] = mBloomTexWeights[i][1] =
			mBloomTexWeights[i][2] = Ogre::Math::gaussianDistribution(i, 0, deviation);
		mBloomTexWeights[i][3] = 1.0f;
		mBloomTexOffsetsHorz[i][0] = i * texelSize;
		mBloomTexOffsetsHorz[i][1] = 0.0f;
		mBloomTexOffsetsVert[i][0] = 0.0f;
		mBloomTexOffsetsVert[i][1] = i * texelSize;
	}
	// 'post' samples
	for(int i = 8; i < 15; ++i)
	{
		mBloomTexWeights[i][0] = mBloomTexWeights[i][1] =
			mBloomTexWeights[i][2] = mBloomTexWeights[i - 7][0];
		mBloomTexWeights[i][3] = 1.0f;

		mBloomTexOffsetsHorz[i][0] = -mBloomTexOffsetsHorz[i - 7][0];
		mBloomTexOffsetsHorz[i][1] = 0.0f;
		mBloomTexOffsetsVert[i][0] = 0.0f;
		mBloomTexOffsetsVert[i][1] = -mBloomTexOffsetsVert[i - 7][1];
	}
}

void GaussianListener::notifyMaterialSetup(Ogre::uint32 pass_id,Ogre::MaterialPtr& mat)
{
	switch (pass_id)
	{
	case 701:
		{
			mat->load();
			Ogre::GpuProgramParametersSharedPtr fparams = mat->getBestTechnique()->getPass(0)->getFragmentProgramParameters();
			fparams->setNamedConstant("sampleOffsets",mBloomTexOffsetsHorz[0],15);
			fparams->setNamedConstant("sampleWeights",mBloomTexWeights[0],15);
			break;
		}
	case 700:
		{
			mat->load();
			Ogre::GpuProgramParametersSharedPtr fparams =
				mat->getTechnique(0)->getPass(0)->getFragmentProgramParameters();
			fparams->setNamedConstant("sampleOffsets", mBloomTexOffsetsVert[0], 15);
			fparams->setNamedConstant("sampleWeights", mBloomTexWeights[0], 15);
			break;
		}
	}
}

void GaussianListener::notifyMaterialRender(Ogre::uint32 pass_id, Ogre::MaterialPtr &mat)
{}

//listener
inline Ogre::String operator +(const Ogre::String& l,const CEGUI::String& o)
{
	return l+o.c_str();
}

OgreSample5Listener::OgreSample5Listener(CEGUIBaseApp * app,Ogre::Root *root,Ogre::SceneManager* sm,Ogre::RenderWindow* rw, OgreBites::SdkCameraMan *cm,
	CEGUI::OgreRenderer *or) : CEGUIBaseListener(app,root,sm,rw,cm,or)
	,mTranslateVector(.0f,0.f,.0f)
	,mStateOn(true)
	,mNumScreenShots(0)
	,mWriteToFile(false)
	,mSkipCount(0)
	,mUpdateFreq(50)
	, mSceneDetailIndex(0)
	, mAniso(1)
	, mQuit(false)
	, mLastMousePositionSet(false)
	, mTimeUnitNextToggle(0)
	, mRotX(0)
	, mRotY(0)
	, mProcessMovement(false)
	, mUpdateMovement(false)
	, mCompositorSelectorViewManager(0)
	, mSelfRotateSpeed(20.0f)
	, mDebugTextureTUS(NULL)
	, mSceneNode(NULL)
{
	mDebugRTTImageSets.clear();
}

OgreSample5Listener::~OgreSample5Listener()
{
	delete mCompositorSelectorViewManager;
	mDebugTextureTUS->setContentType(Ogre::TextureUnitState::CONTENT_NAMED);
}

void OgreSample5Listener::createGUI()
{
	CEGUI::System::getSingleton().getGUISheet()->addChildWindow(CEGUI::WindowManager::getSingleton().loadWindowLayout("OgreSample5.layout"));
	mGuiDbg = CEGUI::WindowManager::getSingleton().getWindow("Root2/DebugMenu");

	registerCompositors();
	initDebugRTTWindow();
}

bool OgreSample5Listener::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	if(mWriteToFile)
	{
		mWindow->writeContentsToFile("frame_" + Ogre::StringConverter::toString(++mNumScreenShots) + ".png");
	}

	if (mSceneNode)
	{
		//Ogre::Camera * _camera = mCameraMan->getCamera();
		//mSceneNode->setOrientation(_camera->getOrientation());
		//mSceneNode->setPosition(_camera->getPosition() + (_camera->getOrientation() * Vector3(0.3,-0.15,-1.1)));
		/*const OIS::MouseState & ms = mMouse->getMouseState();
		Ogre::Vector2 point(ms.X.abs,ms.Y.abs);
		Ogre::Ray ray = mCameraMan->getCamera()->getCameraToViewportRay(point.x/ms.width,point.y/ms.height);
		
		const Vector3 & v3 = mCameraMan->getCamera()->getDerivedPosition();//ray.getOrigin();
		mSceneNode->_setDerivedPosition(Vector3(v3.x,v3.y,100));*/
	}

	mApp->frameRenderingQueued(evt);

	return CEGUIBaseListener::frameRenderingQueued(evt);
}

bool OgreSample5Listener::keyPressed(const OIS::KeyEvent& e)
{
	if(e.key == OIS::KC_SYSRQ)
	{
		Ogre::StringStream ss;
		ss << "screenshot_ " << ++mNumScreenShots << ".png";
		mWindow->writeContentsToFile(ss.str());
		mDebugText = "Saved:" + ss.str();
	}
	return CEGUIBaseListener::keyPressed(e);
}

void OgreSample5App::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	if(mSpinny)
		mSpinny->yaw(Ogre::Degree(10 * evt.timeSinceLastFrame));
	CEGUIBaseApp::frameRenderingQueued(evt);
}

void OgreSample5Listener::itemStateChanged(const size_t index,const bool state)
{
	Ogre::String name = mCompositorSelectorViewManager->getItemSelectorText(index);
	Ogre::CompositorManager::getSingleton().setCompositorEnabled(mCameraMan->getCamera()->getViewport(),name,state);
	updateDebugRTTWindow(index);
}

void OgreSample5Listener::registerCompositors()
{
	Ogre::Viewport * vp = mWindow->getViewport(0);
	mCompositorSelectorViewManager = new ItemSelectorViewManager("CompositorSelectorWin");
	mCompositorSelectorViewManager->setItemSelectorController(this);
	Ogre::CompositorManager::ResourceMapIterator resourceIterator = Ogre::CompositorManager::getSingleton().getResourceIterator();

	int i = 0;
	while(resourceIterator.hasMoreElements())
	{
		Ogre::ResourcePtr resource = resourceIterator.getNext();
		const Ogre::String& compositorName = resource->getName();
		if(compositorName == "Ogre/Scene")
			continue;
		if(Ogre::StringUtil::startsWith(compositorName,"DeferredShading",false))
			continue;

		mNames[i++] = compositorName;
		int addPosition =-1;
		if(compositorName == "HDR")
		{
			addPosition = 0;
		}
		try
		{
			Ogre::CompositorInstance * instance = Ogre::CompositorManager::getSingleton().addCompositor(vp,compositorName,addPosition);
			Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp,compositorName,false);
		}
		catch (...)
		{}
	}
	mCompositorSelectorViewManager->initItemSelector(mNames);
}


void OgreSample5Listener::initDebugRTTWindow()
{
	mDebugRTTStaticImage = CEGUI::WindowManager::getSingleton().getWindow("Root2/Imagexx");
	mDebugRTTListbox = static_cast<CEGUI::Listbox*>(CEGUI::WindowManager::getSingleton().getWindow("Root2/DebugMenu"));
	/*mOverlay = (Ogre::OverlayContainer*)Ogre::OverlayManager::getSingleton().createOverlayElementFromTemplate("SdkTrays/Picture","",Ogre::String("overlayContainer"));		
	mOverlay->setPosition(1000,100);
	mOverlay->setDimensions(300,280);
	//mOverlay->getChild("overlayContainer/PictureFrame")->setPosition(22.1f,22.1f);
	mOverlay->getChild("overlayContainer/PictureFrame")->hide();
	mOverlay->setColour(Ogre::ColourValue(0x00000000));*/
	FontPtr font = FontManager::getSingleton().getByName("SdkTrays/Chinese");
	font->load();
	mOverlay = (Ogre::OverlayContainer*)OverlayManager::getSingleton().createOverlayElementFromTemplate("Text/Chinese","","ChineseText");

	//3D object
	//Entity * ent = mSceneMgr->createEntity("3D","ninja.mesh");
	//ent->setRenderQueueGroup(RENDER_QUEUE_OVERLAY);
	//ent->setMaterialName("Examples/StormySkyBox");

	//mSceneNode = OGRE_NEW SceneNode(NULL);//new SceneNode(mSceneMgr);//mSceneMgr->createSceneNode("node");
//	mSceneNode->translate( Vector3(0.4075,-0.2490,-10) ); 
	//mSceneNode->setPosition(0,0,-100);
	//mSceneNode->attachObject(ent);

	//mSceneNode->translate(Vector3(0,0,0),Node::TS_PARENT);

	//mOverlay->setDimensions(77.7f,77.7f);
	//mOverlay->setDimensions(0,280);
	//mOverlay->getChild("ChineseText/Chinese")->setPosition(0,0);
	//mOverlay->getChild("ChineseText/Chinese")->setCaption(Ogre::String("sdfafddsgsdgsgsdgxx"));
	//mOverlay->setMaterialName("SdkTrays/Shade");	

	//mOverlay->setCaption("sdfafddsgsdgsgsdgxx");
	//static_cast<TextAreaOverlayElement*>(mOverlay->getChild("ChineseText/MenuCaption"))->setCaption("dasdsafasf");//show();
	mOverlay->getChild("ChineseText/Chinese")->setCaption(DisplayString(L"造成今日之老大中国者，则中国老朽之冤业也；制出将来之少年中国者，则中国少年之责任也。彼老朽者何足道，彼与此世界作别之日不远矣，而我少年乃新来而与世界为缘。如僦屋者然，彼明日将迁居他方，而我今日始入此室处，将迁居者，不爱护其窗栊，不洁治其庭庑，俗人恒情，亦何足怪。若我少年者前程浩浩，后顾茫茫，中国而为牛、为马、为奴、为隶，则烹脔鞭之惨酷，惟我少年当之；中国如称霸宇内、主盟地球，则指挥顾盼之尊荣，惟我少年享之。于彼气息奄奄、与鬼为邻者何与焉？彼而漠然置之，犹可言也；我而漠然置之，不可言也。使举国之少年而果为少年也，则吾中国为未来之国，其进步未可量也；使举国之少年而亦为老大也，则吾中国为过去之国，其澌亡可翘足而待也。故今日之责任，不在他人，而全在我少年。少年智则国智，少年富则国富，少年强则国强，少年独立则国独立，少年自由则国自由，少年进步则国进步，少年胜于欧洲，则国胜于欧洲，少年雄于地球，则国雄于地球。红日初升，其道大光；河出伏流，一泻汪洋；潜龙腾渊，鳞爪飞扬；乳虎啸谷，百兽震惶；鹰隼试翼，风尘吸张；奇花初胎，皇皇；干将发硎，有作其芒；天戴其苍，地履其黄；纵有千古，横有八荒；前途似海，来日方长。美哉，我少年中国，与天不老!壮哉，我中国少年，与国无疆! "));
	//mOverlay->getChild("ChineseText/Chinese")->setCaption(mOverlay->getChild("ChineseText/Chinese")->getCaption().asUTF8_c_str());
	//mOverlay->setColour(Ogre::ColourValue(0.4f,0.5f,0.6f));
	//static_cast<TextAreaOverlayElement*>(mOverlay->getChild("ChineseText/MenuCaption"))->setFontName("SdkTrays/Value");
	//static_cast<TextAreaOverlayElement*>(mOverlay->getChild("ChineseText/Chinese"))->_update();
	//static_cast<TextAreaOverlayElement*>(mOverlay->getChild("ChineseText/Chinese"))->setWidth(324.7f);
	//static_cast<TextAreaOverlayElement*>(mOverlay->getChild("ChineseText/Chinese"))->setHeight(234.7f);
	Ogre::Overlay* overlay = Ogre::OverlayManager::getSingleton().create("overlay");
	//mSceneNode->_notifyRootNode();
	overlay->add2D(mOverlay);
	//overlay->add3D(mSceneNode);
	overlay->hide();
	
	//mDebugTextureTUS->setTextureAddressingMode(Ogre::TextureUnitState::TAM_MIRROR);
	mDebugRTTListbox->setFont("DejaVuSans-10");
	mDebugRTTListbox->subscribeEvent(CEGUI::Listbox::EventSelectionChanged,CEGUI::Event::Subscriber(&OgreSample5Listener::handleRttSelection,this));
}

bool OgreSample5Listener::handleRttSelection(const CEGUI::EventArgs& e)
{
	if(mDebugRTTListbox->getSelectedCount() > 0)
	{
		CEGUI::Imageset* imgSet = (CEGUI::Imageset*)mDebugRTTListbox->getFirstSelectedItem()->getUserData();
		//ImageSetList::iterator imgSet = mDebugRTTImageSets.end();

		//CEGUI::Imageset* imgSet = it->;
		//Ogre::TexturePtr* tex = (Ogre::TexturePtr*)mDebugRTTListbox->getFirstSelectedItem()->getUserData();
		//CEGUI::String im = imgSet->getName();
		///im.insert(0,"set:");
		//im.append(" image:RttImage");
		///CEGUI::String im = CEGUI::PropertyHelper::imageToString(&(imgSet->getImage("RttImage")));
		//mDebugRTTStaticImage->setProperty("Image",im.c_str());

		//mOverlay->show();
		//mDebugTextureTUS = mOverlay->getMaterial()->getBestTechnique()->getPass(0)->getTextureUnitState(0);
		//mDebugTextureTUS->setContentType(Ogre::TextureUnitState::CONTENT_COMPOSITOR);
		
		/*mDebugTextureTUS->setTextureFiltering(FO_ANISOTROPIC,FO_ANISOTROPIC,FO_LINEAR);

		CEGUI::ListboxItem* item = mDebugRTTListbox->getFirstSelectedItem();
		Ogre::String str1(item->getText().c_str());
		Ogre::String str2 = str1;
		mDebugTextureTUS->setCompositorReference(str1.erase(str1.find(':'),str1.size()-str1.find(':')),str2.erase(0,str2.find(':')+1));*/
	}
	else
	{
		//mDebugTextureTUS->setContentType(TextureUnitState::CONTENT_NAMED);
		//mOverlay->hide();
		//mDebugRTTStaticImage->setProperty("Image","");
	}
	return true;
}

void OgreSample5Listener::updateDebugRTTWindow(int id)
{
	mDebugRTTListbox->resetList();
	mDebugRTTStaticImage->setProperty("Image","");

	ImageSetList::iterator isIt = mDebugRTTImageSets.begin();
	for(;isIt != mDebugRTTImageSets.end();++isIt)
	{
		CEGUI::ImagesetManager::getSingleton().destroy(isIt->getName());
	}
	mDebugRTTImageSets.clear();
	Ogre::set<Ogre::String>::type uniqueTextureNames;
	Ogre::Viewport* vp = mWindow->getViewport(0);
	Ogre::CompositorChain* chain = Ogre::CompositorManager::getSingleton().getCompositorChain(vp);
	Ogre::CompositorChain::InstanceIterator it = chain->getCompositors();
	while(it.hasMoreElements())
	{
		Ogre::CompositorInstance* inst = it.getNext();
		if(inst->getEnabled())
		{
			Ogre::CompositionTechnique::TextureDefinitionIterator texIt = inst->getTechnique()->getTextureDefinitionIterator();
			while(texIt.hasMoreElements())
			{
				Ogre::CompositionTechnique::TextureDefinition* texDef = texIt.getNext();
				const Ogre::String& instName = inst->getTextureInstanceName(texDef->name,0);
				//Ogre::Compositor::inst->getTextureInstance(instName,0);
				
				Ogre::TexturePtr& texture = inst->getTextureInstance(texDef->name,0);
				//TexturePtr->
				CEGUI::Texture& tex = mOgreRenderer->createTexture(texture);
				if(uniqueTextureNames.find(instName) == uniqueTextureNames.end())
				{
					CEGUI::Imageset & imgSet = CEGUI::ImagesetManager::getSingleton().create(instName.c_str(),tex);
					mDebugRTTImageSets.push_back(imgSet);
					imgSet.defineImage((CEGUI::utf8 *)"RttImage",CEGUI::Point(0.0f,0.0f),tex.getSize(),CEGUI::Point(.0f,0.0f));
					CEGUI::ListboxTextItem * item = new CEGUI::ListboxTextItem((inst->getCompositor()->getName() + Ogre::String(":") + texDef->name ).c_str(),0,&imgSet);
					item->setSelectionBrushImage("TaharezLook","ListboxSelectionBrush");
					item->setSelectionColours(CEGUI::colour(0,0,1));
					mDebugRTTListbox->addItem(item);
					uniqueTextureNames.insert(instName);
				}
			}
		}
	}
}

void OgreSample5App::doSome()
{

	static bool firstTime = true;
	if(firstTime)
	{
		Ogre::CompositorManager& compMgr = Ogre::CompositorManager::getSingleton();
		compMgr.registerCompositorLogic("GaussianBlur",new GaussianBlurLogic);
		compMgr.registerCompositorLogic("HDR",new HDRLogic);
		compMgr.registerCompositorLogic("Heat Vision",new HeatVisionLogic);
		firstTime = false;
	}	
	//createTextures();
	createEffects();
}

void OgreSample5App::createFrameListener()
{
	OgreSample5Listener* listener = new OgreSample5Listener(this,mRoot,mSceneMgr,mWindow,mCameraMan,mOgreRenderer);
	listener->initialise();
	mListener = listener;
}

void OgreSample5App::createScene()
{

	setupScene();

}

void OgreSample5App::cleanupScene()
{
	mDebugTextureTUS->setContentType(TextureUnitState::CONTENT_NAMED);
	Ogre::CompositorManager::getSingleton().removeCompositorChain(mCameraMan->getCamera()->getViewport());
	mCompositorNames.clear();
}

void OgreSample5App::setupScene()
{
	mSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE);
	mSceneMgr->setShadowFarDistance(1000);
    
	//Ogre::MovableObject::setDefaultVisibilityFlags(0x00000001);

	// Set ambient light
	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.3f, 0.3f, 0.2f));

	Ogre::Light* l = mSceneMgr->createLight("Light2");
	Ogre::Vector3 dir(-1,-1,0);
	dir.normalise();
	l->setType(Ogre::Light::LT_DIRECTIONAL);
	l->setDirection(dir);
	l->setDiffuseColour(1, 1, 0.8f);
	l->setSpecularColour(1, 1, 1);

	Ogre::Entity* pEnt;

	// House
	pEnt = mSceneMgr->createEntity( "1", "tudorhouse.mesh" );
	Ogre::SceneNode* n1 = mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(350, 450, -200));
	n1->attachObject( pEnt );

	pEnt = mSceneMgr->createEntity( "2", "tudorhouse.mesh" );
	Ogre::SceneNode* n2 = mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(-350, 450, -200));
	n2->attachObject( pEnt );

	pEnt = mSceneMgr->createEntity( "3", "knot.mesh" );
	mSpinny = mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(0, 0, 300));
	mSpinny->attachObject( pEnt );
	pEnt->setMaterialName("Examples/MorningCubeMap");

	mSceneMgr->setSkyBox(true, "Examples/MorningSkyBox");


	Ogre::Plane plane;
	plane.normal = Ogre::Vector3::UNIT_Y;
	plane.d = 100;
	Ogre::MeshManager::getSingleton().createPlane("Myplane",
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,
		1500, 1500, 10, 10, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);
	Ogre::Entity* pPlaneEnt = mSceneMgr->createEntity( "plane", "Myplane" );
	pPlaneEnt->setMaterialName("Examples/Rockwall");
	pPlaneEnt->setCastShadows(false);
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(pPlaneEnt);

	mCameraMan->getCamera()->setPosition(-400, 50, 900);
	mCameraMan->getCamera()->lookAt(0,80,0);
}

void OgreSample5App::createEffects()
{
	Ogre::CompositorPtr comp3 = Ogre::CompositorManager::getSingleton().create("Motion Blur",Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	{
		Ogre::CompositionTechnique* t = comp3->createTechnique();
		{
			Ogre::CompositionTechnique::TextureDefinition * def = t->createTextureDefinition("scene");
			def->width = 0;
			def->height = 0;
			def->formatList.push_back(Ogre::PF_R8G8B8);
		}
		{
			Ogre::CompositionTechnique::TextureDefinition* def = t->createTextureDefinition("sum");
			def->width = 0;
			def->height = 0;
			def->formatList.push_back(Ogre::PF_R8G8B8);
		}
		{
			Ogre::CompositionTechnique::TextureDefinition *def = t->createTextureDefinition("temp");
			def->width = 0;
			def->height = 0;
			def->formatList.push_back(Ogre::PF_R8G8B8);
		}
		/// Render scene
		{
			Ogre::CompositionTargetPass *tp = t->createTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_PREVIOUS);
			tp->setOutputName("scene");
		}
		/// Initialisation pass for sum texture
		{
			Ogre::CompositionTargetPass *tp = t->createTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_PREVIOUS);
			tp->setOutputName("sum");
			tp->setOnlyInitial(true);
		}
		/// Do the motion blur
		{
			Ogre::CompositionTargetPass *tp = t->createTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_NONE);
			tp->setOutputName("temp");
			{ 
				Ogre::CompositionPass *pass = tp->createPass();
				pass->setType(Ogre::CompositionPass::PT_RENDERQUAD);
				pass->setMaterialName("Ogre/Compositor/Combine");
				pass->setInput(0, "scene");
				pass->setInput(1, "sum");
			}
		}
		/// Copy back sum texture
		{
			Ogre::CompositionTargetPass *tp = t->createTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_NONE);
			tp->setOutputName("sum");
			{ Ogre::CompositionPass *pass = tp->createPass();
			pass->setType(Ogre::CompositionPass::PT_RENDERQUAD);
			pass->setMaterialName("Ogre/Compositor/Copyback");
			pass->setInput(0, "temp");
			}
		}
		/// Display result
		{
			Ogre::CompositionTargetPass *tp = t->getOutputTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_NONE);
			{ 
				Ogre::CompositionPass *pass = tp->createPass();
				pass->setType(Ogre::CompositionPass::PT_RENDERQUAD);
				pass->setMaterialName("Ogre/Compositor/MotionBlur");
				pass->setInput(0, "sum");
			}
		}
	}
	/// Heat vision effect
	Ogre::CompositorPtr comp4 = Ogre::CompositorManager::getSingleton().create(
		"Heat Vision", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME
		);
	{
		Ogre::CompositionTechnique *t = comp4->createTechnique();
		t->setCompositorLogicName("Heat Vision");
		{
			Ogre::CompositionTechnique::TextureDefinition *def = t->createTextureDefinition("scene");
			def->width = 256;
			def->height = 256;
			def->formatList.push_back(Ogre::PF_R8G8B8);
		}
		{
			Ogre::CompositionTechnique::TextureDefinition *def = t->createTextureDefinition("temp");
			def->width = 256;
			def->height = 256;
			def->formatList.push_back(Ogre::PF_R8G8B8);
		}
		/// Render scene
		{
			Ogre::CompositionTargetPass *tp = t->createTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_PREVIOUS);
			tp->setOutputName("scene");
		}
		/// Light to heat pass
		{
			Ogre::CompositionTargetPass *tp = t->createTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_NONE);
			tp->setOutputName("temp");
			{
				Ogre::CompositionPass *pass = tp->createPass();
				pass->setType(Ogre::CompositionPass::PT_RENDERQUAD);
				pass->setIdentifier(0xDEADBABE); /// Identify pass for use in listener
				pass->setMaterialName("Fury/HeatVision/LightToHeat");
				pass->setInput(0, "scene");
			}
		}
		/// Display result
		{
			Ogre::CompositionTargetPass *tp = t->getOutputTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_NONE);
			{
				Ogre::CompositionPass *pass = tp->createPass();
				pass->setType(Ogre::CompositionPass::PT_RENDERQUAD);
				pass->setMaterialName("Fury/HeatVision/Blur");
				pass->setInput(0, "temp");
			}
		}
	}
}

void OgreSample5App::createTextures()
{
	using namespace Ogre;
	TexturePtr tex = TextureManager::getSingleton().createManual("HalftoneVolume","General",TEX_TYPE_3D,64,64,64,0,PF_A8);
	HardwarePixelBufferSharedPtr ptr = tex->getBuffer(0,0);
	ptr->lock(HardwareBuffer::HBL_DISCARD);
	const PixelBox & pb = ptr->getCurrentLock();
	Ogre::uint8 *data = static_cast<Ogre::uint8*>(pb.data);

	size_t height = pb.getHeight();
	size_t width = pb.getWidth();
	size_t depth = pb.getDepth();
	size_t rowPitch = pb.rowPitch;
	size_t slicePitch = pb.slicePitch;

	for (size_t z = 0; z < depth; ++z)
	{
		for (size_t y = 0; y < height; ++y)
		{
			for(size_t x = 0; x < width; ++x)
			{
				float fx = 32-(float)x+0.5f;
				float fy = 32-(float)y+0.5f;
				float fz = 32-((float)z)/3+0.5f;
				float distanceSquare = fx*fx+fy*fy+fz*fz;
				data[slicePitch*z + rowPitch*y + x] =  0x00;
				if (distanceSquare < 1024.0f)
					data[slicePitch*z + rowPitch*y + x] +=  0xFF;
			}
		}
	}
	ptr->unlock();

	Ogre::Viewport *vp = mRoot->getAutoCreatedWindow()->getViewport(0); 

	TexturePtr tex2 = TextureManager::getSingleton().createManual(
		"DitherTex",
		"General",
		TEX_TYPE_2D,
		vp->getActualWidth(),vp->getActualHeight(),1,
		0,
		PF_A8
		);

	HardwarePixelBufferSharedPtr ptr2 = tex2->getBuffer(0,0);
	ptr2->lock(HardwareBuffer::HBL_DISCARD);
	const PixelBox &pb2 = ptr2->getCurrentLock();
	Ogre::uint8 *data2 = static_cast<Ogre::uint8*>(pb2.data);

	size_t height2 = pb2.getHeight();
	size_t width2 = pb2.getWidth();
	size_t rowPitch2 = pb2.rowPitch;

	for (size_t y = 0; y < height2; ++y)
	{
		for(size_t x = 0; x < width2; ++x)
		{
			data2[rowPitch2*y + x] = Ogre::Math::RangeRandom(64.0,192);
		}
	}

	ptr2->unlock();
}

