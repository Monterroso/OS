#include "filesys/inode.h"
#include <list.h>
#include <debug.h>
#include <round.h>
#include <string.h>
#include "filesys/filesys.h"
#include "filesys/free-map.h"
#include "threads/malloc.h"

/* Identifies an inode. */
#define INODE_MAGIC 0x494e4f44

#define DIRECT_CNT 123
#define INDIRECT_CNT 1
#define DBL_INDIRECT_CNT 1
#define SECTOR_CNT (DIRECT_CNT + INDIRECT_CNT + DBL_INDIRECT_CNT)

#define POINTS_PER_SEC 128

#define MAX_BLOCKS (DIRECT_CNT + (INDIRECT_CNT * POINTS_PER_SEC) + (DBL_INDIRECT_CNT * POINTS_PER_SEC * POINTS_PER_SEC))



/* On-disk inode.
   Must be exactly BLOCK_SECTOR_SIZE bytes long. */
  //  struct inode_disk
  // {
  //   block_sector_t start;               /* First data sector. */
  //   off_t length;                       /* File size in bytes. */
  //   unsigned magic;                     /* Magic number. */
  //   uint32_t unused[125];               /* Not used. */
  // };

 struct inode_disk
   {
     block_sector_t sectors[SECTOR_CNT]; /* Sectors. */
     enum inode_type type;               /* FILE_INODE or DIR_INODE. */
     off_t length;                       /* File size in bytes. */
     unsigned magic;                     /* Magic number. */
   };

/* Returns the number of sectors to allocate for an inode SIZE
   bytes long. */
static inline size_t
bytes_to_sectors (off_t size)
{
  return DIV_ROUND_UP (size, BLOCK_SECTOR_SIZE);
}

/* In-memory inode. */
/* In-memory inode. */
// struct inode
//   {
//     struct list_elem elem;              /* Element in inode list. */
//     block_sector_t sector;              /* Sector number of disk location. */
//     int open_cnt;                       /* Number of openers. */
//     bool removed;                       /* True if deleted, false otherwise. */
//     int deny_write_cnt;                 /* 0: writes ok, >0: deny writes. */
//     struct inode_disk data;             /* Inode content. */
//   };

struct inode
  {
    struct list_elem elem;              /* Element in inode list. */
    block_sector_t sector;              /* Sector number of disk location. */
    int open_cnt;                       /* Number of openers. */
    bool removed;                       /* True if deleted, false otherwise. */
    struct lock lock;                   /* Protects the inode. */

    struct lock deny_write_lock;        /* Protects members below. */
    struct condition no_writers_cond;   /* Signaled when no writers. */
    int deny_write_cnt;                 /* 0: writes ok, >0: deny writes. */
    int writer_cnt;                     /* Number of writers. */

    struct inode_disk data;             /* Inode content. */
  };

struct inode_disk*
get_disk(const struct inode *inode)


/* Returns the block device sector that contains byte offset POS
   within INODE.
   Returns -1 if INODE does not contain data for a byte at offset
   POS. */

static block_sector_t
byte_to_sector (const struct inode *inode, off_t pos)
{
  ASSERT (inode != NULL);
  if (pos < inode->data.length)
    return inode->data.start + pos / BLOCK_SECTOR_SIZE;
  else
    return -1;
}

/* List of open inodes, so that opening a single inode twice
   returns the same `struct inode'. */
static struct list open_inodes;

/* Initializes the inode module. */
void
inode_init (void)
{
  list_init (&open_inodes);
}

/* Initializes an inode with LENGTH bytes of data and
   writes the new inode to sector SECTOR on the file system
   device.
   Returns true if successful.
   Returns false if memory or disk allocation fails. */
bool
inode_create (block_sector_t sector, off_t length)
{
  struct inode_disk *disk_inode = NULL;
  bool success = false;

  ASSERT (length >= 0);

  /* If this assertion fails, the inode structure is not exactly
     one sector in size, and you should fix that. */
  ASSERT (sizeof *disk_inode == BLOCK_SECTOR_SIZE);

  disk_inode = calloc (1, sizeof *disk_inode);


  //and an array to hold all of our bits.


  if (disk_inode != NULL)
    {
      //size_t sectors = bytes_to_sectors (length);

      //we have the number of sectors equal to this.
      size_t sectors = get_extra_pointers(length)
      + DIV_ROUND_UP(length, BLOCK_SIZE);

      disk_inode->length = length;
      disk_inode->magic = INODE_MAGIC;

      //instead of free map allocate, we will use bit_get_sectors
      //to see if we are able to access the number of
      //if (free_map_allocate (sectors, &disk_inode->start))

      //so we need to get the number of pointers
      block_sector_t * sector_locs = malloc(sectors * sizeof(block_sector_t));

      if( get_sectors(blocks_needed, sector_locs) == NULL) {
        {
          block_write (fs_device, sector, disk_inode);
          if (sectors > 0)
            {
              static char zeros[BLOCK_SECTOR_SIZE];
              size_t i;

              for (i = 0; i < sectors; i++)
                //change to write to these sectors
                //block_write (fs_device, disk_inode.start + i, zeros);

                block_write (fs_device, sector_locs[i], zeros);
            }
          success = true;

        }
      free (disk_inode);
    }
  free (sector_locs);
  return success;
}

/* Reads an inode from SECTOR
   and returns a `struct inode' that contains it.
   Returns a null pointer if memory allocation fails. */
struct inode *
inode_open (block_sector_t sector)
{
  struct list_elem *e;
  struct inode *inode;

  /* Check whether this inode is already open. */
  for (e = list_begin (&open_inodes); e != list_end (&open_inodes);
       e = list_next (e))
    {
      inode = list_entry (e, struct inode, elem);
      if (inode->sector == sector)
        {
          inode_reopen (inode);
          return inode;
        }
    }

  /* Allocate memory. */
  inode = malloc (sizeof *inode);
  if (inode == NULL)
    return NULL;

  /* Initialize. */
  list_push_front (&open_inodes, &inode->elem);
  inode->sector = sector;
  inode->open_cnt = 1;
  inode->deny_write_cnt = 0;
  inode->removed = false;
  block_read (fs_device, inode->sector, &inode->data);
  return inode;
}

/* Reopens and returns INODE. */
struct inode *
inode_reopen (struct inode *inode)
{
  if (inode != NULL)
    inode->open_cnt++;
  return inode;
}

/* Returns INODE's inode number. */
block_sector_t
inode_get_inumber (const struct inode *inode)
{
  return inode->sector;
}

/* Closes INODE and writes it to disk.
   If this was the last reference to INODE, frees its memory.
   If INODE was also a removed inode, frees its blocks. */
void
inode_close (struct inode *inode)
{
  /* Ignore null pointer. */
  if (inode == NULL)
    return;

  /* Release resources if this was the last opener. */
  if (--inode->open_cnt == 0)
    {
      /* Remove from inode list and release lock. */
      list_remove (&inode->elem);

      /* Deallocate blocks if removed. */
      if (inode->removed)
        {
          //we need to iterate through all of our nodes and release them.

          free_map_release (inode->sector, 1);


          // free_map_release (inode->data.start,
          //                   bytes_to_sectors (inode->data.length));

          //now we go, and free all of our blocks. 
        }

      free (inode);
    }
}

/* Marks INODE to be deleted when it is closed by the last caller who
   has it open. */
void
inode_remove (struct inode *inode)
{
  ASSERT (inode != NULL);
  inode->removed = true;
}

/* Reads SIZE bytes from INODE into BUFFER, starting at position OFFSET.
   Returns the number of bytes actually read, which may be less
   than SIZE if an error occurs or end of file is reached. */
off_t
inode_read_at (struct inode *inode, void *buffer_, off_t size, off_t offset)
{
  uint8_t *buffer = buffer_;
  off_t bytes_read = 0;
  uint8_t *bounce = NULL;

  while (size > 0)
    {
      /* Disk sector to read, starting byte offset within sector. */
      block_sector_t sector_idx = byte_to_sector (inode, offset);
      int sector_ofs = offset % BLOCK_SECTOR_SIZE;

      /* Bytes left in inode, bytes left in sector, lesser of the two. */
      off_t inode_left = inode_length (inode) - offset;
      int sector_left = BLOCK_SECTOR_SIZE - sector_ofs;
      int min_left = inode_left < sector_left ? inode_left : sector_left;

      /* Number of bytes to actually copy out of this sector. */
      int chunk_size = size < min_left ? size : min_left;
      if (chunk_size <= 0)
        break;

      if (sector_ofs == 0 && chunk_size == BLOCK_SECTOR_SIZE)
        {
          /* Read full sector directly into caller's buffer. */
          block_read (fs_device, sector_idx, buffer + bytes_read);
        }
      else
        {
          /* Read sector into bounce buffer, then partially copy
             into caller's buffer. */
          if (bounce == NULL)
            {
              bounce = malloc (BLOCK_SECTOR_SIZE);
              if (bounce == NULL)
                break;
            }
          block_read (fs_device, sector_idx, bounce);
          memcpy (buffer + bytes_read, bounce + sector_ofs, chunk_size);
        }

      /* Advance. */
      size -= chunk_size;
      offset += chunk_size;
      bytes_read += chunk_size;
    }
  free (bounce);

  return bytes_read;
}

/* Writes SIZE bytes from BUFFER into INODE, starting at OFFSET.
   Returns the number of bytes actually written, which may be
   less than SIZE if end of file is reached or an error occurs.
   (Normally a write at end of file would extend the inode, but
   growth is not yet implemented.) */
off_t
inode_write_at (struct inode *inode, const void *buffer_, off_t size,
                off_t offset)
{
  const uint8_t *buffer = buffer_;
  off_t bytes_written = 0;
  uint8_t *bounce = NULL;

  if (inode->deny_write_cnt)
    return 0;

  while (size > 0)
    {
      /* Sector to write, starting byte offset within sector. */
      block_sector_t sector_idx = byte_to_sector (inode, offset);
      int sector_ofs = offset % BLOCK_SECTOR_SIZE;

      /* Bytes left in inode, bytes left in sector, lesser of the two. */
      off_t inode_left = inode_length (inode) - offset;
      int sector_left = BLOCK_SECTOR_SIZE - sector_ofs;
      int min_left = inode_left < sector_left ? inode_left : sector_left;

      /* Number of bytes to actually write into this sector. */
      int chunk_size = size < min_left ? size : min_left;
      if (chunk_size <= 0)
        break;

      if (sector_ofs == 0 && chunk_size == BLOCK_SECTOR_SIZE)
        {
          /* Write full sector directly to disk. */
          block_write (fs_device, sector_idx, buffer + bytes_written);
        }
      else
        {
          /* We need a bounce buffer. */
          if (bounce == NULL)
            {
              bounce = malloc (BLOCK_SECTOR_SIZE);
              if (bounce == NULL)
                break;
            }

          /* If the sector contains data before or after the chunk
             we're writing, then we need to read in the sector
             first.  Otherwise we start with a sector of all zeros. */
          if (sector_ofs > 0 || chunk_size < sector_left)
            block_read (fs_device, sector_idx, bounce);
          else
            memset (bounce, 0, BLOCK_SECTOR_SIZE);
          memcpy (bounce + sector_ofs, buffer + bytes_written, chunk_size);
          block_write (fs_device, sector_idx, bounce);
        }

      /* Advance. */
      size -= chunk_size;
      offset += chunk_size;
      bytes_written += chunk_size;
    }
  free (bounce);

  return bytes_written;
}

/* Disables writes to INODE.
   May be called at most once per inode opener. */
void
inode_deny_write (struct inode *inode)
{
  inode->deny_write_cnt++;
  ASSERT (inode->deny_write_cnt <= inode->open_cnt);
}

/* Re-enables writes to INODE.
   Must be called once by each inode opener who has called
   inode_deny_write() on the inode, before closing the inode. */
void
inode_allow_write (struct inode *inode)
{
  ASSERT (inode->deny_write_cnt > 0);
  ASSERT (inode->deny_write_cnt <= inode->open_cnt);
  inode->deny_write_cnt--;
}

/* Returns the length, in bytes, of INODE's data. */
off_t
inode_length (const struct inode *inode)
{
  return inode->data.length;
}


/*Given a length, we calculate how many extra pointers are used*/
int
get_extra_pointers(off_t pos) {

  pos -= DIRECT_CNT;

  off_t extra_pointers = 0;

  int i = 0;

  //we check if the direct pointers can cover it
  if (pos > 0 ) {
    //this checks for pointers to pointers that we will need to use

    for(i = 0; i < INDIRECT_CNT; i++) {

      //each indirect pointer requires one pointer
      extra_pointers++;

      //we decreate the amount that we need
      pos -= POINTS_PER_SEC;

      //we want to check if we don't have enough for a full indirect block
      if (pos <= 0) {
        break;
      }
    }
  }

  if (pos > 0) {
    int j = 0;

    for(i = 0; i < DBL_INDIRECT_CNT; i++) {

      //each indirect pointer requires one pointer
      extra_pointers++;

      for(j = 0; j < POINTS_PER_SEC; j++) {

        extra_pointers++;
        //we decreate the amount that we need
        pos -= POINTS_PER_SEC;

        //we want to check if we don't have enough for a full indirect block
        if (pos <= 0) {
          break;
        }
      }
    }
  }

  return extra_pointers;
}

/* Calculates the number of blocks that we need in order to extend the
inode in question by pos amount. It also includes the number of blocks
to serve as holders for our pointers. It returns -1 if we cannot possibly
have enough space*/
int
num_sectors_needed(const struct inode *inode, off_t pos) {

  //DIV_ROUND_UP should take care of the rounding here




  cur_block_num = DIV_ROUND_UP (inode->data.length, BLOCK_SECTOR_SIZE);

  need_block_num = DIV_ROUND_UP(pos, BLOCK_SECTOR_SIZE);

  //we first check if we can possibly have enough space
  if (need_block_num > MAX_BLOCKS) {
    return -1;
  }

  int extra_needed = get_extra_pointers(pos) - get_extra_pointers(inode.length);

  if (extra_needed < 0) {
    extra_needed = 0;
  }


  //now we have accounted for all of our extra pointers.

  if (cur_block_num >= need_block_num) {
    return 0;
  }
  else {
    return need_block_num - cur_block_num + extra_needed;
  }
}


/*We are given an inode, and we find the sector which corresponds to the
pos that we are given. Serves as a replacement for byte_to_sector*/
static block_sector_t
byte_to_sector_new (const struct inode *inode, off_t pos) {

  //the new off is how many sectors we will use
  int new_off = DIV_ROUND_UP((pos, BLOCK_SIZE);

  //if the file is smaller, we just want to return null
  if (inode->data.length <= new_off) {
    return null;
  }

  //if it's a direct pointer, we return the direct pointer
  if (new_off < DIRECT_CNT) {
    return inode->sectors[new_off];
  }


  new_off -= DIRECT_CNT;

  else if (new_off < INDIRECT_CNT * POINTS_PER_SEC) {
    int indir_counter = DIV_ROUND_UP(new_off, INDIRECT_CNT * POINTS_PER_SEC) - 1;

    int indir_elem = new_off % POINTS_PER_SEC;

    return inode->block.data[DIRECT_CNT + indir_counter][indir_elem];
  }



  new_off -= INDIRECT_CNT * POINTS_PER_SEC;

  //now however, we know it's in a double pointeer
  ASSERT(new_off < DBL_INDIRECT_CNT * POINTS_PER_SEC * POINTS_PER_SEC);

  int doub_counter = DIV_ROUND_UP(new_off, POINTS_PER_SEC * POINTS_PER_SEC) - 1;

  int double_elem = new_off % POINTS_PER_SEC * POINTS_PER_SEC;

  int sing_elem = double_elem % POINTS_PER_SEC;

  return blocks[directpointers + indirectpointers + doubcounter][double_elem][sing_elem]

}


/*Given an inode, a number of sectors, and an array containing sectors
which are reserved, we add those sectors to this inode's data. */
void
count_alloc(const struct inode *inode, off_t num_sects, block_sector_t[] sector_locs) {

  num_blocks_using =  DIV_ROUND_UP(inode->data.length, BLOCK_SIZE);

  //number we have placed
  num_placed = 0;

  //this keeps track of the pointers we've iterated through
  //so far
  num_iterated = 0;

  int i= 0;
  int j = 0;
  int k = 0;

  for (i = 0; i < SECTOR_CNT; i++) {

    //we check if direct pointer, indirect, or doubleindirect
    if (i < DIRECT_CNT) {
      iterated_through++;

      //We check if we've moved onto a section that isn't already taken
      if (iterated_through > num_blocks_using)

        //since this is a direct pointer, we just add to the direct pointer
        block[i] = sector_loc[num_placed];

        num_placed++;

        if (num_placed == num_sects) {
          return;
        }
    }

    else if (i < DIRECT_CNT + INDIRECT_CNT * POINTS_PER_SEC){
      for(j = 0; j < POINTS_PER_SEC; j++) {

        iterated_through++;

        if (iterated_through > num_blocks_using);

          //since this is an indirect pointer, we add to the pointer there
          inode->data.block[i][j] = sector_loc[num_placed];

          //and we want to show we used one additional one
          num_placed++;

          if (num_placed == num_sects) {
            return;
          }
        }
    }
    //otherwise, we know we're in a double pointer

    else {
      for(j = 0; j < POINTS_PER_SEC; j++) {

        for(k = 0; k < POINTS_PER_SEC; k++) {

          iterated_through++;

          if (iterated_through > used_so_far) {

            //since this is an doubleindirect pointer, we add to the pointer there
            inode->data.block[i][j][k] = sector_loc[num_placed];

            //and we want to show we used one additional one
            num_placed++;

            if (num_placed == num_sects){
              return;
            }
          }
        }
      }
    }
  }
}


block_sector_t*
get_data_block(const struct inode *inode, off_t pos, bool extend) {


  int sectors_need = num_sectors_needed(inode, pos);

  //If the number of sectors we need is larger than the ones we have
  //and we can't extend, or it exceeds the file size, we return NULL
  if ((sectors_need > DIV_ROUND_UP(inode->data.length, BLOCK_SIZE)
  && extend == false) || sectors_need == -1) {
    return NULL;
  }

  //now we allocate the blocks

  int[] sector_locs = calloc(1, sectors_need);

  block_sector_t* sectors = get_sectors(sectors_need, int[] sector_locs);

  //if it fails, we return
  if (sectors == NULL) {
    return NULL;
  }

  //now, we just allocate our sectors
  count_alloc(inode, sectors_need, sector_locs);
}
