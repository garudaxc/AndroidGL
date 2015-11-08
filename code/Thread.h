#include "Platfrom.h"

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


class Thread
{
public:
	Thread();
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
