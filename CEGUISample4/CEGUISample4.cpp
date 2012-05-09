#include "CEGUISample4.h"

void CEGUISample4App::createGUI()
{
	// we will use of the WindowManager.
	WindowManager& winMgr = WindowManager::getSingleton ();

	// load scheme and set up defaults
	SchemeManager::getSingleton().create(SKIN ".scheme");
	System::getSingleton().setDefaultMouseCursor (SKIN, "MouseArrow");
	// Ensure font is loaded
	// First font gets set as the default font automatically
	FontManager::getSingleton().create("DejaVuSans-10.font");

	// load an image to use as a background
	ImagesetManager::getSingleton().createFromImageFile("BackgroundImage", "GPN-2000-001437.tga");

	// here we will use a StaticImage as the root, then we can use it to place a background image
	Window* background = winMgr.createWindow (SKIN "/StaticImage");
	// set area rectangle
	background->setArea (URect (cegui_reldim (0), cegui_reldim (0),
		cegui_reldim (1), cegui_reldim (1)));
	// disable frame and standard background
	background->setProperty ("FrameEnabled", "false");
	background->setProperty ("BackgroundEnabled", "false");
	// set the background image
	background->setProperty ("Image", "set:BackgroundImage image:full_image");
	// install this as the root GUI sheet
	System::getSingleton ().setGUISheet (background);

	System::getSingleton().setDefaultTooltip(SKIN "/Tooltip");
	String prefix = "TabControlDemo/";
	background->addChildWindow(winMgr.loadWindowLayout("TabControlDemo.layout",prefix));
	TabControl * tc =static_cast<TabControl *>(winMgr.getWindow("TabControlDemo/Frame/TabControl"));

	tc->addTab (winMgr.loadWindowLayout("TabPage1.layout",prefix));
	tc->addTab (winMgr.loadWindowLayout("TabPage2.layout",prefix));

	WindowManager::WindowIterator it = WindowManager::getSingleton().getIterator();
	for(;!it.isAtEnd();++it)
	{
		const char* windowName = it.getCurrentValue()->getName().c_str();
		printf("Name : %s\n",windowName);
	}

	static_cast<PushButton *> (
		winMgr.getWindow("TabControlDemo/Page1/AddTab"))->subscribeEvent (
		PushButton::EventClicked,
		Event::Subscriber(&CEGUISample4App::handleAddTab, this));

	// Click to visit this tab
	static_cast<PushButton *> (
		winMgr.getWindow("TabControlDemo/Page1/Go"))->subscribeEvent (
		PushButton::EventClicked,
		Event::Subscriber (&CEGUISample4App::handleGoto, this));

	// Click to make this tab button visible (when scrolling is required)
	static_cast<PushButton *> (
		winMgr.getWindow("TabControlDemo/Page1/Show"))->subscribeEvent (
		PushButton::EventClicked,
		Event::Subscriber (&CEGUISample4App::handleShow, this));

	static_cast<PushButton *> (
		winMgr.getWindow("TabControlDemo/Page1/Del"))->subscribeEvent (
		PushButton::EventClicked,
		Event::Subscriber (&CEGUISample4App::handleDel, this));

	RadioButton * rb =static_cast<RadioButton *>(
		winMgr.getWindow("TabControlDemo/Page1/TabPaneTop"));
	rb->setSelected(tc->getTabPanePosition() == TabControl::Top);
	rb->subscribeEvent(
		RadioButton::EventSelectStateChanged,
		Event::Subscriber (&CEGUISample4App::handleTabPanePos, this));

	rb = static_cast<RadioButton *> (
		winMgr.getWindow("TabControlDemo/Page1/TabPaneBottom"));
	rb->setSelected (tc->getTabPanePosition () == TabControl::Bottom);
	rb->subscribeEvent (
		RadioButton::EventSelectStateChanged,
		Event::Subscriber (&CEGUISample4App::handleTabPanePos, this));
	Scrollbar * sb = static_cast<Scrollbar *>(
		winMgr.getWindow("TabControlDemo/Page1/TabHeight"));
	sb->setScrollPosition(tc->getTabHeight().d_offset);
	sb->subscribeEvent(
		Scrollbar::EventScrollPositionChanged,
		Event::Subscriber(&CEGUISample4App::handleTabHeight,this));

	sb = static_cast<Scrollbar *> (
		winMgr.getWindow("TabControlDemo/Page1/TabPadding"));
	sb->setScrollPosition (tc->getTabTextPadding ().d_offset);
	sb->subscribeEvent (
		Scrollbar::EventScrollPositionChanged,
		Event::Subscriber (&CEGUISample4App::handleTabPadding, this));

	refreshPageList();
}

void CEGUISample4App::refreshPageList()
{
	WindowManager& winMgr = WindowManager::getSingleton();

	Listbox * lbox = NULL;
	TabControl * tc = NULL;
	if(winMgr.isWindowPresent("TabControlDemo/Page1/PageList"))
	{
		lbox = static_cast<Listbox *>(winMgr.getWindow("TabControlDemo/Page1/PageList"));
	}
	if(winMgr.isWindowPresent("TabControlDemo/Frame/TabControl"))
	{
		tc = static_cast<TabControl *>(winMgr.getWindow("TabControlDemo/Frame/TabControl"));
	}
	if(lbox && tc)
	{
		lbox->resetList();
		for(unsigned int i =0;i < tc->getTabCount();i++)
		{
			lbox->addItem(new MyListItem(tc->getTabContentsAtIndex(i)->getName()));
		}
	}
}

bool CEGUISample4App::handleTabPanePos (const EventArgs& args)
{
	TabControl::TabPanePosition tpp;
	switch (static_cast<const WindowEventArgs&>(args).window->getID())
	{
	case 0:
		tpp = TabControl::Top;
		break;
	case 1:
		tpp = TabControl::Bottom;
		break;
	default:
		return false;		
	}

	WindowManager& winMgr = WindowManager::getSingleton();
	if(winMgr.isWindowPresent("TabControlDemo/Frame/TabControl"))
	{
		static_cast<TabControl *>(winMgr.getWindow("TabControlDemo/Frame/TabControl"))->setTabPanePosition(tpp);
	}
	return true;
}

bool CEGUISample4App::handleTabHeight (const EventArgs& e)
{
	Scrollbar *sb = static_cast<Scrollbar *> (
		static_cast<const WindowEventArgs&> (e).window);

	// Check if the window exists
	WindowManager& winMgr = WindowManager::getSingleton ();
	if (winMgr.isWindowPresent("TabControlDemo/Frame/TabControl"))
	{
		static_cast<TabControl *> (WindowManager::getSingleton ().getWindow (
			"TabControlDemo/Frame/TabControl"))->setTabHeight (
			UDim (0, sb->getScrollPosition ()));
	}
	return true;
}

bool CEGUISample4App::handleTabPadding (const EventArgs& e)
{
	Scrollbar *sb = static_cast<Scrollbar *> (
		static_cast<const WindowEventArgs&> (e).window);

	// Check if the window exists
	WindowManager& winMgr = WindowManager::getSingleton ();
	if (winMgr.isWindowPresent("TabControlDemo/Frame/TabControl"))
	{
		static_cast<TabControl *> (WindowManager::getSingleton ().getWindow (
			"TabControlDemo/Frame/TabControl"))->setTabTextPadding (
			UDim (0, sb->getScrollPosition ()));
	}
	return true;
}

bool CEGUISample4App::handleAddTab(const EventArgs&)
{
	WindowManager& winMgr = WindowManager::getSingleton();

	if(winMgr.isWindowPresent("TabControlDemo/Frame/TabControl"))
	{
		TabControl * tc = static_cast<TabControl *>(winMgr.getWindow("TabControlDemo/Frame/TabControl"));
		for(int num =3;num <=16;num++)
		{
			std::stringstream prefix;
			prefix << "TabControlDemp/Page" << num;
			if(winMgr.isWindowPresent(prefix.str()))
				continue;

			Window * pg = NULL;
			CEGUI_TRY
			{
				pg = winMgr.loadWindowLayout("TabPage.layout",CEGUI::String(prefix.str()));
			}
			CEGUI_CATCH(CEGUI::Exception&)
			{
				MessageBox(0,L"无此文件",L"无此文件",0);
				break;
			}
			prefix << "Text";
			if(winMgr.isWindowPresent(prefix.str()))
			{
				Window * txt = winMgr.getWindow(prefix.str());
				txt->setText(PageText[num - 3]);

				std::stringstream pgname;
				pgname << "Page" << num;
				pg->setText(pgname.str());
				tc->addTab(pg);

				refreshPageList();
				break;
			}
		}
	}
	return true;
}

bool CEGUISample4App::handleGoto(const CEGUI::EventArgs &args)
{
	WindowManager& winMgr = WindowManager::getSingleton();

	Listbox* lbox = NULL;
	TabControl * tc = NULL;
	if(winMgr.isWindowPresent("TabControlDemo/Page1/PageList"))
	{
		lbox = static_cast<Listbox *>(winMgr.getWindow("TabControlDemo/Page1/PageList"));
	}
	if(winMgr.isWindowPresent("TabControlDemo/Frame/TabControl"))
	{
		tc = static_cast<TabControl *>(winMgr.getWindow("TabControlDemo/Frame/TabControl"));
	}
	if(lbox && tc)
	{
		ListboxItem * lbi = lbox->getFirstSelectedItem();
		if(lbi)
			tc->setSelectedTab(lbi->getText());
	}
	return true;
}

bool CEGUISample4App::handleShow(const CEGUI::EventArgs &args)
{
	WindowManager& winMgr = WindowManager::getSingleton ();
	// Check if the windows exists
	Listbox *lbox = NULL;
	TabControl *tc = NULL;
	if (winMgr.isWindowPresent("TabControlDemo/Page1/PageList"))
	{
		lbox = static_cast<Listbox *> (winMgr.getWindow(
			"TabControlDemo/Page1/PageList"));
	}
	if (winMgr.isWindowPresent("TabControlDemo/Frame/TabControl"))
	{
		tc = static_cast<TabControl *>(winMgr.getWindow (
			"TabControlDemo/Frame/TabControl"));
	}
	if (lbox && tc)
	{
		ListboxItem *lbi = lbox->getFirstSelectedItem ();
		if (lbi)
		{
			tc->makeTabVisible(lbi->getText());
		}
	}
	return true;
}

bool CEGUISample4App::handleDel(const CEGUI::EventArgs &args)
{
	WindowManager& winMgr = WindowManager::getSingleton ();
	// Check if the windows exists
	Listbox *lbox = NULL;
	TabControl *tc = NULL;
	if (winMgr.isWindowPresent("TabControlDemo/Page1/PageList"))
	{
		lbox = static_cast<Listbox *> (winMgr.getWindow(
			"TabControlDemo/Page1/PageList"));
	}
	if (winMgr.isWindowPresent("TabControlDemo/Frame/TabControl"))
	{
		tc = static_cast<TabControl *>(winMgr.getWindow (
			"TabControlDemo/Frame/TabControl"));
	}
	if (lbox && tc)
	{
		ListboxItem *lbi = lbox->getFirstSelectedItem ();
		if (lbi)
		{
			tc->removeTab(lbi->getText());
			winMgr.destroyWindow(lbi->getText());
			refreshPageList();
		}
	}
	return true;
}