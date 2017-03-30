#include "Input.h"
#include "MyLog.h"

namespace FancyTech
{

	static Input input_;
	Input* GInput = &input_;

	static EventReceiver* EventStaticInit = NULL;

	Input::Input() {
	}

	Input::~Input()
	{
	}
	
	void Input::Create()
	{
		EventReceiver* ptr = EventStaticInit;
		while (ptr != NULL)	{
			Register(ptr);
			ptr = ptr->InitNext;
		}
	}

	void Input::Register(EventReceiver* receiver)
	{
		receivers_.push_back(receiver);
	}

	void Input::Dispatch(const Event& event)
	{
		for (auto it = receivers_.begin(); it != receivers_.end(); ++it) {
			(*it)->OnEvent(event);
		}
	}

	EventReceiver::EventReceiver()
	{
		InitNext = EventStaticInit;
		EventStaticInit = this;
	}

	EventReceiver::~EventReceiver()
	{
	}

}