#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>

#include "vtfs.h"

struct dentry *vtfs_mount(struct file_system_type *fs_type,
                          int flags,
                          const char *dev_name,
                          void *data);
void vtfs_kill_sb(struct super_block *sb);

static struct file_system_type vtfs_type = {
    .owner = THIS_MODULE,
    .name = "vtfs",
    .mount = vtfs_mount,
    // TODO
    // .init_fs_context = vtfs_init_fs_context,
    .kill_sb = vtfs_kill_sb,
    .fs_flags = FS_REQUIRES_DEV,
};

struct dentry *vtfs_mount(struct file_system_type *fs_type,
                          int flags,
                          const char *dev_name,
                          void *data) {
    struct dentry *dentry =
        mount_bdev(fs_type, flags, dev_name, data, vtfs_sb_init);
    if (IS_ERR(dentry))
        printk("'%s' mount failure\n", dev_name);
    else
        printk("'%s' mount success\n", dev_name);

    return dentry;
}

void vtfs_kill_sb(struct super_block *sb) {
    // 1. Release file system resources
    // Free any in-memory structures specific to your file system

    // 2. Write back any pending changes (if your file system supports writing)

    // 3. Close the backing device (if applicable)
    // If your FS uses a block device, close it here

    // 4. Call file system-agnostic cleanup
    kill_block_super(sb); // Or kill_litter_super(sb), depending on your needs

    // 5. Additional cleanup
    // Any other cleanup tasks specific to your file system
    printk("vtfs: super block destroyed\n");
}

static int __init vtfs_module_init(void)
{
    printk(KERN_INFO "Hello, world!\n");

    return 0;
}

static void __exit vtfs_module_exit(void)
{
    printk(KERN_INFO "Goodbye, world!\n");
}


module_init(vtfs_module_init);
module_exit(vtfs_module_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tverous");
MODULE_DESCRIPTION("A simple example Linux module.");