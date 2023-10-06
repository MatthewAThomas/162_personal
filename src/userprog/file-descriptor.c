#include "userprog/process.h"
#include <debug.h>
#include <inttypes.h>
#include <round.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <list.h>
#include "userprog/file-descriptor.h"
#include "filesys/file.h"

// Todo: test / review the functions in this file

/* 
    Looks for the FD list_elem with the FD number N. 
    Returns NULL if there is no list_elem with the FD number N. 
    Works by iterating from beginning to end to find
*/
struct fd* find(struct fd_table* fd_table, int fd) {
    //struct fd* file_desc;
    struct list_elem* e;
    for (e = list_begin(fd_table->fds); e != list_end(fd_table->fds); e = list_next(e)) {
        struct fd* file_desc = list_entry(e, struct fd, list_fd);
        if (file_desc != NULL && file_desc->val == fd) {
            return &file_desc;
        }
    } 
    return NULL;
}

    //   struct list_elem *e;

    //   for (e = list_begin (&foo_list); e != list_end (&foo_list);
    //        e = list_next (e))
    //     {
    //       struct foo *f = list_entry (e, struct foo, elem);
    //       ...do something with f...
    //     }


/*
    Removes the given FD from the FD table and returns the associated FD. Returns NULL if it does not exist. 
*/
struct fd* pop(struct fd_table* fd_table, int fd) {
    struct fd* file_desc = find(fd_table, fd);
    struct list_elem* e = &(file_desc->list_fd);
    list_remove(e); // returns a value, but we don't want to keep the list_elem
    return file_desc;
}

/*
    Removes the given FD from the FD table. Returns -1 if it does not exist. 
*/
int remove(struct fd_table* fd_table, int fd) {
    // -1 on failure, 0 on success
    struct fd* file_desc = find(fd_table, fd);
    if (list_remove(&(file_desc->list_fd)) == NULL) {
        return -1;
    }
    return 0;    
}

/*
    Adds the given FD to the FD table. Returns the FD.
*/
struct fd* add(struct fd_table* fd_table, struct file* file) {
    struct fd* file_descriptor; 
    file_descriptor->val = fd_table->next_unused_fd;
    fd_table->next_unused_fd += 1;
    file_descriptor->file = file;
    struct list_elem e;
    file_descriptor->list_fd = e;
    list_push_back(fd_table->fds, &e);
    return file_descriptor;
}

/* 
    Returns the file pointer associated with the given FD.
*/
struct file* get_file_pointer(struct fd_table *fd_table, int fd) {
    return find(fd_table, fd)->file;
}


/*
    Initializes FD table.
*/    
void init_table(struct fd_table *fd_table) {
    // struct list fds;
    // list_init(&fds);
    // fd_table->fds = &fds;
    // fd_table->next_unused_fd = 2;

    struct list *fds = malloc(sizeof(struct list));
    list_init(fds);
    fd_table->fds = fds;
    fd_table->next_unused_fd = 2;
}


