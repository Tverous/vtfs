#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/buffer_head.h>
#include <linux/mpage.h>

#include "vtfs.h"

const struct file_operations vtfs_file_ops = {
    .owner = THIS_MODULE,
    .read_iter = generic_file_read_iter,
    .write_iter = generic_file_write_iter,
    .llseek = generic_file_llseek,
    // .mmap = generic_file_mmap,
};

const struct address_space_operations vtfs_aops = {
    .readpage = vtfs_readpage,
    .writepage = vtfs_writepage,
    .write_begin = vtfs_write_begin,
    .write_end = vtfs_write_end
};