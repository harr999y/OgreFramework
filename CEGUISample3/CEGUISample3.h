#ifndef _CEGUISAMPLE3_
#define _CEGUISAMPLE3_

#include "../CEGUIBase/CEGUIBaseApp.h"

class CEGUISample3App:public CEGUIBaseApp
{
public:
	void createGUI();

protected:
	CEGUI::Window * mTreeWindow;
	static const unsigned int TreeID;
	static const unsigned int EditBoxID;

	bool handleRootKeyDown(const CEGUI::EventArgs& args);
	bool handleEventSelectionChanged(const CEGUI::EventArgs& args);
	bool handleEventBranchOpened(const CEGUI::EventArgs& args);
	bool handleEventBranchClosed(const CEGUI::EventArgs& args);
};

#endif