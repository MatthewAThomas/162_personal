#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"
#include <stdint.h>

#include "userprog/process.h"
#include <list.h>

// FDs
struct fd_table { // each process gets an fd_table --> need to add to process.h and initialize in start_process
  struct list* fds; 
  int next_unused_fd; // maybe should just go to next unused one instead of reusing given ones
    // starts at 2
    // PID already within struct process therefore not necessary to keep here
};

struct fd {
    struct list_elem* list_fd;
    int val; // FD value
    file* file; 
}

// static or not static?
static list_elem* find(struct* fd_table, int fd);
static list_elem* pop(struct* fd_table, int fd);
static int remove(struct* fd_table, int fd); // -1 on failure, 0 on success
static int add(struct* fd_table, file* file);
static fd_table* init_table();
