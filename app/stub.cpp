#include <pthread.h>
#include <stdio.h>

#include "sgx_taskqueue.h"

extern "C" void waitfornewtask(void * para);
extern "C" int ocall_pthread_mutex_lock(void * mutex);
extern "C" int ocall_pthread_mutex_unlock(void * mutex);
extern "C" int ocall_pthread_cond_wait(void * cond, void * mutex);
extern "C" int ocall_pthread_cond_signal(void *cond);
extern "C" void ocall_print_string(const char * str);

void ocall_print_string(const char * str)
{
    printf("%s", str);
}

void waitfornewtask(void * para)
{
	SwitchlessTaskQue * taskqueue = (SwitchlessTaskQue *)para;

	if (!taskqueue)
		return;

	taskqueue->waitfornewtask();	
}

int ocall_pthread_mutex_lock(void * mutex)
{
	return pthread_mutex_lock((pthread_mutex_t *)mutex);
}

int ocall_pthread_mutex_unlock(void * mutex)
{
	return pthread_mutex_unlock((pthread_mutex_t *)mutex);		
}

int ocall_pthread_cond_wait(void * cond, void * mutex)
{	
	return pthread_cond_wait((pthread_cond_t *)cond, (pthread_mutex_t *)mutex);
}

int ocall_pthread_cond_signal(void *cond)
{
	return pthread_cond_signal((pthread_cond_t *)cond);
}