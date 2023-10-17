#include <linux/fs.h>

struct vtfs_superblock {
    struct super_block *sb; // Pointer to the VFS superblock
    
    unsigned long magic;      // Magic number to identify our FS
    unsigned long block_size; // Block size
    
    .allocate_inode = allocate_inode,
    .delete_inode = delete_inode,
}

// static struct super_operations vtfs_super_ops = {
//     .statfs = vtfs_statfs,              // function to get file system statistics
//     .drop_inode = generic_delete_inode, // can use the generic version
//     // ... other operations
// };

/*
 * myfs_alloc_inode
 * Allocate a new inode for myfs filesystem.
 */
static struct inode * allocate_inode(struct super_block *sb)
{
    struct vtfs_inode *mi;

    mi = kzalloc(sizeof(*mi));
    if (!mi)
    {
        /* Memory allocation failed */
        return NULL;
    }

    inode_init_once(&mi->vfs_inode);


    return &mi->vfs_inode;
}
