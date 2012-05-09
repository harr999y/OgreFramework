#include "CEGUISample8.h"
#include <stdlib.h>
#include <stdio.h>
#include <string>

void CEGUISample8App::createGUI()
{
	using namespace CEGUI;

	// we will use of the WindowManager.
	WindowManager& winMgr = WindowManager::getSingleton();

	// load scheme and set up defaults
	SchemeManager::getSingleton().create("TaharezLook.scheme");
	System::getSingleton().setDefaultMouseCursor("TaharezLook", "MouseArrow");
	FontManager::getSingleton().create("DejaVuSans-10.font");

	// load an image to use as a background
	ImagesetManager::getSingleton().createFromImageFile("BackgroundImage", "GPN-2000-001437.tga");

	// here we will use a StaticImage as the root, then we can use it to place a background image
	Window* background = winMgr.createWindow("TaharezLook/StaticImage", "root_wnd");
	// set position and size
	background->setPosition(UVector2(cegui_reldim(0), cegui_reldim( 0)));
	background->setSize(UVector2(cegui_reldim(1), cegui_reldim( 1)));
	// disable frame and standard background
	background->setProperty("FrameEnabled", "false");
	background->setProperty("BackgroundEnabled", "false");
	// set the background image
	background->setProperty("Image", "set:BackgroundImage image:full_image");
	// install this as the root GUI sheet
	System::getSingleton().setGUISheet(background);

	createDemoWindow();
	initDemoEventWiring();
}

void CEGUISample8App::createDemoWindow()
{
	using namespace CEGUI;
	WindowManager& winMgr = WindowManager::getSingleton();
	Window* root = winMgr.getWindow("root_wnd");

	MultiColumnList * mcl = static_cast<MultiColumnList *>(winMgr.createWindow("TaharezLook/MultiColumnList","Demo6/MainList"));
	root->addChildWindow(mcl);
	mcl->setPosition(UVector2(cegui_reldim(0.01f),cegui_reldim(0.1f)));
	mcl->setSize(UVector2(cegui_reldim(0.5f), cegui_reldim( 0.8f)));

	FrameWindow* fwnd = static_cast<FrameWindow*>(winMgr.createWindow("TaharezLook/FrameWindow", "Demo6/ControlPanel"));
	root->addChildWindow(fwnd);
	fwnd->setPosition(UVector2(cegui_reldim(0.53f), cegui_reldim( 0.03f)));
	fwnd->setMaxSize(UVector2(cegui_reldim(1.0f), cegui_reldim( 1.0f)));
	fwnd->setSize(UVector2(cegui_reldim(0.44f), cegui_reldim( 0.94f)));
	fwnd->setText("Demo 6 - Control Panel");

	Combobox * cbbo = static_cast<Combobox*>(winMgr.createWindow("TaharezLook/Combobox","Demo6/ControlPanel/SelModeBox"));
	fwnd->addChildWindow(cbbo);
	cbbo->setPosition(UVector2(cegui_reldim(0.04f), cegui_reldim( 0.06f)));
	cbbo->setSize(UVector2(cegui_reldim(0.66f), cegui_reldim( 0.33f)));

	ListboxTextItem * itm;
	const CEGUI::Image * sel_img = &ImagesetManager::getSingleton().get("TaharezLook").getImage("MultiListSelectionBrush");
	itm = new ListboxTextItem("Full Row (Single)",0);
	itm->setSelectionBrushImage(sel_img);
	cbbo->addItem(itm);
	itm = new ListboxTextItem("Full Row (Multiple)", 1);
	itm->setSelectionBrushImage(sel_img);
	cbbo->addItem(itm);
	itm = new ListboxTextItem("Full Column (Single)", 2);
	itm->setSelectionBrushImage(sel_img);
	cbbo->addItem(itm);
	itm = new ListboxTextItem("Full Column (Multiple)", 3);
	itm->setSelectionBrushImage(sel_img);
	cbbo->addItem(itm);
	itm = new ListboxTextItem("Single Cell (Single)", 4);
	itm->setSelectionBrushImage(sel_img);
	cbbo->addItem(itm);
	itm = new ListboxTextItem("Single Cell (Multiple)", 5);
	itm->setSelectionBrushImage(sel_img);
	cbbo->addItem(itm);
	itm = new ListboxTextItem("Nominated Column (Single)", 6);
	itm->setSelectionBrushImage(sel_img);
	cbbo->addItem(itm);
	itm = new ListboxTextItem("Nominated Column (Multiple)", 7);
	itm->setSelectionBrushImage(sel_img);
	cbbo->addItem(itm);
	ListboxTextItem* pStore = itm;
	itm = new ListboxTextItem("Nominated Row (Single)", 8);
	itm->setSelectionBrushImage(sel_img);
	cbbo->addItem(itm);
	itm = new ListboxTextItem("Nominated Row (Multiple)", 9);
	itm->setSelectionBrushImage(sel_img);
	cbbo->addItem(itm);
	cbbo->setReadOnly(true);

	pStore->setText("FuckFuck");
	cbbo->setSortingEnabled(true);

	Window* st = winMgr.createWindow("TaharezLook/StaticText", "Demo6/ControlPanel/ColumnPanel");
	fwnd->addChildWindow(st);
	st->setPosition(UVector2(cegui_reldim(0.02f), cegui_reldim( 0.12f)));
	st->setSize(UVector2(cegui_reldim(0.96f), cegui_reldim( 0.25f)));
	st->setText("Column Control");
	st->setProperty("VertFormatting", "TopAligned");
	Window* label = winMgr.createWindow("TaharezLook/StaticText", "Demo6/ControlPanel/Label1");
	st->addChildWindow(label);
	label->setProperty("FrameEnabled", "false");
	label->setProperty("BackgroundEnabled", "false");
	label->setPosition(UVector2(cegui_reldim(0.02f), cegui_reldim( 0.2f)));
	label->setSize(UVector2(cegui_reldim(0.2f), cegui_reldim( 0.12f)));
	label->setText("ID Code:");
	label = winMgr.createWindow("TaharezLook/StaticText", "Demo6/ControlPanel/Label2");
	st->addChildWindow(label);
	label->setProperty("FrameEnabled", "false");
	label->setProperty("BackgroundEnabled", "false");
	label->setPosition(UVector2(cegui_reldim(0.23f), cegui_reldim( 0.2f)));
	label->setSize(UVector2(cegui_reldim(0.2f), cegui_reldim( 0.12f)));
	label->setText("Width:");
	label = winMgr.createWindow("TaharezLook/StaticText", "Demo6/ControlPanel/Label3");
	st->addChildWindow(label);
	label->setProperty("FrameEnabled", "false");
	label->setProperty("BackgroundEnabled", "false");
	label->setPosition(UVector2(cegui_reldim(0.44f), cegui_reldim( 0.2f)));
	label->setSize(UVector2(cegui_reldim(0.2f), cegui_reldim( 0.12f)));
	label->setText("Caption:");

	PushButton* btn = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "Demo6/ControlPanel/ColumnPanel/AddColButton"));
	st->addChildWindow(btn);
	btn->setPosition(UVector2(cegui_reldim(0.81f), cegui_reldim( 0.32f)));
	btn->setSize(UVector2(cegui_reldim(0.15f), cegui_reldim( 0.2f)));
	btn->setText("Add");
	Editbox* ebox = static_cast<Editbox*>(winMgr.createWindow("TaharezLook/Editbox", "Demo6/ControlPanel/ColumnPanel/NewColIDBox"));
	st->addChildWindow(ebox);
	ebox->setPosition(UVector2(cegui_reldim(0.02f), cegui_reldim( 0.32f)));
	ebox->setSize(UVector2(cegui_reldim(0.2f), cegui_reldim( 0.2f)));
	ebox->setValidationString("\\d*");
	ebox->setText("Test -- ");
	ebox = static_cast<Editbox*>(winMgr.createWindow("TaharezLook/Editbox", "Demo6/ControlPanel/ColumnPanel/NewColWidthBox"));
	st->addChildWindow(ebox);
	ebox->setPosition(UVector2(cegui_reldim(0.23f), cegui_reldim( 0.32f)));
	ebox->setSize(UVector2(cegui_reldim(0.2f), cegui_reldim( 0.2f)));
	ebox->setValidationString("\\d*");

	ebox = static_cast<Editbox*>(winMgr.createWindow("TaharezLook/Editbox", "Demo6/ControlPanel/ColumnPanel/NewColTextBox"));
	st->addChildWindow(ebox);
	ebox->setPosition(UVector2(cegui_reldim(0.44f), cegui_reldim( 0.32f)));
	ebox->setSize(UVector2(cegui_reldim(0.36f), cegui_reldim( 0.2f)));
	ebox->setValidationString(".*");

	label = winMgr.createWindow("TaharezLook/StaticText", "Demo6/ControlPanel/Label4");
	st->addChildWindow(label);
	label->setProperty("FrameEnabled", "false");
	label->setProperty("BackgroundEnabled", "false");
	label->setPosition(UVector2(cegui_reldim(0.02f), cegui_reldim( 0.55f)));
	label->setSize(UVector2(cegui_reldim(0.2f), cegui_reldim( 0.12f)));
	label->setText("ID Code:");

	ebox = static_cast<Editbox*>(winMgr.createWindow("TaharezLook/Editbox", "Demo6/ControlPanel/ColumnPanel/DelColIDBox"));
	st->addChildWindow(ebox);
	ebox->setPosition(UVector2(cegui_reldim(0.02f), cegui_reldim( 0.67f)));
	ebox->setSize(UVector2(cegui_reldim(0.2f), cegui_reldim( 0.2f)));
	ebox->setValidationString("\\d*");

	btn = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "Demo6/ControlPanel/ColumnPanel/DelColButton"));
	st->addChildWindow(btn);
	btn->setPosition(UVector2(cegui_reldim(0.25f), cegui_reldim( 0.67f)));
	btn->setSize(UVector2(cegui_reldim(0.4f), cegui_reldim( 0.2f)));
	btn->setText("Delete Column");

	// Row control box
	st = winMgr.createWindow("TaharezLook/StaticText", "Demo6/ControlPanel/RowControl");
	fwnd->addChildWindow(st);
	st->setPosition(UVector2(cegui_reldim(0.02f), cegui_reldim( 0.38f)));
	st->setSize(UVector2(cegui_reldim(0.96f), cegui_reldim( 0.25f)));
	st->setText("Row Control");
	st->setProperty("VertFormatting", "TopAligned");

	label = winMgr.createWindow("TaharezLook/StaticText", "Demo6/ControlPanel/Label5");
	st->addChildWindow(label);
	label->setProperty("FrameEnabled", "false");
	label->setProperty("BackgroundEnabled", "false");
	label->setPosition(UVector2(cegui_reldim(0.02f), cegui_reldim( 0.2f)));
	label->setSize(UVector2(cegui_reldim(0.2f), cegui_reldim( 0.12f)));
	label->setText("Col ID:");

	label = winMgr.createWindow("TaharezLook/StaticText", "Demo6/ControlPanel/Label6");
	st->addChildWindow(label);
	label->setProperty("FrameEnabled", "false");
	label->setProperty("BackgroundEnabled", "false");
	label->setPosition(UVector2(cegui_reldim(0.23f), cegui_reldim( 0.2f)));
	label->setSize(UVector2(cegui_reldim(0.55f), cegui_reldim( 0.12f)));
	label->setText("Item Text:");

	ebox = static_cast<Editbox*>(winMgr.createWindow("TaharezLook/Editbox", "Demo6/ControlPanel/ColumnPanel/RowColIDBox"));
	st->addChildWindow(ebox);
	ebox->setPosition(UVector2(cegui_reldim(0.02f), cegui_reldim( 0.32f)));
	ebox->setSize(UVector2(cegui_reldim(0.2f), cegui_reldim( 0.2f)));
	ebox->setValidationString("\\d*");

	ebox = static_cast<Editbox*>(winMgr.createWindow("TaharezLook/Editbox", "Demo6/ControlPanel/ColumnPanel/RowTextBox"));
	st->addChildWindow(ebox);
	ebox->setPosition(UVector2(cegui_reldim(0.23f), cegui_reldim( 0.32f)));
	ebox->setSize(UVector2(cegui_reldim(0.55f), cegui_reldim( 0.2f)));
	ebox->setValidationString(".*");

	btn = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "Demo6/ControlPanel/ColumnPanel/AddRowButton"));
	st->addChildWindow(btn);
	btn->setPosition(UVector2(cegui_reldim(0.81f), cegui_reldim( 0.32f)));
	btn->setSize(UVector2(cegui_reldim(0.15f), cegui_reldim( 0.2f)));
	btn->setText("Add");

	label = winMgr.createWindow("TaharezLook/StaticText", "Demo6/ControlPanel/Label7");
	st->addChildWindow(label);
	label->setProperty("FrameEnabled", "false");
	label->setProperty("BackgroundEnabled", "false");
	label->setPosition(UVector2(cegui_reldim(0.02f), cegui_reldim( 0.55f)));
	label->setSize(UVector2(cegui_reldim(0.2f), cegui_reldim( 0.12f)));
	label->setText("Row Idx:");

	ebox = static_cast<Editbox*>(winMgr.createWindow("TaharezLook/Editbox", "Demo6/ControlPanel/ColumnPanel/DelRowIdxBox"));
	st->addChildWindow(ebox);
	ebox->setPosition(UVector2(cegui_reldim(0.02f), cegui_reldim( 0.67f)));
	ebox->setSize(UVector2(cegui_reldim(0.2f), cegui_reldim( 0.2f)));
	ebox->setValidationString("\\d*");

	btn = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "Demo6/ControlPanel/ColumnPanel/DelRowButton"));
	st->addChildWindow(btn);
	btn->setPosition(UVector2(cegui_reldim(0.25f), cegui_reldim( 0.67f)));
	btn->setSize(UVector2(cegui_reldim(0.4f), cegui_reldim( 0.2f)));
	btn->setText("Delete Row");

	// set item box
	st = winMgr.createWindow("TaharezLook/StaticText", "Demo6/ControlPanel/SetItemPanel");
	fwnd->addChildWindow(st);
	st->setPosition(UVector2(cegui_reldim(0.02f), cegui_reldim( 0.65f)));
	st->setSize(UVector2(cegui_reldim(0.96f), cegui_reldim( 0.25f)));
	st->setText("Item Modification");
	st->setProperty("VertFormatting", "TopAligned");

	label = winMgr.createWindow("TaharezLook/StaticText", "Demo6/ControlPanel/Label8");
	st->addChildWindow(label);
	label->setProperty("FrameEnabled", "false");
	label->setProperty("BackgroundEnabled", "false");
	label->setPosition(UVector2(cegui_reldim(0.02f), cegui_reldim( 0.2f)));
	label->setSize(UVector2(cegui_reldim(0.2f), cegui_reldim( 0.12f)));
	label->setText("Row Idx:");

	label = winMgr.createWindow("TaharezLook/StaticText", "Demo6/ControlPanel/Label9");
	st->addChildWindow(label);
	label->setProperty("FrameEnabled", "false");
	label->setProperty("BackgroundEnabled", "false");
	label->setPosition(UVector2(cegui_reldim(0.23f), cegui_reldim( 0.2f)));
	label->setSize(UVector2(cegui_reldim(0.2f), cegui_reldim( 0.12f)));
	label->setText("Col ID:");

	label = winMgr.createWindow("TaharezLook/StaticText", "Demo6/ControlPanel/Label10");
	st->addChildWindow(label);
	label->setProperty("FrameEnabled", "false");
	label->setProperty("BackgroundEnabled", "false");
	label->setPosition(UVector2(cegui_reldim(0.44f), cegui_reldim( 0.2f)));
	label->setSize(UVector2(cegui_reldim(0.2f), cegui_reldim( 0.12f)));
	label->setText("Item Text:");

	ebox = static_cast<Editbox*>(winMgr.createWindow("TaharezLook/Editbox", "Demo6/ControlPanel/ColumnPanel/SetItemRowBox"));
	st->addChildWindow(ebox);
	ebox->setPosition(UVector2(cegui_reldim(0.02f), cegui_reldim( 0.32f)));
	ebox->setSize(UVector2(cegui_reldim(0.2f), cegui_reldim( 0.2f)));
	ebox->setValidationString("\\d*");

	ebox = static_cast<Editbox*>(winMgr.createWindow("TaharezLook/Editbox", "Demo6/ControlPanel/ColumnPanel/SetItemIDBox"));
	st->addChildWindow(ebox);
	ebox->setPosition(UVector2(cegui_reldim(0.23f), cegui_reldim( 0.32f)));
	ebox->setSize(UVector2(cegui_reldim(0.2f), cegui_reldim( 0.2f)));
	ebox->setValidationString("\\d*");

	ebox = static_cast<Editbox*>(winMgr.createWindow("TaharezLook/Editbox", "Demo6/ControlPanel/ColumnPanel/SetItemTextBox"));
	st->addChildWindow(ebox);
	ebox->setPosition(UVector2(cegui_reldim(0.44f), cegui_reldim( 0.32f)));
	ebox->setSize(UVector2(cegui_reldim(0.36f), cegui_reldim( 0.2f)));
	ebox->setValidationString(".*");

	btn = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "Demo6/ControlPanel/ColumnPanel/SetItemButton"));
	st->addChildWindow(btn);
	btn->setPosition(UVector2(cegui_reldim(0.81f), cegui_reldim( 0.32f)));
	btn->setSize(UVector2(cegui_reldim(0.15f), cegui_reldim( 0.2f)));
	btn->setText("Set");

	label = winMgr.createWindow("TaharezLook/StaticText", "Demo6/ControlPanel/RowCount");
	st->addChildWindow(label);
	label->setProperty("FrameEnabled", "false");
	label->setProperty("BackgroundEnabled", "false");
	label->setPosition(UVector2(cegui_reldim(0.02f), cegui_reldim( 0.55f)));
	label->setSize(UVector2(cegui_reldim(1.0f), cegui_reldim( 0.12f)));
	label->setText("Current Row Count:");

	label = winMgr.createWindow("TaharezLook/StaticText", "Demo6/ControlPanel/ColCount");
	st->addChildWindow(label);
	label->setProperty("FrameEnabled", "false");
	label->setProperty("BackgroundEnabled", "false");
	label->setPosition(UVector2(cegui_reldim(0.02f), cegui_reldim( 0.67f)));
	label->setSize(UVector2(cegui_reldim(1.0f), cegui_reldim( 0.12f)));
	label->setText("Current Column Count:");

	label = winMgr.createWindow("TaharezLook/StaticText", "Demo6/ControlPanel/SelCount");
	st->addChildWindow(label);
	label->setProperty("FrameEnabled", "false");
	label->setProperty("BackgroundEnabled", "false");
	label->setPosition(UVector2(cegui_reldim(0.02f), cegui_reldim( 0.79f)));
	label->setSize(UVector2(cegui_reldim(1.0f), cegui_reldim( 0.12f)));
	label->setText("Current Selected Count:");

	btn = static_cast<PushButton*>(winMgr.createWindow("TaharezLook/Button", "Demo6/QuitButton"));
	fwnd->addChildWindow(btn);
	btn->setPosition(UVector2(cegui_reldim(0.25f), cegui_reldim( 0.93f)));
	btn->setSize(UVector2(cegui_reldim(0.50f), cegui_reldim( 0.05f)));
	btn->setText("Quit This Demo!");
}

void CEGUISample8App::initDemoEventWiring()
{
	using namespace CEGUI;
	WindowManager& winMgr = WindowManager::getSingleton();

	// subscribe handler that adds a new column
	winMgr.getWindow("Demo6/ControlPanel/ColumnPanel/AddColButton")->
		subscribeEvent(PushButton::EventClicked, Event::Subscriber(&CEGUISample8App::handleAddColumn, this));

	// subscribe handler that deletes a column
	winMgr.getWindow("Demo6/ControlPanel/ColumnPanel/DelColButton")->
		subscribeEvent(PushButton::EventClicked, Event::Subscriber(&CEGUISample8App::handleDeleteColumn, this));

	// subscribe handler that adds a new row
	winMgr.getWindow("Demo6/ControlPanel/ColumnPanel/AddRowButton")->
		subscribeEvent(PushButton::EventClicked, Event::Subscriber(&CEGUISample8App::handleAddRow, this));

	// subscribe handler that deletes a row
	winMgr.getWindow("Demo6/ControlPanel/ColumnPanel/DelRowButton")->
		subscribeEvent(PushButton::EventClicked, Event::Subscriber(&CEGUISample8App::handleDeleteRow, this));

	// subscribe handler that sets the text for an existing item
	winMgr.getWindow("Demo6/ControlPanel/ColumnPanel/SetItemButton")->
		subscribeEvent(PushButton::EventClicked, Event::Subscriber(&CEGUISample8App::handleSetItem, this));

	// subscribe handler that processes a change in the 'selection mode' combobox
	if(winMgr.getWindow("Demo6/ControlPanel/SelModeBox"))
		winMgr.getWindow("Demo6/ControlPanel/SelModeBox")->subscribeEvent(Combobox::EventListSelectionAccepted, Event::Subscriber(&CEGUISample8App::handleSelectModeChanged, this));

	// subscribe handler that processes a change in the item(s) selected in the list
	winMgr.getWindow("Demo6/MainList")->
		subscribeEvent(MultiColumnList::EventSelectionChanged, Event::Subscriber(&CEGUISample8App::handleSelectChanged, this));

	// subscribe handler that processes a change in the list content.
	winMgr.getWindow("Demo6/MainList")->
		subscribeEvent(MultiColumnList::EventListContentsChanged, Event::Subscriber(&CEGUISample8App::handleContentsChanged, this));

}

bool CEGUISample8App::handleAddColumn(const CEGUI::EventArgs &e)
{
	using namespace CEGUI;

	// get access to the widgets that contain details about the column to add
	MultiColumnList* mcl = static_cast<MultiColumnList*>(WindowManager::getSingleton().getWindow("Demo6/MainList"));
	Editbox* idbox = static_cast<Editbox*>(WindowManager::getSingleton().getWindow("Demo6/ControlPanel/ColumnPanel/NewColIDBox"));
	Editbox* widthbox = static_cast<Editbox*>(WindowManager::getSingleton().getWindow("Demo6/ControlPanel/ColumnPanel/NewColWidthBox"));
	Editbox* textbox = static_cast<Editbox*>(WindowManager::getSingleton().getWindow("Demo6/ControlPanel/ColumnPanel/NewColTextBox"));

	CEGUI::uint id = atoi(idbox->getText().c_str());
	float width = atof(widthbox->getText().c_str());
	String text = textbox->getText();

	idbox->setText("");
	widthbox->setText("");
	textbox->setText("");

	if(width < 10.0f)
		width = 10.0f;

	mcl->addColumn(text,id,cegui_absdim(width));

	return true;
}

bool CEGUISample8App::handleDeleteColumn(const CEGUI::EventArgs &e)
{
	using namespace CEGUI;

	// get access to the widgets that contain details about the column to delete
	MultiColumnList* mcl = static_cast<MultiColumnList*>(WindowManager::getSingleton().getWindow("Demo6/MainList"));
	Editbox* idbox = static_cast<Editbox*>(WindowManager::getSingleton().getWindow("Demo6/ControlPanel/ColumnPanel/DelColIDBox"));

	// obtain the id of the column to be deleted
	CEGUI::uint id = atoi(idbox->getText().c_str());

	// attempt to delete the column, ignoring any errors.
	CEGUI_TRY
	{
		mcl->removeColumnWithID(id);
	}
	CEGUI_CATCH (InvalidRequestException)
	{}

	// reset the delete column ID box.
	idbox->setText("");

	// event was handled.
	return true;	
}

bool CEGUISample8App::handleAddRow(const CEGUI::EventArgs &e)
{
	using namespace CEGUI;

	// get access to the widgets that contain details about the row to add
	MultiColumnList* mcl = static_cast<MultiColumnList*>(WindowManager::getSingleton().getWindow("Demo6/MainList"));
	Editbox* idbox = static_cast<Editbox*>(WindowManager::getSingleton().getWindow("Demo6/ControlPanel/ColumnPanel/RowColIDBox"));
	Editbox* textbox = static_cast<Editbox*>(WindowManager::getSingleton().getWindow("Demo6/ControlPanel/ColumnPanel/RowTextBox"));

	// get the ID of the initial column item to set
	CEGUI::uint id = atoi(idbox->getText().c_str());
	// get the text that is to be set initially into the specified column of the new row
	String text = textbox->getText();

	// reset input boxes
	idbox->setText("");
	textbox->setText("");

	// construct a new ListboxTextItem with the required string
	ListboxTextItem* item = new ListboxTextItem(text);
	// set the selection brush to use for this item.
	item->setSelectionBrushImage(&ImagesetManager::getSingleton().get("TaharezLook").getImage("MultiListSelectionBrush"));

	// attempt to add a new row, using the new ListboxTextItem as the initial content for one of the columns
	CEGUI_TRY
	{
		mcl->addRow(item, id);
	}
	// something went wrong, so cleanup the ListboxTextItem
	CEGUI_CATCH (InvalidRequestException)
	{
		delete item;
	}

	// event was handled.
	return true;
}

bool CEGUISample8App::handleDeleteRow(const CEGUI::EventArgs&)
{
	using namespace CEGUI;

	// get access to the widgets that contain details about the row to delete.
	MultiColumnList* mcl = static_cast<MultiColumnList*>(WindowManager::getSingleton().getWindow("Demo6/MainList"));
	Editbox* idxbox = static_cast<Editbox*>(WindowManager::getSingleton().getWindow("Demo6/ControlPanel/ColumnPanel/DelRowIdxBox"));

	// get index of row to delete.
	CEGUI::uint idx = atoi(idxbox->getText().c_str());

	// attempt to delete the row, ignoring any errors.
	CEGUI_TRY
	{
		mcl->removeRow(idx);
	}
	CEGUI_CATCH (InvalidRequestException)
	{}

	// clear the row index box
	idxbox->setText("");

	// event was handled.
	return true;
}

bool CEGUISample8App::handleSetItem(const CEGUI::EventArgs&)
{
	using namespace CEGUI;

	// get access to the widgets that contain details about the item to be modified
	MultiColumnList* mcl = static_cast<MultiColumnList*>(WindowManager::getSingleton().getWindow("Demo6/MainList"));
	Editbox* idbox = static_cast<Editbox*>(WindowManager::getSingleton().getWindow("Demo6/ControlPanel/ColumnPanel/SetItemIDBox"));
	Editbox* rowbox = static_cast<Editbox*>(WindowManager::getSingleton().getWindow("Demo6/ControlPanel/ColumnPanel/SetItemRowBox"));
	Editbox* textbox = static_cast<Editbox*>(WindowManager::getSingleton().getWindow("Demo6/ControlPanel/ColumnPanel/SetItemTextBox"));

	// get ID of column to be affected
	CEGUI::uint id = atoi(idbox->getText().c_str());
	// get index of row to be affected
	CEGUI::uint row = atoi(rowbox->getText().c_str());
	// get new text for item
	String text = textbox->getText();

	// reset input boxes
	idbox->setText("");
	rowbox->setText("");
	textbox->setText("");

	// create a new ListboxTextItem using the new text string
	ListboxTextItem* item = new ListboxTextItem(text);
	// set the selection brush to be used for this item.
	item->setSelectionBrushImage(&ImagesetManager::getSingleton().get("TaharezLook").getImage("MultiListSelectionBrush"));

	// attempt to set the new item in place
	CEGUI_TRY
	{
		mcl->setItem(item, id, row);
	}
	// something went wrong, so cleanup the ListboxTextItem.
	CEGUI_CATCH (InvalidRequestException)
	{
		delete item;
	}

	// event was handled.
	return true;
}

bool CEGUISample8App::handleSelectChanged(const CEGUI::EventArgs&)
{
	using namespace CEGUI;

	// Get access to the list
	MultiColumnList* mcl = static_cast<MultiColumnList*>(WindowManager::getSingleton().getWindow("Demo6/MainList"));

	// update the selected count
	std::string tmp("Current Selected Count: ");

	char buff[16];
	sprintf(buff, "%d", mcl->getSelectedCount());

	tmp += buff;

	WindowManager::getSingleton().getWindow("Demo6/ControlPanel/SelCount")->setText(tmp);

	// event was handled.
	return true;
}

bool CEGUISample8App::handleSelectModeChanged(const CEGUI::EventArgs&)
{
	using namespace CEGUI;

	// get access to list
	MultiColumnList* mcl = static_cast<MultiColumnList*>(WindowManager::getSingleton().getWindow("Demo6/MainList"));
	// get access to the combobox
	Combobox* combo = static_cast<Combobox*>(WindowManager::getSingleton().getWindow("Demo6/ControlPanel/SelModeBox"));

	// find the selected item in the combobox
	ListboxItem* item = combo->findItemWithText(combo->getText(), 0);

	// set new selection mode according to ID of selected ListboxItem
	if (item)
	{
		switch (item->getID())
		{
		case 0:
			mcl->setSelectionMode(MultiColumnList::RowSingle);
			break;

		case 1:
			mcl->setSelectionMode(MultiColumnList::RowMultiple);
			break;

		case 2:
			mcl->setSelectionMode(MultiColumnList::ColumnSingle);
			break;

		case 3:
			mcl->setSelectionMode(MultiColumnList::ColumnMultiple);
			break;

		case 4:
			mcl->setSelectionMode(MultiColumnList::CellSingle);
			break;

		case 5:
			mcl->setSelectionMode(MultiColumnList::CellMultiple);
			break;

		case 6:
			mcl->setSelectionMode(MultiColumnList::NominatedColumnSingle);
			break;

		case 7:
			mcl->setSelectionMode(MultiColumnList::NominatedColumnMultiple);
			break;

		case 8:
			mcl->setSelectionMode(MultiColumnList::NominatedRowSingle);
			break;

		case 9:
			mcl->setSelectionMode(MultiColumnList::NominatedRowMultiple);
			break;

		default:
			mcl->setSelectionMode(MultiColumnList::RowSingle);
			break;

		}
	}

	// event was handled.
	return true;
}

bool CEGUISample8App::handleContentsChanged(const CEGUI::EventArgs&)
{
	using namespace CEGUI;

	// get access to required widgets
	MultiColumnList* mcl = static_cast<MultiColumnList*>(WindowManager::getSingleton().getWindow("Demo6/MainList"));
	Window* colText = WindowManager::getSingleton().getWindow("Demo6/ControlPanel/ColCount");
	Window* rowText = WindowManager::getSingleton().getWindow("Demo6/ControlPanel/RowCount");

	std::string tmp;
	char buff[16];

	// update the column count
	tmp = "Current Column Count: ";
	sprintf(buff, "%d", mcl->getColumnCount());
	tmp += buff;
	colText->setText(tmp);

	// update the row count
	tmp = "Current Row Count: ";
	sprintf(buff, "%d", mcl->getRowCount());
	tmp += buff;
	rowText->setText(tmp);

	// event was handled.
	return true;
}
