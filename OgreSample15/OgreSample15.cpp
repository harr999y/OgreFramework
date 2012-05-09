#include "OgreSample15.h"

void OgreSample15App::loadMaterialControlsFile(MaterialControlsContainer & controlsContainer,const Ogre::String & fileName)
{
	Ogre::ConfigFile cf;
	try 
	{
		cf.load(fileName,Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,"\t;=",true);

		Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

		Ogre::String secName,typeName,materialName,dataString;

		while (seci.hasMoreElements())
		{
			secName  = seci.peekNextKey();
			Ogre::ConfigFile::SettingsMultiMap * settings = seci.getNext();
			if (!secName.empty() && settings)
			{
				materialName = cf.getSetting("material",secName);
				Ogre::MaterialManager::getSingleton().load(materialName,Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
				MaterialControls newMaterialControls(secName,materialName);
				controlsContainer.push_back(newMaterialControls);
				size_t idx = controlsContainer.size() - 1;
				Ogre::ConfigFile::SettingsMultiMap::iterator i;
				for (i=settings->begin();i!=settings->end();i++)
				{
					typeName = i->first;
					dataString = i->second;
					if (typeName == "control")
					{
						controlsContainer[idx].addControl(dataString);
					}
				}
			}
		}
	}
	catch (Ogre::Exception e)
	{}
}

void OgreSample15App::loadAllMaterialControlFiles(MaterialControlsContainer & controlsContainer)
{
	Ogre::StringVectorPtr fileStringVector = Ogre::ResourceGroupManager::getSingleton().findResourceNames("Popular","*.controls");
	Ogre::StringVector::iterator controlsFileNameIterator = fileStringVector->begin();
	while (controlsFileNameIterator != fileStringVector->end())
	{
		loadMaterialControlsFile(controlsContainer,* controlsFileNameIterator);
		++controlsFileNameIterator;
	}
}

void MaterialControls::addControl(const Ogre::String & params)
{
	Ogre::StringVector vecparams = Ogre::StringUtil::split(params,",");
	ShaderControl newControl;
	Ogre::StringUtil::trim(vecparams[0]);
	newControl.Name = vecparams[0];
	Ogre::StringUtil::trim(vecparams[1]);
	newControl.ParamName = vecparams[1];

	Ogre::StringUtil::trim(vecparams[2]);
	if (vecparams[2] == "GPU_VERTEX")
		newControl.ValType = GPU_VERTEX;
	else if (vecparams[2] == "GPU_FRAGMENT")
		newControl.ValType = GPU_FRAGMENT;

	newControl.MinVal = Ogre::StringConverter::parseReal(vecparams[3]);
	newControl.MaxVal = Ogre::StringConverter::parseReal(vecparams[4]);
	newControl.ElementIndex = Ogre::StringConverter::parseInt(vecparams[5]);

	mShaderControlsContainer.push_back(newControl);
}

void OgreSample15App::createGUI()
{
	using namespace CEGUI;
	CEGUI::SchemeManager::getSingleton().create("VanillaSkin.Scheme");
	loadAllMaterialControlFiles(mMaterialControlsContainer);
	CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
	mRootWindow->addChildWindow(winMgr.loadWindowLayout("OgreSample15.layout"));
	
	mList = (CEGUI::Listbox*)winMgr.getWindow("OgreSample15/list");

	MaterialControlsIterator it = mMaterialControlsContainer.begin();
	for (int i=0;it != mMaterialControlsContainer.end();it++,i++)
	{
		ListboxTextItem * item = new ListboxTextItem(it->getDisplayName(),i);
		/*item->setSelectionColours(CEGUI::ColourRect(CEGUI::colour(0.1f,0.1f,0.2f,0.9f),CEGUI::colour(0.3f,0.4f,0.5f,0.9f),CEGUI::colour(0.6f,0.7f,0.8f,0.9f),CEGUI::colour(0.9f,0.9f,0.9f,0.9f)));
		item->setTextColours(CEGUI::colour(1,0.1f,0.2f,0.9f),CEGUI::colour(1,0.4f,0.5f,0.9f),CEGUI::colour(1,0.7f,0.8f,0.9f),CEGUI::colour(0.9f,0.9f,0.9f,0.9f));*/
		item->setSelectionBrushImage("TaharezLook", "MultiListSelectionBrush");
		mList->addItem(item);
	}
	mList->setItemSelectState(size_t(0),true);
	mList->subscribeEvent(Listbox::EventSelectionChanged,CEGUI::Event::Subscriber(&OgreSample15App::handleListSelect,this));
	mActiveMaterial = (Ogre::MaterialPtr)Ogre::MaterialManager::getSingleton().getByName(mMaterialControlsContainer[0].getMaterialName());
	mCurrentMaterial = 0;
	mCurrentPage = 0;
	mTab = NULL;
	//createPages();
}

void OgreSample15App::createPages()
{
	using namespace CEGUI;
	mNumPages = floor((float)(mMaterialControlsContainer[mCurrentMaterial].getShaderControlCount() / CONTROLS_PER_PAGE)) + 1;
	CEGUI::DefaultWindow * window ;
	CEGUI::Slider * slider;
	CEGUI::DefaultWindow* text;
	CEGUI::DefaultWindow* num;
	float uniformVal = 0.01f;
	char t[30];
	
	if (mTab)
	{
		mRootWindow->removeChildWindow(CEGUI::WindowManager::getSingleton().getWindow("OgreSample15_2"));
		CEGUI::WindowManager::getSingleton().destroyWindow(CEGUI::WindowManager::getSingleton().getWindow("OgreSample15_2"));
	}
	
	mRootWindow->addChildWindow(CEGUI::WindowManager::getSingleton().loadWindowLayout("OgreSample15_page.layout"));
	mTab = (CEGUI::TabControl *)CEGUI::WindowManager::getSingleton().getWindow("OgreSample15/page");
	Ogre::GpuProgramParametersSharedPtr activeParameters;
	initMaterial();
	for (size_t i=0;i<mNumPages;i++)
	{
		/*if (CEGUI::WindowManager::getSingleton().isWindowPresent(mTab->getChild(i)->getName()))
		{
			mTab->removeTab(i);		
			CEGUI::WindowManager::getSingleton().destroyWindow(mTab->getChild(i));
			mTab->removeChildWindow(i);
		}*/
		sprintf_s(t,"%d / %d",(int)(i+1),(int)mNumPages);
		window = (CEGUI::DefaultWindow *)CEGUI::WindowManager::getSingleton().createWindow("DefaultWindow");
		window->setText(CEGUI::String(t));
		window->setArea(UDim(0.f,0),UDim(0.f,2),UDim(1.0f,0),UDim(1.0f,0));
		for (size_t j=0;j<CONTROLS_PER_PAGE && i*CONTROLS_PER_PAGE + j < mMaterialControlsContainer[mCurrentMaterial].getShaderControlCount();j++)
		{
			const ShaderControl& ActiveShaderDef = mMaterialControlsContainer[mCurrentMaterial].getShaderControl(i * CONTROLS_PER_PAGE + j);
			slider = (CEGUI::Slider *)CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/Slider");
			slider->setID(i * CONTROLS_PER_PAGE + j);
			slider->setArea(UDim(0.6f,0),UDim(0.17f * j,2),UDim(0.23f,0),UDim(0.18f,0));

			switch(ActiveShaderDef.ValType)
			{
			case GPU_VERTEX:
			case GPU_FRAGMENT:
				{
					if (ActiveShaderDef.ValType == GPU_VERTEX)
					{
						activeParameters = mActiveVertexParameters;
					}
					else
						activeParameters = mActiveFragmentParameters;
					if(!activeParameters.isNull())
					{
						const Ogre::GpuConstantDefinition& def = activeParameters->getConstantDefinition(ActiveShaderDef.ParamName);
						ActiveShaderDef.PhysicalcalIndex = def.physicalIndex;		
						const float* pFloat = activeParameters->getFloatPointer(ActiveShaderDef.PhysicalcalIndex);
						uniformVal = pFloat[ActiveShaderDef.ElementIndex];
					}			
				}
				break;
			case MAT_SPECULAR:
				{
					// get the specular values from the material pass

					Ogre::ColourValue OldSpec(mActivePass->getSpecular());
					uniformVal = OldSpec[ActiveShaderDef.ElementIndex];
				}
				break;

			case MAT_DIFFUSE:
				{
					// get the diffuse values from the material pass

					Ogre::ColourValue OldSpec(mActivePass->getDiffuse());
					uniformVal = OldSpec[ActiveShaderDef.ElementIndex];
				}
				break;

			case MAT_AMBIENT:
				{
					// get the ambient values from the material pass

					Ogre::ColourValue OldSpec(mActivePass->getAmbient());
					uniformVal = OldSpec[ActiveShaderDef.ElementIndex];
				}
				break;

			case MAT_SHININESS:
				{
					// get the ambient values from the material pass
					uniformVal = mActivePass->getShininess();
				}

				break;

			case MAT_EMISSIVE:
				{
					// get the ambient values from the material pass

					//ColourValue OldSpec(mActivePass->gete());
					//activeScrollWidget->setScrollPosition( OldSpec.val[ActiveShaderDef->ElementIndex] );
				}
				break;
			}

			uniformVal = uniformVal / (ActiveShaderDef.MaxVal - ActiveShaderDef.MinVal);
			//slider->setCurrentValue(uniformVal);
			//slider->setProperty("CurrentValue",CEGUI::PropertyHelper::floatToString(uniformVal));
			window->addChildWindow(slider);
			text = (CEGUI::DefaultWindow *)CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticText");
			text->setArea(UDim(0.0f,0),UDim(0.16f * j,0),UDim(0.6f,0),UDim(0.18f,0));
			text->setText(CEGUI::String(ActiveShaderDef.Name));
			window->addChildWindow(text);
			num = (CEGUI::DefaultWindow *)CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/StaticText",CEGUI::String("Num") + CEGUI::PropertyHelper::intToString(i * CONTROLS_PER_PAGE + j));
			num->setArea(UDim(0.83f,0),UDim(0.17f * j,0),UDim(0.17f,0),UDim(0.18f,0));
			num->setText(CEGUI::PropertyHelper::floatToString(slider->getCurrentValue() * (ActiveShaderDef.MaxVal - ActiveShaderDef.MinVal) + ActiveShaderDef.MinVal));
			window->addChildWindow(num);
			slider->subscribeEvent(CEGUI::Slider::EventValueChanged,Event::Subscriber(&OgreSample15App::handleSlider,this));
			//num->subscribeEvent(CEGUI::Slider::EventValueChanged,Event::Subscriber(&OgreSample15App::handleSlider,this));

			//window->subscribeEvent(CEGUI::Window::Event,Event::Subscriber(&OgreSample15App::handleSlider,this))
		}
		mTab->addTab(window);
		
	}
	changePage();
}

void OgreSample15App::initMaterial()
{		
	if (!mActiveMaterial.isNull() && mActiveMaterial->getNumSupportedTechniques())
	{
		Ogre::Technique * currentTechnique = mActiveMaterial->getSupportedTechnique(0);
		if (currentTechnique)
		{
			mActivePass = currentTechnique->getPass(0);
			if (mActivePass)
			{
				if (mActivePass->hasFragmentProgram())
				{
					mActiveFragmentProgram = mActivePass->getFragmentProgram();
					mActiveFragmentParameters = mActivePass->getFragmentProgramParameters();
				}
				if (mActivePass->hasVertexProgram())
				{
					mActiveVertexProgram = mActivePass->getVertexProgram();
					mActiveVertexParameters = mActivePass->getVertexProgramParameters();
				}
			}
		}
	}
}

void OgreSample15App::changePage()
{
	mTab->makeTabVisible(mCurrentPage);
}

void OgreSample15App::createScene()
{
	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.3f, 0.3f, 0.3f));
	mSceneMgr->setSkyBox(true, "SkyBox", 1000);

	mMainNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();

	Ogre::Plane oceanSurface;
	oceanSurface.normal = Ogre::Vector3::UNIT_Y;
	oceanSurface.d = 20;
	Ogre::MeshManager::getSingleton().createPlane("OceanSurface",Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,oceanSurface,1000,1000,50,50,true,1,1,1,Ogre::Vector3::UNIT_Z);

	mOceanSurfaceEnt = mSceneMgr->createEntity( "OceanSurface", "OceanSurface" );
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(mOceanSurfaceEnt);
	mOceanSurfaceEnt->setMaterialName(mMaterialControlsContainer[mCurrentMaterial].getMaterialName());
	createPages();
}

bool OgreSample15App::handleListSelect(const CEGUI::EventArgs &args)
{
	CEGUI::ListboxTextItem * item = (CEGUI::ListboxTextItem*)mList->getFirstSelectedItem();
	if (item)
	{
		size_t id = item->getID();
		mCurrentPage = 0;
		mActiveMaterial = Ogre::MaterialManager::getSingleton().getByName(mMaterialControlsContainer[id].getMaterialName());
		if (id != mCurrentMaterial)
		{	
			mCurrentMaterial = id;
			createPages();
			mOceanSurfaceEnt->setMaterialName(mMaterialControlsContainer[mCurrentMaterial].getMaterialName());
		}
	}
	return true;
}

bool OgreSample15App::handleSlider(const CEGUI::EventArgs & args)
{
	CEGUI::Slider * slider = static_cast<CEGUI::Slider*>(static_cast<const CEGUI::WindowEventArgs&>(args).window);
	float val = slider->getCurrentValue();	
	CEGUI::DefaultWindow * num = (CEGUI::DefaultWindow *)slider->getParent()->getChild(CEGUI::String("Num") + CEGUI::PropertyHelper::intToString(slider->getID()));
	const ShaderControl& ActiveShaderDef = mMaterialControlsContainer[mCurrentMaterial].getShaderControl(slider->getID());
	num->setText(CEGUI::PropertyHelper::floatToString(val * (ActiveShaderDef.MaxVal - ActiveShaderDef.MinVal) + ActiveShaderDef.MinVal));
	if (mActivePass)
	{
		switch (ActiveShaderDef.ValType)
		{
		case GPU_FRAGMENT:
		case GPU_VERTEX:
			{
				Ogre::GpuProgramParametersSharedPtr activeParameters = (ActiveShaderDef.ValType == GPU_VERTEX) ? mActiveVertexParameters : mActiveFragmentParameters;
				if (!activeParameters.isNull())
				{
					activeParameters->_writeRawConstant(ActiveShaderDef.PhysicalcalIndex + ActiveShaderDef.ElementIndex,val);
				}
			}
			break;
		case MAT_SPECULAR:
			{
				Ogre::ColourValue OldSpec(mActivePass->getSpecular());
				OldSpec[ActiveShaderDef.ElementIndex] = val;
				mActivePass->setSpecular(OldSpec);
			}
			break;
		case MAT_DIFFUSE:
			{
				// get the specular values from the material pass
				ColourValue OldSpec(mActivePass->getDiffuse());
				OldSpec[ActiveShaderDef.ElementIndex] = val;
				mActivePass->setDiffuse( OldSpec );
			}
			break;

		case MAT_AMBIENT:
			{
				// get the specular values from the material pass
				ColourValue OldSpec(mActivePass->getAmbient());
				OldSpec[ActiveShaderDef.ElementIndex] = val;
				mActivePass->setAmbient( OldSpec );
			}
			break;

		case MAT_SHININESS:
			// get the specular values from the material pass
			mActivePass->setShininess( val );
			break;

		case MAT_EMISSIVE:
			break;
		}
	}

	return true;
}