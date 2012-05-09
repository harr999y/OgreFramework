#include "OgreSample13.h"

Ogre::String meshes[] =
{
	"razor",
	"knot",
	"tudorhouse",
	"WoodPallet"
};


void OgreSample13App::createScene()
{
	// Set ambient light
	mSceneMgr->setAmbientLight(ColourValue(0.2f, 0.2f, 0.2f));
	Light* l = mSceneMgr->createLight("MainLight");
	//add a skybox
	mSceneMgr->setSkyBox(true, "Examples/MorningSkyBox", 1000);
	//setup the light
	l->setType(Light::LT_DIRECTIONAL);
	l->setDirection(-0.5, -0.5, 0);


	Plane plane;
	plane.normal = Vector3::UNIT_Y;
	plane.d = 100;
	MeshManager::getSingleton().createPlane("Myplane",
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,
		1500,1500,20,20,true,1,5,5,Vector3::UNIT_Z);
	Entity* pPlaneEnt = mSceneMgr->createEntity( "plane", "Myplane" );
	pPlaneEnt->setMaterialName("Examples/Rockwall");
	pPlaneEnt->setCastShadows(false);
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(pPlaneEnt);

	CompositorManager::getSingleton().addCompositor(mCameraMan->getCamera()->getViewport(),"Bloom");

	//const GpuProgramManager::SyntaxCodes & syntaxCodes = GpuProgramManager::getSingleton().getSupportedSyntax();

	mNumMeshes = 160;
	mNumRendered = 0;
	mSelectedMesh = 0;
	mBurnAmount = 0;
	mCurrentGeomOpt = INSTANCE_OPT;
	createCurrentGeomOpt();
	mTimer = new Ogre::Timer();
	mLastTime = mTimer->getMicroseconds() / 1000000.0f;
}

void OgreSample13App::createCurrentGeomOpt()
{
	objectCount = mNumMeshes;
	mNumRendered = 1;

	while (objectCount > maxObjectsPerBatch)
	{
		mNumRendered++;
		objectCount -= maxObjectsPerBatch;
	}
	assert(mSelectedMesh < numTypeMeshes);
	MeshPtr m = MeshManager::getSingleton().getByName(meshes[mSelectedMesh] + ".mesh");
	if (m.isNull())
	{
		m = MeshManager::getSingleton().load(meshes[mSelectedMesh] + ".mesh",ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
	}
	const Real radius = m->getBoundingSphereRadius();

	posMatrices.resize(mNumRendered);  ///??????
	posMatrices.reserve(mNumRendered); ///????????

	vector<Vector3 *>::type posMatCurr;
	posMatCurr.resize(mNumRendered);
	posMatCurr.reserve(mNumRendered);
	for (size_t i=0;i<mNumRendered;i++)
	{
		posMatrices[i] = new Vector3[mNumMeshes];
		posMatCurr[i] = posMatrices[i];
	}

	size_t i = 0,j = 0;
	for (size_t p=0;p<mNumMeshes;p++)
	{
		for (size_t k=0;k<mNumRendered;k++)
		{
			posMatCurr[k]->x = radius * i;
			posMatCurr[k]->y = k * radius;
			posMatCurr[k]->z = radius * j;
			posMatCurr[k]++;
		}
		if (++j == 10)
		{
			j=0;
			i++;
		}
	}
	posMatCurr.clear();

	switch(mCurrentGeomOpt)
	{
	case INSTANCE_OPT:
		createInstanceGeom();
		break;
	case STATIC_OPT:
		createStaticGeom();
		break;
	case ENTITY_OPT:
		createEntityGeom();
		break;
	}
}

void OgreSample13App::createInstanceGeom()
{
	/*if (Root::getSingleton().getRenderSystem()->getCapabilities()->hasCapability(RSC_VERTEX_PROGRAM) == false)
	{
	}*/

	Entity * ent = mSceneMgr->createEntity(meshes[mSelectedMesh],meshes[mSelectedMesh] + ".mesh");

	renderInstance.reserve(mNumRendered);
	renderInstance.resize(mNumRendered);

	InstancedGeometry * batch = new InstancedGeometry(mSceneMgr,meshes[mSelectedMesh] + "s");
	batch->setCastShadows(true);
	batch->setBatchInstanceDimensions(Vector3(1000000,1000000,1000000));
	const size_t batchSize = (mNumMeshes > maxObjectsPerBatch) ? maxObjectsPerBatch : mNumMeshes;

	setupInstancedMaterialToEntity(ent);

	for (size_t i=0;i<batchSize;i++)
	{
		batch->addEntity(ent,Vector3::ZERO);
	}
	batch->setOrigin(Vector3::ZERO);
	batch->build();

	for (size_t k=0;k<mNumRendered-1;k++)
	{
		batch->addBatchInstance();
	}

	InstancedGeometry::BatchInstanceIterator regIt = batch->getBatchInstanceIterator();
	size_t k=0;
	while (regIt.hasMoreElements())
	{
		InstancedGeometry::BatchInstance * r = regIt.getNext();
		InstancedGeometry::BatchInstance::InstancedObjectIterator bit = r->getObjectIterator();
		int j=0;
		while (bit.hasMoreElements())
		{
			InstancedGeometry::InstancedObject* obj = bit.getNext();
			const Vector3 position (posMatrices[k][j]);
			obj->setPosition(position);
			++j;
		}
		k++;
	}
	batch->setVisible(true);
	renderInstance[0] = batch;
	mSceneMgr->destroyEntity(ent);
}

void OgreSample13App::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	burnCPU();
}

void OgreSample13App::burnCPU()
{
	double mStartTime = mTimer->getMicroseconds() / 1000000.0f;
	double mCurTime = mStartTime;
	double mStopTime = mLastTime + mBurnAmount;
	double mCPUUsage;

	while (mCurTime < mStopTime)
	{
		mCurTime = mTimer->getMicroseconds() / 1000000.0f;
	}
	if (mCurTime - mLastTime > 0.00001f)
	{
		mCPUUsage = (mCurTime - mStartTime) / (mCurTime - mLastTime) * 100.0f;
	}
	else
		mCPUUsage = FLT_MAX;
	mLastTime = mTimer->getMicroseconds() / 1000000.0f;
}

void OgreSample13App::destroyCurrentGeomOpt()
{
	switch(mCurrentGeomOpt)
	{
	case INSTANCE_OPT: destroyInstanceGeom(); break;
	case STATIC_OPT: destroyStaticGeom(); break;
	case ENTITY_OPT: destroyEntityGeom(); break;
	}

	assert(mNumRendered == posMatrices.size());
	for (size_t i=0;i<mNumRendered;i++)
	{
		delete [] posMatrices[i];
	}
	posMatrices.clear();
}

void OgreSample13App::setupInstancedMaterialToEntity(Ogre::Entity *ent)
{
	for (Ogre::uint i=0;i<ent->getNumSubEntities();++i)
	{
		SubEntity * se = ent->getSubEntity(i);
		String materialName = se->getMaterialName();
		se->setMaterialName(buildInstancedMaterial(materialName));
	}
}

String OgreSample13App::buildInstancedMaterial(const String &originalMaterialName)
{
	if (StringUtil::endsWith(originalMaterialName,"/instanced"))
	{
		return originalMaterialName;
	}

	MaterialPtr originalMaterial = MaterialManager::getSingleton().getByName(originalMaterialName);
	const String instancedMaterialName(originalMaterial.isNull() ? "Instancing" : originalMaterialName + "/Instanced");
	MaterialPtr instancedMaterial = MaterialManager::getSingleton().getByName(instancedMaterialName);
	if (instancedMaterial.isNull())
	{
		instancedMaterial = originalMaterial->clone(instancedMaterialName);
		instancedMaterial->load();
		Technique::PassIterator pIt = instancedMaterial->getBestTechnique()->getPassIterator();
		while(pIt.hasMoreElements())
		{
			Pass * const p = pIt.getNext();
			p->setVertexProgram("Instancing",false);
			p->setShadowCasterVertexProgram("InstancingShadowCaster");
		}
	}
	instancedMaterial->load();
	return instancedMaterialName;
}

void OgreSample13App::destroyInstanceGeom()
{
	delete renderInstance[0];
	renderInstance.clear();
}

void OgreSample13App::createStaticGeom()
{
	Entity * ent = mSceneMgr->createEntity(meshes[mSelectedMesh],meshes[mSelectedMesh] + ".mesh");
	renderStatic.reserve(mNumRendered);
	renderStatic.resize(mNumRendered);

	StaticGeometry * geom = new StaticGeometry(mSceneMgr,meshes[mSelectedMesh] + "s");
	geom->setRegionDimensions(Vector3(1000000,1000000,1000000));
	size_t k=0;
	size_t y=0;

	for(size_t i=0;i<mNumMeshes;i++)
	{
		if (y == maxObjectsPerBatch)
		{
			y=0;
			k++;
		}
		geom->addEntity(ent,posMatrices[k][y]);
		y++;
	}
	geom->setCastShadows(true);
	geom->build();
	renderStatic[0] = geom;
	mSceneMgr->destroyEntity(ent);
}

void OgreSample13App::destroyStaticGeom()
{
	delete renderStatic[0];
	renderStatic.clear();
}

void OgreSample13App::createEntityGeom()
{
	size_t k=0;
	size_t y=0;
	renderEntity.reserve(mNumMeshes);
	renderEntity.resize(mNumMeshes);
	nodes.reserve(mNumMeshes);
	nodes.resize(mNumMeshes);

	for (size_t i=0;i<mNumMeshes;i++)
	{
		if ( y == maxObjectsPerBatch)
		{
			y=0;
			k++;
		}

		nodes[i] = mSceneMgr->getRootSceneNode()->createChildSceneNode("node" + StringConverter::toString(i));
		renderEntity[i] = mSceneMgr->createEntity(meshes[mSelectedMesh] + StringConverter::toString(i),meshes[mSelectedMesh] + ".mesh");
		nodes[i]->attachObject(renderEntity[i]);
		nodes[i]->setPosition(posMatrices[k][y]);

		y++;
	}
}

void OgreSample13App::destroyEntityGeom()
{
	size_t i;
	size_t j=0;
	for (i=0;i<mNumMeshes;i++)
	{
		String name = nodes[i]->getName();
		mSceneMgr->destroySceneNode(name);
		mSceneMgr->destroyEntity(renderEntity[i]);
		j++;
	}
}

void OgreSample13App::setCurrentGeometryOpt(CurrentGeomOpt opt)
{
	mCurrentGeomOpt = opt;
}

void OgreSample13App::createGUI()
{
	CEGUI::WindowManager& windowMgr = CEGUI::WindowManager::getSingleton();
	CEGUI::FontManager::getSingleton().create("chinese-15.font");

	mRootWindow->addChildWindow(windowMgr.loadWindowLayout("OgreSample13.layout"));

	CEGUI::Combobox * cb = (CEGUI::Combobox * )windowMgr.getWindow("OgreSample13/select1");
	cb->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted,CEGUI::Event::Subscriber(&OgreSample13App::handleSelected,this));
	cb->addItem(new CEGUI::ListboxTextItem("Instancing",0));
	cb->addItem(new CEGUI::ListboxTextItem("Static Geometry",1));
	cb->addItem(new CEGUI::ListboxTextItem("Independent Entities",2));

	cb = (CEGUI::Combobox * )windowMgr.getWindow("OgreSample13/select2");
	cb->addItem(new CEGUI::ListboxTextItem(meshes[0],0));
	cb->addItem(new CEGUI::ListboxTextItem(meshes[1],1));
	cb->addItem(new CEGUI::ListboxTextItem(meshes[2],2));
	cb->addItem(new CEGUI::ListboxTextItem(meshes[3],3));
	cb->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted,CEGUI::Event::Subscriber(&OgreSample13App::handleSelected,this));
	windowMgr.getWindow("OgreSample13/slider1")->subscribeEvent(CEGUI::Slider::EventValueChanged,CEGUI::Event::Subscriber(&OgreSample13App::handleSliders,this));
	windowMgr.getWindow("OgreSample13/slider2")->subscribeEvent(CEGUI::Slider::EventValueChanged,CEGUI::Event::Subscriber(&OgreSample13App::handleSliders,this));
	windowMgr.getWindow("OgreSample13/checkbox1")->subscribeEvent(CEGUI::Checkbox::EventCheckStateChanged,CEGUI::Event::Subscriber(&OgreSample13App::handleCheckboxs,this));
	windowMgr.getWindow("OgreSample13/checkbox2")->subscribeEvent(CEGUI::Checkbox::EventCheckStateChanged,CEGUI::Event::Subscriber(&OgreSample13App::handleCheckboxs,this));
}

bool OgreSample13App::handleCheckboxs(const CEGUI::EventArgs & args)
{
	CEGUI::Checkbox * cb = static_cast<CEGUI::Checkbox *>(static_cast<const CEGUI::WindowEventArgs&>(args).window);
	if (cb->getName() == CEGUI::String("OgreSample13/checkbox2"))
	{
		if (cb->isSelected())
		{
			mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);
			mSceneMgr->setShadowFarDistance(1000.0f);
		}
		else
		{
			mSceneMgr->setShadowTechnique(SHADOWTYPE_NONE);
		}
	}
	else
	{
		CompositorManager::getSingleton().setCompositorEnabled(mCameraMan->getCamera()->getViewport(),"Bloom",cb->isSelected());
	}
	return true;
}

bool OgreSample13App::handleSelected(const CEGUI::EventArgs & args)
{
	CEGUI::Combobox * cb = static_cast<CEGUI::Combobox *>(static_cast<const CEGUI::WindowEventArgs&>(args).window);
	if (cb->getName() == CEGUI::String("OgreSample13/select1"))
	{
		CurrentGeomOpt selectedOption = (CurrentGeomOpt)cb->getSelectedItem()->getID();
		destroyCurrentGeomOpt();
		setCurrentGeometryOpt(selectedOption);
		createCurrentGeomOpt();
	}
	else
	{
		destroyCurrentGeomOpt();
		mSelectedMesh = cb->getSelectedItem()->getID();
		createCurrentGeomOpt();
	}
	return true;
}

bool OgreSample13App::handleSliders(const CEGUI::EventArgs & args)
{
	CEGUI::Slider * cb = static_cast<CEGUI::Slider *>(static_cast<const CEGUI::WindowEventArgs&>(args).window);
	if (cb->getName() == CEGUI::String("OgreSample13/slider1"))
	{
		destroyCurrentGeomOpt();
		mNumMeshes = cb->getCurrentValue() * 1000.0f;
		CEGUI::WindowManager::getSingleton().getWindow("OgreSample13/num1")->setText(CEGUI::PropertyHelper::intToString(mNumMeshes));
		createCurrentGeomOpt();
	}
	else
	{
		mBurnAmount = cb->getCurrentValue() * 16.67;
		CEGUI::WindowManager::getSingleton().getWindow("OgreSample13/num2")->setText(CEGUI::PropertyHelper::floatToString(mBurnAmount));
	}

	return true;
}