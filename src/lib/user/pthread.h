#ifndef __LIB_USER_PTHREAD_H
#define __LIB_USER_PTHREAD_H

#include <debug.h>
#include <stdbool.h>


struct pthread {
  struct list_elem pthread_elem; // for pthread_list
  uint8_t* user_stack;
  struct thread* kernel_thread; 
  struct semaphore user_sema; // used in joins
  // use exit code in pcb after joins or termination
  bool has_joined;  // pthread_join should fail if called on a thread that has already                     
                    // been joined on
}

void add_pthread(pthread* curr);
struct pthread* find_pthread(tid_t tid);

/* Thread identifiers and thread function */
typedef void (*pthread_fun)(void*);
typedef void (*stub_fun)(pthread_fun, void*);
typedef int tid_t;
#define TID_ERROR ((tid_t)-1)

tid_t pthread_create(pthread_fun fun, void* arg);
void pthread_exit(void) NO_RETURN;
bool pthread_join(tid_t);

#endif /* lib/user/pthread.h */
