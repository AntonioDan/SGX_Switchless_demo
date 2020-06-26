#include <stdio.h>
#include "sgx_taskqueue.h"

#ifdef TRUSTED
#include "utility.h"
#else
#include "pthread.h"
#endif

#ifdef TRUSTED
#define pthread_cond_t void
#define pthread_mutex_t void
#endif

Taskqueue::~Taskqueue()
{
}

int Taskqueue::insert_task(Taskqueue::task_t * p_task)
{
	if (((tail + 1) % MAX_TASK_NUM) == head)
		return -1;

	tque[tail] = p_task;

	tail = (tail + 1) % MAX_TASK_NUM;

	return 0;
}

Taskqueue::task_t * Taskqueue::get_task()
{	
	Taskqueue::task_t * tmptask;

	if (head == tail)
		return NULL;

	tmptask = tque[head];

	head = (head + 1) % MAX_TASK_NUM;

	return tmptask;	
}

SwitchlessTaskQue::SwitchlessTaskQue(void * p_mutex, void *p_cond):m_p_tqueMutex(p_mutex), m_p_tqueCond(p_cond), m_shutdown(0)
{
}

SwitchlessTaskQue::~SwitchlessTaskQue()
{
}

int SwitchlessTaskQue::insert_task(Taskqueue::task_t * p_task)
{
	int ret;

	ret = m_queue.insert_task(p_task);
	if (ret != 0) {		
		printf("Error: failed to insert task.\n");
		
		return ret;
	}		

	pthread_cond_signal((pthread_cond_t *)m_p_tqueCond);
	
	return 0;
}

int SwitchlessTaskQue::waitfornewtask()
{
	pthread_mutex_lock((pthread_mutex_t *)m_p_tqueMutex);

	if (!isEmpty()) {	
		pthread_mutex_unlock((pthread_mutex_t *)m_p_tqueMutex);	
		return 0;
	}

	pthread_cond_wait((pthread_cond_t *)m_p_tqueCond, (pthread_mutex_t *)m_p_tqueMutex);

	pthread_mutex_unlock((pthread_mutex_t *)m_p_tqueMutex);

	return 0;
}

Taskqueue::task_t * SwitchlessTaskQue::get_task()
{
	return m_queue.get_task();
}

void SwitchlessTaskQue::shutdown()
{
	m_shutdown = 1;	

	pthread_cond_signal((pthread_cond_t *)m_p_tqueCond);
}
