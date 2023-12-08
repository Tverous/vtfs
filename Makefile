KVERSION := $(shell uname -r)
KERNEL_DIR = /usr/src/linux-$(KVERSION)/
 
obj-m += vtfs.o
# vtfs-objs := fs.o super.o inode.o file.o dir.o extent.o
vtfs-objs := init.o super.o inode.o

MKFS = mkfs.vtfs

all: $(MKFS)
	make -C $(KERNEL_DIR) M=$(PWD) modules

IMAGE ?= test.img
IMAGESIZE ?= 200
# To test max files(40920) in directory, the image size should be at least 159.85 MiB
# 40920 * 4096(block size) ~= 159.85 MiB

$(MKFS): mkfs.c
	$(CC) -std=gnu99 -Wall -o $@ $<

$(IMAGE): $(MKFS)
	dd if=/dev/zero of=${IMAGE} bs=1M count=${IMAGESIZE}
	./$< $(IMAGE)

check: all
	script/test.sh $(IMAGE) $(IMAGESIZE) $(MKFS)

clean:
	make -C $(KERNEL_DIR) M=$(PWD) clean
	rm -f *~ $(PWD)/*.ur-safe
	rm -f $(MKFS) $(IMAGE)

.PHONY: all clean