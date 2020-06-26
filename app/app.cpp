#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "sgx_urts.h"
#include "sgx_taskqueue.h"

#include "enclave_u.h"

#define ENCLAVE_NAME "libtestenclave.signed.so"

typedef struct switchless_ecall_ctx {
    sgx_enclave_id_t eid;
    SwitchlessTaskQue * tque;
} switchless_ecall_ctx_t;

void* start_switchless_ecall(void * arg)
{
    sgx_status_t ret, retval;
    switchless_ecall_ctx_t * p_switchless_call_ctx = (switchless_ecall_ctx_t *)arg;    

    ret = enclave_switchless_init(p_switchless_call_ctx->eid, &retval, p_switchless_call_ctx->tque);
    if ((ret != SGX_SUCCESS) || (retval != SGX_SUCCESS)) {
        printf("fail to init switchelss context.\n");        
    }

    ret = enclave_switchless_ecall(p_switchless_call_ctx->eid, &retval);
    if ((ret != SGX_SUCCESS) || (retval != SGX_SUCCESS)) {
        printf("fail to do switchelss ecall.\n");              
    }
    
    return NULL;
}

int main(int argc, char * argv[])
{    
    pthread_t thread1, thread2;
	sgx_status_t ret;    
    sgx_enclave_id_t eid;
    switchless_ecall_ctx_t switchless_ctx;

    pthread_mutex_t mutex;
    pthread_cond_t cond;

    (void)argc;
    (void)argv;

	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);

    SwitchlessTaskQue * tqueue = new SwitchlessTaskQue(&mutex, &cond);
  
    ret = sgx_create_enclave(ENCLAVE_NAME, 1, NULL, NULL, &eid, NULL);
    if (ret != SGX_SUCCESS) {
        printf("sgx_create_enclave() return error code 0x%x.\n", ret);
    }
    printf("succeed to load enclave.\n");
    
    switchless_ctx.eid = eid;
    switchless_ctx.tque = tqueue;

    if (pthread_create(&thread1, NULL, start_switchless_ecall, &switchless_ctx) != 0) {
        printf("failed to start switchless ecall thread.\n");
        sgx_destroy_enclave(eid);
        exit(-1);        
    } 

    if (pthread_create(&thread2, NULL, start_switchless_ecall, &switchless_ctx) != 0) {
        printf("failed to start switchless ecall thread.\n");
        sgx_destroy_enclave(eid);
        exit(-1);        
    } 

    Taskqueue::task_t addtask;
    twopara_t *p_twopara = new twopara_t;
    p_twopara->para1 = 100;
    p_twopara->para2 = 101;
    addtask.type = Taskqueue::TASK_CALC_ADD;
    addtask.para = p_twopara;

    Taskqueue::task_t multask;
    twopara_t *p_twopara_mul = new twopara_t;
    p_twopara_mul->para1 = 100;
    p_twopara_mul->para2 = 101;
    multask.type = Taskqueue::TASK_CALC_MUL;
    multask.para = p_twopara_mul;
    
    Taskqueue::task_t addtask_1;
    twopara_t *p_twopara_add1 = new twopara_t;
    p_twopara_add1->para1 = 200;
    p_twopara_add1->para2 = 201;
    addtask_1.type = Taskqueue::TASK_CALC_ADD;
    addtask_1.para = p_twopara_add1;

    Taskqueue::task_t multask_1;
    twopara_t *p_twopara_mul_1 = new twopara_t;
    p_twopara_mul_1->para1 = 200;
    p_twopara_mul_1->para2 = 201;
    multask_1.type = Taskqueue::TASK_CALC_MUL;
    multask_1.para = p_twopara_mul_1;

    if ((tqueue->insert_task(&addtask) != 0)
        || (tqueue->insert_task(&multask) != 0)
        || (tqueue->insert_task(&addtask_1) != 0)
        || (tqueue->insert_task(&multask_1) != 0)) {
        printf("failed to add task to queue.\n");
    }       
       
    sleep(1);

    printf("Add task's result is %d\n", p_twopara->result);
    printf("Mul task's result is %d\n", p_twopara_mul->result);  
    printf("Add task_1's result is %d\n", p_twopara_add1->result);
    printf("Mul task_1's result is %d\n", p_twopara_mul_1->result);  

    /**
     *  here, it calls shutdown twice, because we need two pthread_cond_signal() to close two working threads, which uses pthread_cond_t as sync mechnanism
     * 
     **/ 
    tqueue->shutdown();  
    tqueue->shutdown();
    pthread_join(thread1, NULL);  
    pthread_join(thread2, NULL);

    delete p_twopara;
    delete p_twopara_mul;
    delete p_twopara_add1;
    delete p_twopara_mul_1;

	sgx_destroy_enclave(eid);
	return 0;
}
