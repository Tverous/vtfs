#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/buffer_head.h>

#include "vtfs.h"

struct inode *vtfs_get_inode(struct super_block *sb, unsigned long ino)
{
    struct buffer_head *bh = NULL;
    struct vtfs_inode *vtfs_inode = NULL;
    struct inode *inode = NULL;

    uint32_t inode_block = ino / VTFS_INODES_PER_BLOCK + 1;
    uint32_t inode_offset = ino % VTFS_INODES_PER_BLOCK;


    // Read inode from disk
    bh = sb_bread(sb, inode_block);
    if (!bh) {
        printk(KERN_ERR "vtfs: unable to read inode block\n");
        ret = -EIO;
        goto failed;
    }

    /* Get a locked inode from Linux */
    inode = iget_locked(sb, ino);
    if (!inode) {
        printk(KERN_ERR "vtfs: unable to get inode\n");
        return ERR_PTR(-ENOMEM);
    }

    /* Fill inode with data from disk */
    vtfs_inode = (struct vtfs_inode *)bh->b_data + inode_offset;
    inode->i_ino = ino;
    inode->i_sb = sb;
    // TODO
    // inode->i_op = &vtfs_inode_ops;

    inode->i_mode = vtfs_inode->i_mode;
    // inode->i_uid = vtfs_inode->i_uid;
    // inode->i_gid = vtfs_inode->i_gid;
    inode->i_size = vtfs_inode->i_size;
    inode->i_blocks = vtfs_inode->i_blocks;
    
    // TODO: inode_set_ctime()...
    inode->i_atime.tv_sec = vtfs_inode->i_atime;
    inode->i_mtime.tv_sec = vtfs_inode->i_mtime;
    inode->i_ctime.tv_sec = vtfs_inode->i_ctime;
    inode->i_atime.tv_nsec = inode->i_mtime.tv_nsec = inode->i_ctime.tv_nsec = 0;

    set_nlink(inode, vtfs_inode->i_nlink);

    // TODO: define inode operations
    printk("inode i_mode: %d\n", inode->i_mode);
    if (S_ISDIR(inode->i_mode))
        inode->i_fop = &vtfs_dir_ops;
    else if (S_ISREG(inode->i_mode))
        inode->i_fop = &vtfs_file_ops;
    else
        printk(KERN_ERR "vtfs: unknown inode type\n");


    brelse(bh);
    unlock_new_inode(inode);

    return inode;

failed:
    brelse(bh);
    iget_failed(inode);
    return ERR_PTR(ret);
}