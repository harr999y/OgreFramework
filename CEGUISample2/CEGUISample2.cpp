#include "CEGUISample2.h"

using namespace CEGUI;

void CEGUISample2App::createGUI()
{
	// we will make extensive use of the WindowManager.
    WindowManager& winMgr = WindowManager::getSingleton();

    // load scheme and set up defaults
    SchemeManager::getSingleton().create("TaharezLook.scheme");
    System::getSingleton().setDefaultMouseCursor("TaharezLook", "MouseArrow");
    // We need a font
	CEGUI::FontManager::getSingleton().create("DejaVuSans-10.font");
    // Font defaulting
	if(CEGUI::FontManager::getSingleton().isDefined("DejaVuSans-10"))
    {
		System::getSingleton().setDefaultFont("DejaVuSans-10");
    }

	CEGUI::ImagesetManager::getSingleton().createFromImageFile("BackgroundImage","GPN-2000-001437.tga");
	// here we will use a StaticImage as the root, then we can use it to place a background image
    Window* background = winMgr.createWindow("TaharezLook/StaticImage", "background_wnd");
    // set position and size
    background->setPosition(UVector2(cegui_reldim(0), cegui_reldim( 0)));
    background->setSize(UVector2(cegui_reldim(1), cegui_reldim( 1)));
    // disable frame and standard background
	background->setProperty("FrameEnabled","false");
	background->setProperty("BackgroundEnabled","false");
	background->setProperty("Image","set:BackgroundImage image:full_image");
	::System::getSingleton().setGUISheet(background);
	background->addChildWindow(winMgr.loadWindowLayout("TextDemo.layout"));

	initStaticText();
	initSingleLineEdit();
	initMultiLineEdit();
}

void CEGUISample2App::initStaticText()
{
	initRadio("TextDemo/HorzLeft",0,true);
	initRadio("TextDemo/HorzRight", 0, false);
    initRadio("TextDemo/HorzCentered", 0, false);
    // New group!
    initRadio("TextDemo/VertTop", 1, true);
    initRadio("TextDemo/VertBottom", 1, false);
    initRadio("TextDemo/VertCentered", 1, false);

    subscribeEvent("TextDemo/Wrap", Checkbox::EventCheckStateChanged, Event::Subscriber(&CEGUISample2App::formatChangedHandler, this));
    subscribeEvent("TextDemo/HorzLeft", RadioButton::EventSelectStateChanged, Event::Subscriber(& CEGUISample2App::formatChangedHandler, this));
    subscribeEvent("TextDemo/HorzRight", RadioButton::EventSelectStateChanged, Event::Subscriber(& CEGUISample2App::formatChangedHandler, this));
    subscribeEvent("TextDemo/HorzCentered", RadioButton::EventSelectStateChanged, Event::Subscriber(& CEGUISample2App::formatChangedHandler, this));
    subscribeEvent("TextDemo/VertTop", RadioButton::EventSelectStateChanged, Event::Subscriber(& CEGUISample2App::formatChangedHandler, this));
    subscribeEvent("TextDemo/VertBottom", RadioButton::EventSelectStateChanged, Event::Subscriber(& CEGUISample2App::formatChangedHandler, this));
    subscribeEvent("TextDemo/VertCentered", RadioButton::EventSelectStateChanged, Event::Subscriber(& CEGUISample2App::formatChangedHandler, this));
}

void CEGUISample2App::initSingleLineEdit()
{
	WindowManager& winMgr = WindowManager::getSingleton();
	if(winMgr.isWindowPresent("TextDemo/editAge"))
	{
		static_cast<::Editbox*>(winMgr.getWindow("TextDemo/editAge"))->setValidationString("[0-9]*");
	}

	if(winMgr.isWindowPresent("TextDemo/editAge"))
	{
		::Editbox* passwd = static_cast<Editbox*>(winMgr.getWindow("TextDemo/editPasswd"));
		passwd->setValidationString("[A-Za-z0-9]*");
		passwd->setTextMasked(true);
	}
}

void CEGUISample2App::initMultiLineEdit()
{
	subscribeEvent("TextDemo/forceScroll",::Checkbox::EventCheckStateChanged,Event::Subscriber(&CEGUISample2App::vertScrollChangedHandler,this));
}

void CEGUISample2App::initRadio(const CEGUI::String &radio, int group, bool selected)
{
	WindowManager& winMgr = WindowManager::getSingleton();
    if (winMgr.isWindowPresent(radio))
    {
		::RadioButton* button = static_cast<RadioButton*>(winMgr.getWindow(radio));
		button->setGroupID(group);
		button->setSelected("");
	}
}

void CEGUISample2App::subscribeEvent(const CEGUI::String &widgetName, const CEGUI::String &eventName, const CEGUI::Event::Subscriber &method)
{
	::WindowManager& winMgr = ::WindowManager::getSingleton();
	if(winMgr.isWindowPresent(widgetName))
	{
		Window* window = winMgr.getWindow(widgetName);
		window->subscribeEvent(eventName,method);
	}
}

bool CEGUISample2App::isCheckboxSelected(const CEGUI::String &checkbox)
{
	WindowManager& winMgr = WindowManager::getSingleton();

	if(winMgr.isWindowPresent(checkbox))
	{
		Checkbox* button = static_cast<Checkbox*>(winMgr.getWindow(checkbox));
		return button->isSelected();
	}
	return false;
}

bool CEGUISample2App::isRadioSelected(const CEGUI::String& radio)
{
    WindowManager& winMgr = WindowManager::getSingleton();
    // Check
    if (winMgr.isWindowPresent(radio))
    {
        RadioButton* button = static_cast<RadioButton*>(winMgr.getWindow(radio));
        return button->isSelected();
    }
    return false;
}

bool CEGUISample2App::formatChangedHandler(const CEGUI::EventArgs &e)
{
	::WindowManager& winMgr = ::WindowManager::getSingleton();

	if(winMgr.isWindowPresent("TextDemo/StaticText"))
	{
		Window* st = winMgr.getWindow("TextDemo/StaticText");
		if(isRadioSelected("TextDemo/VertTop"))
			st->setProperty("VertFormatting","TopAligned");
		else if(isRadioSelected("TextDemo/VertBottom"))
			st->setProperty("VertFormatting","BottomAligned");
        else if (isRadioSelected("TextDemo/VertCentered"))
            st->setProperty("VertFormatting", "VertCentred");

		bool wrap = isCheckboxSelected("TextDemo/Wrap");

		if(isRadioSelected("TextDemo/HorzLeft"))
			st->setProperty("HorzFormatting",wrap ? "WordWrapLeftAligned" : "LeftAligned");
		else if (isRadioSelected("TextDemo/HorzRight"))
            st->setProperty("HorzFormatting", wrap ? "WordWrapRightAligned" : "RightAligned");
        else if (isRadioSelected("TextDemo/HorzCentered"))
            st->setProperty("HorzFormatting", wrap ? "WordWrapCentred" : "HorzCentred");
	}
	return true;
}

bool CEGUISample2App::vertScrollChangedHandler(const CEGUI::EventArgs &e)
{
	WindowManager& winMgr = WindowManager::getSingleton();
	if(winMgr.isWindowPresent("TextDemo/editMulti"))
	{
		MultiLineEditbox* multiEdit = static_cast<::MultiLineEditbox*>(winMgr.getWindow("TextDemo/editMulti"));
		multiEdit->setShowVertScrollbar(isCheckboxSelected("TextDemo/forceScroll"));
	}
	return true;
}
