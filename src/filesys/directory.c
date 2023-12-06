#include "filesys/directory.h"
#include <stdio.h>
#include <string.h>
#include <list.h>
#include "filesys/filesys.h"
#include "filesys/inode.h"
#include "threads/malloc.h"
#include "threads/thread.h"


/* A directory. */
struct dir {
  struct inode* inode; /* Backing store. */
  off_t pos;           /* Current position. */

  struct dir* parent; /* Parent directory. */
  // struct list children; /* Refers to directory children (not directory entries). */
  // struct list_elem elem; /* Used to add to list of parent directory's children. */
  // TODO: init children in directory creation, add children to parent's list in making dir
  // resolve lookup with lookup() and check if the file is a directory or file
};

/* A single directory entry. Refers to a file that can be either a directory or a normal file. */
struct dir_entry {
  block_sector_t inode_sector; /* Sector number of header. */
  char name[NAME_MAX + 1];     /* Null terminated file name. */
  bool in_use;                 /* In use or free? */
};

static bool lookup(const struct dir* dir, const char* name, struct dir_entry* ep, off_t* ofsp);
// struct dir* find_dir_child(struct fd_table* fd_table, int fd) {
//     for (struct list_elem* e = list_begin(&fd_table->fds); e != list_end(&fd_table->fds); e = list_next(e)) {
//         struct fd* file_desc = list_entry(e, struct fd, list_fd);
//         if (file_desc != NULL && file_desc->val == fd) {
//             return file_desc;
//         }
//     } 
//     return NULL;
// }


/* Need to call dir_close on directory after returning. */
struct dir* get_dir_from_entry(struct dir_entry* entry) {
  // assumes in_use
  // should find a more efficient way to do this
  struct inode* inode = inode_open(entry->inode_sector);
  // if (is_directory(inode)) {
  if (1) {
    struct dir* dir = dir_open(inode);
    inode_close(inode); 
    return dir;
  }
  inode_close(inode);
  return NULL;
}

/* Returns NULL if something fails.*/
struct dir* get_dir_from_path(char* path) {
  return get_dir_from_entry(get_dir_entry_from_path(path));
}

struct dir_entry* get_dir_entry_from_path(char* path) {
  struct dir_entry* curr;
  off_t* offset;
  if (lookup(thread_current()->cwd, path, curr, offset)) {
    return curr;
  }
  return NULL;
}

bool is_path(char* path) {
  if (strchr(path, '/') != NULL) return true;
  return false;
}

/* Separates the directory in PATH from the file or last directory. 
Puts the name of the final directory or file in DEST. 
Removes the directory from path. */
bool separate_parent_and_child(char** path, char** dest) {
  // basically cut out at the last / filename
  int path_len = strlen(*path);
  char* last = strrchr(*path, '/'); 
  int last_len = strlen(last);
  // finds the last entry (the new directory name);
  // todo: check if test directories end in / or not
  char prev_dir[path_len - last_len + 1];
  strlcpy(prev_dir, *path, path_len - last_len); // need room for \'0'
  prev_dir[path_len - last_len] = '\0'; // strncpy does not null terminate
  *dest = (last + 1); // strrchr gets the index of the '/', so we need to skip the first character
  *path = prev_dir;
}



/* Creates a directory with space for ENTRY_CNT entries in the
   given SECTOR.  Returns true if successful, false on failure. */
bool dir_create(block_sector_t sector, size_t entry_cnt) {
  struct inode* inode;
  bool status = inode_create(sector, entry_cnt * sizeof(struct dir_entry));
  // add flag here to inode
  // if (status) {
  //   inode = inode_open(sector);
  //   set_dir_status(inode, true);
  //   inode_close(inode);
  //   return status;
  // }
  return status;
}

/* Opens and returns the directory for the given INODE, of which
   it takes ownership.  Returns a null pointer on failure. */
struct dir* dir_open(struct inode* inode) {
  struct dir* dir = calloc(1, sizeof *dir);
  if (inode != NULL && dir != NULL) {
    dir->inode = inode;
    dir->pos = 0;
    return dir;
  } else {
    inode_close(inode);
    free(dir);
    return NULL;
  }
}

/* Opens the root directory and returns a directory for it.
   Return true if successful, false on failure. */
struct dir* dir_open_root(void) {
  return dir_open(inode_open(ROOT_DIR_SECTOR));
}

/* Opens and returns a new directory for the same inode as DIR.
   Returns a null pointer on failure. */
struct dir* dir_reopen(struct dir* dir) {
  return dir_open(inode_reopen(dir->inode));
}

/* Destroys DIR and frees associated resources. */
void dir_close(struct dir* dir) {
  if (dir != NULL) {
    inode_close(dir->inode);
    free(dir);
  }
  // TODO: empty the lists and everything
}

/* Returns the inode encapsulated by DIR. */
struct inode* dir_get_inode(struct dir* dir) {
  return dir->inode;
}

/* Searches DIR for a file with the given NAME.
   If successful, returns true, sets *EP to the directory entry
   if EP is non-null, and sets *OFSP to the byte offset of the
   directory entry if OFSP is non-null.
   otherwise, returns false and ignores EP and OFSP. */
static bool lookup(const struct dir* dir, const char* name, struct dir_entry* ep, off_t* ofsp) {
  struct dir_entry e;
  size_t ofs;

  ASSERT(dir != NULL);
  ASSERT(name != NULL);
  if (strchr(name, '/') != NULL) { // when given a path instead of the file name only
    int len = strlen(name);
    struct dir* curr;
    if (name[0] == '/') {
      curr = dir_open_root();
      dir_close(curr);
    }
    else {
      curr = dir;
      // thread_current()->pcb->cwd;
    }

    char tokenizer_input[len + 1];
    strlcpy(tokenizer_input, name, len + 1); // len + 1 for null character?
    char* rest = NULL; // or = name
    char* token;
    off_t* new_ofsp;
    for (token = strtok_r(tokenizer_input, "/", &rest); token != NULL; token = strtok_r(NULL, "/", &rest)) {
      if (strcmp(token, "..") == 0) {
        curr = curr->parent;
      }
      else if (strcmp(token, ".") == 0 || strcmp(token, "") == 0) {
        continue; 
        // no change, CWD
        // OR
        // when first character is / or last character is /
        // therefore, do nothing
      }
      else {
        // not a special character
        // check current directory for the given directory
        new_ofsp = 0; // reset with each new entry to be checked
        struct dir_entry* entry;
        // off_t curr_ofs;
        if (lookup(curr, token, entry, new_ofsp)) { // check current directory for given entry
          return false;
        }
        
        e = *entry;
        if (!e.in_use) return false; // indicates that directory was deleted 
        // new_ofsp = &curr_ofs;
        // dir_entry e being next directory or current path
        curr = get_dir_from_entry(entry); // TODO: update curr to next dir based on directory entry ENTRY
        // TODO: properly add value for
        // make in case last else not called
        // how to set ofs?
      }
    }
    if (ep != NULL) // triggers when done going through rest of path
      *ep = e;
    if (ofsp != NULL)
      *ofsp = new_ofsp;
    return true;
  } 
  else { // when only checking current dir for a filename (not path)
    for (ofs = 0; inode_read_at(dir->inode, &e, sizeof e, ofs) == sizeof e; ofs += sizeof e) {
      if (e.in_use && !strcmp(name, e.name)) {
        if (ep != NULL)
          *ep = e;
        if (ofsp != NULL)
          *ofsp = ofs;
        return true;
      }
    }
  }

  return false;
}

// original lookup
/* 
static bool lookup(const struct dir* dir, const char* name, struct dir_entry* ep, off_t* ofsp) {
  struct dir_entry e;
  size_t ofs;

  ASSERT(dir != NULL);
  ASSERT(name != NULL);

  for (ofs = 0; inode_read_at(dir->inode, &e, sizeof e, ofs) == sizeof e; ofs += sizeof e)
    if (e.in_use && !strcmp(name, e.name)) {
      if (ep != NULL)
        *ep = e;
      if (ofsp != NULL)
        *ofsp = ofs;
      return true;
    }
  return false;
}

*/

/* Searches DIR for a file with the given NAME
   and returns true if one exists, false otherwise.
   On success, sets *INODE to an inode for the file, otherwise to
   a null pointer.  The caller must close *INODE. */
bool dir_lookup(const struct dir* dir, const char* name, struct inode** inode) {
  struct dir_entry e;

  ASSERT(dir != NULL);
  ASSERT(name != NULL);

  if (lookup(dir, name, &e, NULL))
    *inode = inode_open(e.inode_sector);
  else
    *inode = NULL;

  return *inode != NULL;
}

/* Adds a file named NAME to DIR, which must not already contain a
   file by that name.  The file's inode is in sector
   INODE_SECTOR.
   Returns true if successful, false on failure.
   Fails if NAME is invalid (i.e. too long) or a disk or memory
   error occurs. */
bool dir_add(struct dir* dir, const char* name, block_sector_t inode_sector) {
  struct dir_entry e;
  off_t ofs;
  bool success = false;

  ASSERT(dir != NULL);
  ASSERT(name != NULL);

  /* Check NAME for validity. */
  if (*name == '\0' || strlen(name) > NAME_MAX)
    return false;

  /* Check that NAME is not in use. */
  struct dir* curr = dir;
  // if (strnchr(name, '/') != NULL) { // name is a path 
    
  //   struct dir* curr = get_dir_from_path(name); // would need to remove name of file
  //   if (curr == NULL) {
  //     goto done;
  //   } 
  // }

  if (lookup(dir, name, NULL, NULL))
    goto done;

  /* Set OFS to offset of free slot.
     If there are no free slots, then it will be set to the
     current end-of-file.

     inode_read_at() will only return a short read at end of file.
     Otherwise, we'd need to verify that we didn't get a short
     read due to something intermittent such as low memory. */
  for (ofs = 0; inode_read_at(dir->inode, &e, sizeof e, ofs) == sizeof e; ofs += sizeof e)
    if (!e.in_use)
      break;

  /* Write slot. */
  e.in_use = true;
  strlcpy(e.name, name, sizeof e.name);
  e.inode_sector = inode_sector;
  success = inode_write_at(dir->inode, &e, sizeof e, ofs) == sizeof e;

done:
  return success;
}

/* Removes any entry for NAME in DIR.
   Returns true if successful, false on failure,
   which occurs only if there is no file with the given NAME. */
bool dir_remove(struct dir* dir, const char* name) {
  struct dir_entry e;
  struct inode* inode = NULL;
  bool success = false;
  off_t ofs;

  ASSERT(dir != NULL);
  ASSERT(name != NULL);

  /* Find directory entry. */
  if (!lookup(dir, name, &e, &ofs))
    goto done;

  /* Open inode. */
  inode = inode_open(e.inode_sector);
  if (inode == NULL)
    goto done;

  /* Erase directory entry. */
  e.in_use = false;
  if (inode_write_at(dir->inode, &e, sizeof e, ofs) != sizeof e)
    goto done;

  /* Remove inode. */
  inode_remove(inode);
  success = true;

done:
  inode_close(inode);
  return success;
}

/* Reads the next directory entry in DIR and stores the name in
   NAME.  Returns true if successful, false if the directory
   contains no more entries. */
bool dir_readdir(struct dir* dir, char name[NAME_MAX + 1]) {
  struct dir_entry e;

  while (inode_read_at(dir->inode, &e, sizeof e, dir->pos) == sizeof e) {
    dir->pos += sizeof e;
    if (e.in_use) {
      strlcpy(name, e.name, NAME_MAX + 1);
      return true;
    }
  }
  return false;
}
