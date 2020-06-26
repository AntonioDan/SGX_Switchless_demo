#ifndef _UTILITY_H_
#define _UTILITY_H_

#ifdef __cplusplus
extern "C" {
#endif

int printf(const char * fmt, ...);

int pthread_mutex_lock(void * mutex);
int pthread_mutex_unlock(void * mutex);
int pthread_cond_wait(void * cond, void * mutex);
int pthread_cond_signal(void *cond);

#ifdef __cplusplus
}
#endif

#endif
