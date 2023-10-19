#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
#include "userprog/process.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

void check_valid_ptr(void *ptr);
void syscall_init(void);
bool sys_create(char* file, unsigned initial_size);
bool sys_remove(char* file);
int sys_open(char* name);
int sys_filesize(int fd);
int sys_read(int fd, void* buffer, unsigned size);
int sys_write(int fd, void* buffer, unsigned size);
void sys_seek(int fd, unsigned position);
unsigned sys_tell(int fd);
void sys_close(int fd);
int sys_practice(int i);
void sys_halt(void);
void sys_exit(int status);
pid_t sys_exec(char* cmd_line);
int sys_wait(int pid);

#endif /* userprog/syscall.h */