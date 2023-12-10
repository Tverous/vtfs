#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "vtfs.h"

struct superblock {
    struct vtfs_sb_info info;
    char padding[4064]; /* Padding to match block size */
};

/* Returns ceil(a/b) */
static inline uint32_t idiv_ceil(uint32_t a, uint32_t b)
{
    uint32_t ret = a / b;
    if (a % b)
        return ret + 1;
    return ret;
}

static struct superblock *write_superblock(int fd, struct stat *fstats)
{
    int ret;
    struct superblock *sb;
    uint32_t num_inodes = 0, num_blocks = 0, num_free_blocks = 0,
             num_free_inodes = 0, num_inode_store_block = 0, num_block_bitmap_block = 0,
             num_inode_bitmap_block = 0;
    uint32_t mod;

    sb = malloc(sizeof(struct superblock));
    if (!sb)
        return NULL;

    num_blocks = fstats->st_size / VTFS_BLOCK_SIZE;
    num_inodes = num_blocks;
    mod = num_inodes % VTFS_INODES_PER_BLOCK;
    if (mod)
        num_inodes += mod;
    num_inode_store_block = idiv_ceil(num_inodes, VTFS_INODES_PER_BLOCK);
    num_inode_bitmap_block = idiv_ceil(num_inodes, VTFS_BLOCK_SIZE * 8);
    num_block_bitmap_block = idiv_ceil(num_blocks, VTFS_BLOCK_SIZE * 8);

    memset(sb, 0, sizeof(struct superblock));
    sb->info.magic = VTFS_MAGIC;
    sb->info.num_blocks = num_blocks;
    sb->info.num_inodes = num_inodes;
    sb->info.num_free_blocks = num_free_blocks;
    sb->info.num_free_inodes = num_free_inodes;
    sb->info.num_inode_store_block = num_inode_store_block;
    sb->info.num_block_bitmap_block = num_inode_bitmap_block;
    sb->info.num_inode_bitmap_block = num_inode_bitmap_block;

    ret = write(fd, sb, sizeof(struct superblock));
    if (ret != sizeof(struct superblock)) {
        free(sb);
        return NULL;
    }

    printf(
        "Superblock: (%ld)\n"
        "\tmagic=%#x\n"
        "\num_blocks=%u\n"
        "\num_inodes=%u (num_inode_store_block=%u blocks)\n"
        "\num_block_bitmap_block=%u\n"
        "\num_inode_bitmap_block=%u\n"
        "\num_free_inodes=%u\n"
        "\num_free_blocks=%u\n",
        sizeof(struct superblock), sb->info.magic, sb->info.num_blocks,
        sb->info.num_inodes, sb->info.num_inode_store_block, sb->info.num_block_bitmap_block,
        sb->info.num_inode_bitmap_block, sb->info.num_free_inodes,
        sb->info.num_free_blocks);

    return sb;
}

static int write_inode_store(int fd, struct superblock *sb)
{
    struct vtfs_inode *root_inode = NULL;
    char *block = malloc(VTFS_BLOCK_SIZE);
    if (!block)
        return -1;

    memset(block, 0, VTFS_BLOCK_SIZE);

    /* Root inode (inode 0) */
    root_inode = (struct vtfs_inode *)block;
    

    /* Reset inode store blocks to zero */

}

int main(int argc, char **argv)
{
    int ret = EXIT_SUCCESS, fd;
    long int min_size;
    struct stat stat_buf;
    struct superblock *sb = NULL;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s disk\n", argv[0]);
        return EXIT_FAILURE;
    }

    /* Open disk image */
    fd = open(argv[1], O_RDWR);
    if (fd == -1) {
        perror("open():");
        return EXIT_FAILURE;
    }

    /* Get image size */
    ret = fstat(fd, &stat_buf);
    if (ret) {
        perror("fstat():");
        ret = EXIT_FAILURE;
        goto fclose;
    }

    /* Check if image is large enough */
    // TODO: why 100 * VTFS_BLOCK_SIZE?
    min_size = 100 * VTFS_BLOCK_SIZE;
    if (stat_buf.st_size <= min_size) {
        fprintf(stderr, "File is not large enough (size=%ld, min size=%ld)\n",
                stat_buf.st_size, min_size);
        ret = EXIT_FAILURE;
        goto fclose;
    }

    /* Write superblock (block 0) */
    sb = write_superblock(fd, &stat_buf);
    if (!sb) {
        perror("write_superblock():");
        ret = EXIT_FAILURE;
        goto fclose;
    }


free_sb:
    free(sb);
fclose:
    close(fd);

    return ret;
}
