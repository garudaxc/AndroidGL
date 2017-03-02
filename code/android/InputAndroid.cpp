#include "Input.h"
#include <android/input.h>
#include "MyLog.h"

namespace FancyTech
{

	void DispatchAndroidEvent(int action, float x, float y)
	{
		GLog.LogInfo("action %d", action);

		Event event;

		event.Type = Event::Unknown;
		event.fxPos = x;
		event.fyPos = y;

		if (action == AMOTION_EVENT_ACTION_DOWN) {
			event.Type = Event::TouchDown;
		} else if (action == AMOTION_EVENT_ACTION_UP) {
			event.Type = Event::TouchUp;
		} else if (action == AMOTION_EVENT_ACTION_MOVE) {
			event.Type = Event::MouseMove;
		}

		if (event.Type != Event::Unknown) {
			GInput->Dispatch(event);
		}
	}



}