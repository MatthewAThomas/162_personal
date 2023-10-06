#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "userprog/process.h"
#include "file-descriptor.h"
#include "filesys/file.h"

#include "lib/kernel/console.c" // putbuf not declared in console.h; should we change console.h ?

static void syscall_handler(struct intr_frame*);

void syscall_init(void) { intr_register_int(0x30, 3, INTR_ON, syscall_handler, "syscall"); }

static void syscall_handler(struct intr_frame* f UNUSED) {
  uint32_t* args = ((uint32_t*)f->esp);

  /*
   * The following print statement, if uncommented, will print out the syscall
   * number whenever a process enters a system call. You might find it useful
   * when debugging. It will cause tests to fail, however, so you should not
   * include it in your final submission.
   */

  /* printf("System call number: %d\n", args[0]); */

  if (args[0] == SYS_EXIT) {
    f->eax = args[1];
    printf("%s: exit(%d)\n", thread_current()->pcb->process_name, args[1]);
    process_exit();
  }
  // Start of File Syscall
  else if (args[0] == SYS_CREATE) {
    // printf("System call number: %d\n", args[0]);
  }
  else if (args[0] == SYS_REMOVE) {
      // printf("System call number: %d\n", args[0]);
  }
  else if (args[0] == SYS_OPEN) {
      // printf("System call number: %d\n", args[0]);
  }
  else if (args[0] == SYS_FILESIZE) {
      // printf("System call number: %d\n", args[0]);
  }
  else if (args[0] == SYS_READ) {
      // printf("System call number: %d\n", args[0]);
  }
  else if (args[0] == SYS_WRITE) {
      // printf("System call number: %d\n", args[0]);
      // how the hell do I get int fd, const void *buffer, unsigned size using args?
      // args[1] : fd
      // args[2] : buffer
      // args[2] : size unsigned

      // check if a file is open
        // return error
      // write to a file using write_file in "filesys/file.h"
      // return the number of bytes actually written (which is returned from write_file)

      int fd = args[1];
      const void *buffer = args[2];
      unsigned size = args[3];

      // get file and fd_table. You can find it in process.h and file.h
      struct file *file = get_file_pointer(fd);
      struct fd_table *fd_table = thread_current()->pcb->fd_table;

      // check if file is open (maybe function in file-descriptor.c) return -1 if not
      if (find(fd_table, fd) == NULL) {
        f->eax = -1;
        // need to exit kernel
        return;
      }

      if (file -> deny_write) {
        f->eax = -1;
        // need to exit kernel
        return;
      }

      if(fd == 1) { // stdout case
        putbuf((void char*) buffer, (size_t) size);  
      } else {
        int bytes_written = write_file(file, buffer, (off_t) size);
        f -> eax = bytes_written;
      }
  }

  // refer to file.c instead from filesys/// write from buffer to corresponding file pointed by file descriptor
// synchronization is not assumed in this code thus it has to be wrapped by mutex or semaphore
// int write_from_buffer(int fd, const *buffer, unsigned size) {
//     // initialize buffer start
//     // initialize buffer end
//     // initialize read_buffer_count
//     int read_count;
//     if(fd == 1) {

//     } else {
//         read_count = file_write(fd, buffer, size);
//     }
//     // while read_buffer_count < size:
//     //    write_to_file() <- there's gotta be a helper function that exists already : Im looking for an existing helper function rn
//     //    update start, update end
//     //    update read_buffer_count
//     //    check you can still write ex) any error
    
//     // return read_buffer_count
//     return read_count;
// //}
  else if (args[0] == SYS_SEEK) {
      // printf("System call number: %d\n", args[0]);
  }
  else if (args[0] == SYS_TELL) {
      // printf("System call number: %d\n", args[0]);
  }
  else if (args[0] == SYS_CLOSE) {
      // printf("System call number: %d\n", args[0]);
  }

}
