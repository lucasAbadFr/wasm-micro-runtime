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
#define FOLDER_PATH LFS_MONTPOINT "/folder"

#define CASE_FILE 0

int bench_fs_mkdir() {
    int rc;

    bench_start(); 
    rc = mkdir(FOLDER_PATH, 0777);
    bench_end();
    printf("[fs_mkdir] mkdir returned %d\n", rc);

    rc = unlink(FOLDER_PATH);

    return 0;
}
