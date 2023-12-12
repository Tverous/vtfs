#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/buffer_head.h>

#include "vtfs.h"

static int vtfs_iterate(struct file *dir, struct dir_context *ctx)
{
    struct inode *inode = file_inode(dir);
    // TODO
    // inode info block
    struct super_block *sb = inode->i_sb;
    struct buffer_head *bh = NULL;
    struct vtfs_dir *vtfs_dir = NULL;
    // struct vtfs_dir_entry *vtfs_dir_entry = NULL;
    struct vtfs_file *vtfs_file = NULL;

    
}

const struct file_operations vtfs_dir_ops = {
    .owner = THIS_MODULE,
    // .read = generic_read_dir,
    .iterate_shared = vtfs_iterate,
};

