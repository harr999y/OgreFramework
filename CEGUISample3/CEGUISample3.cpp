#include "CEGUISample3.h"
#include <CEGUIPropertyHelper.h>

#if defined(__WIN32__) || defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#define MOMINMAX
#include "windows.h"
#include <mmsystem.h>
#endif

#include <ctime>
#include <cstdlib>

const unsigned int CEGUISample3App::TreeID = 1;
const unsigned int CEGUISample3App::EditBoxID = 2;

#define USE_VANILIA 0

#if USE_VANILIA
#define SCHEME_FILE_NAME   "VanillaSkin.scheme"
#define IMAGES_FILE_NAME   "Vanilla-Images"
#define STATICIMAGE_NAME   "Vanilla/StaticImage"
#define TOOLTIP_NAME       "Vanilla/Tooltip"
#define LAYOUT_FILE_NAME   "TreeDemo.layout"
#define BRUSH_NAME         "GenericBrush"

#else
#define SCHEME_FILE_NAME   "TaharezLook.scheme"
#define IMAGES_FILE_NAME   "TaharezLook"
#define STATICIMAGE_NAME   "TaharezLook/StaticImage"
#define TOOLTIP_NAME       "TaharezLook/Tooltip"
#define LAYOUT_FILE_NAME   "TreeDemoTaharez.layout"
#define BRUSH_NAME         "TextSelectionBrush"
#endif

using namespace CEGUI;

int randInt(int low,int high)
{
	int num;
	CEGUI::uint32 range;

	range = high - low + 1;
	if(range > RAND_MAX)
		num = (int) (((CEGUI::uint32)rand() * (CEGUI::uint32)rand()) % range);
	else 
		num = (rand()) % range;

	return (num + low);
}

void CEGUISample3App::createGUI()
{
	CEGUI::Tree * theTree;
	TreeItem * newTreeCtrlEntryLvl1;
	TreeItem * newTreeCtrlEntryLvl2;
	TreeItem * newTreeCtrlEntryLvl3;
	TreeItem * newTreeCtrlEntryParent;
	CEGUI::Image * iconArray[9];

#if defined(__WIN32__) || defined(_WIN32)
	srand(time(NULL));
#endif
	// Get window manager which we will use for a few jobs here.
	WindowManager& winMgr = WindowManager::getSingleton();

	//   CEGUI::Logger::getSingleton().setLoggingLevel(CEGUI::Standard);
	//   CEGUI::Logger::getSingleton().setLoggingLevel(CEGUI::Informative);
	CEGUI::Logger::getSingleton().setLoggingLevel(CEGUI::Insane);

	// Load the scheme to initialise the skin which we use in this sample
	SchemeManager::getSingleton().create(SCHEME_FILE_NAME);

	// set default mouse image
	System::getSingleton().setDefaultMouseCursor(IMAGES_FILE_NAME, "MouseArrow");

	// load an image to use as a background
	ImagesetManager::getSingleton().createFromImageFile("BackgroundImage", "GPN-2000-001437.tga");

	// Load some icon images for our test tree
	// Imageset *iconImages = ImagesetManager::getSingleton().create("TreeIcons.imageset");
	Imageset& drives = ImagesetManager::getSingleton().create("DriveIcons.imageset");

	// here we will use a StaticImage as the root, then we can use it to place a background image
	Window* background = winMgr.createWindow(STATICIMAGE_NAME);
	
	background->setArea(URect(cegui_reldim(0),cegui_reldim(0),cegui_reldim(1),cegui_reldim(1)));
	background->setProperty("FrameEnabled", "false");

	background->setProperty("BackgroundEnabled", "false");
	// set the background image
	background->setProperty("Image", "set:BackgroundImage image:full_image");
	// install this as the root GUI sheet
	System::getSingleton().setGUISheet(background);

	//   CEGUI::System::getSingleton().setTooltip(TOOLTIP_NAME);

	CEGUI::FontManager::getSingleton().create("DejaVuSans-10.font");
	//	if(!FontManager::getSingleton().isFontPresent("Commonwealth-10"))
	//		FontManager::getSingleton().createFont("Commonwealth-10.font");

	mTreeWindow = winMgr.loadWindowLayout(LAYOUT_FILE_NAME);

	background->addChildWindow(mTreeWindow);

	// listen for key presses on the root window.
	background->subscribeEvent(Window::EventKeyDown,Event::Subscriber(&CEGUISample3App::handleRootKeyDown,this));

	theTree = (Tree *)mTreeWindow->getChild(TreeID);
	theTree->initialise();
	theTree->subscribeEvent(Tree::EventSelectionChanged,Event::Subscriber(&CEGUISample3App::handleEventSelectionChanged,this));
	theTree->subscribeEvent(Tree::EventBranchOpened,Event::Subscriber(&CEGUISample3App::handleEventBranchOpened,this));
	theTree->subscribeEvent(Tree::EventBranchClosed,Event::Subscriber(&CEGUISample3App::handleEventBranchClosed,this));

	background->activate();

	CEGUI::Imageset& iconImages = drives;
	iconArray[0] = (CEGUI::Image *)&iconImages.getImage("Artic");
	iconArray[1] = (CEGUI::Image *)&iconImages.getImage("Black");
	iconArray[2] = (CEGUI::Image *)&iconImages.getImage("Sunset");
	iconArray[3] = (CEGUI::Image *)&iconImages.getImage("DriveStack");
	iconArray[4] = (CEGUI::Image *)&iconImages.getImage("GlobalDrive");
	iconArray[5] = (CEGUI::Image *)&iconImages.getImage("Blue");
	iconArray[6] = (CEGUI::Image *)&iconImages.getImage("Lime");
	iconArray[7] = (CEGUI::Image *)&iconImages.getImage("Silver");
	iconArray[8] = (CEGUI::Image *)&iconImages.getImage("GreenCandy");

	newTreeCtrlEntryLvl1 = new TreeItem("Tree Item Level 1a");
	newTreeCtrlEntryLvl1->setIcon(drives.getImage("Black"));
	newTreeCtrlEntryLvl1->setSelectionBrushImage(IMAGES_FILE_NAME,BRUSH_NAME);
	theTree->addItem(newTreeCtrlEntryLvl1);

	// Create a second-level TreeCtrlEntry and attach it to the top-most TreeCtrlEntry
	newTreeCtrlEntryLvl2 = new TreeItem("Tree Item Level 2a (1a)");
	newTreeCtrlEntryLvl2->setIcon(drives.getImage("Artic"));
	newTreeCtrlEntryLvl2->setSelectionBrushImage(IMAGES_FILE_NAME, BRUSH_NAME);
	newTreeCtrlEntryLvl1->addItem(newTreeCtrlEntryLvl2);
	// Create a third-level TreeCtrlEntry and attach it to the above TreeCtrlEntry
	newTreeCtrlEntryLvl3 = new TreeItem("Tree Item Level 3a (2a)");
	newTreeCtrlEntryLvl3->setIcon(drives.getImage("Blue"));
	newTreeCtrlEntryLvl3->setSelectionBrushImage(IMAGES_FILE_NAME, BRUSH_NAME);
	newTreeCtrlEntryLvl2->addItem(newTreeCtrlEntryLvl3);
	// Create another third-level TreeCtrlEntry and attach it to the above TreeCtrlEntry
	newTreeCtrlEntryLvl3 = new TreeItem("Tree Item Level 3b (2a)");
	newTreeCtrlEntryLvl3->setIcon(drives.getImage("Lime"));
	newTreeCtrlEntryLvl3->setSelectionBrushImage(IMAGES_FILE_NAME, BRUSH_NAME);
	newTreeCtrlEntryLvl2->addItem(newTreeCtrlEntryLvl3);
	// Create another second-level TreeCtrlEntry and attach it to the top-most TreeCtrlEntry
	newTreeCtrlEntryLvl2 = new TreeItem("Tree Item Level 2b (1a)");
	newTreeCtrlEntryLvl2->setIcon(drives.getImage("Sunset"));
	newTreeCtrlEntryLvl2->setSelectionBrushImage(IMAGES_FILE_NAME, BRUSH_NAME);
	newTreeCtrlEntryLvl1->addItem(newTreeCtrlEntryLvl2);
	// Create another second-level TreeCtrlEntry and attach it to the top-most TreeCtrlEntry
	newTreeCtrlEntryLvl2 = new TreeItem("Tree Item Level 2c (1a)");
	newTreeCtrlEntryLvl2->setIcon(drives.getImage("Silver"));
	newTreeCtrlEntryLvl2->setSelectionBrushImage(IMAGES_FILE_NAME, BRUSH_NAME);
	newTreeCtrlEntryLvl1->addItem(newTreeCtrlEntryLvl2);

	// Create another top-most TreeCtrlEntry
	newTreeCtrlEntryLvl1 = new TreeItem("Tree Item Level 1b");
	newTreeCtrlEntryLvl1->setSelectionBrushImage(IMAGES_FILE_NAME, BRUSH_NAME);
	newTreeCtrlEntryLvl1->setIcon(drives.getImage("DriveStack"));
	newTreeCtrlEntryLvl1->setDisabled(true); // Let's disable this one just to be sure it works
	theTree->addItem(newTreeCtrlEntryLvl1);
	// Create a second-level TreeCtrlEntry and attach it to the top-most TreeCtrlEntry
	newTreeCtrlEntryLvl2 = new TreeItem("Tree Item Level 2a (1b)");
	newTreeCtrlEntryLvl2->setSelectionBrushImage(IMAGES_FILE_NAME, BRUSH_NAME);
	newTreeCtrlEntryLvl1->addItem(newTreeCtrlEntryLvl2);
	// Create another second-level TreeCtrlEntry and attach it to the top-most TreeCtrlEntry
	newTreeCtrlEntryLvl2 = new TreeItem("Tree Item Level 2b (1b)");
	newTreeCtrlEntryLvl2->setSelectionBrushImage(IMAGES_FILE_NAME, BRUSH_NAME);
	newTreeCtrlEntryLvl1->addItem(newTreeCtrlEntryLvl2);

	newTreeCtrlEntryLvl1 = new TreeItem("Tree Item Level 1c");
	newTreeCtrlEntryLvl1->setSelectionBrushImage(IMAGES_FILE_NAME, BRUSH_NAME);
	theTree->addItem(newTreeCtrlEntryLvl1);

	// Now let's create a whole bunch of items automatically
	int levelIndex = 3;
	int idepthIndex;
	int childIndex;
	int childCount;
	unsigned int iconIndex;
	String itemText;
	while (levelIndex < 10)
	{
		idepthIndex = 0;
		itemText = "Tree Item Level " + PropertyHelper::intToString(levelIndex) + " Depth " + PropertyHelper::intToString(idepthIndex);
		newTreeCtrlEntryLvl1 = new TreeItem(itemText);
		// Set a random icon for the item.  Sometimes blank (on purpose).
		iconIndex = randInt(0, (sizeof(iconArray) / sizeof(iconArray[0])) + 2);
		if (iconIndex < sizeof(iconArray) / sizeof(iconArray[0]))
			newTreeCtrlEntryLvl1->setIcon(*iconArray[iconIndex]);
		newTreeCtrlEntryLvl1->setSelectionBrushImage(IMAGES_FILE_NAME, BRUSH_NAME);
		theTree->addItem(newTreeCtrlEntryLvl1);
		newTreeCtrlEntryParent = newTreeCtrlEntryLvl1;

#if 1
		childIndex = 0;
		childCount = randInt(0, 3);
		while (childIndex < childCount)
		{
			itemText = "Tree Item Level " + PropertyHelper::intToString(levelIndex) + " Depth " + PropertyHelper::intToString(idepthIndex + 1) + " Child " + PropertyHelper::intToString(childIndex + 1);
			newTreeCtrlEntryLvl2 = new TreeItem(itemText);
			// Set a random icon for the item.  Sometimes blank (on purpose).
			iconIndex = randInt(0, (sizeof(iconArray) / sizeof(iconArray[0]) + 2));
			if (iconIndex < sizeof(iconArray) / sizeof(iconArray[0]))
				newTreeCtrlEntryLvl2->setIcon(*iconArray[iconIndex]);
			newTreeCtrlEntryLvl2->setSelectionBrushImage(IMAGES_FILE_NAME, BRUSH_NAME);
			newTreeCtrlEntryParent->addItem(newTreeCtrlEntryLvl2);
			++childIndex;
		}
#endif

		while (idepthIndex < 15)
		{
			itemText = "Tree Item Level " + PropertyHelper::intToString(levelIndex) + " Depth " + PropertyHelper::intToString(idepthIndex + 1);
			newTreeCtrlEntryLvl2 = new TreeItem(itemText);
			// Set a random icon for the item.  Sometimes blank (on purpose).
			iconIndex = randInt(0, (sizeof(iconArray) / sizeof(iconArray[0]) + 2));
			if (iconIndex < sizeof(iconArray) / sizeof(iconArray[0]))
				newTreeCtrlEntryLvl2->setIcon(*iconArray[iconIndex]);
			newTreeCtrlEntryLvl2->setSelectionBrushImage(IMAGES_FILE_NAME, BRUSH_NAME);
			newTreeCtrlEntryParent->addItem(newTreeCtrlEntryLvl2);
			newTreeCtrlEntryParent = newTreeCtrlEntryLvl2;

#if 1
			childIndex = 0;
			childCount = randInt(0, 3);
			while (childIndex < childCount)
			{
				itemText = "Tree Item Level " + PropertyHelper::intToString(levelIndex) + " Depth " + PropertyHelper::intToString(idepthIndex + 1) + " Child " + PropertyHelper::intToString(childIndex + 1);
				newTreeCtrlEntryLvl2 = new TreeItem(itemText);
				// Set a random icon for the item.  Sometimes blank (on purpose).
				iconIndex = randInt(0, (sizeof(iconArray) / sizeof(iconArray[0]) + 2));
				if (iconIndex < sizeof(iconArray) / sizeof(iconArray[0]))
					newTreeCtrlEntryLvl2->setIcon(*iconArray[iconIndex]);
				newTreeCtrlEntryLvl2->setSelectionBrushImage(IMAGES_FILE_NAME, BRUSH_NAME);
				newTreeCtrlEntryParent->addItem(newTreeCtrlEntryLvl2);
				++childIndex;
			}
#endif
			++idepthIndex;
		}
		++levelIndex;
	}
}

bool CEGUISample3App::handleEventSelectionChanged(const CEGUI::EventArgs &args)
{
	using namespace CEGUI;

	const CEGUI::TreeEventArgs& treeArgs = static_cast<const TreeEventArgs&>(args);
	Editbox * editBox = (Editbox *)mTreeWindow->getChild(EditBoxID);

	//   TreeCtrlEntry *selectedItem = theTree->getFirstSelectedItem();      // the first selection in the list (may be more)
	//   TreeCtrlEntry *selectedItem = theTree->getLastSelectedItem();       // the last (time-wise) selected by the user
	TreeItem * selectedItem = treeArgs.treeItem;

	if(selectedItem)
	{
#if 0	
		// A convoluted way to get the item text.
		// Just here to test findFirstItemWithText.
		selectedItem =((Tree *)mTreeWindow->getChild(TreeID))->findFirstItemWithText(selectedItem->getText());
		if (selectedItem)
			editBox->setText("Selected: " + selectedItem->getText());
		else
			editBox->setText("findItemWithText failed!");
#else

		editBox->setText("Selected: " + selectedItem->getText());
#endif
	}
	else
		editBox->setText("None Selected");

	return true;
}

bool CEGUISample3App::handleRootKeyDown(const CEGUI::EventArgs &args)
{
	using namespace CEGUI;

	const CEGUI::KeyEventArgs keyArgs = static_cast<const KeyEventArgs&>(args);

	switch(keyArgs.scancode)
	{
	case Key::F12:
		break;

	default:
		return false;
	}
	return true;
}

bool CEGUISample3App::handleEventBranchOpened(const CEGUI::EventArgs &args)
{
	using namespace CEGUI;

	const TreeEventArgs& treeArgs = static_cast<const TreeEventArgs&>(args);
	Editbox * editBox = (Editbox *)mTreeWindow->getChild(EditBoxID);
	editBox->setText("Opened: " + treeArgs.treeItem->getText());
	return true;
}

bool CEGUISample3App::handleEventBranchClosed(const CEGUI::EventArgs& args)
{
	using namespace CEGUI;

	const TreeEventArgs& treeArgs = static_cast<const TreeEventArgs&>(args);
	Editbox *editBox = (Editbox *)mTreeWindow->getChild(EditBoxID);
	editBox->setText("Closed: " + treeArgs.treeItem->getText());
	return true;
}

