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


int bench_fs_fread() {
    FILE *file;
    int rc = 0;
    char buffer[1024];

    file = fopen(FILE_PATH, "w+");
    printf("[fs_read] fopen returned %p\n", file);

    rc = fwrite(text_1024, sizeof(char), 1024, file);
    printf("[fs_read] fwrite returned %d\n", rc);

    rc = fseek(file, 0, SEEK_SET);
    printf("[fs_read] fseek returned %d\n", rc);

    bench_start();
    rc = fread(buffer, sizeof(char), 1024, file);  
    bench_end();
    printf("[fs_read] fread returned %d\n", rc);

    fclose(file);
    unlink(FILE_PATH);

    return 0;
}
