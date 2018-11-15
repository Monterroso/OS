Design Document for Project 3: File Systems
============================================

## Group Members

* Noah Poole <_jellyfish_@berkeley.edu>
* Johnathan Chow <johnathan.chow18@berkeley.edu>
* William Ju <w99j99@berkeley.edu>
* Marcus Monterroso <mmonterroso@berkeley.edu>

# Task 1: Buffer Cache

## Data Structures and Functions
```
block.c
struct list * cached_sectors;
int cache_size = 0;

struct cache_entry {
  struct list_elem list_elem;
  struct block * b;
  int data[BLOCK_SECTOR_SIZE];
  struct lock * lock;
  int sector;
  int dirty;
  int tou;
}

block_write();

block_read();

```  
## Algorithms
Most of the code will be in `block.c` as it contains the most basic functions for reading and writing to disk.  We will create a new structure called a `cache_entry` which will contain a sector's cache data and metadata.  We need to add a dirty bit so we can know if a sector needs to be written back to disk when flushed from the cache.  As well, `tou` keeps track of a sector's time of use so we can implement a LRU replacement policy.  Anytime a sector is accessed, we will increment `tou` for all other cache entries.

In `block_write()` and `block_read()`, we will add the sector to `cached_sectors`, removing the LRU sector if we have a capacity miss.  If the replaced sector had the dirty bit checked, we call `block->ops->write` on the cached data.  We acquire the lock for that `cache_entry`.  If the sector is already in the list, we will set its `tou` to 0.  As well, we increment the `tou` of every other entry.  Now, we can read or write the data in the `cache_entry`.  If we write to the struct, we need to set the dirty bit.  Finally, we release the lock.

## Synchronization
We have included a lock for each `cache_entry`.  At the beginning of any read or write operation, we must acquire the lock of that sector first.  When we finish the operation, we may release the lock.  This prevents any concurrent accesses of the same sector but allows concurrency for different sectors.

## Rationale
### Advantages
- Simple list abstraction for cache
- LRU policy provides good hit and miss rates
- Lock for each sector allows for as much concurrency as possible

### Disadvantages
- Replacement policy does not account for temporal or spacial locality
- Iterating through cache to increment `tou` on every access may be slow

# Task 2: Extensible files

## Data Structures and Functions
inode.c
```
struct inode_disk
  block_sector_t direct[125];           
  block_sector_t indirect;             
  block_sector_t double_indirect;       
  unsigned magic;  //different number    
 
struct inode
  remove inode_disk data  
```
free-map.c
```
static struct lock *maplock;
bool free_map_allocate_extra (size_t cnt, block_sector_t *sectorp)
//same as free_map_allocate but stores each cnt instead of just first one
```
## Algorithms
We remove data from inode so it can always be accessed by the buffer cache.
The problem with the current implementation is that the system will search and only use a blog that is big enough to fit everything. This can lead to fragmentation. To solve this, we will write `free_map_allocate_extra()` which will solve that problem. It will see if there is enough blocks to put the data in and if it can insert the data, it will store the location of the data too for future reference. We will be changing the structure of inode_disk and inode's initializers so when the program needs to allocate more data in inode_write_at(), it's done with the new modified data structure. 

## Synchronization
We add a lock to free-map to guarantee atomicity of the allocation operations. We dont want things partially allocated because that would lead to alot of problems.

## Rationale
The `free_map_allocate_extra()` was designed to prevent fragmentation and was the most direct solution. A lock on free-map itself also solves consistency problems if multiple things want this resource. The removal of `inode_disk data` was mostly done so it's more accessible, as in not just within that inode. The pointers put into `inode_disk` are there to make the data accesses faster.

### Advantages
- Will not have to sort data each time there is something inserted or removed.

### Disadvantages
- We have to search through all nodes when inserting or removing.

# Task 3: Subdirectories

## Data Structures and Functions
```
inode.c
struct disk_inode {
  boolean isdir; //true if diretory, false if file
  int numfiles; //empty dir if 0. 
}

syscall.c
syscall_handler (struct intr_frame *f UNUSED)

file.c
fileinumber(struct file) 
fileisdir(struct file)

thread.c
struct thread{
	char* currdirectory; //keep track which dir it is in
}

```  
## Algorithms
We will need to modify `syscall_handler` to implement the new syscalls by calling their proper helper functions. 'readdir' will call 'dir_readdir()' from directory.c. 'inumber' will return the sector number of the inode and 'isdir' will return the isdir value from disk_inode.

For mkdir, we will do what we do with creating a file, but instead we set isdir to false instead. For chdir, we will keep track of a thread's current directory in the thread struct. We will change this value when we call chdir. 

In addition, many open, remove, and create in filesys have to be modified to work not only on just the root directory. 

## Synchronization
We will not be able to remove a directory if there are any files or other directories in it. We do this by keeping track of the number of elements in the directory (numFiles) and only remove when this value is 0. 

## Rationale
Another way to check whether a directory is empty or not is by using a boolean, but ultimately, it is easier to check an empty directory by incrementing and decrementing numFiles when creating or removing a file/directory. 

### Advantages
- Consistent always using absolute path

### Disadvantages
- Have to trace through file path

# Additional Questions
To implement a write behind cache, you can write dirty blocks to disk when the disk isn't busy. When another process needs to go to disk, we have to finish writing the dirty block from the cache. In order to reduce the chances of this, we can restrict ourselves to writing to disk every 1 minute. To implement read ahead cache, you can create a list of the most used files that aren't already in a cache, and then fetch the most used data blocks ahead of time. Everytime another file is put into the cache, it will add to the count of the list of most used files.
