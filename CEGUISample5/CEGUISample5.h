#ifndef _CEGUISAMPLE5_
#define _CEGUISAMPLE5_

#include "../CEGUIBase/CEGUIBaseApp.h"

class CEGUISample5App: public CEGUIBaseApp
{
public:
	void createGUI();
	void createMenu(CEGUI::Window * bar);
	bool demoNewDialog(const CEGUI::EventArgs& args);
	bool hotkeysHandler(const CEGUI::EventArgs &args);

private:
	CEGUI::WindowManager * mWindowMgr;
	CEGUI::System * mSystem;
	CEGUI::Window * mWindow;
	CEGUI::Font * mFont;
	CEGUI::ScrollablePane * mPane;
};

#endif