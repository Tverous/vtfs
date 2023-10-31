#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include "vtfs.h"

static struct file_system_type vtfs_file_system_type = {
    .owner = THIS_MODULE,
    .name = "vtfs",
//    .mount = vtfs_mount,
//    .kill_sb = vtfs_kill_sb,
    .fs_flags = FS_REQUIRES_DEV,
    .next = NULL,
};

static int __init vtfs_init(void)
{
    int ret = 0;
//    int ret = vtfs_init_inode_cache();
//    if (ret) {
//        pr_err("inode cache creation failed\n");
//        goto end;
//    }

    ret = register_filesystem(&vtfs_file_system_type);
    if (ret) {
        pr_err("register_filesystem() failed\n");
        goto end;
    }

    pr_info("module loaded\n");
end:
    return ret;
}

static void __exit vtfs_exit(void)
{
    int ret = unregister_filesystem(&vtfs_file_system_type);
    if (ret)
        pr_err("unregister_filesystem() failed\n");

//    vtfs_destroy_inode_cache();

    pr_info("module unloaded\n");
}

module_init(vtfs_init);
module_exit(vtfs_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("VT, VA");
