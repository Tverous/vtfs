## Prerequisite

Install linux kernel header in advance.
```shell
$ sudo apt install linux-headers-$(uname -r)
```

## Build and Run

You can build the kernel module and tool with `make`.
Generate test image via `make test.img`, which creates a zeroed file of 50 MiB.

You can then mount this image on a system with the vtfs kernel module installed.
Let's test kernel module:
```shell
$ sudo insmod vtfs.ko
```

Corresponding kernel message:
```
vtfs: module loaded
```

Generate test image by creating a zeroed file of 50 MiB. We can then mount
this image on a system with the vtfs kernel module installed.
```shell
$ mkdir -p test
$ dd if=/dev/zero of=test.img bs=1M count=50
$ ./mkfs.vtfs test.img
$ sudo mount -o loop -t vtfs test.img test
```

You shall get the following kernel messages:
```
vtfs: '/dev/loop?' mount success
```
