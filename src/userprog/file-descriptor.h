#ifndef FILE_DESCRIPTOR_H
#define FILE_DESCRIPTOR_H

#include "threads/thread.h"
#include <stdint.h>

#include "userprog/process.h"
#include <list.h>
#include "filesys/file.h"



// Todo: test / review the functions in this file

// FDs
struct fd_table { // each process gets an fd_table --> need to add to process.h and initialize in start_process
  //struct list* fds; 
  struct list fds;
  int next_unused_fd; // maybe should just go to next unused one instead of reusing given ones
    // starts at 2
    // PID already within struct process therefore not necessary to keep here
};

struct fd {
    struct list_elem list_fd;
    int val; // FD value
    struct file* file; 
};

// static or not static?
struct fd* find(struct fd_table *table, int fd);
int remove(struct fd_table *table, int fd); // -1 on failure, 0 on success
struct fd* add(struct fd_table *table, struct file* file);
void init_table(struct fd_table* table);
struct file* get_file_pointer(struct fd_table* fd_table, int fd);

// void free_table(struct fd_table *fd_table);



// initialize fd_table
// struct fd_table table;
// struct list fds;
// list_init(&fds);
// table->fds = &fds;
// fd_table->next_unused_fd = 2;
// need to add #include "userprog/file-descriptor.h.h"

// struct list my_list;
// list_init (&my_list);

#endif