#include <iostream>
#include <cvt/gui/Application.h>
#include <cvt/gui/Window.h>

#include <cvt/gui/EventTimer.h>
#include <cvt/util/Time.h>

using namespace cvt;

class MyTimer : public EventTimer
{
	public:
	MyTimer()
	{
	}

	void timeoutEvent()
	{
		std::cout << "Timer: " << _last.elapsedMilliSeconds() << std::endl;
		_last.reset();
	}

	Time _last;
};

int main(int argc, char* argv[])
{
	MyTimer mytimer;
    Window w( "Test" );
	w.setSize( 640, 480 );
	w.setVisible( true );

	Application::instance()->registerTimer( 30, &mytimer );

    Application::run();
	return 0;
}
