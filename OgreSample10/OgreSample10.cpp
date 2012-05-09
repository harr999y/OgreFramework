#include "OgreSample10.h"

void OgreSample10App::frameRenderingQueued(const Ogre::FrameEvent &evt)
{
	if (mPlayAnimation)
	{
		mSpeakAnimState->addTime(evt.timeSinceLastFrame);
	}
}

void OgreSample10App::createScene()
{
	// setup some basic lighting for our scene
	mSceneMgr->setAmbientLight(ColourValue(0.5, 0.5, 0.5));
	mSceneMgr->createLight()->setPosition(40, 60, 50);
	mSceneMgr->createLight()->setPosition(-120, -80, -50);

	mHeadMesh = MeshManager::getSingleton().load("facial.mesh",ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	mManualKeyFrame = mHeadMesh->createAnimation("Manual",0)->createVertexTrack(4,VAT_POSE)->createVertexPoseKeyFrame(0);

	for (int i=0;i<15;i++)
	{
		mManualKeyFrame->addPoseReference(i,0);
	}
	Entity* head = mSceneMgr->createEntity("Head", "facial.mesh");
	mSceneMgr->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(0, 0, -10))->attachObject(head);
	mCameraMan->getCamera()->setPosition(0,35,100);
	mCameraMan->getCamera()->setDirection(0,0,-1);
	// get the animation states
	mSpeakAnimState = head->getAnimationState("Speak");
	mPlayAnimation = true;
	mSpeakAnimState->setEnabled(true);
	mSpeakAnimState->setLoop(true);
	mManualAnimState = head->getAnimationState("Manual");

	setupControls();


}

void OgreSample10App::createGUI()
{
	CEGUI::FontManager::getSingleton().create("chinese-15.font");
	CEGUI::Window * window = CEGUI::WindowManager::getSingleton().loadWindowLayout("OgreSample10.layout");
	mRootWindow->addChildWindow(window);
	mButton = (CEGUI::Checkbox*)window->getChild("Root/button");
	mFrameWindow1 = (CEGUI::FrameWindow*)window->getChild("Root/frame1");
	mFrameWindow2 = (CEGUI::FrameWindow*)window->getChild("Root/frame2");

}

void OgreSample10App::setupControls()
{
	CEGUI::Slider * slider;
	CEGUI::Editbox * editBox;
	int num1 = 0,num2 = 0;
	for (unsigned int i=0;i<mManualKeyFrame->getPoseReferences().size();i++)
	{
		CEGUI::String sliderName = CEGUI::String((CEGUI::utf8*)"姿势") + CEGUI::PropertyHelper::intToString(i);
		CEGUI::String poseName = mHeadMesh->getPose(i)->getName().c_str();
		slider = (CEGUI::Slider *)CEGUI::WindowManager::getSingleton().createWindow("WindowsLook/Slider");
		editBox = (CEGUI::Editbox *)CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/Editbox");
		editBox->setFont("Chinese-small");
		editBox->setText((CEGUI::utf8*)sliderName.c_str());
		editBox->setReadOnly(true);
		slider->setID(i);
		slider->setAlwaysOnTop(true);

		slider->subscribeEvent(CEGUI::Slider::EventValueChanged,CEGUI::Event::Subscriber(&OgreSample10App::handleSlides,this));
		if (poseName.find("Expression") != std::string::npos)
		{
			slider->setArea(UDim(0.2f,10),UDim(0.2f * num1,10),UDim(0.7f,0),UDim(0.1f,30));
			editBox->setArea(UDim(0.001f,0),UDim(0.2f * num1,10),UDim(0.2f,0),UDim(0.1f,30));
			num1++;
			mFrameWindow1->addChildWindow(slider);
			mFrameWindow1->addChildWindow(editBox);
			mExpressions.push_back(slider);
		}
		else
		{
			slider->setArea(UDim(0.2f,10),UDim(0.09f * num2,10),UDim(0.7f,0),UDim(0.1f,30));
			editBox->setArea(UDim(0.001f,0),UDim(0.09f * num2,10),UDim(0.2f,0),UDim(0.1f,30));
			editBox->setHeight(UDim(0.12f,0));
			slider->setHeight(UDim(0.10f,0));
			num2++;
			mFrameWindow2->addChildWindow(slider);	
			mFrameWindow2->addChildWindow(editBox);
			mMouthShapes.push_back(slider);
		}
	}
	mButton->subscribeEvent(CEGUI::Checkbox::EventCheckStateChanged,CEGUI::Event::Subscriber(&OgreSample10App::handleCheckBox,this));

	mFrameWindow1->hide();
	mFrameWindow2->hide();
}

bool OgreSample10App::handleCheckBox(const CEGUI::EventArgs& args)
{
	if (mPlayAnimation)
	{
		mPlayAnimation = false;
		mSpeakAnimState->setEnabled(false);
		mSpeakAnimState->setLoop(false);
		mFrameWindow1->show();
		mFrameWindow2->show();	
		mManualAnimState->setEnabled(true);
		//mFrameWindow1->activate();
		//mFrameWindow2->activate();
	}
	else
	{
		mPlayAnimation = true;
		mManualAnimState->setEnabled(false);
		mSpeakAnimState->setEnabled(true);
		mSpeakAnimState->setLoop(true);
		mFrameWindow1->hide();
		mFrameWindow2->hide();
	}
	return true;
}

bool OgreSample10App::handleSlides(const CEGUI::EventArgs &args)
{
	const CEGUI::Slider * slider = static_cast<CEGUI::Slider *>(static_cast<const CEGUI::WindowEventArgs&>(args).window);
	Ogre::ushort i = slider->getID();
	mManualKeyFrame->updatePoseReference(i,slider->getCurrentValue());
	mManualAnimState->getParent()->_notifyDirty();
	return true;
}