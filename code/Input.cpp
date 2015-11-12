#include "Input.h"

namespace FancyTech
{

	Input input_;
	Input* GInput = NULL;

	static EventReceiver* EventStaticInit = NULL;

	Input::Input() {
		EventReceiver* ptr = EventStaticInit;
		while (ptr != NULL)	{
			Register(ptr);
			ptr = ptr->InitNext;
		}

		GInput = this;
	}

	Input::~Input()
	{
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


	EventReceiver::EventReceiver() :InitNext(NULL)
	{
		if (GInput != NULL) {
			GInput->Register(this);
		} else {
			InitNext = EventStaticInit;
			EventStaticInit = this;
		}
	}

	EventReceiver::~EventReceiver()
	{
	}




}