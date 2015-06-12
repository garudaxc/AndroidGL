#include "Thread.h"
#include "MyLog.h"

Mutex::Mutex()	
{
	pthread_mutex_init(&lock_, NULL);
}

Mutex::~Mutex()	
{
	pthread_mutex_destroy(&lock_);
}

void Mutex::Lock()
{
	pthread_mutex_lock(&lock_);
}

void Mutex::Unlock()
{
	pthread_mutex_unlock(&lock_);
}

bool Mutex::TryLock()
{
	int r = pthread_mutex_trylock(&lock_);
	return (r == 0);
}




//////////////////////////////////////////////////////////////////////////


static void* ThreadFunc_s(void* arg)
{
	Thread* thread = (Thread*)arg;

	void* r = thread->Run();

	return r;
}


Thread::Thread()
{
}

Thread::~Thread()
{
}


bool Thread::Create()
{
	int err = pthread_create(&pid_, NULL, ThreadFunc_s, this);
	if (err != 0){
		GLog.LogError("Create thread failed!");
		return false;
	}
	
	return true;
}