#include "CEGUISample5.h"

void CEGUISample5App::createGUI()
{
	using namespace CEGUI;

	// this sample will use WindowsLook
	SchemeManager::getSingleton().create("WindowsLook.scheme");

	// load the default font
	mFont = &FontManager::getSingleton().create("DejaVuSans-10.font");

	// to look more like a real application, we override the autoscale setting
	// for both skin and font
	Imageset& wndlook = ImagesetManager::getSingleton().get("WindowsLook");
	wndlook.setAutoScalingEnabled(false);
	mFont->setProperty("AutoScaled", "false");

	// set the mouse cursor
	mSystem = System::getSingletonPtr();
	mSystem->setDefaultMouseCursor(&wndlook.getImage("MouseArrow"));

	// set the default tooltip type
	mSystem->setDefaultTooltip("WindowsLook/Tooltip");

	// We need the window manager to set up the test interface :)
	mWindowMgr = WindowManager::getSingletonPtr();

	// create a root window
	// this will be a static, to give a nice app'ish background
	mWindow = mWindowMgr->createWindow("WindowsLook/Static");
	mWindow->setProperty("FrameEnabled", "false");
	// root window will take care of hotkeys
    mWindow->subscribeEvent(Window::EventKeyDown, Event::Subscriber(&CEGUISample5App::hotkeysHandler, this));
    mSystem->setGUISheet(mWindow);

	UDim barBottom(0,mFont->getLineSpacing(2));

	Window * bar = mWindowMgr->createWindow("WindowsLook/Menubar");
	bar->setArea(UDim(0.0f,0.0f),UDim(0.0f,0.0f),UDim(1.0f,0.0f),barBottom);
	bar->setAlwaysOnTop(true);
	mWindow->addChildWindow(bar);

	createMenu(bar);

	mPane = static_cast<ScrollablePane *>(mWindowMgr->createWindow("WindowsLook/ScrollablePane"));
	mPane->setArea(URect(UDim(0,0),barBottom,UDim(1,0),UDim(1,0)));
	mPane->setContentPaneAutoSized(false);
	mPane->setContentPaneArea(CEGUI::Rect(0,0,3000,3000));
	mWindow->addChildWindow(mPane);

	Window * dlg = mWindowMgr->createWindow("WindowsLook/FrameWindow");
	dlg->setMinSize(UVector2(UDim(0,250),UDim(0,100)));
	dlg->setSize(UVector2(UDim(0,250),UDim(0,100)));
	dlg->setText("sss");
	mPane->addChildWindow(dlg);
}

void CEGUISample5App::createMenu(CEGUI::Window *bar)
{
	using namespace CEGUI;
	Window * file = mWindowMgr->createWindow("WindowsLook/MenuItem");
	file->setText("File");
	bar->addChildWindow(file);
	
	Window * popup = mWindowMgr->createWindow("WindowsLook/PopupMenu");
	file->addChildWindow(popup);

	Window* item = mWindowMgr->createWindow("WindowsLook/MenuItem");
    item->setText("Shit");
	popup->addChildWindow(item);

	// demo menu item
	Window* demo = mWindowMgr->createWindow("WindowsLook/MenuItem");
	demo->setText("Demo");
    bar->addChildWindow(demo);

    // demo popup
    popup = mWindowMgr->createWindow("WindowsLook/PopupMenu");
    demo->addChildWindow(popup);

    // demo -> new window
    item = mWindowMgr->createWindow("WindowsLook/MenuItem");
    item->setText("New dialog");
	item->setTooltipText("Hotkey: Space");
	item->subscribeEvent("Clicked",Event::Subscriber(&CEGUISample5App::demoNewDialog,this));
	popup->addChildWindow(item);
}

bool CEGUISample5App::demoNewDialog(const CEGUI::EventArgs &args)
{
	using namespace CEGUI;

	// add a dialog to this pane so we have some more stuff to drag around :)
	Window* dlg = mWindowMgr->createWindow("WindowsLook/FrameWindow");
	dlg->setMinSize(UVector2(UDim(0,200),UDim(0,100)));
	dlg->setSize(UVector2(UDim(0,200),UDim(0,100)));
	dlg->setText("too!");
	
	UVector2 uni_center(UDim(0.5f,0), UDim(0.5f,0));
	Vector2 center = CoordConverter::windowToScreen(*mWindow,uni_center);
	Vector2 target = CoordConverter::screenToWindow(*mPane->getContentPane(),center);
	dlg->setPosition(UVector2(UDim(0,target.d_x-100), UDim(0,target.d_y-50)));

	mPane->addChildWindow(dlg);
	return true;
}

/*************************************************************************
    Handler for global hotkeys
*************************************************************************/
bool CEGUISample5App::hotkeysHandler(const CEGUI::EventArgs& e)
{
    using namespace CEGUI;
    const KeyEventArgs& k = static_cast<const KeyEventArgs&>(e);

    switch (k.scancode)
    {
    // space is a hotkey for demo -> new dialog
    case Key::Space:
        // this handler does not use the event args at all so this is fine :)
        // though maybe a bit hackish...
        demoNewDialog(e);
        break;

    // no hotkey found? event not used...
    default:
        return false;
    }

    return true;
}
