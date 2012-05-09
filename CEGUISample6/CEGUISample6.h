#ifndef _CEGUISAMPLE6_
#define _CEGUISAMPLE6_

#include "../CEGUIBase/CEGUIBaseApp.h"

using namespace CEGUI;

static struct
{
	utf8 * Language;
	utf8 * Font;
	utf8 * Text;
} LangList [] =
{	
	/*{ (utf8 *)"English",
	(utf8*)"DejaVuSans-10",
	(utf8 *)"THIS IS SOME TEXT IN UPPERCASE\n"
	"and this is lowercase...\n"
	"Try Catching The Brown Fox While It's Jumping Over The Lazy Dog" },*/
	{
		(utf8 *)"English",
            (utf8 *)"DejaVuSans-10",
			(utf8 *)"Hello,Let's play a game called ML"
	},
	{
		(utf8 *)"Chinese~~~@@#￥￥",
			(utf8 *)"Chinese-small",
			(utf8 *)"啦啦啦，啦啦啦，背好书包上学啦！"
	}/*,
		{ (utf8 *)"Korean",
		(utf8*)"Batang-26",
		(utf8 *)"한국어를 선택\n 트리거 검지\n 광석 비축부족\n" }*/
};
#define MIN_POINT_SIZE 26.0f

class MyListItem : public ListboxTextItem
{
public:
    MyListItem (const String& text, CEGUI::uint item_id = 0) :
        ListboxTextItem(text, item_id)
    {
        setSelectionBrushImage("TaharezLook", "MultiListSelectionBrush");
    }
};

class CEGUISample6App : public CEGUIBaseApp
{
public:
	void createGUI();
	bool handleAntialiased(const CEGUI::EventArgs &args);
	bool handleAutoScaled(const CEGUI::EventArgs &args);
	bool handleFontSelection(const CEGUI::EventArgs &args);
	bool handleLangSelection(const CEGUI::EventArgs &args);
	bool handlePointSize(const CEGUI::EventArgs &args);
	void setFontDesc();
	void updateTextWindows();
};

#endif