#include <pthread.h>


class Mutex
{
public:
	Mutex();
	~Mutex();

	void	Lock();
	void	Unlock();
	bool	TryLock();

private:
	pthread_mutex_t		lock_;
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

	bool	Create();

	virtual void*		Run() = 0;

private:

	pthread_t	pid_;
};
