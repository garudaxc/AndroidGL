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





//////////////////////////////////////////////////////////////////////////


struct ThreadImpl
{
	pthread_t	pid;
};


static void* ThreadFunc_s(void* arg)
{
	Thread* thread = (Thread*)arg;

	void* r = thread->Run();

	return r;
}


Thread::Thread()
{
	impl_ = new ThreadImpl;
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

}

void Thread::Resume()
{

}

void Thread::Stop()
{

}

bool Thread::Check()
{
	return true;
}

bool Thread::Create()
{
	int err = pthread_create(&impl_->pid, NULL, ThreadFunc_s, this);
	if (err != 0){
		GLog.LogError("Create thread failed!");
		return false;
	}
	
	return true;
}