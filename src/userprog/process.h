#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"
#include <stdint.h>
#include "userprog/file-descriptor.h" // added 

// At most 8MB can be allocated to the stack
// These defines will be used in Project 2: Multithreading
#define MAX_STACK_PAGES (1 << 11)
#define MAX_THREADS 127

/* PIDs and TIDs are the same type. PID should be
   the TID of the main thread of the process */
typedef tid_t pid_t;

/* Thread functions (Project 2: Multithreading) */
typedef void (*pthread_fun)(void*);
typedef void (*stub_fun)(pthread_fun, void*);

/* The process control block for a given process. Since
   there can be multiple threads per process, we need a separate
   PCB from the TCB. All TCBs in a process will have a pointer
   to the PCB, and the PCB will have a pointer to the main thread
   of the process, which is `special`. */
struct process {
  /* Owned by process.c. */
  uint32_t* pagedir;          /* Page directory. */
  char process_name[16];      /* Name of the main thread */
  struct thread* main_thread; /* Pointer to main thread */

  struct fd_table* fd_table; /* Pointer to the FD table. */

  struct list children; /* Simon added lst */
  struct semaphore list_sema;  
  struct shared_data* shared_data;
};

// parent process list - visibilty to sharea data's of its own child
// as a process itself - will have a pointer to its own shared data

 // shared data will be access by its own process and parent process using children list
struct shared_data {
  bool load; /* Indicate child process is successfully loaded*/
  //struct semaphore load_sema; /* Signal loading is completed whether it succeed or failed*/
  struct semaphore wait_sema;
  pid_t pid; /* my pid */
  struct list_elem elem; /* make it iterable*/
  int ref_count; /* set it free only when it is 0 i.e. no lost child!*/
  int exit_code; /*meta data to hold exit status even after process/thread is gone*/
};

/* project 1 process helper*/
void init_shared_data(struct shared_data* shared_data);
/* end of helper*/

/* Find the shared data struct of a (child) process */
//struct shared_data* find_shared_data(struct list children, int pid);
// struct shared_data* find_shared_data (struct process *, int);
void add_child(int child_pid);


void userprog_init(void);

pid_t process_execute(const char* file_name);
int process_wait(pid_t);
void process_exit(void);
void process_activate(void);

bool is_main_thread(struct thread*, struct process*);
pid_t get_pid(struct process*);

tid_t pthread_execute(stub_fun, pthread_fun, void*);
tid_t pthread_join(tid_t);
void pthread_exit(void);
void pthread_exit_main(void);
struct process *find_process(int pid);


#endif /* userprog/process.h */
