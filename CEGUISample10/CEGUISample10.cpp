#include "CEGUISample10.h"

namespace TimerProperties
{
	CEGUI::String Delay::get(const CEGUI::PropertyReceiver * receiver) const
	{
		return CEGUI::PropertyHelper::floatToString(static_cast<const Timer*>(receiver)->getDelay());
	}
	void Delay::set(CEGUI::PropertyReceiver *receiver, const CEGUI::String &value)
	{
		static_cast<Timer*>(receiver)->setDelay(CEGUI::PropertyHelper::stringToFloat(value));
	}
}

//Constants

const CEGUI::String Timer::WidgetTypeName("Timer");
const CEGUI::String Timer::EventNamespace("Timer");
const CEGUI::String Timer::EventTimerAlarm("EventTimerAlarm");

//Timer
TimerProperties::Delay Timer::mDelayProperty; //给类中的私有静态成员可以这么赋值~~

Timer::Timer(const CEGUI::String &type, const CEGUI::String &name) : Window(type,name),mDelay(0),mCurrentValue(0),mStarted(false)
{
	CEGUI::UVector2 size(CEGUI::UDim(1.0f,0.0f),CEGUI::UDim(1.0f,0.0f));
	setMaxSize(size);
	setSize(size);
	addTimerProperties();
}

void Timer::start()
{
	mStarted = true;
}

void Timer::stop()
{
	mStarted = false;
}

bool Timer::isStarted() const
{
	return mStarted ? true : false;
}

void Timer::setDelay(float delay)
{
	mDelay = delay;
}

float Timer::getDelay() const
{
	return mDelay;
}

void Timer::updateSelf(float elapsed)
{
	if(mDelay > 0 && mStarted)
	{
		mCurrentValue += elapsed;
		if(mCurrentValue >= mDelay)
		{
			mCurrentValue -= mDelay;
			CEGUI::WindowEventArgs args(this);
			fireEvent(EventTimerAlarm,args,EventNamespace);
		}
	}
}

void Timer::addTimerProperties()
{
	addProperty(&mDelayProperty);
}

TimerFactory& getTimerFactory()
{
	static TimerFactory sFactory;
	return sFactory;
}


//App
void CEGUISample10App::createGUI()
{
    using namespace CEGUI;
    // Register Timer Window
    WindowFactoryManager::getSingleton().addFactory( &getTimerFactory() );

    mGameStarted = false;

    // Get window manager which we wil use for a few jobs here.
    WindowManager& winMgr = WindowManager::getSingleton();

	// Load the scheme to initialse the VanillaSkin which we use in this sample
    SchemeManager::getSingleton().create("VanillaSkin.scheme");
    SchemeManager::getSingleton().create("TaharezLook.scheme");
    System::getSingleton().setDefaultTooltip("TaharezLook/Tooltip");

    // set default mouse image
    System::getSingleton().setDefaultMouseCursor("Vanilla-Images", "MouseArrow");

    // Load font
    FontManager::getSingleton().create("DejaVuSans-10.font");

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
	mAlarm = (Timer *)winMgr.createWindow("Timer");
	background->addChildWindow(mAlarm);
	mAlarm->setDelay(.5f);

	Window* frame = winMgr.createWindow("Vanilla/FrameWindow");
	mAlarm->addChildWindow(frame);
	frame->setXPosition(UDim(.3f,.0f));
	frame->setYPosition(UDim(.15f,.0f));
	frame->setWidth(UDim(.4f,.0f));
	frame->setHeight(UDim(.7f,.0f));
	frame->setText("CEGUI MineSweeper");

	Window* action = winMgr.createWindow("DefaultWindow");
	frame->addChildWindow(action);
	action->setXPosition(UDim(.03f,0.0f));
	action->setYPosition(UDim(.10f,.0f));
	action->setWidth(UDim(.94f,.0f));
	action->setHeight(UDim(.1f,.0f));
	mCounter = (Editbox*)winMgr.createWindow("Vanilla/Editbox","mine_counter");
	action->addChildWindow(mCounter);
	mCounter->setText("0");
	mCounter->setTooltipText("Number of Mine");
	mCounter->setReadOnly(true);
	mCounter->setXPosition(UDim(0.0f, 0.0f));
	mCounter->setYPosition(UDim(0.0f, 0.0f));
    mCounter->setWidth(UDim(0.3f, 0.0f));
    mCounter->setHeight(UDim(1.0f, 0.0f));
	Window* newGame = winMgr.createWindow("Vanilla/Button", "new_game");
    action->addChildWindow(newGame);
    newGame->setText("Start");
    newGame->setTooltipText("Start a new game");
    newGame->setXPosition(UDim(0.35f, 0.0f));
    newGame->setYPosition(UDim(0.0f, 0.0f));
    newGame->setWidth(UDim(0.3f, 0.0f));
    newGame->setHeight(UDim(1.0f, 0.0f));
    newGame->subscribeEvent(PushButton::EventClicked,  Event::Subscriber(&CEGUISample10App::handleGameStartClicked, this));

    mTimer = (Editbox*)winMgr.createWindow("Vanilla/Editbox", "timer");
    action->addChildWindow(mTimer);
    mTimer->setText("0");
    mTimer->setTooltipText("Time elapsed");
    mTimer->setReadOnly(true);
    mTimer->setXPosition(UDim(0.7f, 0.0f));
    mTimer->setYPosition(UDim(0.0f, 0.0f));
    mTimer->setWidth(UDim(0.3f, 0.0f));
    mTimer->setHeight(UDim(1.0f, 0.0f));
    mAlarm->subscribeEvent(Timer::EventTimerAlarm, Event::Subscriber(&CEGUISample10App::handleUpdateTimer, this));

    // Board button grid
    Window* grid = winMgr.createWindow("DefaultWindow");
    frame->addChildWindow(grid);
    grid->setXPosition(UDim(0.03f, 0.0f));
    grid->setYPosition(UDim(0.23f, 0.0f));
    grid->setWidth(    UDim(0.94f, 0.0f));
    grid->setHeight(   UDim(0.74f, 0.0f));

	const float inc = 1.0f / MinesweeperSize;
	for(size_t i = 0;i < MinesweeperSize; ++i)
	{
		Window* row = winMgr.createWindow("DefaultWindow");
		row->setArea(URect(UDim(0.0f,0.f),UDim(inc * i,0.0f),UDim(1.0f,0.0f),UDim(inc * (i+1),0.0f)));
		grid->addChildWindow(row);
		for(size_t j=0;j<MinesweeperSize;++j)
		{
			mButtonsMapping[i][j].mCol = j;
			mButtonsMapping[i][j].mRow = i;
			mButtons[i][j] = (PushButton*)winMgr.createWindow("Vanilla/Button");
			row->addChildWindow(mButtons[i][j]);
			mButtons[i][j]->setArea(URect(UDim(inc * j, 0), UDim(0,0),UDim(inc * (j + 1), 0), UDim(1,0)));
			mButtons[i][j]->setEnabled(false);
			mButtons[i][j]->setUserData(&(mButtonsMapping[i][j]));
			mButtons[i][j]->setID(0);
            // Connect event handlers
            mButtons[i][j]->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&CEGUISample10App::handleMineButtonClicked, this));
            mButtons[i][j]->subscribeEvent(Window::EventMouseButtonDown, Event::Subscriber(&CEGUISample10App::handleMineButtonDown, this));
		}
	}

	mResult = winMgr.createWindow("Vanilla/StaticText");
	grid->addChildWindow(mResult);
    mResult->setXPosition(UDim(0.0, 0.0));
    mResult->setYPosition(UDim(0.0, 0.0));
    mResult->setWidth(UDim(1.0, 0.0));
    mResult->setHeight(UDim(1.0, 0.0));
    mResult->setAlwaysOnTop(true);
    mResult->setProperty("HorzFormatting", "HorzCentred");
    mResult->setVisible(false);
    mResult->setAlpha(0.67f);
    // activate the background window
    background->activate();
}

bool CEGUISample10App::handleGameStartClicked(const CEGUI::EventArgs &event)
{
	mResult->setVisible(false);
	boardReset();
	boardPositionMines();
	for(size_t i=0;i<MinesweeperSize;++i)
	{
		for(size_t j=0;j<MinesweeperSize;++j)
		{
			mButtons[i][j]->setProperty("DisabledTextColour","FF000000");
			mButtons[i][j]->setText("");
			mButtons[i][j]->setEnabled(true);
		}
	}
	mCounter->setText(CEGUI::PropertyHelper::uintToString(MineCount));
	mTimerStartTime = ::clock();
	mTimeValue = 0;
	mTimer->setText("0");
	mGameStarted = true;
	mAlarm->start();
	return true;
}

bool CEGUISample10App::handleMineButtonClicked(const CEGUI::EventArgs &event)
{
	const CEGUI::WindowEventArgs* evt = static_cast<const CEGUI::WindowEventArgs *>(&event);
	CEGUI::PushButton* button = static_cast<CEGUI::PushButton *>(evt->window);
	Location* buttonLoc = static_cast<Location *>(button->getUserData());
	if(button->getID() > 0)
		return true;
	if(boardDiscover(*buttonLoc))
	{
		button->setText(CEGUI::PropertyHelper::uintToString(mBoard[buttonLoc->mRow][buttonLoc->mCol]));
		if(isGameWin())
			gameEnd(true);
	}
	else
	{
		for(size_t i=0;i<MinesweeperSize;++i)
		{
			for(size_t j=0;j<MinesweeperSize;++j)
			{
				if(!mButtons[i][j]->isDisabled())
				{
					if(mBoard[i][j] > 8)
					{
						mButtons[i][j]->setText("B");
						mButtons[i][j]->setProperty("DisabledTextColour","FFFF1010");
					}
					else
					{
						mButtons[i][j]->setText(CEGUI::PropertyHelper::uintToString(mBoard[i][j]));
					}
				}
				mButtons[i][j]->setEnabled(false);
			}
		}
		gameEnd(false);
	}
	return true;
}

bool CEGUISample10App::handleMineButtonDown(const CEGUI::EventArgs &event)
{
    const CEGUI::MouseEventArgs& me = static_cast<const CEGUI::MouseEventArgs&>(event);
    if (me.button == CEGUI::RightButton)
    {
        CEGUI::Window* button = me.window;
        if (!button->isDisabled())
        {
            if (button->getID() == 0)
            {
                button->setID(1);
                button->setText("F");

            }
            else
            {
                button->setID(0);
                button->setText("");
            }
            return true;
        }
    }	

	return false;
}

bool CEGUISample10App::handleUpdateTimer(const CEGUI::EventArgs &event)
{
	if(mGameStarted)
	{
		clock_t time = ::clock();
		time -= mTimerStartTime;
		if(time != mTimeValue)
		{
			mTimer->setText(CEGUI::PropertyHelper::uintToString(time / CLOCKS_PER_SEC));	
			mTimeValue = time;
		}
	}
	return true;
}

void CEGUISample10App::boardReset()
{
    mBoardCellDiscovered = 0;
    for(size_t i = 0 ; i < MinesweeperSize ; ++i)
    {
        for(size_t j = 0 ; j < MinesweeperSize ; ++j)
        {
            mBoard[i][j] = 0;
        }
    }
}

void CEGUISample10App::boardPositionMines()
{
    size_t x = 0 ;
    size_t y = 0 ;
    ::srand(::clock());
    for(size_t i = 0 ; i < MineCount ; ++i)
    {
        do
        {
            x = (size_t) ((float)MinesweeperSize * (::rand() / (RAND_MAX + 1.0)));
            y = (size_t) ((float)MinesweeperSize * (::rand() / (RAND_MAX + 1.0)));
        }
        while(mBoard[x][y] > 8);

        mBoard[x][y] += 10;
        if (x > 0)
        {
            if (y > 0)
               ++ mBoard[x - 1][y - 1];

            ++ mBoard[x - 1][y    ];

            if (y < MinesweeperSize - 1)
                ++ mBoard[x - 1][y + 1];
        }

        if (y > 0)
            ++ mBoard[x    ][y - 1];

        if (y < MinesweeperSize - 1)
            ++ mBoard[x    ][y + 1];

        if (x < MinesweeperSize - 1)
        {
            if (y > 0)
                ++ mBoard[x + 1][y - 1];

            ++ mBoard[x + 1][y    ];

            if (y < MinesweeperSize - 1)
                ++ mBoard[x + 1][y + 1];
        }
    }
}

bool CEGUISample10App::isGameWin()
{
    return mBoardCellDiscovered + MineCount == MinesweeperSize * MinesweeperSize;
}

void CEGUISample10App::gameEnd(bool victory)
{
	mGameStarted =false;
	mAlarm->stop();
	CEGUI::String message;
	if(victory)
		message = CEGUI::String((CEGUI::utf8*)"You win!!!");
	else
		message = CEGUI::String((CEGUI::utf8*)"You lose!!!");
	mResult->setText(message);
	mResult->setVisible(true);
}

bool CEGUISample10App::boardDiscover(const Location& loc)
{
    CEGUI::PushButton* btn = mButtons[loc.mRow][loc.mCol];
    if (btn->isDisabled() || btn->getID() > 0)
        return true;

    if (mBoard[loc.mRow][loc.mCol] > 8)
        return false;
    mButtons[loc.mRow][loc.mCol]->setText(CEGUI::PropertyHelper::uintToString(mBoard[loc.mRow][loc.mCol]));
    mButtons[loc.mRow][loc.mCol]->setEnabled(false);
    ++mBoardCellDiscovered;
    // Discover surrounding case
    if (mBoard[loc.mRow][loc.mCol] == 0)
    {
        Location l;
        if (loc.mRow > 0)
        {
            l.mRow = loc.mRow - 1;
            if ( loc.mCol > 0)
            {
                l.mCol = loc.mCol - 1;
                boardDiscover(l);
            }
            l.mCol = loc.mCol;
            boardDiscover(l);
            if ( loc.mCol < MinesweeperSize - 1)
            {
                l.mCol = loc.mCol + 1;
                boardDiscover(l);
            }
        }
        l.mRow = loc.mRow;
        if ( loc.mCol > 0)
        {
            l.mCol = loc.mCol - 1;
            boardDiscover(l);
        }
        if ( loc.mCol < MinesweeperSize  - 1)
        {
            l.mCol = loc.mCol + 1;
            boardDiscover(l);
        }
        if (loc.mRow < MinesweeperSize - 1)
        {
            l.mRow = loc.mRow + 1;
            if ( loc.mCol > 0)
            {
                l.mCol = loc.mCol - 1;
                boardDiscover(l);
            }
            l.mCol = loc.mCol;
            boardDiscover(l);
            if ( loc.mCol < MinesweeperSize - 1)
            {
                l.mCol = loc.mCol + 1;
                boardDiscover(l);
            }
        }
    }
    return true;
}
 