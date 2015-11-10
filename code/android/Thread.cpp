#include "Thread.h"
#include "MyLog.h"
#include <pthread.h>



struct MutexImpl
{
	pthread_mutex_t lock;
};


Mutex::Mutex()	
{
	impl_ = new MutexImpl();
	pthread_mutex_init(&impl_->lock, NULL);
}

Mutex::~Mutex()	
{
	pthread_mutex_destroy(&impl_->lock);
	delete impl_;
}

void Mutex::Lock()
{
	pthread_mutex_lock(&impl_->lock);
}

void Mutex::Unlock()
{
	pthread_mutex_unlock(&impl_->lock);
}

bool Mutex::TryLock()
{
	int r = pthread_mutex_trylock(&impl_->lock);
	return (r == 0);
}


struct EventImpl
{
	pthread_mutex_t	mutex;
	pthread_cond_t	condition;
	bool			signaled;

	EventImpl()
	{
		pthread_mutex_init(&mutex, NULL);
		pthread_cond_init(&condition, NULL);
		signaled = true;
	}
};


Event::Event()
{
	impl_ = new struct EventImpl();
}

Event::~Event()
{
	delete impl_;
}

void Event::Reset()
{
	pthread_mutex_lock(&impl_->mutex);
	impl_->signaled = false;
	pthread_mutex_unlock(&impl_->mutex);
}

void Event::Signal()
{
	pthread_mutex_lock(&impl_->mutex);
	impl_->signaled = true;
	pthread_mutex_unlock(&impl_->mutex);
	pthread_cond_signal(&impl_->condition);
}

int Event::WaitOn(int timeout)
{
	if (timeout != 0) {
		GLog.LogError("to be implement : event wait for time");
		return (int)MY_WAIT_NONE;
	}

	pthread_mutex_lock(&impl_->mutex);
	while (!impl_->signaled) {
		pthread_cond_wait(&impl_->condition, &impl_->mutex);
	}
	pthread_mutex_unlock(&impl_->mutex);

	return (int)MY_WAIT_SIGNALED;
}

//////////////////////////////////////////////////////////////////////////


struct ThreadImpl
{
	string		name;
	pthread_t	pid;
	Event		event;
	bool	toStop;

	ThreadImpl(const string& _name) :name(_name), toStop(false)
	{		
	}
};


static void* ThreadFunc_s(void* arg)
{
	Thread* thread = (Thread*)arg;

	void* r = thread->Run();

	return r;
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
	return (uint32_t)pthread_self();
}

uint32_t Thread::Id() const
{
	return (uint32_t)impl_->pid;
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

	void* result = NULL;
	int err = pthread_join(impl_->pid, &result);
	if (err != 0) {
		GLog.LogError("stop thread error, code: %d", err);
	} else {
		GLog.LogInfo("thread %s stoped", impl_->name.c_str());
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
	int err = pthread_create(&impl_->pid, NULL, ThreadFunc_s, this);
	if (err != 0){
		GLog.LogError("Create thread %s failed!", impl_->name.c_str());
		return false;
	}
	
	return true;
}