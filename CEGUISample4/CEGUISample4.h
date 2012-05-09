#ifndef _CEGUISAMPLE4_
#define _CEGUISAMPLE4_

#include "../CEGUIBase/CEGUIBaseApp.h"

using namespace CEGUI;

#define SKIN "TaharezLook"

static const char *PageText [] = 
{
    "This is page three",
    "And this is page four, it's not too different from page three, isn't it?",
    "Yep, you guessed, this is page five",
    "And this is page six",
    "Seven",
    "Eight",
    "Nine. Quite boring, isn't it?",
    "Ten",
    "Eleven",
    "Twelve",
    "Thirteen",
    "Fourteen",
    "Fifteen",
    "And, finally, sixteen. Congrats, you found the last page!",
};

class CEGUISample4App:public CEGUIBaseApp
{
public:
	void createGUI();
	bool handleAddTab(const CEGUI::EventArgs& args);
	bool handleDel(const CEGUI::EventArgs& args);
	bool handleGoto(const CEGUI::EventArgs& args);
	bool handleShow(const CEGUI::EventArgs& args);
	bool handleTabHeight(const CEGUI::EventArgs& args);
	bool handleTabPadding(const CEGUI::EventArgs& args);
	bool handleTabPanePos(const CEGUI::EventArgs& args);
	void refreshPageList();
};

class MyListItem : public ListboxTextItem
{
public:
	MyListItem(const CEGUI::String& text) : ListboxTextItem(text)
	{
		setSelectionBrushImage(SKIN,"MultiListSelectionBrush");
	}
};
#endif
