#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/buffer_head.h>
#include <linux/slab.h>
#include <linux/statfs.h>

#include "vtfs.h"

static void vtfs_put_super(struct super_block *sb);
static int vtfs_sync_fs(struct super_block *sb, int wait);
static int vtfs_statfs(struct dentry *dentry, struct kstatfs *buf);

static struct super_operations const vtfs_super_ops = {
    // .alloc_inode = vtfs_alloc_inode,   // Allocate a new inode
    // .destroy_inode = vtfs_destroy_inode, // Destroy an inode
    // .write_inode = vtfs_write_inode,   // Write changes to an inode to disk
    // .drop_inode = vtfs_drop_inode,     // Drop an inode (optional)
    .put_super = vtfs_put_super,       // Clean up and free the superblock
    .sync_fs = vtfs_sync_fs,           // Sync the file system
    .statfs = vtfs_statfs,             // Get file system statistics
    // .remount_fs = vtfs_remount_fs,     // Remount file system with new mount options
};

static void vtfs_put_super(struct super_block *sb)
{
    struct vtfs_sb_info *sb_data = sb->s_fs_info;

    // TODO
    // Free any in-memory structures specific to your file system
    kfree(sb_data->inode_bitmap);
    kfree(sb_data->block_bitmap);
    kfree(sb_data);

    // Call file system-agnostic cleanup
    kill_block_super(sb); // Or kill_litter_super(sb), depending on your needs

    printk("vtfs: super block destroyed\n");
}

static int vtfs_sync_fs(struct super_block *sb, int wait)
{
    // TODO
    // Write back any pending changes (if your file system supports writing)
    return 0;
}   

static int vtfs_statfs(struct dentry *dentry, struct kstatfs *buf)
{
    struct super_block *sb = dentry->d_sb;
    struct vtfs_sb_info *sb_data = sb->s_fs_info;

    // TODO
    // Fill in the file system statistics
    buf->f_type = VTFS_MAGIC;
    buf->f_bsize = VTFS_BLOCK_SIZE;
    buf->f_blocks = sb_data->num_blocks;
    buf->f_bfree = sb_data->num_free_blocks;
    buf->f_bavail = sb_data->num_free_blocks;
    buf->f_files = sb_data->num_inodes;
    buf->f_ffree = sb_data->num_free_inodes;
    buf->f_namelen = VTFS_MAX_FILE_NAME;

    return 0;
}

int vtfs_sb_init(struct super_block *sb, void *data, int slient)
{
    struct buffer_head *bh;
    struct vtfs_sb_info *sb_data;
    struct inode *root_inode = NULL;
    int  ret = 0;

    // Init super block
    sb->s_magic = VTFS_MAGIC;
    sb_set_blocksize(sb, VTFS_BLOCK_SIZE);
    sb->s_maxbytes = VTFS_MAX_FILE_SIZE;
    sb->s_op = &vtfs_super_ops;

    // Read super block from disk
    bh = sb_bread(sb, 0);
    if (!bh) {
        return -EIO;
    }

    // Copy super block data from disk to memory
    struct vtfs_sb_info *vtfs_sb_data = (struct vtfs_sb_info *)bh->b_data;

    // check if the file system is valid
    printk("vtsb data->magic = %d\n", vtfs_sb_data->magic);
    if (vtfs_sb_data->magic != VTFS_MAGIC) {
        // printk(KERN_ERR "vtfs: invalid file system\n");
        printk("vtfs: invalid file system\n");
        ret = -EINVAL;
        goto release;
    }
    
    // Set super block data
    sb_data = kzalloc(sizeof(struct vtfs_sb_info), GFP_KERNEL);
    if (!sb_data) {
        printk(KERN_ERR "vtfs: unable to allocate memory for super block\n");
        ret = -ENOMEM;
        goto release;
    }

    // Set super block data
    sb_data->num_blocks = vtfs_sb_data->num_blocks;
    sb_data->num_inodes = vtfs_sb_data->num_inodes;
    sb_data->num_free_blocks = vtfs_sb_data->num_free_blocks;
    sb_data->num_free_inodes = vtfs_sb_data->num_free_inodes;
    sb_data->num_inode_store_block = vtfs_sb_data->num_inode_store_block;
    sb_data->num_block_bitmap_block = vtfs_sb_data->num_block_bitmap_block;
    sb_data->num_inode_bitmap_block = vtfs_sb_data->num_inode_bitmap_block;

    brelse(bh);

    // allocate and copy blocks pointing by block bitmap
    sb_data->inode_bitmap = kzalloc(sb_data->num_inode_bitmap_block * VTFS_BLOCK_SIZE, GFP_KERNEL);
    if (!sb_data->inode_bitmap) {
        // printk(KERN_ERR "vtfs: unable to allocate memory for inode bitmap\n");
        printk("vtfs: unable to allocate memory for inode bitmap\n");
        ret = -ENOMEM;
        goto free_sb_data;
    }
    for (int i = 0; i < sb_data->num_inode_bitmap_block; i++) {
        bh = sb_bread(sb, i + 1);
        if (!bh) {
            // printk(KERN_ERR "vtfs: unable to read inode bitmap block\n");
            printk("vtfs: unable to read inode bitmap block\n");
            ret = -EIO;
            goto free_ifree;
        }
        memcpy(sb_data->inode_bitmap + i * VTFS_BLOCK_SIZE, bh->b_data, VTFS_BLOCK_SIZE);

        brelse(bh);
    }
    

    // allocate and copy blocks pointing by inode bitmap
    sb_data->block_bitmap = kzalloc(sb_data->num_block_bitmap_block * VTFS_BLOCK_SIZE, GFP_KERNEL);
    if (!sb_data->block_bitmap) {
        // printk(KERN_ERR "vtfs: unable to allocate memory for block bitmap\n");
        printk("vtfs: unable to allocate memory for block bitmap\n");
        ret = -ENOMEM;
        goto free_ifree;
    }
    for (int i = 0; i < sb_data->num_block_bitmap_block; i++) {
        bh = sb_bread(sb, i + 1 + sb_data->num_inode_bitmap_block);
        if (!bh) {
            // printk(KERN_ERR "vtfs: unable to read block bitmap block\n");
            printk("vtfs: unable to read block bitmap block\n");
            ret = -EIO;
            goto free_bfree;
        }
        memcpy(sb_data->block_bitmap + i * VTFS_BLOCK_SIZE, bh->b_data, VTFS_BLOCK_SIZE);

        brelse(bh);
    }


    // TODO
    sb->s_fs_info = sb_data;

    root_inode = vtfs_get_inode(sb, 1);
    if (!root_inode) {
        ret = PTR_ERR(root_inode);
        goto free_bfree;
    }

    sb->s_root = d_make_root(root_inode);
    if (!sb->s_root) {
        // printk(KERN_ERR "vtfs: unable to get root inode\n");
        printk("vtfs: unable to get root inode\n");
        ret = -ENOMEM;
        goto iput;
    }

    return 0;

iput:
    iput(root_inode);
free_bfree:
    kfree(sb_data->block_bitmap);
free_ifree:
    kfree(sb_data->inode_bitmap);
free_sb_data:
    kfree(sb_data);
release:
    brelse(bh);

    return ret;
}