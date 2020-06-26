#ifndef _SGX_TASKQUEUE_H_
#define _SGX_TASKQUEUE_H_

#include <stdint.h>
#define MAX_TASK_NUM 100

typedef struct twopara {
	int para1;
	int para2;
	int result;
}twopara_t;

class Taskqueue 
{
	public:
		enum tasktype {
			TASK_CALC_ADD,
			TASK_CALC_MUL,
		};

		typedef struct task {
			uint8_t type;
			void * para;
		} task_t;

	public:
		Taskqueue() : tasknum(MAX_TASK_NUM), head(0), tail(0){}
		~Taskqueue();

	public:
		/**
		 * This function add task to the queue, it would wake up the condition variable
		 *
		 * @param p_task : this is the task to add
		 **/
		int insert_task(task_t * p_task);

		/**
		 * This function gets task from the queue, if the queue is empty, but we don't block here. If we aim to do switchless ecall, when the queue is empty, enclave can decide to retry before waiting on the condition variable
		 *
		 **/ 
		Taskqueue::task_t * get_task();

		bool isEmpty() {return (head == tail);}
		bool isFull() {return (((tail + 1) % MAX_TASK_NUM) == head);}

	private:
		int tasknum;
		int head, tail;
		task_t * tque[MAX_TASK_NUM];
		//void * p_tqueMutex; // this is pointer to pthread_mutex_t
		//void * p_tqueCond; // this is pointer to pthread_cond_t
};

/**
 * This class would be used by both untrusted and trusted part
 **/
struct SwitchlessTaskQue
{
	public:
		/**
		 * This function would initiatize mutex and conditional variable; it also initialize the task queue
		 **/ 
		SwitchlessTaskQue(void * p_mutex, void *p_cond);
		~SwitchlessTaskQue();

	public:
		/**
		 * This function add task to the queue and set the condition variable.
		 * The typical usage is untrusted part add task to the queue which is shared with enclave
		 **/ 
		int insert_task(Taskqueue::task_t *p_task);

		/**
		 * This function wait for new task to come, it supports enclave to wait for new task, it's called in OCALL routine
		 **/ 
		int waitfornewtask();

		/**
		 * This function gets task from the queue, if the queue is empty, it just return NULL.
		 * enclave would implement his own logic, either to wait on condition variable or retry
		 **/ 
		Taskqueue::task_t * get_task();

		void shutdown();
		bool needexit() {return (m_shutdown == 1);}
		bool isEmpty() {return m_queue.isEmpty();}

	public:
		void * m_p_tqueMutex; // this is pointer to pthread_mutex_t
		void * m_p_tqueCond; // this is pointer to pthread_cond_t
		
		Taskqueue m_queue;
		int m_shutdown;
};
#endif
