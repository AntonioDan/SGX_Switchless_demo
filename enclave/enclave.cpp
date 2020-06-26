#include "stdio.h"
#include "sgx_error.h"
#include "sgx_taskqueue.h"
#include "utility.h"
#include "enclave_t.h"

SwitchlessTaskQue * m_taskqueue = NULL;

extern "C" sgx_status_t switchless_ecall();
extern "C" sgx_status_t switchless_init(void * taskque);

void process_task(Taskqueue::task_t * p_task)
{
	if (!p_task)
		return;
	
	switch (p_task->type)
	{
		case Taskqueue::TASK_CALC_ADD:
		{
			twopara_t * p_para = (twopara_t *)(p_task->para);

			p_para->result = p_para->para1 + p_para->para2;
		}
		break;

		case Taskqueue::TASK_CALC_MUL:
		{
			twopara_t * p_para = (twopara_t *)p_task->para;

			p_para->result = p_para->para1 * p_para->para2;
		}
		break;

	default:
		break;
	}
}

sgx_status_t switchless_init(void * taskque)
{
	if (!taskque)
		return SGX_ERROR_INVALID_PARAMETER;

	m_taskqueue = (SwitchlessTaskQue *)taskque;

	return SGX_SUCCESS;
}

sgx_status_t switchless_ecall()
{
	int retval;
	int retry = 0;
	Taskqueue::task_t *p_task = NULL;
	sgx_status_t ret = SGX_SUCCESS;

	if (!m_taskqueue)
		return SGX_ERROR_UNEXPECTED;

	while (1)
	{
		pthread_mutex_lock(m_taskqueue->m_p_tqueMutex);		

		if (m_taskqueue->needexit())
		{
			while (!m_taskqueue->isEmpty())
			{
				p_task = m_taskqueue->get_task();

				process_task(p_task);				
			}

			pthread_mutex_unlock(m_taskqueue->m_p_tqueMutex);
			break;
		}		

		if (!m_taskqueue->isEmpty())
		{			
			p_task = m_taskqueue->get_task();
			pthread_mutex_unlock(m_taskqueue->m_p_tqueMutex);
			retry = 0;

			process_task(p_task);			
		} else {
			pthread_mutex_unlock(m_taskqueue->m_p_tqueMutex);

			if (retry++ < 3)
				continue;

			// do ocall to wait for the conditional variable 						
			retval = m_taskqueue->waitfornewtask();

			if (retval != 0) {
				ret = SGX_ERROR_UNEXPECTED;
				break;
			}
			
			retry = 0;
		}
		
	}

	return ret;
}
