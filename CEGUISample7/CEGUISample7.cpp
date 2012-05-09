#include "CEGUISample7.h"

void CEGUISample7App::createGUI()
{
	using namespace CEGUI;

	// Get window manager which we wil use for a few jobs here.
	WindowManager& winMgr = WindowManager::getSingleton();
	// Load the scheme to initialse the VanillaSkin which we use in this sample
	SchemeManager::getSingleton().create("VanillaSkin.scheme");
	// set default mouse image
	System::getSingleton().setDefaultMouseCursor("Vanilla-Images", "MouseArrow");

	// load an image to use as a background
	ImagesetManager::getSingleton().createFromImageFile("BackgroundImage", "GPN-2000-001437.tga");

	// here we will use a StaticImage as the root, then we can use it to place a background image
	Window* background = winMgr.createWindow("Vanilla/StaticImage");
	// set area rectangle
	background->setArea(URect(cegui_reldim(0), cegui_reldim(0), cegui_reldim(1), cegui_reldim(1)));
	// disable frame and standard background
	background->setProperty("FrameEnabled", "false");
	background->setProperty("BackgroundEnabled", "false");
	// set the background image
	background->setProperty("Image", "set:BackgroundImage image:full_image");
	// install this as the root GUI sheet
	System::getSingleton().setGUISheet(background);

	FontManager::getSingleton().create("DejaVuSans-10.font");

	// load some demo windows and attach to the background 'root'
	background->addChildWindow(winMgr.loadWindowLayout("VanillaWindows.layout"));

	mConsole = new DemoConsole("Demo");

	// listen for key presses on the root window.
	background->subscribeEvent(Window::EventKeyDown, Event::Subscriber(&CEGUISample7App::handleRootKeyDown, this));

	// activate the background window
	background->activate();
}

CEGUISample7App::~CEGUISample7App()
{
	delete mConsole;
}

bool CEGUISample7App::handleRootKeyDown(const CEGUI::EventArgs &args)
{
	using namespace CEGUI;

	const KeyEventArgs& keyArgs = static_cast<const KeyEventArgs&>(args);

	switch (keyArgs.scancode)
	{
	case Key::F12:
		mConsole->toggleVisibility();
		break;

	default:
		return false;
	}

	return true;		
}

const unsigned int DemoConsole::SubmitButtonID = 1;
const unsigned int DemoConsole::EntryBoxID = 2;
const unsigned int DemoConsole::HistoryID = 3;

DemoConsole::DemoConsole(const CEGUI::String &mIdName,CEGUI::Window* parent) : 
	mWindow(CEGUI::WindowManager::getSingleton().loadWindowLayout("VanillaConsole.layout",mIdName)),mHistoryPos(0)
{
	using namespace CEGUI;

	mWindow->setDestroyedByParent(false);
	// Do events wire-up
	mWindow->getChild(EntryBoxID)->subscribeEvent(Window::EventKeyDown, Event::Subscriber(&DemoConsole::handleKeyDown, this));

	mWindow->getChild(EntryBoxID)->subscribeEvent(Editbox::EventTextAccepted,Event::Subscriber(&DemoConsole::handleSubmit,this));

	mWindow->getChild(SubmitButtonID)->subscribeEvent(PushButton::EventClicked,Event::Subscriber(&DemoConsole::handleSubmit,this));

	parent = parent ? parent : CEGUI::System::getSingleton().getGUISheet();

	if(parent)
		parent->addChildWindow(mWindow);
}

DemoConsole::~DemoConsole()
{
	CEGUI::WindowManager::getSingleton().destroyWindow(mWindow);
}

void DemoConsole::toggleVisibility()
{
	mWindow->isVisible(true) ? mWindow->hide() : mWindow->show();
}

bool DemoConsole::isVisible() const
{
	return mWindow->isVisible();
}

bool DemoConsole::handleSubmit(const CEGUI::EventArgs &args)
{
	using namespace CEGUI;
	Editbox* editbox = static_cast<Editbox*>(mWindow->getChild(EntryBoxID));
	String edit_text(editbox->getText()); //current text 
	if(!edit_text.empty())
	{
		mHistory.push_back(edit_text);
		mHistoryPos = mHistory.size();
		edit_text += '\n';
		MultiLineEditbox* history = static_cast<MultiLineEditbox*>(mWindow->getChild(HistoryID));
		history->setText(history->getText() + edit_text);
		history->setCaratIndex(-1);//????
		editbox->setText("");
	}
	editbox->activate();
	return true;
}

bool DemoConsole::handleKeyDown(const CEGUI::EventArgs &args)
{
	using namespace CEGUI;

	// get the text entry editbox
	Editbox* editbox = static_cast<Editbox*>(mWindow->getChild(EntryBoxID));

	switch (static_cast<const KeyEventArgs&>(args).scancode)
	{
	case Key::ArrowUp :
		mHistoryPos = ceguimax(mHistoryPos-1,-1);
		if(mHistoryPos >= 0)
		{
			editbox->setText(mHistory[mHistoryPos]);
			editbox->setCaratIndex(static_cast<unsigned int >(-1));
		}
		else
			editbox->setText("");
		editbox->activate();
		break;
	case Key::ArrowDown :
		mHistoryPos = ceguimin(mHistoryPos+ 1, static_cast<int>(mHistory.size()));
		if (mHistoryPos < static_cast<int>(mHistory.size()))
		{
			editbox->setText(mHistory[mHistoryPos]);
			editbox->setCaratIndex(static_cast<unsigned int>(-1));
		}
		else
		{
			editbox->setText("");
		}

		editbox->activate();
		break;

	default:
		return false;
	}
	return true;

}