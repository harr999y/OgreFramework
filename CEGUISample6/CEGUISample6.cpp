#include "CEGUISample6.h"

using namespace CEGUI;

void CEGUISample6App::createGUI()
{
	// we will use of the WindowManager.
	WindowManager& winMgr = WindowManager::getSingleton ();

	// load scheme and set up defaults
	SchemeManager::getSingleton().create("TaharezLook.scheme");
	SchemeManager::getSingleton().create("VanillaSkin.scheme");
	System::getSingleton().setDefaultMouseCursor ("TaharezLook", "MouseArrow");

	/*FontManager::getSingleton().createFreeTypeFont("DefaultFont",10,true,"DejaVuSans.ttf","",false,640.0f,480.f,XREA_RETURN,CEGUI::String("0xF0000FFF"));
	System::getSingleton().setDefaultFont("DefaultFont");*/
	FontManager::getSingleton().createAll("*.font","fonts");

	// load an image to use as a background
	//ImagesetManager::getSingleton().createFromImageFile("BackgroundImage", "GPN-2000-001437.tga");

	// here we will use a StaticImage as the root, then we can use it to place a background image
	//Window* background = winMgr.createWindow ("TaharezLook/StaticImage");
	// set area rectangle
	//background->setArea (URect (cegui_reldim (0), cegui_reldim (0),
	//	cegui_reldim (1), cegui_reldim (1)));
	// disable frame and standard background
	//background->setProperty ("FrameEnabled", "false");
	//background->setProperty ("BackgroundEnabled", "false");
	// set the background image
	//background->setProperty("Image","set:BackgroundImage image:full_image");
	// install this as the root GUI sheet
	//System::getSingleton ().setGUISheet (background);

	// set tooltip styles (by default there is none)
	System::getSingleton ().setDefaultTooltip ("TaharezLook/Tooltip");

	// load some demo windows and attach to the background 'root'
	mRootWindow->addChildWindow (winMgr.loadWindowLayout ("FontDemo.layout"));

	// Add the font names to the listbox
	Listbox *lbox = static_cast<Listbox *> (winMgr.getWindow ("FontDemo/FontList"));
	lbox->setFont("DejaVuSans-10");

	FontManager::FontIterator fi = FontManager::getSingleton().getIterator();    
	while (!fi.isAtEnd())
	{
		// exclude the special DefaultFont!
		if (fi.getCurrentKey() != String("DejaVuSans-10"))
			lbox->addItem(new MyListItem(fi.getCurrentKey()));
		++fi;
	}
	lbox->subscribeEvent(Listbox::EventSelectionChanged,Event::Subscriber(&CEGUISample6App::handleFontSelection,this));
	lbox->setItemSelectState(unsigned int (0),true);

	lbox = static_cast<Listbox *> (winMgr.getWindow ("FontDemo/LangList"));
	lbox->setFont("DejaVuSans-10");
	for(unsigned int i = 0;i < (sizeof(LangList) / sizeof(LangList[0])); i++)
	{
		if(FontManager::getSingleton().isDefined(String(LangList[i].Font)))
			lbox->addItem(new MyListItem(LangList[i].Language,i));
	}

	lbox->subscribeEvent(Listbox::EventSelectionChanged,Event::Subscriber (&CEGUISample6App::handleLangSelection, this));

	lbox->setItemSelectState (size_t (1), true);

	//winMgr.getWindow("FontDemo/FontSample")->setProperty("WordWrap","false");
	
	winMgr.getWindow("FontDemo/AutoScaled")->subscribeEvent (
		Checkbox::EventCheckStateChanged,
		Event::Subscriber (&CEGUISample6App::handleAutoScaled, this));
	winMgr.getWindow("FontDemo/Antialiased")->subscribeEvent (
		Checkbox::EventCheckStateChanged,
		Event::Subscriber (&CEGUISample6App::handleAntialiased, this));
	winMgr.getWindow("FontDemo/PointSize")->subscribeEvent (
		Scrollbar::EventScrollPositionChanged,
		Event::Subscriber (&CEGUISample6App::handlePointSize, this));
}

void CEGUISample6App::setFontDesc()
{
	WindowManager& winMgr = WindowManager::getSingleton ();

	MultiLineEditbox *mle = static_cast<MultiLineEditbox *>
		(winMgr.getWindow("FontDemo/FontSample"));

	// Query the font from the textbox
	Font *f = mle->getFont ();

	// Build up the font name...
	String s = f->getProperty ("Name");
	if (f->isPropertyPresent ("PointSize")) 
		s += "." + f->getProperty ("PointSize");//??????????????????????????

	// ...and set it
	winMgr.getWindow("FontDemo/FontDesc")->setText (s);
}

/** Called when the used selects a different font from the font list.*/
bool CEGUISample6App::handleFontSelection (const EventArgs& args)
{
	// Access the listbox which sent the event
	Listbox *lbox = static_cast<Listbox*> (
 		static_cast<const WindowEventArgs&> (args).window);

	if (lbox->getFirstSelectedItem ())
	{	// Read the fontname and get the font by that name
		Font *font = &FontManager::getSingleton ().get(
			lbox->getFirstSelectedItem ()->getText ());

		// Tell the textbox to use the newly selected font
		WindowManager& winMgr = WindowManager::getSingleton ();
		winMgr.getWindow("FontDemo/FontSample")->setFont (font);

		bool b = font->isPropertyPresent ("AutoScaled");
		Checkbox *cb = static_cast<Checkbox *> (winMgr.getWindow("FontDemo/AutoScaled"));
		cb->setEnabled (b);
		if (b)
			cb->setSelected (PropertyHelper::stringToBool (font->getProperty ("AutoScaled")));

 		b = font->isPropertyPresent ("Antialiased");
		cb = static_cast<Checkbox *> (winMgr.getWindow("FontDemo/Antialiased"));
		cb->setEnabled (b);
		if (b)
			cb->setSelected (PropertyHelper::stringToBool (font->getProperty ("Antialiased")));

		b = font->isPropertyPresent ("PointSize");
		Scrollbar *sb = static_cast<Scrollbar *> (
			winMgr.getWindow("FontDemo/PointSize"));
		sb->setEnabled (b);

		// Set the textbox' font to have the current scale
		if (font->isPropertyPresent("PointSize"))
			font->setProperty ("PointSize",
			PropertyHelper::intToString (
			int (MIN_POINT_SIZE + sb->getScrollPosition ())));
  
		setFontDesc ();
	}

	return true;
}

bool CEGUISample6App::handleAutoScaled (const EventArgs& args)
{
	WindowManager& winMgr = WindowManager::getSingleton ();

	Checkbox *cb = static_cast<Checkbox *> (
		static_cast<const WindowEventArgs&> (args).window);

	MultiLineEditbox *mle = static_cast<MultiLineEditbox *>
		(winMgr.getWindow("FontDemo/FontSample"));

	Font *f = mle->getFont ();
	f->setProperty ("AutoScaled",
		PropertyHelper::boolToString (cb->isSelected ()));

	updateTextWindows();
	return true;
}

bool CEGUISample6App::handleAntialiased (const EventArgs& args)
{
	WindowManager& winMgr = WindowManager::getSingleton ();

	Checkbox *cb = static_cast<Checkbox *> (
		static_cast<const WindowEventArgs&> (args).window);

	MultiLineEditbox *mle = static_cast<MultiLineEditbox *>
		(winMgr.getWindow("FontDemo/FontSample"));

	Font *f = mle->getFont ();
	f->setProperty ("Antialiased",
		PropertyHelper::boolToString (cb->isSelected ()));

	updateTextWindows();
	return true;
}

bool CEGUISample6App::handlePointSize (const EventArgs& args)
{
	WindowManager& winMgr = WindowManager::getSingleton ();

	Scrollbar *sb = static_cast<Scrollbar *> (
		static_cast<const WindowEventArgs&> (args).window);

	Font *f = winMgr.getWindow ("FontDemo/FontSample")->getFont ();

	f->setProperty ("PointSize",
		PropertyHelper::intToString (
		int (MIN_POINT_SIZE + sb->getScrollPosition ())));

	setFontDesc ();

	updateTextWindows();
	return true;
}

/** User selects a new language. Change the textbox content, and start with
the recommended font. */
bool CEGUISample6App::handleLangSelection (const EventArgs& args)
{
	// Access the listbox which sent the event
	Listbox *lbox = static_cast<Listbox *> (
		static_cast<const WindowEventArgs&> (args).window);

	if (lbox->getFirstSelectedItem ())
	{
		ListboxItem* sel_item = lbox->getFirstSelectedItem();
		int idx = sel_item ? sel_item->getID() : 0;
		const String fontName(LangList[idx].Font);

		WindowManager& winMgr = WindowManager::getSingleton ();
		// Access the font list
		Listbox *fontList = static_cast<Listbox*>(winMgr.getWindow ("FontDemo/FontList"));
		ListboxItem* lbi = fontList->findItemWithText(fontName, 0);
		// Select correct font when not set already
		if (lbi && !lbi->isSelected())
		{	// This will cause 'handleFontSelection' to get called(!)
			fontList->setItemSelectState(lbi, true);
		}

		// Finally, set the sample text for the selected language
		winMgr.getWindow ("FontDemo/FontSample")->setText ((utf8*)LangList [idx].Text);
	}

	return true;
}

//! Ensure window content and layout is updated.
void CEGUISample6App::updateTextWindows()
{
	WindowManager& winMgr(WindowManager::getSingleton());
	MultiLineEditbox* eb = static_cast<MultiLineEditbox*>(
		winMgr.getWindow("FontDemo/FontSample"));
	// this is a hack to force the editbox to update it's state, and is
	// needed because no facility currently exists for a font to notify that
	// it's internal size or state has changed (ideally all affected windows
	// should receive EventFontChanged - this should be a TODO item!)
	eb->setWordWrapping(false);
	eb->setWordWrapping(true);
	// inform lists of updated data too
	Listbox* lb = static_cast<Listbox*>(winMgr.getWindow("FontDemo/LangList"));
	lb->handleUpdatedItemData();
	lb = static_cast<Listbox*>(winMgr.getWindow("FontDemo/FontList"));
	lb->handleUpdatedItemData();
}