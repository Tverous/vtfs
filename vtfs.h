#ifndef VTFS_H
#define VTFS_H

// #include <linux/version.h>
#include <stdint.h>

struct vtfs_inode
{
    uint32_t i_mode;   /* File mode */
    uint32_t i_uid;    /* Owner id */
    uint32_t i_gid;    /* Group id */
    uint32_t i_size;   /* Size in bytes */
    uint32_t i_ctime;  /* Inode change time */
    uint32_t i_atime;  /* Access time */
    uint32_t i_mtime;  /* Modification time */
    uint32_t i_blocks; /* Block count */
    uint32_t i_nlink;  /* Hard links count */
    uint32_t ei_block; /* Block with list of extents for this file */
    char i_data[32];   /* store symlink content */
};

struct vtfs_superblock {
    struct super_block *sb;


    uint32_t magic; /* Magic number */
    
    // uint32_t nr_blocks; /* Total number of blocks (incl sb & inodes) */
    // uint32_t nr_inodes; /* Total number of inodes */

    // uint32_t nr_istore_blocks; /* Number of inode store blocks */
    // uint32_t nr_ifree_blocks;  /* Number of inode free bitmap blocks */
    // uint32_t nr_bfree_blocks;  /* Number of block free bitmap blocks */

    // uint32_t nr_free_inodes; /* Number of free inodes */
    // uint32_t nr_free_blocks; /* Number of free blocks */
}


// struct vtfs_super_operations {

// }




#endif /* VTFS_H */