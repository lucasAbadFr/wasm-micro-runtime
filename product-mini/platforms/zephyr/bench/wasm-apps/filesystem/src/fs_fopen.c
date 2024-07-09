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

int bench_fs_fopen() {
    FILE *file;

    bench_start();
    file = fopen(FILE_PATH, "w+");
    bench_end();

    printf("[fs_fopen] fopen returned %p\n", file);
    fclose(file);
    unlink(FILE_PATH);

    return 0;
}
