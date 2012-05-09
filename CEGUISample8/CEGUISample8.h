#ifndef _CEGUISAMPLE8_
#define _CEGUISAMPLE8_

#include "../CEGUIBase/CEGUIBaseApp.h"

class CEGUISample8App:public CEGUIBaseApp
{
public:
	void createGUI();

protected:
	void createDemoWindow();
	void initDemoEventWiring();

	bool handleAddColumn(const CEGUI::EventArgs& e);
	bool handleDeleteColumn(const CEGUI::EventArgs& e);
	bool handleAddRow(const CEGUI::EventArgs& e);
	bool handleDeleteRow(const CEGUI::EventArgs& e);
	bool handleSetItem(const CEGUI::EventArgs& e);
	bool handleSelectChanged(const CEGUI::EventArgs& e);
	bool handleSelectModeChanged(const CEGUI::EventArgs& e);
	bool handleContentsChanged(const CEGUI::EventArgs& e);
};

#endif