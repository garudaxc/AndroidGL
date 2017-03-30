#include "Thread.h"
#include "assert.h"
#include <vector>
#include "MyLog.h"

using namespace std;

namespace FancyTech
{

	class SectionLock
	{
	public:

		SectionLock()
		{
			InitializeCriticalSection(&m_cs);
		}

		~SectionLock()
		{
			DeleteCriticalSection(&m_cs);
		}

		void Lock()
		{
			EnterCriticalSection(&m_cs);
		}

		void Unlock()
		{
			LeaveCriticalSection(&m_cs);
		}

	private:
		CRITICAL_SECTION		m_cs;
	};



	struct MutexImpl
	{
		HANDLE	mutex;
	};

	Mutex::Mutex()
	{
		impl_ = new MutexImpl();

		impl_->mutex = CreateMutex(NULL, FALSE, NULL);
		if (impl_->mutex == NULL) {
			exit(0);
		}
	}

	Mutex::~Mutex()
	{
		CloseHandle(impl_->mutex);
		delete impl_;
	}

	void Mutex::Lock()
	{
		DWORD obj = WaitForSingleObject(impl_->mutex, INFINITE);
		if (obj == WAIT_FAILED) {
			GLog.LogError("WaitForSingleObject failed %d", GetLastError());
		}
	}

	void Mutex::Unlock()
	{
		BOOL result = ReleaseMutex(impl_->mutex);
	}

	bool Mutex::TryLock()
	{
		DWORD obj = WaitForSingleObject(impl_->mutex, 0);
		if (obj == WAIT_FAILED) {
			GLog.LogError("WaitForSingleObject failed %d", GetLastError());
		}

		return (obj == WAIT_OBJECT_0);
	}



	//////////////////////////////////////////////////////////////////////////



	struct EventImpl
	{
		HANDLE	hEvent;
	};

	Event::Event()
	{
		impl_ = new struct EventImpl();
		impl_->hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	}

	Event::~Event()
	{
		CloseHandle(impl_->hEvent);
		delete impl_;
	}


	void Event::Signal()
	{
		SetEvent(impl_->hEvent);
	}

	void Event::Reset()
	{
		ResetEvent(impl_->hEvent);
	}

	int Event::WaitOn(int timeOut)
	{
		uint32_t result = WaitForSingleObject(impl_->hEvent, timeOut == 0 ? INFINITE : timeOut);

		if (result == WAIT_TIMEOUT)
		{
			return (int)FANCY_WAIT_TIMEOUT;
		}

		if (result == WAIT_OBJECT_0)
		{
			return (int)FANCY_WAIT_SIGNALED;
		}

		return (int)FANCY_WAIT_NONE;
	}



#if 0


	class AutoResetEvent : public Event
	{
	public:
		AutoResetEvent()
		{
			m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
			assert(m_hEvent);
		}

		virtual ~AutoResetEvent()
		{
			CloseHandle(m_hEvent);
		}

		inline void		SignalAndReset();

		inline int		WaitOn(int timeOut);
	};

	void AutoResetEvent::SignalAndReset()
	{
		SetEvent(m_hEvent);
	}

	int AutoResetEvent::WaitOn(int timeOut)
	{
		uint32_t result = WaitForSingleObject(m_hEvent, timeOut == 0 ? INFINITE : timeOut);

		if (result == WAIT_TIMEOUT)
		{
			return (int)H3D_WAIT_TIMEOUT;
		}

		if (result == WAIT_OBJECT_0)
		{
			return (int)H3D_WAIT_SIGNALED;
		}

		return (int)H3D_WAIT_NONE;
	}


	class EventPool
	{
	public:

		EventPool();
		~EventPool();

		void	RegisterEvent(Event* pEvent);
		void	UnRegisterEvent(Event* pEvent);

		int		GetNumEvent() const;

		int		WaitAll(int nMilliseconds);
		int		WaitAny(int nMilliseconds);

	private:
		vector<HANDLE>	m_Handles;
	};


	EventPool::EventPool()
	{

	}

	EventPool::~EventPool()
	{

	}

	void EventPool::RegisterEvent(Event* pEvent)
	{
		m_Handles.push_back(pEvent->GetHandle());
	}

	void EventPool::UnRegisterEvent(Event* pEvent)
	{
		vector<HANDLE>::iterator it = std::find(m_Handles.begin(), m_Handles.end(), pEvent->GetHandle());
		assert(it != m_Handles.end());

		m_Handles.erase(it);
	}

	int EventPool::GetNumEvent() const
	{
		return m_Handles.size();
	}

	int EventPool::WaitAll(int nMilliseconds)
	{
		if (m_Handles.empty())
		{
			return (int)H3D_WAIT_NONE;
		}
		uint32_t result = WaitForMultipleObjects(m_Handles.size(), &m_Handles[0], TRUE,
			nMilliseconds == 0 ? INFINITE : nMilliseconds);
		if (result == WAIT_TIMEOUT)
		{
			//LogInfo(BEAM_INFO,OutPut_Console, "WaitForMultipleObjects time out!\n");
			return (int)H3D_WAIT_TIMEOUT;
		}

		if (result - WAIT_OBJECT_0 < m_Handles.size())
		{
			return (int)H3D_WAIT_SIGNALED;
		}

		return (int)H3D_WAIT_NONE;
	}

	int EventPool::WaitAny(int nMilliseconds)
	{
		if (m_Handles.empty())
		{
			return (int)H3D_WAIT_NONE;
		}

		uint32_t result = WaitForMultipleObjects(m_Handles.size(), &m_Handles[0], FALSE,
			nMilliseconds == 0 ? INFINITE : nMilliseconds);
		if (result == WAIT_TIMEOUT)
		{
			//LogInfo(BEAM_INFO,OutPut_Console, "WaitForMultipleObjects time out!\n");
			return (int)H3D_WAIT_TIMEOUT;
		}

		if (result - WAIT_OBJECT_0 < m_Handles.size())
		{
			return (int)H3D_WAIT_SIGNALED;
		}

		return (int)H3D_WAIT_NONE;
	}

#endif

	//////////////////////////////////////////////////////////////////////////


	struct ThreadImpl
	{
		string	name;
		HANDLE	hThread;
		DWORD	threadId;
		bool	toStop;
		Event	event;

		ThreadImpl(const string& _name) :name(_name), hThread(NULL), threadId(0), toStop(false)
		{
			event.Signal();
		}
	};

	static DWORD WINAPI ThreadFunc_s(LPVOID arg)
	{
		Thread* thread = (Thread*)arg;

		void* r = thread->Run();

		return 0;
	}


	Thread::Thread(const string& name)
	{
		impl_ = new ThreadImpl(name);
	}

	Thread::~Thread()
	{
		delete impl_;
	}

	uint32_t Thread::CurrentThreadId()
	{
		return (uint32_t)GetCurrentThreadId();
	}

	uint32_t Thread::Id() const
	{
		return (uint32_t)impl_->threadId;
	}

	void Thread::Suspend()
	{
		impl_->event.Reset();
	}

	void Thread::Resume()
	{
		impl_->event.Signal();
	}

	void Thread::Stop()
	{
		impl_->toStop = true;
		Resume();

		if (impl_->hThread != NULL) {
			uint32_t result = WaitForSingleObject(impl_->hThread, INFINITE);
			if (result != WAIT_OBJECT_0) {
				GLog.LogError("stop thread error! id %d", impl_->threadId);
			}

			CloseHandle(impl_->hThread);
			impl_->hThread = NULL;
			impl_->threadId = 0;
			impl_->toStop = false;
		}
	}

	bool Thread::Check()
	{
		impl_->event.WaitOn(0);

		if (impl_->toStop) {
			return false;
		}

		return true;
	}

	bool Thread::Create()
	{
		impl_->hThread = ::CreateThread(
			NULL,              // default security attributes
			0,                 // use default stack size  
			ThreadFunc_s,        // thread function 
			(LPVOID)this,             // argument to thread function 
			0/*CREATE_SUSPENDED*/,         // use default creation flags 
			(DWORD*)&impl_->threadId);   // returns the thread identifier 
		assert(impl_->threadId != NULL);

		if (impl_->hThread == NULL)	{
			GLog.LogError("Create thread failed");
			return false;
		}

		return true;
	}

}