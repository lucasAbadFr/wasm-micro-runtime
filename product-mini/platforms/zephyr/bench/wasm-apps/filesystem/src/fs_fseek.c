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


int bench_fs_fseek() {
    FILE *file;
    int rc = 0;
    char buffer[256];

    file = fopen(FILE_PATH, "w+");
    printf("[fs_seek] fopen returned %p\n", file);

    rc = fwrite(text_256, sizeof(char), 256, file);
    printf("[fs_seek] fwrite returned %d\n", rc);

    bench_start();
    rc = fseek(file, 0, SEEK_CUR);
    bench_end();
    printf("[fs_seek] fseek returned %d\n", rc);

    fclose(file);
    unlink(FILE_PATH);

    return 0;
}
