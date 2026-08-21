#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "fs.h"

#define DISK_PATH "test_extended_disk"

/*
============ HELPERS ============
*/
void setup_disk()
{
    if(access(DISK_PATH, F_OK) == 0)
    {
        remove(DISK_PATH);
    }
    int retval = fs_format(DISK_PATH);
    if(retval < 0)
    {
        fprintf(stderr, "Setup failed: fs_format failed.\n");
        exit(1);
    }
}

void mount_disk()
{
    if(fs_mount(DISK_PATH) < 0)
    {
        fprintf(stderr, "Setup failed: fs_mount failed.\n");
        exit(1);
    }
}

void cleanup()
{
    remove(DISK_PATH);
}

/*
============ fs_format TESTS ============
*/

void test_format_1_basic_success()
{
    printf("\n[format_1] Basic format success\n");
    if(access(DISK_PATH, F_OK) == 0)
        remove(DISK_PATH);
    int retval = fs_format(DISK_PATH);
    if(retval != 0)
    {
        fprintf(stderr, "Test failed: fs_format returned %d, expected 0.\n", retval);
        exit(1);
    }
    printf("PASSED\n");
}

void test_format_2_overwrite_existing()
{
    printf("\n[format_2] Format overwrites existing disk\n");
    setup_disk();
    mount_disk();
    fs_create("old_file");
    fs_write("old_file", "old_data", 8);
    fs_unmount();

    int retval = fs_format(DISK_PATH);
    if(retval != 0)
    {
        fprintf(stderr, "Test failed: fs_format returned %d on overwrite.\n", retval);
        exit(1);
    }
    mount_disk();
    char names[MAX_FILES][MAX_FILENAME];
    int count = fs_list(names, MAX_FILES);
    if(count != 0)
    {
        fprintf(stderr, "Test failed: expected 0 files after re-format, got %d.\n", count);
        exit(1);
    }
    fs_unmount();
    printf("PASSED\n");
}

void test_format_3_disk_file_created()
{
    printf("\n[format_3] Disk file exists after format\n");
    if(access(DISK_PATH, F_OK) == 0)
        remove(DISK_PATH);
    fs_format(DISK_PATH);
    if(access(DISK_PATH, F_OK) != 0)
    {
        fprintf(stderr, "Test failed: disk file does not exist after format.\n");
        exit(1);
    }
    printf("PASSED\n");
}

void test_format_4_empty_filesystem()
{
    printf("\n[format_4] Freshly formatted filesystem has 0 files\n");
    setup_disk();
    mount_disk();
    char names[MAX_FILES][MAX_FILENAME];
    int count = fs_list(names, MAX_FILES);
    if(count != 0)
    {
        fprintf(stderr, "Test failed: expected 0 files, got %d.\n", count);
        exit(1);
    }
    fs_unmount();
    printf("PASSED\n");
}

void test_format_5_metadata_blocks_reserved()
{
    printf("\n[format_5] Data blocks available after format (metadata reserved)\n");
    setup_disk();
    mount_disk();
    fs_create("bigfile");
    char data[BLOCK_SIZE * MAX_DIRECT_BLOCKS];
    memset(data, 'A', sizeof(data));
    int retval = fs_write("bigfile", data, sizeof(data));
    if(retval != 0)
    {
        fprintf(stderr, "Test failed: cannot write 48KB to fresh disk (ret=%d).\n", retval);
        exit(1);
    }
    fs_unmount();
    printf("PASSED\n");
}

/*
============ fs_mount TESTS ============
*/

void test_mount_1_basic_success()
{
    printf("\n[mount_1] Basic mount success\n");
    setup_disk();
    int retval = fs_mount(DISK_PATH);
    if(retval != 0)
    {
        fprintf(stderr, "Test failed: fs_mount returned %d, expected 0.\n", retval);
        exit(1);
    }
    fs_unmount();
    printf("PASSED\n");
}

void test_mount_2_nonexistent_file()
{
    printf("\n[mount_2] Mount nonexistent file returns -1\n");
    int retval = fs_mount("no_such_disk_xyz_999.img");
    if(retval != -1)
    {
        fprintf(stderr, "Test failed: fs_mount returned %d, expected -1.\n", retval);
        exit(1);
    }
    printf("PASSED\n");
}

void test_mount_3_data_persists()
{
    printf("\n[mount_3] Data persists across mount/unmount\n");
    setup_disk();
    mount_disk();
    fs_create("persist");
    fs_write("persist", "hello", 5);
    fs_unmount();

    mount_disk();
    char buf[64];
    memset(buf, 0, sizeof(buf));
    int n = fs_read("persist", buf, 64);
    if(n != 5)
    {
        fprintf(stderr, "Test failed: expected 5 bytes, got %d.\n", n);
        exit(1);
    }
    if(memcmp(buf, "hello", 5) != 0)
    {
        fprintf(stderr, "Test failed: data mismatch after remount.\n");
        exit(1);
    }
    fs_unmount();
    printf("PASSED\n");
}

void test_mount_4_multiple_files_persist()
{
    printf("\n[mount_4] Multiple files persist after remount\n");
    setup_disk();
    mount_disk();
    fs_create("file_a");
    fs_create("file_b");
    fs_create("file_c");
    fs_unmount();

    mount_disk();
    char names[MAX_FILES][MAX_FILENAME];
    int count = fs_list(names, MAX_FILES);
    if(count != 3)
    {
        fprintf(stderr, "Test failed: expected 3 files, got %d.\n", count);
        exit(1);
    }
    fs_unmount();
    printf("PASSED\n");
}

void test_mount_5_superblock_persisted()
{
    printf("\n[mount_5] Superblock state persists (can create after remount)\n");
    setup_disk();
    mount_disk();
    fs_create("x");
    fs_create("y");
    fs_unmount();

    mount_disk();
    int retval = fs_create("z");
    if(retval != 0)
    {
        fprintf(stderr, "Test failed: fs_create returned %d after remount.\n", retval);
        exit(1);
    }
    fs_unmount();
    printf("PASSED\n");
}

/*
============ fs_unmount TESTS ============
*/

void test_unmount_1_flushes_writes()
{
    printf("\n[unmount_1] Unmount flushes written data\n");
    setup_disk();
    mount_disk();
    fs_create("flush");
    fs_write("flush", "data123", 7);
    fs_unmount();

    mount_disk();
    char buf[64];
    memset(buf, 0, sizeof(buf));
    int n = fs_read("flush", buf, 64);
    if(n != 7)
    {
        fprintf(stderr, "Test failed: expected 7 bytes, got %d.\n", n);
        exit(1);
    }
    fs_unmount();
    printf("PASSED\n");
}

void test_unmount_2_file_size_persisted()
{
    printf("\n[unmount_2] File size metadata persisted\n");
    setup_disk();
    mount_disk();
    fs_create("sized");
    char data[5000];
    memset(data, 'X', sizeof(data));
    fs_write("sized", data, sizeof(data));
    fs_unmount();

    mount_disk();
    char buf[8192];
    memset(buf, 0, sizeof(buf));
    int n = fs_read("sized", buf, sizeof(buf));
    if(n != 5000)
    {
        fprintf(stderr, "Test failed: expected 5000 bytes, got %d.\n", n);
        exit(1);
    }
    fs_unmount();
    printf("PASSED\n");
}

void test_unmount_3_delete_persisted()
{
    printf("\n[unmount_3] Delete persisted after unmount\n");
    setup_disk();
    mount_disk();
    fs_create("gone");
    fs_delete("gone");
    fs_unmount();

    mount_disk();
    char names[MAX_FILES][MAX_FILENAME];
    int count = fs_list(names, MAX_FILES);
    if(count != 0)
    {
        fprintf(stderr, "Test failed: expected 0 files, got %d.\n", count);
        exit(1);
    }
    fs_unmount();
    printf("PASSED\n");
}

void test_unmount_4_bitmap_persisted()
{
    printf("\n[unmount_4] Bitmap state persisted (freed blocks reusable)\n");
    setup_disk();
    mount_disk();
    fs_create("bmap");
    char data[BLOCK_SIZE * 3];
    memset(data, 'B', sizeof(data));
    fs_write("bmap", data, sizeof(data));
    fs_unmount();

    mount_disk();
    fs_delete("bmap");
    fs_create("bmap2");
    int retval = fs_write("bmap2", data, sizeof(data));
    if(retval != 0)
    {
        fprintf(stderr, "Test failed: cannot reuse freed blocks (ret=%d).\n", retval);
        exit(1);
    }
    fs_unmount();
    printf("PASSED\n");
}

void test_unmount_5_multiple_cycles()
{
    printf("\n[unmount_5] Multiple mount/unmount cycles\n");
    setup_disk();
    for(int i = 0; i < 5; i++)
    {
        mount_disk();
        char name[MAX_FILENAME];
        snprintf(name, MAX_FILENAME, "cycle%d", i);
        fs_create(name);
        fs_unmount();
    }
    mount_disk();
    char names[MAX_FILES][MAX_FILENAME];
    int count = fs_list(names, MAX_FILES);
    if(count != 5)
    {
        fprintf(stderr, "Test failed: expected 5 files, got %d.\n", count);
        exit(1);
    }
    fs_unmount();
    printf("PASSED\n");
}

/*
============ fs_create TESTS ============
*/

void test_create_1_basic_success()
{
    printf("\n[create_1] Basic create success\n");
    setup_disk();
    mount_disk();
    int retval = fs_create("newfile");
    if(retval != 0)
    {
        fprintf(stderr, "Test failed: fs_create returned %d, expected 0.\n", retval);
        exit(1);
    }
    fs_unmount();
    printf("PASSED\n");
}

void test_create_2_duplicate_returns_neg1()
{
    printf("\n[create_2] Duplicate filename returns -1\n");
    setup_disk();
    mount_disk();
    fs_create("dup");
    int retval = fs_create("dup");
    if(retval != -1)
    {
        fprintf(stderr, "Test failed: fs_create returned %d for duplicate, expected -1.\n", retval);
        exit(1);
    }
    fs_unmount();
    printf("PASSED\n");
}

void test_create_3_no_free_inodes()
{
    printf("\n[create_3] No free inodes returns -2\n");
    setup_disk();
    mount_disk();
    for(int i = 0; i < MAX_FILES; i++)
    {
        char name[MAX_FILENAME];
        snprintf(name, MAX_FILENAME, "file%d", i);
        int retval = fs_create(name);
        if(retval != 0)
        {
            fprintf(stderr, "Test failed: fs_create returned %d for file%d.\n", retval, i);
            exit(1);
        }
    }
    int retval = fs_create("overflow");
    if(retval != -2)
    {
        fprintf(stderr, "Test failed: fs_create returned %d, expected -2.\n", retval);
        exit(1);
    }
    fs_unmount();
    printf("PASSED\n");
}

void test_create_4_max_length_filename()
{
    printf("\n[create_4] Max length filename (28 chars)\n");
    setup_disk();
    mount_disk();
    // 28 chars + null terminator = 29 = MAX_FILENAME
    const char *long_name = "abcdefghijklmnopqrstuvwxyzab";
    int retval = fs_create(long_name);
    if(retval != 0)
    {
        fprintf(stderr, "Test failed: fs_create returned %d for 28-char name.\n", retval);
        exit(1);
    }
    char names[MAX_FILES][MAX_FILENAME];
    int count = fs_list(names, MAX_FILES);
    if(count != 1)
    {
        fprintf(stderr, "Test failed: expected 1 file, got %d.\n", count);
        exit(1);
    }
    if(strcmp(names[0], long_name) != 0)
    {
        fprintf(stderr, "Test failed: filename mismatch. Got '%s', expected '%s'.\n", names[0], long_name);
        exit(1);
    }
    fs_unmount();
    printf("PASSED\n");
}

void test_create_5_new_file_size_zero()
{
    printf("\n[create_5] Newly created file has size 0\n");
    setup_disk();
    mount_disk();
    fs_create("empty");
    char buf[64];
    int n = fs_read("empty", buf, 64);
    if(n != 0)
    {
        fprintf(stderr, "Test failed: expected 0 bytes from new file, got %d.\n", n);
        exit(1);
    }
    fs_unmount();
    printf("PASSED\n");
}

/*
============ fs_delete TESTS ============
*/

void test_delete_1_basic_success()
{
    printf("\n[delete_1] Basic delete success\n");
    setup_disk();
    mount_disk();
    fs_create("todelete");
    int retval = fs_delete("todelete");
    if(retval != 0)
    {
        fprintf(stderr, "Test failed: fs_delete returned %d, expected 0.\n", retval);
        exit(1);
    }
    fs_unmount();
    printf("PASSED\n");
}

void test_delete_2_nonexistent_returns_neg1()
{
    printf("\n[delete_2] Delete nonexistent file returns -1\n");
    setup_disk();
    mount_disk();
    int retval = fs_delete("nosuchfile");
    if(retval != -1)
    {
        fprintf(stderr, "Test failed: fs_delete returned %d, expected -1.\n", retval);
        exit(1);
    }
    fs_unmount();
    printf("PASSED\n");
}

void test_delete_3_frees_inode_for_reuse()
{
    printf("\n[delete_3] Delete frees inode for reuse\n");
    setup_disk();
    mount_disk();
    for(int i = 0; i < MAX_FILES; i++)
    {
        char name[MAX_FILENAME];
        snprintf(name, MAX_FILENAME, "f%d", i);
        fs_create(name);
    }
    int retval = fs_create("extra");
    if(retval != -2)
    {
        fprintf(stderr, "Test failed: expected -2 when all inodes full, got %d.\n", retval);
        exit(1);
    }
    fs_delete("f0");
    retval = fs_create("reused");
    if(retval != 0)
    {
        fprintf(stderr, "Test failed: fs_create returned %d after freeing inode.\n", retval);
        exit(1);
    }
    fs_unmount();
    printf("PASSED\n");
}

void test_delete_4_frees_blocks_for_reuse()
{
    printf("\n[delete_4] Delete frees data blocks for reuse\n");
    setup_disk();
    mount_disk();
    fs_create("big1");
    char data[BLOCK_SIZE * MAX_DIRECT_BLOCKS];
    memset(data, 'Z', sizeof(data));
    fs_write("big1", data, sizeof(data));
    fs_delete("big1");
    fs_create("big2");
    int retval = fs_write("big2", data, sizeof(data));
    if(retval != 0)
    {
        fprintf(stderr, "Test failed: cannot reuse freed blocks (ret=%d).\n", retval);
        exit(1);
    }
    fs_unmount();
    printf("PASSED\n");
}

void test_delete_5_removed_from_listing()
{
    printf("\n[delete_5] Deleted file removed from fs_list\n");
    setup_disk();
    mount_disk();
    fs_create("stay");
    fs_create("go");
    fs_delete("go");
    char names[MAX_FILES][MAX_FILENAME];
    int count = fs_list(names, MAX_FILES);
    if(count != 1)
    {
        fprintf(stderr, "Test failed: expected 1 file, got %d.\n", count);
        exit(1);
    }
    if(strcmp(names[0], "stay") != 0)
    {
        fprintf(stderr, "Test failed: wrong file remains. Got '%s'.\n", names[0]);
        exit(1);
    }
    fs_unmount();
    printf("PASSED\n");
}

/*
============ fs_list TESTS ============
*/

void test_list_1_empty_filesystem()
{
    printf("\n[list_1] Empty filesystem returns 0\n");
    setup_disk();
    mount_disk();
    char names[MAX_FILES][MAX_FILENAME];
    int count = fs_list(names, MAX_FILES);
    if(count != 0)
    {
        fprintf(stderr, "Test failed: expected 0, got %d.\n", count);
        exit(1);
    }
    fs_unmount();
    printf("PASSED\n");
}

void test_list_2_multiple_files()
{
    printf("\n[list_2] Lists multiple files\n");
    setup_disk();
    mount_disk();
    fs_create("alpha");
    fs_create("beta");
    fs_create("gamma");
    char names[MAX_FILES][MAX_FILENAME];
    int count = fs_list(names, MAX_FILES);
    if(count != 3)
    {
        fprintf(stderr, "Test failed: expected 3 files, got %d.\n", count);
        exit(1);
    }
    fs_unmount();
    printf("PASSED\n");
}

void test_list_3_respects_max_files()
{
    printf("\n[list_3] Respects max_files parameter\n");
    setup_disk();
    mount_disk();
    fs_create("a");
    fs_create("b");
    fs_create("c");
    fs_create("d");
    fs_create("e");
    char names[5][MAX_FILENAME];
    int count = fs_list(names, 2);
    if(count != 2)
    {
        fprintf(stderr, "Test failed: expected 2 (max_files), got %d.\n", count);
        exit(1);
    }
    fs_unmount();
    printf("PASSED\n");
}

void test_list_4_reflects_deletions()
{
    printf("\n[list_4] Reflects deletions\n");
    setup_disk();
    mount_disk();
    fs_create("x");
    fs_create("y");
    fs_create("z");
    fs_delete("y");
    char names[MAX_FILES][MAX_FILENAME];
    int count = fs_list(names, MAX_FILES);
    if(count != 2)
    {
        fprintf(stderr, "Test failed: expected 2 files, got %d.\n", count);
        exit(1);
    }
    fs_unmount();
    printf("PASSED\n");
}

void test_list_5_all_256_files()
{
    printf("\n[list_5] Lists all 256 files\n");
    setup_disk();
    mount_disk();
    for(int i = 0; i < MAX_FILES; i++)
    {
        char name[MAX_FILENAME];
        snprintf(name, MAX_FILENAME, "file%d", i);
        fs_create(name);
    }
    char names[MAX_FILES][MAX_FILENAME];
    int count = fs_list(names, MAX_FILES);
    if(count != 256)
    {
        fprintf(stderr, "Test failed: expected 256 files, got %d.\n", count);
        exit(1);
    }
    fs_unmount();
    printf("PASSED\n");
}

/*
============ fs_write TESTS ============
*/

void test_write_1_basic_write_and_read()
{
    printf("\n[write_1] Basic write and read back\n");
    setup_disk();
    mount_disk();
    fs_create("wfile");
    int retval = fs_write("wfile", "hello world", 11);
    if(retval != 0)
    {
        fprintf(stderr, "Test failed: fs_write returned %d, expected 0.\n", retval);
        exit(1);
    }
    char buf[64];
    memset(buf, 0, sizeof(buf));
    int n = fs_read("wfile", buf, 64);
    if(n != 11 || memcmp(buf, "hello world", 11) != 0)
    {
        fprintf(stderr, "Test failed: data mismatch after write.\n");
        exit(1);
    }
    fs_unmount();
    printf("PASSED\n");
}

void test_write_2_nonexistent_returns_neg1()
{
    printf("\n[write_2] Write to nonexistent file returns -1\n");
    setup_disk();
    mount_disk();
    int retval = fs_write("ghost", "data", 4);
    if(retval != -1)
    {
        fprintf(stderr, "Test failed: fs_write returned %d, expected -1.\n", retval);
        exit(1);
    }
    fs_unmount();
    printf("PASSED\n");
}

void test_write_3_overwrite_replaces()
{
    printf("\n[write_3] Overwrite replaces content entirely\n");
    setup_disk();
    mount_disk();
    fs_create("over");
    fs_write("over", "first_data_here", 15);
    fs_write("over", "second", 6);
    char buf[64];
    memset(buf, 0, sizeof(buf));
    int n = fs_read("over", buf, 64);
    if(n != 6)
    {
        fprintf(stderr, "Test failed: expected 6 bytes after overwrite, got %d.\n", n);
        exit(1);
    }
    if(memcmp(buf, "second", 6) != 0)
    {
        fprintf(stderr, "Test failed: content not replaced.\n");
        exit(1);
    }
    fs_unmount();
    printf("PASSED\n");
}

void test_write_4_max_48kb()
{
    printf("\n[write_4] Write max file size 48KB\n");
    setup_disk();
    mount_disk();
    fs_create("maxfile");
    char data[BLOCK_SIZE * MAX_DIRECT_BLOCKS];
    memset(data, 'M', sizeof(data));
    int retval = fs_write("maxfile", data, sizeof(data));
    if(retval != 0)
    {
        fprintf(stderr, "Test failed: fs_write returned %d for 48KB.\n", retval);
        exit(1);
    }
    char buf[BLOCK_SIZE * MAX_DIRECT_BLOCKS];
    memset(buf, 0, sizeof(buf));
    int n = fs_read("maxfile", buf, sizeof(buf));
    if(n != (int)sizeof(data))
    {
        fprintf(stderr, "Test failed: read back %d bytes, expected %d.\n", n, (int)sizeof(data));
        exit(1);
    }
    if(buf[0] != 'M' || buf[sizeof(data) - 1] != 'M')
    {
        fprintf(stderr, "Test failed: data corruption in 48KB file.\n");
        exit(1);
    }
    fs_unmount();
    printf("PASSED\n");
}

void test_write_5_out_of_space()
{
    printf("\n[write_5] Write returns -2 when out of disk space\n");
    setup_disk();
    mount_disk();
    // 2550 data blocks total. Fill with files of 12 blocks each.
    // 212 files * 12 blocks = 2544 blocks used, 6 remain.
    char data[BLOCK_SIZE * MAX_DIRECT_BLOCKS];
    memset(data, 'F', sizeof(data));
    for(int i = 0; i < 212; i++)
    {
        char name[MAX_FILENAME];
        snprintf(name, MAX_FILENAME, "fill%d", i);
        fs_create(name);
        fs_write(name, data, sizeof(data));
    }
    // 6 blocks remain, try writing 12 blocks => should fail
    fs_create("toobig");
    int retval = fs_write("toobig", data, sizeof(data));
    if(retval != -2)
    {
        fprintf(stderr, "Test failed: fs_write returned %d, expected -2.\n", retval);
        exit(1);
    }
    fs_unmount();
    printf("PASSED\n");
}

/*
============ fs_read TESTS ============
*/

void test_read_1_basic_read()
{
    printf("\n[read_1] Basic read\n");
    setup_disk();
    mount_disk();
    fs_create("rfile");
    fs_write("rfile", "test data", 9);
    char buf[64];
    memset(buf, 0, sizeof(buf));
    int n = fs_read("rfile", buf, 64);
    if(n != 9 || memcmp(buf, "test data", 9) != 0)
    {
        fprintf(stderr, "Test failed: read data mismatch.\n");
        exit(1);
    }
    fs_unmount();
    printf("PASSED\n");
}

void test_read_2_nonexistent_returns_neg1()
{
    printf("\n[read_2] Read nonexistent file returns -1\n");
    setup_disk();
    mount_disk();
    char buf[64];
    int retval = fs_read("nope", buf, 64);
    if(retval != -1)
    {
        fprintf(stderr, "Test failed: fs_read returned %d, expected -1.\n", retval);
        exit(1);
    }
    fs_unmount();
    printf("PASSED\n");
}

void test_read_3_buffer_smaller_than_file()
{
    printf("\n[read_3] Buffer smaller than file reads min(file_size, buf_size)\n");
    setup_disk();
    mount_disk();
    fs_create("big");
    char data[1000];
    memset(data, 'R', 1000);
    fs_write("big", data, 1000);
    char buf[100];
    memset(buf, 0, sizeof(buf));
    int n = fs_read("big", buf, 100);
    if(n != 100)
    {
        fprintf(stderr, "Test failed: expected 100 bytes, got %d.\n", n);
        exit(1);
    }
    if(buf[0] != 'R' || buf[99] != 'R')
    {
        fprintf(stderr, "Test failed: data mismatch in partial read.\n");
        exit(1);
    }
    fs_unmount();
    printf("PASSED\n");
}

void test_read_4_buffer_larger_than_file()
{
    printf("\n[read_4] Buffer larger than file reads only file_size bytes\n");
    setup_disk();
    mount_disk();
    fs_create("small");
    fs_write("small", "hi", 2);
    char buf[1024];
    memset(buf, 0, sizeof(buf));
    int n = fs_read("small", buf, 1024);
    if(n != 2)
    {
        fprintf(stderr, "Test failed: expected 2 bytes, got %d.\n", n);
        exit(1);
    }
    fs_unmount();
    printf("PASSED\n");
}

void test_read_5_multi_block_integrity()
{
    printf("\n[read_5] Multi-block read data integrity\n");
    setup_disk();
    mount_disk();
    fs_create("multi");
    // 13000 bytes spans 4 blocks (3 full + partial)
    char data[13000];
    for(int i = 0; i < 13000; i++)
    {
        data[i] = (char)(i % 256);
    }
    fs_write("multi", data, 13000);
    char buf[13000];
    memset(buf, 0, sizeof(buf));
    int n = fs_read("multi", buf, 13000);
    if(n != 13000)
    {
        fprintf(stderr, "Test failed: expected 13000 bytes, got %d.\n", n);
        exit(1);
    }
    if(memcmp(buf, data, 13000) != 0)
    {
        fprintf(stderr, "Test failed: multi-block data corrupted.\n");
        exit(1);
    }
    fs_unmount();
    printf("PASSED\n");
}

/*
============ MAIN ============
*/

int main(int argc, char *argv[])
{
    printf("========================================\n");
    printf("   OnlyFiles Extended Test Suite\n");
    printf("========================================\n");

    // fs_format
    test_format_1_basic_success();
    test_format_2_overwrite_existing();
    test_format_3_disk_file_created();
    test_format_4_empty_filesystem();
    test_format_5_metadata_blocks_reserved();

    // fs_mount
    test_mount_1_basic_success();
    test_mount_2_nonexistent_file();
    test_mount_3_data_persists();
    test_mount_4_multiple_files_persist();
    test_mount_5_superblock_persisted();

    // fs_unmount
    test_unmount_1_flushes_writes();
    test_unmount_2_file_size_persisted();
    test_unmount_3_delete_persisted();
    test_unmount_4_bitmap_persisted();
    test_unmount_5_multiple_cycles();

    // fs_create
    test_create_1_basic_success();
    test_create_2_duplicate_returns_neg1();
    test_create_3_no_free_inodes();
    test_create_4_max_length_filename();
    test_create_5_new_file_size_zero();

    // fs_delete
    test_delete_1_basic_success();
    test_delete_2_nonexistent_returns_neg1();
    test_delete_3_frees_inode_for_reuse();
    test_delete_4_frees_blocks_for_reuse();
    test_delete_5_removed_from_listing();

    // fs_list
    test_list_1_empty_filesystem();
    test_list_2_multiple_files();
    test_list_3_respects_max_files();
    test_list_4_reflects_deletions();
    test_list_5_all_256_files();

    // fs_write
    test_write_1_basic_write_and_read();
    test_write_2_nonexistent_returns_neg1();
    test_write_3_overwrite_replaces();
    test_write_4_max_48kb();
    test_write_5_out_of_space();

    // fs_read
    test_read_1_basic_read();
    test_read_2_nonexistent_returns_neg1();
    test_read_3_buffer_smaller_than_file();
    test_read_4_buffer_larger_than_file();
    test_read_5_multi_block_integrity();

    printf("\n========================================\n");
    printf("   All extended tests PASSED!\n");
    printf("========================================\n");

    cleanup();
    return 0;
}
