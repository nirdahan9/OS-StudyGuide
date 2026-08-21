#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "fs.h"

/*
============ CONSTANTS ============
You may change these constants to test different scenarios.
*/
#define SIZE_OF_FILE 10000
#define NUM_FILES 32
#define MAX_ALLOWED_NUM_FILES 32

/*
============ HELPERS ============
Please do not modify these functions.
*/
void create_disk(const char *disk_path)
{
    // if "disk" file exists, remove it
    if(access(disk_path, F_OK) == 0)
    {
        if(remove(disk_path) == 0)
        {
            printf("Removed existing disk file.\n");
        }
        else
        {
            perror("Failed to remove existing disk file");
            exit(1);
        }
    }
    int retval = fs_format(disk_path);
    if(retval < 0)
    {
        fprintf(stderr, "Test failed: call to fs_format failed: %s\n", disk_path);
        exit(1);
    }
}

int safe_string_compare(const char *s1, const char *s2, int max_length)
{
    if(s1 == NULL || s2 == NULL)
    {
        return -1; // indicate error
    }
    int first_has_null = 0, second_has_null = 0;
    for(int i = 0; i < max_length; i++)
    {
        if(s1[i] == '\0')
        {
            first_has_null = 1;
        }
        if(s2[i] == '\0')
        {
            second_has_null = 1;
        }
    }
    if(first_has_null)
    {
        if(!second_has_null)
        {
            return -1; // s1 is shorter than s2
        }
    }
    else
    {
        if(second_has_null)
        {
            return -1; // s2 is shorter than s1
        }
    }
    if(first_has_null && second_has_null)
    {
        return strcmp(s1, s2); // both strings are null-terminated
    }
    return memcmp(s1, s2, max_length);
}

void check_files_are_created(char expected_filenames[NUM_FILES][MAX_FILENAME])
{
    // check files list
    char list_of_files[NUM_FILES][MAX_FILENAME];
    if(fs_list(list_of_files, NUM_FILES) < 0)
    {
        fprintf(stderr, "Test failed: fs_list failed to list files.\n");
        exit(1);
    }   

    for(int i = 0; i < NUM_FILES; i++)
    {
        if(safe_string_compare(list_of_files[i], expected_filenames[i], MAX_FILENAME) != 0)
        {
            fprintf(stderr, "File %s not found in the filesystem.\n", expected_filenames[i]);
            printf("Expected %d-th file: %s\n", i, expected_filenames[i]);
            printf("Got %d-th file: %s\n", i, list_of_files[i]);
            exit(1);
        }
    }
    printf("All files have been created successfully.\n");
}

void init_filenames(char filenames_array[NUM_FILES][MAX_FILENAME])
{
    for(int i = 0; i < NUM_FILES; i++)
    {
        memset(filenames_array[i], 0, MAX_FILENAME);
        snprintf(filenames_array[i], MAX_FILENAME, "file_%d.txt", i);
    }
}


/*
============ ADDITIONAL TESTS ============
The assignment ("Testing your implementation") says to extend the basic test
above with additional tests:
  - Creating multiple files.
  - Writing files of different sizes.
  - Filling the filesystem to capacity.
  - Deleting files and reusing the space.
  - Testing error conditions (file not found, disk full).

The functions below cover exactly those scenarios (plus a persistence bonus).
Unlike the basic test, they report PASS/FAIL per check and keep going, which is
convenient while implementing fs.c step by step. They reuse the provided
create_disk() helper to start each test from a freshly formatted disk.
*/

#define MAX_FILE_BYTES (MAX_DIRECT_BLOCKS * BLOCK_SIZE)  /* 48 KB */
#define BIGBUF         (MAX_FILE_BYTES + 16)             /* room for the > 48 KB case */

static int g_extra_run = 0, g_extra_passed = 0, g_extra_failed = 0;

#define CHECK(cond, desc)                                              \
    do {                                                              \
        g_extra_run++;                                                 \
        if (cond) {                                                    \
            g_extra_passed++;                                          \
            printf("  [PASS] %s\n", desc);                            \
        } else {                                                       \
            g_extra_failed++;                                          \
            printf("  [FAIL] %s   (line %d)\n", desc, __LINE__);      \
        }                                                             \
    } while (0)

/* Big, reusable buffers (kept off the stack). */
static char extra_wbuf[BIGBUF];
static char extra_rbuf[BIGBUF];

/* Deterministic, position- and seed-dependent byte pattern. */
static void extra_fill(char *buf, int size, int seed)
{
    for (int i = 0; i < size; i++)
    {
        buf[i] = (char)('A' + ((i + seed) % 26));
    }
}

/* Verify buf[0..size) matches extra_fill(seed). Returns 1 if OK, 0 otherwise. */
static int extra_check(const char *buf, int size, int seed)
{
    for (int i = 0; i < size; i++)
    {
        if (buf[i] != (char)('A' + ((i + seed) % 26)))
        {
            return 0;
        }
    }
    return 1;
}

/* Format + mount a fresh "disk"; abort hard if the basics don't work. */
static void extra_fresh_disk(void)
{
    create_disk("disk");
    if (fs_mount("disk") < 0)
    {
        fprintf(stderr, "Test failed: fs_mount failed in additional tests.\n");
        exit(1);
    }
}

/* (1) Creating multiple files: create several, list them, reject duplicates. */
static void test_creating_multiple_files(void)
{
    printf("\n-- Creating multiple files --\n");
    extra_fresh_disk();

    CHECK(fs_create("alpha.txt") == 0,  "create alpha.txt");
    CHECK(fs_create("beta.txt")  == 0,  "create beta.txt");
    CHECK(fs_create("gamma.txt") == 0,  "create gamma.txt");
    CHECK(fs_create("alpha.txt") == -1, "creating a duplicate returns -1");

    char list[MAX_FILES][MAX_FILENAME];
    int n = fs_list(list, MAX_FILES);
    CHECK(n == 3, "fs_list reports exactly 3 files");

    int a = 0, b = 0, g = 0;
    for (int i = 0; i < n; i++)
    {
        if (strcmp(list[i], "alpha.txt") == 0) a++;
        if (strcmp(list[i], "beta.txt")  == 0) b++;
        if (strcmp(list[i], "gamma.txt") == 0) g++;
    }
    CHECK(a == 1 && b == 1 && g == 1, "each created name appears exactly once");

    fs_unmount();
}

/* (2) Writing files of different sizes (0 .. 48 KB) + content verify + partial read. */
static void test_writing_different_sizes(void)
{
    printf("\n-- Writing files of different sizes --\n");
    extra_fresh_disk();

    int sizes[] = { 0, 1, 100, BLOCK_SIZE, BLOCK_SIZE + 1, MAX_FILE_BYTES };
    int nsizes = (int)(sizeof(sizes) / sizeof(sizes[0]));
    char name[MAX_FILENAME];

    for (int s = 0; s < nsizes; s++)
    {
        int sz = sizes[s];
        snprintf(name, MAX_FILENAME, "sz_%d", sz);

        CHECK(fs_create(name) == 0, "create file for size case");
        extra_fill(extra_wbuf, sz, sz);
        CHECK(fs_write(name, extra_wbuf, sz) == 0, "fs_write returns 0");

        memset(extra_rbuf, 0, BIGBUF);
        int r = fs_read(name, extra_rbuf, BIGBUF);
        CHECK(r == sz, "fs_read returns the exact file size");
        CHECK(sz == 0 || extra_check(extra_rbuf, sz, sz), "content matches what was written");
    }

    /* Partial read: 1000-byte file read into a 400-byte buffer -> 400 bytes. */
    CHECK(fs_create("partial") == 0, "create partial-read file");
    extra_fill(extra_wbuf, 1000, 7);
    CHECK(fs_write("partial", extra_wbuf, 1000) == 0, "write 1000 bytes");
    memset(extra_rbuf, 0, BIGBUF);
    CHECK(fs_read("partial", extra_rbuf, 400) == 400, "partial read returns min(file,buffer)=400");
    CHECK(extra_check(extra_rbuf, 400, 7), "partial-read content correct");

    fs_unmount();
}

/* (3) Filling the filesystem to capacity: inode table first, then data blocks. */
static void test_filling_to_capacity(void)
{
    printf("\n-- Filling the filesystem to capacity --\n");
    char name[MAX_FILENAME];

    /* (3a) Inode capacity: 256 files, then the 257th fails with -2. */
    extra_fresh_disk();
    int created = 0;
    for (int i = 0; i < MAX_FILES; i++)
    {
        snprintf(name, MAX_FILENAME, "f%d", i);
        if (fs_create(name) == 0) created++;
    }
    CHECK(created == MAX_FILES, "created MAX_FILES (256) files");
    CHECK(fs_create("one_too_many") == -2, "create beyond inode capacity returns -2");

    char cap_list[MAX_FILES][MAX_FILENAME];
    CHECK(fs_list(cap_list, MAX_FILES) == MAX_FILES, "fs_list reports 256 files");
    fs_unmount();

    /* (3b) Data-block capacity: write 48 KB files until the disk is full (-2). */
    extra_fresh_disk();
    extra_fill(extra_wbuf, MAX_FILE_BYTES, 5);
    int written = 0, rc = 0;
    for (int i = 0; i < MAX_FILES; i++)
    {
        snprintf(name, MAX_FILENAME, "big%d", i);
        if (fs_create(name) != 0) break;            /* should run out of blocks first */
        rc = fs_write(name, extra_wbuf, MAX_FILE_BYTES);
        if (rc == 0) written++;
        else { fs_delete(name); break; }            /* this file got no data; drop it */
    }
    CHECK(rc == -2, "fs_write eventually returns -2 (disk full)");
    CHECK(written > 0, "wrote at least one max-size file before filling up");
    printf("    (filled with %d files of %d KB each)\n", written, MAX_FILE_BYTES / 1024);
    fs_unmount();
}

/* (4) Deleting files and reusing the freed space. */
static void test_deleting_and_reusing(void)
{
    printf("\n-- Deleting files and reusing the space --\n");
    extra_fresh_disk();

    char name[MAX_FILENAME];
    extra_fill(extra_wbuf, MAX_FILE_BYTES, 5);

    /* Fill the disk with 48 KB files. */
    int rc = 0;
    for (int i = 0; i < MAX_FILES; i++)
    {
        snprintf(name, MAX_FILENAME, "big%d", i);
        if (fs_create(name) != 0) break;
        rc = fs_write(name, extra_wbuf, MAX_FILE_BYTES);
        if (rc != 0) { fs_delete(name); break; }
    }
    CHECK(rc == -2, "disk is full after the fill loop");

    /* Delete one file -> frees 12 blocks -> a new 48 KB write must fit again. */
    CHECK(fs_delete("big0") == 0, "delete one file frees its blocks");
    CHECK(fs_create("reuse") == 0, "create a new file after the delete");
    CHECK(fs_write("reuse", extra_wbuf, MAX_FILE_BYTES) == 0, "write reuses the freed blocks");
    memset(extra_rbuf, 0, BIGBUF);
    CHECK(fs_read("reuse", extra_rbuf, BIGBUF) == MAX_FILE_BYTES, "reused file reads back full size");
    CHECK(extra_check(extra_rbuf, MAX_FILE_BYTES, 5), "reused file content is correct");

    /* The deleted file must be gone. */
    CHECK(fs_read("big0", extra_rbuf, BIGBUF) == -1, "deleted file is no longer readable");

    fs_unmount();
}

/* (5) Error conditions: missing file, duplicates, oversized write. */
static void test_error_conditions(void)
{
    printf("\n-- Error conditions --\n");
    extra_fresh_disk();

    CHECK(fs_write("ghost", extra_wbuf, 10) == -1, "write to a missing file returns -1");
    CHECK(fs_read("ghost", extra_rbuf, 10) == -1,  "read from a missing file returns -1");
    CHECK(fs_delete("ghost") == -1,                "delete a missing file returns -1");

    CHECK(fs_create("dup") == 0,  "create dup");
    CHECK(fs_create("dup") == -1, "create duplicate returns -1");

    /* Oversized write (> 48 KB). The exact error code is implementation-defined
     * (-2 out of space, or -3 other) — we only require that it is rejected. */
    CHECK(fs_create("toobig") == 0, "create toobig");
    extra_fill(extra_wbuf, MAX_FILE_BYTES + 1, 9);
    int rc = fs_write("toobig", extra_wbuf, MAX_FILE_BYTES + 1);
    CHECK(rc < 0, "write larger than the 48 KB max is rejected");
    printf("    (oversized write returned %d)\n", rc);

    fs_unmount();
}

/* (bonus) Persistence: data must survive unmount + remount (i.e. be on disk). */
static void test_persistence(void)
{
    printf("\n-- Persistence across unmount + remount (bonus) --\n");
    extra_fresh_disk();

    CHECK(fs_create("keep1") == 0, "create keep1");
    CHECK(fs_create("keep2") == 0, "create keep2");
    extra_fill(extra_wbuf, 5000, 11);
    CHECK(fs_write("keep1", extra_wbuf, 5000) == 0, "write keep1 (5000 bytes)");
    extra_fill(extra_wbuf, 200, 22);
    CHECK(fs_write("keep2", extra_wbuf, 200) == 0, "write keep2 (200 bytes)");

    fs_unmount();  /* must flush metadata to disk */

    CHECK(fs_mount("disk") == 0, "remount the same disk");

    char list[MAX_FILES][MAX_FILENAME];
    CHECK(fs_list(list, MAX_FILES) == 2, "both files survive the remount");

    memset(extra_rbuf, 0, BIGBUF);
    CHECK(fs_read("keep1", extra_rbuf, BIGBUF) == 5000, "keep1 size persisted");
    CHECK(extra_check(extra_rbuf, 5000, 11), "keep1 content persisted");

    memset(extra_rbuf, 0, BIGBUF);
    CHECK(fs_read("keep2", extra_rbuf, BIGBUF) == 200, "keep2 size persisted");
    CHECK(extra_check(extra_rbuf, 200, 22), "keep2 content persisted");

    fs_unmount();
}

/* Run every additional test and print a summary. */
static void run_additional_tests(void)
{
    printf("\n==================== ADDITIONAL TESTS ====================\n");
    test_creating_multiple_files();
    test_writing_different_sizes();
    test_filling_to_capacity();
    test_deleting_and_reusing();
    test_error_conditions();
    test_persistence();
    printf("\n=== Additional tests: %d/%d checks passed ===\n", g_extra_passed, g_extra_run);
}


/*
============ MAIN FUNCTION ============
This is the main function that runs the test.
*/
int main(int argc, char *argv[])
{
    if(NUM_FILES > MAX_ALLOWED_NUM_FILES)
    {
        fprintf(stderr, "NUM_FILES (%d) cannot be greater than %d.\n", NUM_FILES, MAX_ALLOWED_NUM_FILES);
        exit(1);
    }

    create_disk("disk");

    char filenames[NUM_FILES][MAX_FILENAME];
    init_filenames(filenames);

    if(fs_mount("disk") < 0)
    {
        fprintf(stderr, "Test failed: call to fs_mount failed.\n");
        exit(1);
    }

    // init buffers
    char contents[NUM_FILES][SIZE_OF_FILE];
    for(int i = 0; i < NUM_FILES; i++)
    {
        // intialize `contents[i]`
        for(int j = 0; j < SIZE_OF_FILE; j++)
        {
            contents[i][j] = '0' + i % NUM_FILES;
        }
        contents[i][SIZE_OF_FILE - 1] = '\0'; // null-terminate the string
    }

    for(int i = 0; i < NUM_FILES; i++)
    {
        int retval;

        if((retval = fs_create(filenames[i])) < 0)
        {
            fprintf(stderr, "Test failed: fs_create failed (%d) for file %s\n", retval, filenames[i]);
            exit(1);
        }

        if((retval = fs_write(filenames[i], contents[i], SIZE_OF_FILE)) < 0)
        {
            fprintf(stderr, "Test failed: fs_write failed (%d) for file %s\n", retval, filenames[i]);
            exit(1);
        }
    }
    printf("All files written successfully.\n");

    check_files_are_created(filenames);

    char buff[SIZE_OF_FILE];
    for(int i = 0; i < NUM_FILES; i++)
    {
        int retval = fs_read(filenames[i], buff, SIZE_OF_FILE);
        if(retval < 0)
        {
            fprintf(stderr, "Test failed: fs_read failed (%d) to read file %s\n", retval, filenames[i]);
            exit(1);
        }
        if(retval != SIZE_OF_FILE)
        {
            fprintf(stderr, "Test failed: fs_read did not read the expected number of bytes for file %s (expected %d, got %d)\n", filenames[i], SIZE_OF_FILE, retval);
            exit(1);
        }
        if(memcmp(buff, contents[i], SIZE_OF_FILE) != 0)
        {
            fprintf(stderr, "Test failed for filename %s\n", filenames[i]);
            printf("Expected: %s\n", contents[i]);
            printf("Got: %s\n", buff);
            exit(1);
        }
    }
    
    printf("All files read successfully.\n");
    printf("Success!\n");

    fs_unmount();

    /* ---- Extended coverage required by the assignment ---- */
    run_additional_tests();

    if (g_extra_failed != 0)
    {
        printf("\n%d additional check(s) FAILED.\n", g_extra_failed);
        return 1;
    }

    return 0;
}