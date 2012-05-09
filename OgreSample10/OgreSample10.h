#ifndef _OGRESAMPLE10_
#define _OGRESANPLE10_

#include "../CEGUIBase/CEGUIBaseApp.h"

using namespace Ogre;
using namespace CEGUI;

class OgreSample10App : public CEGUIBaseApp
{
public:
	void createScene();
	bool handleSlides(const CEGUI::EventArgs& args);
	bool handleCheckBox(const CEGUI::EventArgs& args);
	void createGUI();
	void setupControls();
	void frameRenderingQueued(const Ogre::FrameEvent& evt);

protected:
	MeshPtr mHeadMesh;
	AnimationState * mSpeakAnimState;
	AnimationState * mManualAnimState;
	VertexPoseKeyFrame * mManualKeyFrame;
	bool mPlayAnimation;
	CEGUI::FrameWindow * mFrameWindow1;
	CEGUI::FrameWindow * mFrameWindow2;
	CEGUI::Checkbox * mButton;
	std::vector<CEGUI::Slider*> mExpressions;
	std::vector<CEGUI::Slider*> mMouthShapes;
};

#endif