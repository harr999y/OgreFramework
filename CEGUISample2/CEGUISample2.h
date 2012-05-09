#ifndef _CEGUISAMPLE2_
#define _CEGUISAMPLE2_

#include "../CEGUIBase/CEGUIBaseApp.h"

using namespace CEGUI;

class CEGUISample2App:public CEGUIBaseApp
{
public:
	void createGUI();

private:
	void initStaticText();
	void initSingleLineEdit();
	void initMultiLineEdit();

	void initRadio(const CEGUI::String& radio,int group,bool selected);
	void subscribeEvent(const CEGUI::String& widgetName,const CEGUI::String& eventName,const Event::Subscriber& method);
	bool isRadioSelected(const CEGUI::String & radio);
	bool isCheckboxSelected(const CEGUI::String & checkbox);
	bool formatChangedHandler(const CEGUI::EventArgs& e);
	bool vertScrollChangedHandler(const EventArgs& e);
};

#endif