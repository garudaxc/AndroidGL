#include "Platfrom.h"
#include <string>

using namespace std;

namespace FancyTech
{

enum
{
	FANCY_WAIT_NONE = 0,
	FANCY_WAIT_TIMEOUT,
	FANCY_WAIT_SIGNALED,
};


class Mutex
{
public:
	Mutex();
	~Mutex();

	void	Lock();
	void	Unlock();
	bool	TryLock();

private:
	struct MutexImpl*	impl_;
};

class AutoLock
{
public:
	AutoLock(Mutex* mutex)
	{
		mutex_ = mutex;
		mutex_->Lock();
	}

	~AutoLock()
	{
		mutex_->Unlock();
	}

private:
	Mutex*	mutex_;
};


class Event
{
public:
	Event();
	~Event();

	void	Reset();
	void	Signal();
	int		WaitOn(int timeout);

private:
	struct EventImpl* impl_;
};


class Thread
{
public:
	Thread(const string& name = "");
	virtual ~Thread();

	static	uint32_t  CurrentThreadId();

	uint32_t	Id() const;

	bool	Create();

	void	Suspend();
	void	Resume();
	void	Stop();

	virtual void*		Run() = 0;

protected:
	// thread should call this function in looping
	// it will be blocked when thread is suspended
	// when it return false loop should break and thread will stop
	bool	Check();

private:

	struct ThreadImpl* impl_;
};


}