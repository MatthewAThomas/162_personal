#include "threads/thread.h"
#include <stdint.h>

#include "userprog/process.h"
#include <list.h>
#include "filesys/file.h"

// Todo: test / review the functions in this file

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
static fd* find(struct* fd_table, int fd);
static fd* pop(struct* fd_table, int fd);
static int remove(struct* fd_table, int fd); // -1 on failure, 0 on success
static fd* add(struct* fd_table, file* file);
static void init_table(fd_table* table);
static file* get_file_pointer(struct* fd_table, int fd);

// initialize fd_table
// struct fd_table table;
// struct list fds;
// list_init(&fds);
// table->fds = &fds;
// fd_table->next_unused_fd = 2;
// need to add #include "userprog/file-descriptor.h.h"

// struct list my_list;
// list_init (&my_list);