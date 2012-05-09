#ifndef _CEGUISAMPLE1_
#define _CEGUISAMPLE1_

#include "../CEGUIBase/CEGUIBaseApp.h"

class CEGUISample1App: public CEGUIBaseApp
{
public:
	void subscribeEvents();
	bool handleItemDropped(const CEGUI::EventArgs& args);
	bool handleCloseButton(const CEGUI::EventArgs& args);
	void createGUI();
	void doSome();
};

#endif