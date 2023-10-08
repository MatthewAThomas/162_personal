#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include "file-descriptor.h" // added starting here
#include "userprog/process.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

void syscall_init(void);

// Start of file operations syscalls
bool sys_create(const char* file, unsigned initial_size);
bool sys_remove(const char* file);
int sys_open(const char* file);
int sys_filesize(int fd);
int sys_read(int fd, void* buffer, unsigned size);
int sys_write(int fd, const void* buffer, unsigned size);
void sys_seek(int fd, unsigned position);
unsigned sys_tell(int fd);
void sys_close(int fd);

// Start of process syscalls
int sys_practice(int i);
void sys_halt(void);
void sys_exit(int status);
// pid_t sys_exec(const char* cmd_line);
// int sys_wait(pid_t pid);
int sys_exec(const char* cmd_line);
int sys_wait(int pid);
#endif /* userprog/syscall.h */
