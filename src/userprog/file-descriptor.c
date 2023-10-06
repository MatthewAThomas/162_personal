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

/* Looks for the FD list_elem with the FD number N. Returns NULL if there is no list_elem with the FD number N. */
static list_elem* find(struct* fd_table, int fd) {
    struct list_elem* e;
    for (e = list_begin(fd_table); e != list_end(fd_table) && e.val != fd; e = list_next(fd_table)); // this is a single statement btw
    if (e.val != fd) {
        return NULL;
    }
    return e;
}

static list_elem* pop(struct* fd_table, int fd) {

}

static int remove(struct* fd_table, int fd) {
    // -1 on failure, 0 on success
}

static int add(struct* fd_table, file* file) {
    struct fd file_descriptor; 
    
}

static fd_table* init_table(); {

}