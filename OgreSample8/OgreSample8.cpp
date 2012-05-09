#include "OgreSample8.h"

using namespace Ogre;
using namespace CEGUI;

//#define USE_RTSHADER_SYSTEM

void OgreSample8App::loadResources()
{
	BaseApp::loadResources();

#ifdef USE_RTSHADER_SYSTEM
	ResourceGroupManager& rgm = ResourceGroupManager::getSingleton();
	Ogre::StringVector groupVector = Ogre::ResourceGroupManager::getSingleton().getResourceGroups();
	Ogre::StringVector::iterator itGroup = groupVector.begin();
	Ogre::StringVector::iterator itGroupEnd = groupVector.end();
	Ogre::String shaderCoreLibsPath;

	for (;itGroup != itGroupEnd ;++itGroup)
	{
		Ogre::ResourceGroupManager::LocationList resLocationList = Ogre::ResourceGroupManager::getSingleton().getResourceLocationList(*itGroup);
		Ogre::ResourceGroupManager::LocationList::iterator it = resLocationList.begin();
		Ogre::ResourceGroupManager::LocationList::iterator itEnd = resLocationList.end();
		bool coreLibsFound = false;

		for (;it != itEnd; it++)
		{
			if ((*it)->archive->getName().find("RTShaderLib") != Ogre::String::npos)
			{
				shaderCoreLibsPath = (*it)->archive->getName() + "/";
				coreLibsFound = true;
				break;
			}
		}

		if (coreLibsFound)
		{
			break;
		}

		rgm.createResourceGroup("RTShaderSystemMaterialsGroup");
		rgm.addResourceLocation(shaderCoreLibsPath + "materials","FileSystem","RTShaderSystemMaterialGroup");
		rgm.initialiseResourceGroup("RTShaderSystemMaterialsGroup");
		rgm.loadResourceGroup("RTShaderSystemMaterialsGroup",true);
	}
	#endif
}

void OgreSample8App::createScene()
{
	mObjectNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();

	setupModes();
	
	mSceneMgr->setAmbientLight(ColourValue::Black);   // disable ambient lighting

	mObjectNode->attachObject(mSceneMgr->getEntity("ogrehead.mesh"));
	// create pivot nodes
	mLightPivot1 = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	mLightPivot2 = mSceneMgr->getRootSceneNode()->createChildSceneNode();

	Light* l;
	Ogre::BillboardSet * bbs;
	// create white light
	l = mSceneMgr->createLight();
	l->setPosition(200, 0, 0);
	l->setDiffuseColour(1, 1, 1);
	l->setSpecularColour(1, 1, 1);
	// create white flare
	bbs = mSceneMgr->createBillboardSet();
	bbs->setMaterialName("Examples/Flare");
	bbs->createBillboard(200,0,0)->setColour(Ogre::ColourValue::White);
	mLightPivot2->attachObject(l);
	mLightPivot2->attachObject(bbs);

	mMeshMenu->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted,CEGUI::Event::Subscriber(&OgreSample8App::handleItemSelected1,this));

	mMaterialMenu->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted,CEGUI::Event::Subscriber(&OgreSample8App::handleItemSelected2,this));
}

void OgreSample8App::setupModes()
{
	StringVector matNames;

	matNames.push_back("Examples/BumpMapping/MultiLight");
	matNames.push_back("Examples/BumpMapping/MultiLightSpecular");
	matNames.push_back("Examples/OffsetMapping/Specular");
	matNames.push_back("Examples/ShowUV");
	matNames.push_back("Examples/ShowNormals");
	matNames.push_back("Examples/ShowTangents");

	matNames.push_back("RTSS/NormalMapping_SinglePass");
	matNames.push_back("RTSS/NormalMapping_MultiPass");

	mPossibilities["ogrehead.mesh"] = matNames;
	mPossibilities["knot.mesh"] = matNames;

	matNames.clear();
	matNames.push_back("Examples/Athene/NormalMapped");
	matNames.push_back("Examples/Athene/NormalMappedSpecular");
	matNames.push_back("Examples/Athene/NormalMappedSpecular");
	matNames.push_back("Examples/ShowUV");
	matNames.push_back("Examples/ShowNormals");
	matNames.push_back("Examples/ShowTangents");
	matNames.push_back("RTSS/Athene/NormalMapping_SinglePass");
	matNames.push_back("RTSS/Athene/NormalMapping_MultiPass");

	mPossibilities["athene.mesh"] = matNames;

	for (std::map<Ogre::String,Ogre::StringVector>::iterator it = mPossibilities.begin();it != mPossibilities.end();it++)
	{
		Ogre::MeshPtr mesh = MeshManager::getSingleton().load(it->first,ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY);

		unsigned short src,dest;
		if (!mesh->suggestTangentVectorBuildParams(VES_TANGENT,src,dest))
		{
			mesh->buildTangentVectors(VES_TANGENT,src,dest);
		}
		Entity* ent = mSceneMgr->createEntity(mesh->getName(),mesh->getName());
		ent->setMaterialName(it->second.front());
	}

	mMeshMenu->addItem(new CEGUI::ListboxTextItem("athene.mesh"));
	mMeshMenu->addItem(new CEGUI::ListboxTextItem("ogrehead.mesh"));
	mMeshMenu->addItem(new CEGUI::ListboxTextItem("knot.mesh"));
	mMeshMenu->setItemSelectState(size_t(0),true);

	const char * a = mMeshMenu->getSelectedItem()->getText().c_str();
	Ogre::StringVector::iterator it = mPossibilities[a].begin();
	Ogre::StringVector::iterator itEnd = mPossibilities[mMeshMenu->getSelectedItem()->getText().c_str()].end();

	for (;it != itEnd;it++)
	{
		mMaterialMenu->addItem(new ListboxTextItem(it->c_str()));
	}
	mMaterialMenu->setItemSelectState(size_t(0),true);
	mSceneMgr->getEntity(mMeshMenu->getSelectedItem()->getText().c_str())->setMaterialName(mMaterialMenu->getSelectedItem()->getText().c_str());
}

void OgreSample8App::createGUI()
{
	mRootWindow->addChildWindow(CEGUI::WindowManager::getSingleton().loadWindowLayout("OgreSample8.layout"));
	mMeshMenu = static_cast<Combobox*>(WindowManager::getSingleton().getWindow("Root/List1"));
	mMaterialMenu = static_cast<Combobox*>(WindowManager::getSingleton().getWindow("Root/List2"));

}

bool OgreSample8App::handleItemSelected1(const CEGUI::EventArgs& args)
{
	mObjectNode->detachAllObjects();
	mObjectNode->attachObject(mSceneMgr->getEntity(mMeshMenu->getSelectedItem()->getText().c_str()));
	mMaterialMenu->clearAllSelections();
	Ogre::StringVector::iterator it = mPossibilities[mMeshMenu->getSelectedItem()->getText().c_str()].begin();
	Ogre::StringVector::iterator itEnd = mPossibilities[mMeshMenu->getSelectedItem()->getText().c_str()].end();

	mMaterialMenu->clearAllSelections();
	for (;it != itEnd;it++)
	{
		mMaterialMenu->addItem(new ListboxTextItem(it->c_str()));
	}

	mMaterialMenu->setItemSelectState(size_t(0),true);
	mSceneMgr->getEntity(mMeshMenu->getSelectedItem()->getText().c_str())->setMaterialName(mMaterialMenu->getSelectedItem()->getText().c_str());
	return true;
}

bool OgreSample8App::handleItemSelected2(const CEGUI::EventArgs& args)
{
	//ListboxTextItem* item = (ListboxTextItem*)(static_cast<const CEGUI::WindowEventArgs&>(args).window);
	mSceneMgr->getEntity(mMeshMenu->getSelectedItem()->getText().c_str())->setMaterialName(mMaterialMenu->getSelectedItem()->getText().c_str());
	return true;
}

void OgreSample8App::frameRenderingQueued(const FrameEvent& evt)
{
	// rotate the light pivots
	mLightPivot1->roll(Degree(evt.timeSinceLastFrame * 130));
	mLightPivot2->roll(Degree(evt.timeSinceLastFrame * 110));
}