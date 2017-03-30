#include "Input.h"
#include <windows.h>
#include "Platfrom.h"

namespace FancyTech
{


	void DispatchWindowsEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		static uint16_t eventType[] = { Event::MouseMove,
			Event::LButtonDown, Event::LButtonUp, Event::LButtonDown,
			Event::RButtonDown, Event::RButtonUp, Event::RButtonDown,
			Event::MButtonDown, Event::MButtonUp, Event::MButtonDown,
			Event::MouseWheel };


		switch (message) {
				// mouse event
			case WM_MOUSEMOVE:
			case WM_LBUTTONDOWN:
			case WM_LBUTTONUP:
			case WM_LBUTTONDBLCLK:
			case WM_MBUTTONDOWN:
			case WM_MBUTTONUP:
			case WM_MBUTTONDBLCLK:
			case WM_RBUTTONDOWN:
			case WM_RBUTTONUP:
			case WM_RBUTTONDBLCLK:
			case WM_MOUSEWHEEL:
			{
								  Event mouseEvent;

								  mouseEvent.Type = eventType[message - 0x0200];
								  mouseEvent.Param = (uint16_t)wParam;

								  mouseEvent.xPos = (int)LOWORD(lParam);
								  mouseEvent.yPos = (int)HIWORD(lParam);

								  if (message == WM_MOUSEWHEEL)
									  mouseEvent.zDelta = (short)HIWORD(wParam);
								  else
									  mouseEvent.zDelta = 0;


								  GInput->Dispatch(mouseEvent);
								  break;
			}

				// key event
			case WM_KEYDOWN:
			case WM_KEYUP:
			{
							 Event keyEvent;
							 keyEvent.Code = (unsigned int)wParam;
							 if (message == WM_KEYDOWN)
								 keyEvent.Type = Event::KeyDown;
							 else
								 keyEvent.Type = Event::KeyUp;

							 GInput->Dispatch(keyEvent);
							 break;
			}
		}

	}


}