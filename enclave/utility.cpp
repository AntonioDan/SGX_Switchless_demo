#include <stdio.h>
#include "string.h"
#include "sgx_error.h"
#include "enclave_t.h"

#include "utility.h"

#define BUFSIZE 256
/*
 * printf:
 *  Invokes OCALL to display the enclave buffer to the terminal.
 */
int printf(const char * fmt, ...)
{
    char buf[BUFSIZE] = {'\0'};
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, BUFSIZE, fmt, ap);
    va_end(ap);

    ocall_print_string(buf);

    return (int)strnlen(buf, BUFSIZE - 1) + 1;
}

int pthread_mutex_lock(void * mutex)
{
	int retval;
	sgx_status_t ret;

	ret = ocall_pthread_mutex_lock(&retval, mutex);
	if (ret != SGX_SUCCESS) {	
		return -1;
	}		

	return retval;
}

int pthread_mutex_unlock(void * mutex)
{
	int retval;
	sgx_status_t ret;

	ret = ocall_pthread_mutex_unlock(&retval, mutex);
	if (ret != SGX_SUCCESS) {		
		return -1;
	}
		

	return retval;
}

int pthread_cond_wait(void * cond, void * mutex)
{
	int retval;
	sgx_status_t ret;

	ret = ocall_pthread_cond_wait(&retval, cond, mutex);
	if (ret != SGX_SUCCESS) {		
		return -1;
	}		

	return retval;
}

int pthread_cond_signal(void *cond)
{
	int retval;
	sgx_status_t ret;

	ret = ocall_pthread_cond_signal(&retval, cond);
	if (ret != SGX_SUCCESS) {		
		return -1;
	}		

	return retval;
}