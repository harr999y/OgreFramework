#ifndef _CEGUISAMPLE7_
#define _CEGUISAMPLE7_

#include "../CEGUIBase/CEGUIBaseApp.h"

class DemoConsole
{
public :
	DemoConsole(const CEGUI::String& mIdName,CEGUI::Window* parent = 0);
	~DemoConsole();

	void toggleVisibility();
	bool isVisible() const;

private:
	static const unsigned int SubmitButtonID;
	static const unsigned int EntryBoxID;
	static const unsigned int HistoryID;

	bool handleSubmit(const CEGUI::EventArgs& args);
	bool handleKeyDown(const CEGUI::EventArgs& args);

	CEGUI::Window * mWindow;
	int mHistoryPos;
	std::vector<CEGUI::String> mHistory;
};

class CEGUISample7App : public CEGUIBaseApp
{
public :
	~CEGUISample7App();
	void createGUI();

	bool handleRootKeyDown(const CEGUI::EventArgs& args);

	DemoConsole *mConsole;
};



#endif