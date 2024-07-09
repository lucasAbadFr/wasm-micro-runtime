#include <stdio.h>
#include <stdlib.h>
#include "../../benchmark.h"
#include <unistd.h> // unlink
#ifdef __wasi__
	
#else /* zephyr */
	#include "zephyr_interface.h"
#endif /* __wasi__ */

#define LFS_MONTPOINT "/lfs"
#define FILE_PATH LFS_MONTPOINT "/test.txt"

int bench_fs_fclose() {
    FILE *file;
    int rc;

    file = fopen(FILE_PATH, "w+");
    printf("[fs_fclose] fopen returned %p\n", file);

    bench_start();    
    rc = fclose(file);
    bench_end();
    printf("[fs_fclose] fclose returned %d\n", rc);

    unlink(FILE_PATH);

    return 0;
}
