#ifndef _CEGUISAMPLE10_
#define _CEGUISAMPLE10_

#include "../CEGUIBase/CEGUIBaseApp.h"
#include <ctime>
#include <cstdlib>

namespace TimerProperties
{
	class Delay : public CEGUI::Property
	{
	public:
		Delay() :Property("Delay", "Property to get/set the current delay used by the timer.Value is a float.", "0.000000"){}
		CEGUI::String get(const CEGUI::PropertyReceiver * receiver) const;
		void set(CEGUI::PropertyReceiver * receiver,const CEGUI::String & value);
	};
}

class Timer : public CEGUI::Window
{
public:
	static const CEGUI::String WidgetTypeName;
	static const CEGUI::String EventNamespace;
	static const CEGUI::String EventTimerAlarm;

	Timer(const CEGUI::String& type,const CEGUI::String& name);
	virtual ~Timer(){}

	void start();
	void stop();
	bool isStarted() const;
	void setDelay(float delay);
	float getDelay() const;

protected:
	virtual void updateSelf(float elapsed);
	virtual bool testClassName_impl(const CEGUI::String& class_name) const 
	{
		if(class_name == "Timer")
			return true;
		return CEGUI::Window::testClassName_impl(class_name);
	}
	
private:
	static TimerProperties::Delay mDelayProperty;
	float mDelay;
	float mStarted;
	float mCurrentValue;
	void addTimerProperties();
};

class TimerFactory : public CEGUI::WindowFactory
{
public :
	TimerFactory() : CEGUI::WindowFactory(Timer::WidgetTypeName)
	{}

	CEGUI::Window * createWindow(const CEGUI::String& name)
	{
		return new Timer(d_type,name);
	}
	void destroyWindow(CEGUI::Window * window)
	{
		delete window;
	}
};

TimerFactory& getTimerFactory();

#define size_t unsigned int 

struct Location
{
	size_t mRow;
	size_t mCol;
};

const size_t MinesweeperSize = 10;
const size_t MineCount = 15;

class CEGUISample10App:public CEGUIBaseApp
{
public :
	void createGUI();

protected:
	bool handleGameStartClicked(const CEGUI::EventArgs& event);
	bool handleMineButtonClicked(const CEGUI::EventArgs& event);
	bool handleMineButtonDown(const CEGUI::EventArgs& event);
	bool handleUpdateTimer(const CEGUI::EventArgs& event);

	void boardReset();
	void boardPositionMines();
	bool isGameWin();
	void gameEnd(bool victory);
	bool boardDiscover(const Location& location);
	CEGUI::PushButton* mButtons[MinesweeperSize][MinesweeperSize];
	Location mButtonsMapping[MinesweeperSize][MinesweeperSize];
	size_t mBoard[MinesweeperSize][MinesweeperSize];
	size_t mBoardCellDiscovered;
	CEGUI::Editbox* mCounter;
	CEGUI::Editbox* mTimer;
	CEGUI::Window * mResult;

	bool mGameStarted;
	clock_t mTimerStartTime;
	clock_t mTimeValue;
	Timer* mAlarm;
};

#endif 