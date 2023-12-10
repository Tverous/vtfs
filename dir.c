#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/buffer_head.h>

#include "vtfs.h"

const struct file_operations vtfs_dir_ops = {
    .read = generic_read_dir,
};