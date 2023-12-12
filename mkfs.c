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
             num_inode_store_block = 0, num_block_bitmap_block = 0,
             num_inode_bitmap_block = 0;
    uint32_t mod;

    sb = malloc(sizeof(struct superblock));
    if (!sb)
        return NULL;

    num_blocks = fstats->st_size / VTFS_BLOCK_SIZE;
    num_inodes = num_blocks;
    mod = num_inodes % VTFS_INODES_PER_BLOCK;
    // if the last inode block is not full
    if (mod)
        // add the remaining inodes to the last block
        num_inodes += mod;
    
    // to get the number of blocks needed to store the inode
    num_inode_store_block = idiv_ceil(num_inodes, VTFS_INODES_PER_BLOCK);
    // number of blocks needed to store the inode bitmap
    num_inode_bitmap_block = idiv_ceil(num_inodes, VTFS_BLOCK_SIZE * 8);
    // number of blocks needed to store the block bitmap
    num_block_bitmap_block = idiv_ceil(num_blocks, VTFS_BLOCK_SIZE * 8);

    memset(sb, 0, sizeof(struct superblock));
    sb->info.magic = VTFS_MAGIC;
    sb->info.num_blocks = num_blocks;
    sb->info.num_inodes = num_inodes;
    
    num_free_blocks = num_blocks - num_inode_store_block - num_inode_bitmap_block - num_block_bitmap_block - 1;
    sb->info.num_free_blocks = num_free_blocks;
    // reserve the first inode for root
    sb->info.num_free_inodes = num_inodes - 1;

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
        "magic=%#x\n"
        "num_blocks=%u\n"
        "num_inodes=%u (num_inode_store_block=%u blocks)\n"
        "num_block_bitmap_block=%u\n"
        "num_inode_bitmap_block=%u\n"
        "num_free_inodes=%u\n"
        "num_free_blocks=%u\n",
        sizeof(struct superblock), sb->info.magic, sb->info.num_blocks,
        sb->info.num_inodes, sb->info.num_inode_store_block, sb->info.num_block_bitmap_block,
        sb->info.num_inode_bitmap_block, sb->info.num_free_inodes,
        sb->info.num_free_blocks);

    return sb;
}

static int write_inode_store(int fd, struct superblock *sb)
{
    int ret = 0;
    struct vtfs_inode *root_inode = NULL;
    char *block = NULL;
    uint32_t first_data_block = 1 + sb->info.num_inode_store_block +
                                sb->info.num_inode_bitmap_block +
                                sb->info.num_block_bitmap_block;
    uint32_t i = 1;
    
    block = malloc(VTFS_BLOCK_SIZE);
    if (!block)
        return -1;
    memset(block, 0, VTFS_BLOCK_SIZE);

    /* Root inode (inode 1) */
    root_inode = (struct vtfs_inode *)block;
    // /*
    //  * Use inode 1 for root.
    //  * If system use glibc, readdir will skip inode 0, and vfs also avoid
    //  * using inode 0
    //  */
    // // root_inode += 1;
    
    root_inode->i_uid = 0;
    root_inode->i_gid = 100;
    root_inode->i_mode = (S_IFDIR | S_IRUSR | S_IRGRP | S_IROTH | S_IWUSR |
                            S_IWGRP | S_IXUSR | S_IXGRP | S_IXOTH);
    root_inode->i_blocks = 1;
    root_inode->i_size = VTFS_BLOCK_SIZE;
    root_inode->i_atime = root_inode->i_mtime = root_inode->i_ctime = 0;
    root_inode->i_nlink = 2;
    root_inode->index_block = first_data_block;

    ret = write(fd, block, VTFS_BLOCK_SIZE);
    if (ret != VTFS_BLOCK_SIZE) {
        perror("write(): write root inode failed\n");
        ret = -1;
        goto free_block;
    }

    printf("inode i_mode: %d\n", root_inode->i_mode);
    printf("inode i_uid: %d\n", root_inode->i_uid);
    printf("inode i_gid: %d\n", root_inode->i_gid);
    printf("innode i_blocks: %d\n", root_inode->i_blocks);
    printf("inode i_size: %d\n", root_inode->i_size);
    
    /* Reset inode store blocks to zero */
    memset(block, 0, VTFS_BLOCK_SIZE);
    for (; i < sb->info.num_inode_store_block; i++) {
        ret = write(fd, block, VTFS_BLOCK_SIZE);
        if (ret != VTFS_BLOCK_SIZE) {
            perror("write(): write inode store failed\n");
            ret = -1;
            goto free_block;
        }
    }
    ret = 0;

     printf(
        "Inode store: wrote %d blocks\n"
        "\tinode size = %ld B\n",
        i, sizeof(struct vtfs_inode));

free_block:
    free(block);


    return ret;
}


static int write_inode_bitmap(int fd, struct superblock *sb)
{
    int ret = 0;
    char *block = NULL;
    uint64_t *inode_bitmap;

    block = malloc(VTFS_BLOCK_SIZE);
    if (!block)
        return -1;


    /* first inode bitmap block should contain the data of used root node */
    inode_bitmap = (uint64_t *)block;
    memset(inode_bitmap, 0xff, VTFS_BLOCK_SIZE);

    inode_bitmap[0] = 0xfffffffffffffffe;
    ret = write(fd, block, VTFS_BLOCK_SIZE);
    if (ret != VTFS_BLOCK_SIZE) {
        perror("write(): write inode bitmap failed\n");
        ret = -1;
        goto free_block;
    }

    /* Reset inode bitmap blocks to zero */
    inode_bitmap[0] = 0xffffffffffffffff;
    for (int i = 1; i < sb->info.num_inode_bitmap_block; i++) {
        ret = write(fd, inode_bitmap, VTFS_BLOCK_SIZE);
        if (ret != VTFS_BLOCK_SIZE) {
            perror("write(): write inode bitmap failed\n");
            ret = -1;
            goto free_block;
        }
    }
    ret = 0;

    printf("Inode bitmap: (%u blocks)\n"
            "Inode bitmap size=%d B\n", 
            sb->info.num_inode_bitmap_block, sb->info.num_inode_bitmap_block * VTFS_BLOCK_SIZE);

free_block:
    free(block);

    return ret;
}

static int write_block_bitmap(int fd, struct superblock *sb)
{
    int ret = 0;
    char *block = NULL;
    uint64_t *block_bitmap;
    // node store blocks
    // Inode free bitmap blocks
    // Block free bitmap blocks
    // The superblock
    // One additional block (often for the root directory)
    uint32_t num_used_blocks = 2 + sb->info.num_inode_store_block +
                               sb->info.num_inode_bitmap_block +
                               sb->info.num_block_bitmap_block;

    block = malloc(VTFS_BLOCK_SIZE);
    if (!block)
        return -1;
    
    block_bitmap = (uint64_t *)block;
    // TODO: first block
    memset(block_bitmap, 0xff, VTFS_BLOCK_SIZE);
    uint32_t i = 0;
    uint64_t line, mask;
    while (num_used_blocks) {
        line = 0xffffffffffffffff;
        for (mask = 0x1; mask; mask <<= 1) {
            line &= ~mask;
            num_used_blocks--;
            if (!num_used_blocks)
                break;
        }
        block_bitmap[i] = htole64(line);
        i++;
    }

    /* other blocks */
    memset(block_bitmap, 0xff, VTFS_BLOCK_SIZE);
    for (int i = 1; i < sb->info.num_block_bitmap_block; i++) {
        ret = write(fd, block_bitmap, VTFS_BLOCK_SIZE);
        if (ret != VTFS_BLOCK_SIZE) {
            perror("write(): write block bitmap failed\n");
            ret = -1;
            goto free_block;
        }
    }
    ret = 0;

    printf("Block bitmap: (%u blocks)\n"
            "Block bitmap size=%d B\n", 
            sb->info.num_block_bitmap_block, sb->info.num_block_bitmap_block * VTFS_BLOCK_SIZE);

free_block:
    free(block);

    return ret;

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

    /* Write inode store (blocks 1 to num_inode_store_block) */
    ret = write_inode_store(fd, sb);
    if (ret) {
        perror("write_inode_store():");
        ret = EXIT_FAILURE;
        goto free_sb;
    }

    /* Write inode bitmap */
    ret = write_inode_bitmap(fd, sb);
    if (ret) {
        perror("write_inode_bitmap():");
        ret = EXIT_FAILURE;
        goto free_sb;
    }

    /* Write block bitmap */
    ret = write_block_bitmap(fd, sb);
    if (ret) {
        perror("write_block_bitmap():");
        ret = EXIT_FAILURE;
        goto free_sb;
    }


free_sb:
    free(sb);
fclose:
    close(fd);

    return ret;
}
