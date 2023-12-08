#ifndef VTFS_H
#define VTFS_H

/* source: https://en.wikipedia.org/wiki/Hexspeak */
#define VTFS_MAGIC 0xCAFEBABE

// The design follows the the Fast File System (FFS) from Berkeley
// partition layout
/*
    *-----------------------*
    | super block           |
    *-----------------------*
    | inode bitmap          |
    *-----------------------*
    | data blocks bitmap    |
    *-----------------------*
    | inode table           |
    *-----------------------*
    | data blocks           |
    *-----------------------*
*/

#define VTFS_BLOCK_SIZE 1024
#define VTFS_MAX_FILE_NAME 255
#define VTFS_MAX_FILE_SIZE 1024 * 1024 * 1024 // 1GB
#define VTFS_INODES_PER_BLOCK (VTFS_BLOCK_SIZE / sizeof(struct vtfs_inode))

struct vtfs_sb_info {
    uint32_t magic;                     // magic number
    
    uint32_t num_blocks;                // total number of blocks (include superblock and inodes)
    uint32_t num_inodes;                // total number of inodes

    uint32_t num_free_blocks;           // number of free blocks
    uint32_t num_free_inodes;           // number of free inodes

    uint32_t num_inode_store_block;     // number of blocks for inode store
    uint32_t num_block_bitmap_block;    // number of blocks for block bitmap
    uint32_t num_inode_bitmap_block;    // number of blocks for inode bitmap

    unsigned long *block_bitmap;        // block bitmap
    unsigned long *inode_bitmap;        // inode bitmap
};

struct vtfs_inode {
    // TODO
    uint32_t i_ino;                 // inode number

    uint32_t i_mode;                // file mode
    uint32_t i_blocks;              // number of blocks
    uint32_t i_size;                // file size
    uint32_t i_atime;               // last access time
    uint32_t i_mtime;               // last modify time
    uint32_t i_ctime;               // inode last change time
    uint32_t i_nlink;               // number of hard links
    // TODO
    uint32_t i_block[15];           // block pointers
    // char i_data[VTFS_BLOCK_SIZE];   // data
};

// super block functions
int vtfs_sb_init(struct super_block *sb, void *data, int slient);

// inode functions
struct inode *vtfs_get_inode(struct super_block *sb, unsigned long ino);

// file functions



#endif /* VTFS_H */