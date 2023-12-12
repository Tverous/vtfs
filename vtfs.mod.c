#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif


static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x9c215423, "iget_locked" },
	{ 0xfe244bf0, "simple_rmdir" },
	{ 0x9921c95a, "simple_rename" },
	{ 0xfe8f2923, "unregister_filesystem" },
	{ 0x9ffe51c6, "d_make_root" },
	{ 0xdcd16a21, "sb_set_blocksize" },
	{ 0x37a0cba, "kfree" },
	{ 0x287ff013, "register_filesystem" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0xa6de336b, "kill_block_super" },
	{ 0xc1bff251, "unlock_new_inode" },
	{ 0x122c3a7e, "_printk" },
	{ 0x4c9e045e, "simple_setattr" },
	{ 0xaadc978b, "make_kuid" },
	{ 0xe57ce94, "simple_unlink" },
	{ 0x8336ca99, "simple_getattr" },
	{ 0x94eb9b93, "__brelse" },
	{ 0x458b8a32, "set_nlink" },
	{ 0x10ad97a2, "__bread_gfp" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x43115cf4, "make_kgid" },
	{ 0x6a44a1ab, "generic_read_dir" },
	{ 0x42656fd3, "mount_bdev" },
	{ 0x9ba0a933, "simple_lookup" },
	{ 0x45a93003, "generic_file_open" },
	{ 0xd07ae855, "kmalloc_trace" },
	{ 0x12f73c0c, "simple_link" },
	{ 0x9b0b90a6, "generic_file_llseek" },
	{ 0xeb233a45, "__kmalloc" },
	{ 0x86892d74, "kmalloc_caches" },
	{ 0x453e7dc, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "015AA9CEDC71E840A8F6E10");
