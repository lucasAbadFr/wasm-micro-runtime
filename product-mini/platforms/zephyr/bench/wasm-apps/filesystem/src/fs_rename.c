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
#define NEW_FOLDER_PATH LFS_MONTPOINT "/new_folder"

#define CASE_FILE 0

int bench_fs_rename() {
    int rc;

    
    rc = mkdir(FOLDER_PATH, 0777);
    printf("[fs_rename] mkdir returned %d\n", rc);

    bench_start(); 
    rc = rename(FOLDER_PATH, NEW_FOLDER_PATH);
    bench_end();
    printf("[fs_rename] rename returned %d\n", rc);

    // DIR *dir = opendir(NEW_FOLDER_PATH);
    // printf("[fs_rename] opendir returned %p\n", dir);
    rc < 0 ? unlink(FOLDER_PATH) 
           : unlink(NEW_FOLDER_PATH);
    

    return 0;
}
