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
struct list * cached_blocks;
int cache_size = 0;

struct cache_entry {
  struct list_elem list_elem;
  struct block * b;
  int sector;
  int dirty;
  int tou;
}

block_write();

block_read();

```  
## Algorithms
Most of the code will be in `block.c` as it contains the most basic functions for reading and writing to disk.  We need to modify the block structure to include a `cache_entry` so each block can be added to our list of cached blocks.  We will create a new structure called a `cache_entry` which will contain a block's cache data and metadata.  We need to add a dirty bit so we can know if a block needs to be written back to disk when flushed from the cache.  As well, `tou` keeps track of a block's time of use so we can implement a LRU replacement policy.

In `block_write()`, we will add the block to `cached_blocks`, removing the LRU block if we have a capacity miss.  

## Synchronization


## Rationale
### Advantages
- 

### Disadvantages
- 

# Task 2: Extensible files

## Data Structures and Functions
inode.c
```
struct inode_disk
  block_sector_t direct[125];           
  block_sector_t indirect;             
  block_sector_t doubly_indirect;       
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


## Synchronization


## Rationale
### Advantages
- 

### Disadvantages
- 

# Task 3: Subdirectories

## Data Structures and Functions
```

```  
## Algorithms


## Synchronization


## Rationale
### Advantages
- 

### Disadvantages
- 

# Additional Questions
