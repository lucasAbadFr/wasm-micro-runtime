#include <stdio.h>
#include <stdlib.h>
#include "../../benchmark.h"
#include <unistd.h> // unlink
#ifdef __wasi__
	#include <sys/stat.h>
#else /* zephyr */
	#include "zephyr_interface.h"
#endif /* __wasi__ */

#define LFS_MONTPOINT "/lfs"
#define FILE_PATH LFS_MONTPOINT "/test.txt"
#define FOLDER_PATH LFS_MONTPOINT "/folder"

#define CASE_FILE 0

int bench_fs_unlink() {
    FILE *file;
    int rc;
#if CASE_FILE
    file = fopen(FILE_PATH, "w+");
    printf("[fs_unlink] fopen returned %p\n", file);

    rc = fclose(file);
    printf("[fs_unlink] fclose returned %d\n", rc);
#else
    rc = mkdir(FOLDER_PATH, 0777);
    printf("[fs_unlink] mkdir returned %d\n", rc);
#endif

    bench_start(); 
#if CASE_FILE 
    rc = unlink(FILE_PATH);
#else
    rc = unlink(FOLDER_PATH);
#endif
    bench_end();
    printf("[fs_unlink] unlink returned %d\n", rc);

    return 0;
}
