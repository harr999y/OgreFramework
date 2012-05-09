#include "CEGUISample1.h"

void CEGUISample1App::subscribeEvents()
{
	using namespace CEGUI;

	WindowManager& wmgr = WindowManager::getSingleton();

	CEGUI_TRY
	{
		Window* main_wnd = wmgr.getWindow("Root/MainWindow");
		main_wnd->subscribeEvent(FrameWindow::EventCloseClicked,Event::Subscriber(&CEGUISample1App::handleCloseButton,this));
	}
	CEGUI_CATCH(CEGUI::Exception&)
	{}

	CEGUI::String base_name = "Root/MainWindow/Slot";

	for(int i = 1;i <= 12; ++i)
	{
		CEGUI_TRY
		{
			CEGUI::Window* wnd = wmgr.getWindow(base_name + CEGUI::PropertyHelper::intToString(i));

			wnd->subscribeEvent(Window::EventDragDropItemDropped,Event::Subscriber(&CEGUISample1App::handleItemDropped,this));
		}
		CEGUI_CATCH(CEGUI::Exception&)
		{}
	}
}

bool CEGUISample1App::handleItemDropped(const CEGUI::EventArgs& args)
{
	using namespace CEGUI;

	const CEGUI::DragDropEventArgs& ddArgs = static_cast<const DragDropEventArgs&>(args);

	if(!ddArgs.window->getChildCount())
	{
		ddArgs.window->addChildWindow(ddArgs.dragDropItem);
		ddArgs.dragDropItem->setPosition(CEGUI::UVector2(CEGUI::UDim(0.05f,0),CEGUI::UDim(0.05f,0)));
	}

	return true;
}

bool CEGUISample1App::handleCloseButton(const CEGUI::EventArgs& args)
{
	return true;
}

void CEGUISample1App::doSome()
{
	subscribeEvents();
}

void CEGUISample1App::createGUI()
{
	CEGUI::SchemeManager::getSingleton().create("WindowsLook.scheme");
	CEGUI::FontManager::getSingleton().create("DejaVuSans-10.font");
	CEGUI::System::getSingleton().setDefaultMouseCursor("WindowsLook","MouseArrow");
	CEGUI::System::getSingleton().setDefaultFont("DejaVuSans-10");
	CEGUI::ImagesetManager::getSingleton().create("DriveIcons.imageset");
	mRootWindow->addChildWindow(CEGUI::WindowManager::getSingleton().loadWindowLayout("DragDropDemo.layout"));
}