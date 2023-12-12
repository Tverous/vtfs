#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/buffer_head.h>

#include "vtfs.h"

static const struct inode_operations vtfs_dir_inode_ops = {
    .lookup = simple_lookup,
    .link = simple_link,
    .unlink = simple_unlink,
    // .symlink = simple_symlink,
    // .mkdir = simple_mkdir,
    .rmdir = simple_rmdir,
    // .mknod = simple_mknod,
    .rename = simple_rename,
};

static const struct inode_operations vtfs_file_inode_ops = {
    .setattr = simple_setattr,
    .getattr = simple_getattr,
};

struct inode *vtfs_get_inode(struct super_block *sb, unsigned long ino)
{
    struct buffer_head *bh = NULL;
    struct vtfs_inode *vtfs_inode = NULL;
    struct inode *inode = NULL;

    struct vtfs_sb_info *sbi = sb->s_fs_info;

    uint32_t inode_block = (ino / VTFS_INODES_PER_BLOCK) + 1;
    uint32_t inode_offset = ino % VTFS_INODES_PER_BLOCK;

    // fail if inode is out of range
    if (ino >= sbi->num_inodes) {
        printk(KERN_ERR "vtfs: inode out of range\n");
        return NULL;
    }

    /* Get a locked inode from Linux */
    inode = iget_locked(sb, ino);
    if (!inode) {
        printk(KERN_ERR "vtfs: unable to get inode\n");
        return NULL;
    }


    bh = sb_bread(sb, inode_block);
    if (!bh) {
        printk(KERN_ERR "vtfs: unable to read inode block\n");
        return NULL;
    }   
    vtfs_inode = (struct vtfs_inode *)bh->b_data;
    vtfs_inode += inode_offset;

    // printk("vtfs: inode uid: %d\n", inode->i_uid);
    // printk("vtfs: inode gid: %d\n", inode->i_gid);
    printk("vtfs: inode mode: %d\n", vtfs_inode->i_mode);
    printk("vtfs: inode size: %lld\n", vtfs_inode->i_size);
    printk("vtfs: inode blocks: %lld\n", vtfs_inode->i_blocks);
    printk("vtfs: inode nlink: %d\n", vtfs_inode->i_nlink);

    /* Fill inode with data from disk */
    inode->i_ino = ino;
    inode->i_sb = sb;
    // TODO
    // inode->i_op = &vtfs_inode_ops;

    

    inode->i_mode = vtfs_inode->i_mode;
    i_uid_write(inode, vtfs_inode->i_uid);
    i_gid_write(inode, vtfs_inode->i_gid);
    inode->i_size = vtfs_inode->i_size;
    inode->i_blocks = vtfs_inode->i_blocks;
    
    // TODO: inode_set_ctime()...
    inode->i_atime.tv_sec = vtfs_inode->i_atime;
    inode->i_mtime.tv_sec = vtfs_inode->i_mtime;
    inode->i_ctime.tv_sec = vtfs_inode->i_ctime;
    inode->i_atime.tv_nsec = inode->i_mtime.tv_nsec = inode->i_ctime.tv_nsec = 0;

    set_nlink(inode, vtfs_inode->i_nlink);

    
    if (S_ISDIR(inode->i_mode)) {
        inode->i_op = &vtfs_dir_inode_ops;
        inode->i_fop = &vtfs_dir_ops;
    }
    else if (S_ISREG(inode->i_mode)) {
        inode->i_op = &vtfs_file_inode_ops;
        inode->i_fop = &vtfs_file_ops;
        // inode->i_mapping->a_ops = &vtfs_aops;
    }
    else
        printk(KERN_ERR "vtfs: unknown inode type\n");


    brelse(bh);
    unlock_new_inode(inode);

    return inode;
}