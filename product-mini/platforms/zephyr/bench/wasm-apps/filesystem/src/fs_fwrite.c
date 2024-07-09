#include <stdio.h>
#include <stdlib.h>
#include "../../benchmark.h"
#include <unistd.h> // unlink
#ifdef __wasi__
#else /* zephyr */
	#include "zephyr_interface.h"
#endif /* __wasi__ */
#include "text.h"
#define LFS_MONTPOINT "/lfs"
#define FILE_PATH LFS_MONTPOINT "/test.txt"


int bench_fs_fwrite() {
    FILE *file;
    int rc = 0;

    file = fopen(FILE_PATH, "w+");
    printf("[fs_write] fopen returned %p\n", file);

    bench_start();
    rc = fwrite(text_1024, sizeof(char), 1024, file);
    bench_end();
    printf("[fs_write] fwrite returned %d\n", rc);

    fclose(file);
    unlink(FILE_PATH);

    return 0;
}
