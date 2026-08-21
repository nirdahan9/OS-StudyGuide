#include "fs.h"

/* ===================================================================== */
/*                          Internal State                               */
/* ===================================================================== */

/* File descriptor of the currently-mounted virtual disk (-1 when none). */
static int disk_fd = -1;

/* In-memory cached metadata. */
static superblock sb;                           
static unsigned char block_bitmap[MAX_BLOCKS / 8];

/* Byte offset of the inode table on disk: starts at block 2 (after the
 * superblock in block 0 and the bitmap in block 1). */
#define INODE_TABLE_OFFSET (2 * BLOCK_SIZE)

/* First data block: blocks 0..9 are metadata (1 superblock + 1 bitmap +
 * 8 inode-table blocks), so file content starts at block 10. */
#define FIRST_DATA_BLOCK 10


/* ===================================================================== */
/*                  Internal Helper Functions (signatures)               */
/* ===================================================================== */

/* Find an inode by filename. */
int find_inode(const char *filename);

/* Find a free inode. */
int find_free_inode();

/* Find a free data block. */
int find_free_block();

/* Mark a block as used. */
void mark_block_used(int block_num);

/* Mark a block as free. */
void mark_block_free(int block_num);

/* Read an inode from disk. */
void read_inode(int inode_num, inode *target);

/* Write an inode to disk. */
void write_inode(int inode_num, const inode *source);

/* Flush the cached superblock (block 0) and block bitmap (block 1) to disk. */
static void flush_metadata();


/* ===================================================================== */
/*                        Filesystem Operations                          */
/* ===================================================================== */

/* Creates and initializes a virtual disk file and prepares the filesystem. */
int fs_format(const char *disk_path)
{
    /* If the file exists, it will be overwritten. */
    int fd = open(disk_path, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        return -1;
    }

    /* Creates a 10 MB file. */
    char zero[BLOCK_SIZE];
    memset(zero, 0, sizeof(zero));
    for (int b = 0; b < MAX_BLOCKS; b++) {
        write(fd, zero, BLOCK_SIZE);
    }

    /* Initializes superblock, bitmap, and inode table. */
    sb.total_blocks = MAX_BLOCKS;
    sb.block_size   = BLOCK_SIZE;
    sb.free_blocks  = MAX_BLOCKS - FIRST_DATA_BLOCK;   /* data blocks only (2550) */
    sb.total_inodes = MAX_FILES;
    sb.free_inodes  = MAX_FILES;
    memset(block_bitmap, 0, sizeof(block_bitmap));
    lseek(fd, 0, SEEK_SET);
    write(fd, &sb, sizeof(sb));

    /* Sets all blocks as free except those used for metadata. */
    for (int b = 0; b < FIRST_DATA_BLOCK; b++) { 
        mark_block_used(b);
    }
    lseek(fd, (off_t)BLOCK_SIZE, SEEK_SET);            /* start of block 1 */
    write(fd, block_bitmap, sizeof(block_bitmap));

    close(fd);
    return 0;
}

/* Loads an existing filesystem from a virtual disk file. */
int fs_mount(const char *disk_path)
{
    /* Opens the virtual disk file. */
    int fd = open(disk_path, O_RDWR);
    if (fd < 0) {
        return -1;
    }

    /* Reads and validates the superblock. */
    superblock temp_sb;
    lseek(fd, 0, SEEK_SET);
    if (read(fd, &temp_sb, sizeof(temp_sb)) != sizeof(temp_sb))
    {
        close(fd);
        return -1;
    }
    if (temp_sb.total_blocks != MAX_BLOCKS || temp_sb.block_size != BLOCK_SIZE
        || temp_sb.total_inodes != MAX_FILES || temp_sb.free_blocks < 0 || temp_sb.free_inodes < 0)
    {
        close(fd);
        return -1;
    }

    /* Loads necessary metadata into memory. */
    sb = temp_sb;
    lseek(fd, (off_t)BLOCK_SIZE, SEEK_SET);
    if (read(fd, block_bitmap, sizeof(block_bitmap)) != sizeof(block_bitmap))
    {
        close(fd);
        return -1;
    }
    disk_fd = fd;
    return 0;
}

/* Ensures all pending changes are written to disk and closes the filesystem. */
void fs_unmount()
{
    if (disk_fd < 0) {
        return;
    }

    /* Flushes any cached data to disk. */
    flush_metadata();

    /* Closes the virtual disk file. */
    close(disk_fd);
    disk_fd = -1;
}


/* ===================================================================== */
/*                           File Operations                             */
/* ===================================================================== */

/* Creates a new empty file in the filesystem. */
int fs_create(const char *filename)
{
    if (disk_fd < 0) {
        return -3; // no filesystem mounted
    }

    /* Checks if the filename already exists. */
    if (find_inode(filename) != -1) {
        return -1; // file already exists
    }

    /* Finds a free inode. */
    int free_inode = find_free_inode();
    if (free_inode == -1) {
        return -2; // no free inodes available
    }

    /* Initializes the inode with the filename and zero size. */
    inode new_inode;
    memset(&new_inode, 0, sizeof(new_inode));
    new_inode.used = 1;
    new_inode.size = 0;
    strncpy(new_inode.name, filename, MAX_FILENAME - 1);
    write_inode(free_inode, &new_inode);

    /* Updates the superblock (decrease free_inodes). */
    sb.free_inodes--;
    flush_metadata();
    return 0;
}

/* Removes a file and frees its blocks. */
int fs_delete(const char *filename)
{
    if (disk_fd < 0) {
        return -2; // no filesystem mounted
    }

    /* Finds the file's inode. */
    int idx = find_inode(filename);
    if (idx == -1) {
        return -1; // file does not exist
    }
    inode file_inode;
    read_inode(idx, &file_inode);

    /* Marks all of the file's blocks as free in the bitmap. */
    for (int j = 0; j < MAX_DIRECT_BLOCKS; j++) {
        if (file_inode.blocks[j] != 0) {
            mark_block_free(file_inode.blocks[j]);
            file_inode.blocks[j] = 0;
            sb.free_blocks++;
        }
    }

    /* Marks the inode as free. */
    file_inode.used = 0;
    write_inode(idx, &file_inode);

    /* Updates the superblock (increase free_blocks and free_inodes). */
    sb.free_inodes++;       // free_blocks was increased above per freed block
    flush_metadata();
    return 0;
}

/* Lists files in the filesystem. */
int fs_list(char filenames[][MAX_FILENAME], int max_files)
{
    if (disk_fd < 0) {
        return -1; // no filesystem mounted
    }

    int count = 0;

    /* Scans the inode table for used inodes. */
    for (int i = 0; i < MAX_FILES && count < max_files; i++) {
        inode current_inode;
        read_inode(i, &current_inode);
        if (current_inode.used) {
            /* Copies filenames to the provided array. */
            strncpy(filenames[count], current_inode.name, MAX_FILENAME);
            count++;
        }
    }

    /* Returns the number of files found. */
    return count;
}

/* Writes data to a file, overwriting any existing content. */
int fs_write(const char *filename, const void *data, int size)
{
    if (disk_fd < 0 || size < 0) {
        return -3; 
    }

    /* Finds the file's inode. */
    int idx = find_inode(filename);
    if (idx == -1) {
        return -1; // file does not exist
    }
    inode file_inode;
    read_inode(idx, &file_inode);

    /* Calculates how many blocks are needed. */
    int blocks_needed = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;   // ceil(size / BLOCK_SIZE)
    if (blocks_needed > MAX_DIRECT_BLOCKS) {
        return -3; // exceeds the 48 KB maximum (12 direct blocks)
    }
    int current_blocks = 0;                                     // the file's own blocks can be reused
    for (int j = 0; j < MAX_DIRECT_BLOCKS; j++) {
        if (file_inode.blocks[j] != 0) {
            current_blocks++;
        }
    }
    if (blocks_needed > sb.free_blocks + current_blocks) {
        return -2; // out of space, even after reusing the file's own blocks
    }

    /* Frees any previously allocated blocks. */
    for (int j = 0; j < MAX_DIRECT_BLOCKS; j++) {
        if (file_inode.blocks[j] != 0) {
            mark_block_free(file_inode.blocks[j]);
            file_inode.blocks[j] = 0;
            sb.free_blocks++;
        }
    }

    /* Allocates new blocks as needed. */
    for (int j = 0; j < blocks_needed; j++) {
        int b = find_free_block();
        mark_block_used(b);
        sb.free_blocks--;
        file_inode.blocks[j] = b;
    }

    /* Writes data to the allocated blocks. */
    const char *src = (const char *)data;
    char buf[BLOCK_SIZE];
    for (int j = 0; j < blocks_needed; j++) {
        int offset = j * BLOCK_SIZE;
        int chunk  = size - offset;
        if (chunk > BLOCK_SIZE) {
            chunk = BLOCK_SIZE;
        }
        memset(buf, 0, BLOCK_SIZE);
        memcpy(buf, src + offset, chunk);
        lseek(disk_fd, (off_t)file_inode.blocks[j] * BLOCK_SIZE, SEEK_SET);
        write(disk_fd, buf, BLOCK_SIZE);
    }

    /* Updates the inode (size and block pointers). */
    file_inode.size = size;
    write_inode(idx, &file_inode);

    /* Updates the bitmap and superblock. */
    flush_metadata();
    return 0;
}

/* Reads file content into a buffer. */
int fs_read(const char *filename, void *buffer, int size)
{
    if (disk_fd < 0 || size < 0) {
        return -3; 
    }

    /* Finds the file's inode. */
    int idx = find_inode(filename);
    if (idx == -1) {
        return -1;
    }
    inode file_inode;
    read_inode(idx, &file_inode);

    /* Determines how many bytes to read (minimum of file size and buffer size). */
    int bytes_to_read = (size < file_inode.size) ? size : file_inode.size;
    if (bytes_to_read <= 0) {
        return 0;
    }

    /* Reads data from the file's blocks into the buffer. */
    char *dst = (char *)buffer;
    int bytes_read = 0;
    for (int j = 0; j < MAX_DIRECT_BLOCKS && bytes_read < bytes_to_read; j++) {
        int b = file_inode.blocks[j];
        if (b == 0) {
            break; // no more data blocks
        }
        int chunk = bytes_to_read - bytes_read;
        if (chunk > BLOCK_SIZE) {
            chunk = BLOCK_SIZE;
        }
        lseek(disk_fd, (off_t)b * BLOCK_SIZE, SEEK_SET);
        read(disk_fd, dst + bytes_read, chunk);
        bytes_read += chunk;
    }

    /* Returns the number of bytes read. */
    return bytes_read;
}

/* ===================================================================== */
/*                  Internal Helper Functions (implemented)              */
/* ===================================================================== */

/* Find an inode by filename. */
int find_inode(const char *filename)
{
    /* Iterate through all inodes to find a match by filename. */
    for (int i = 0; i < MAX_FILES; i++) {
        inode current_inode;
        read_inode(i, &current_inode);
        if (current_inode.used && strcmp(current_inode.name, filename) == 0) {
            return i;
        }
    }
    return -1;
}

/* Find a free inode. */
int find_free_inode()
{
    /* Iterate through all inodes to find a free one. */
    for (int i = 0; i < MAX_FILES; i++) {
        inode current_inode;
        read_inode(i, &current_inode);
        if (!current_inode.used) {
            return i;
        }
    }
    return -1;
}

/* Find a free data block. */
int find_free_block()
{
    /* Iterate through all blocks to find a free one. */
    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (!(block_bitmap[i / 8] & (1 << (i % 8)))) {
            return i;
        }
    }
    return -1;
}

/* Mark a block as used. */
void mark_block_used(int block_num)
{
    block_bitmap[block_num / 8] |= (1 << (block_num % 8));
}

/* Mark a block as free. */
void mark_block_free(int block_num)
{
    block_bitmap[block_num / 8] &= ~(1 << (block_num % 8));
}

/* Read an inode from disk. */
void read_inode(int inode_num, inode *target)
{
    if (inode_num < 0 || inode_num >= MAX_FILES) {
        return; // Invalid inode number
    }
    off_t offset = INODE_TABLE_OFFSET + (off_t)inode_num * sizeof(inode);
    lseek(disk_fd, offset, SEEK_SET);
    read(disk_fd, target, sizeof(inode));
}

/* Write an inode to disk. */
void write_inode(int inode_num, const inode *source)
{
    if (inode_num < 0 || inode_num >= MAX_FILES) {
        return; // Invalid inode number
    }
    off_t offset = INODE_TABLE_OFFSET + (off_t)inode_num * sizeof(inode);
    lseek(disk_fd, offset, SEEK_SET);
    write(disk_fd, source, sizeof(inode));
}

/* Flush the cached superblock (block 0) and block bitmap (block 1) to disk. */
static void flush_metadata()
{
    lseek(disk_fd, 0, SEEK_SET);
    write(disk_fd, &sb, sizeof(sb));
    lseek(disk_fd, (off_t)BLOCK_SIZE, SEEK_SET);
    write(disk_fd, block_bitmap, sizeof(block_bitmap));
}

