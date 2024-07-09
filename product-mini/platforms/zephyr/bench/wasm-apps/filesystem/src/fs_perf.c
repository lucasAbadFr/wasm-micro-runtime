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

#define ITERATIONS 1000

int bench_fs_perf() {
    FILE *file;
    int rc = 0;
    char buffer[64];

    int open_fail = 0;
    int write_fail = 0;
    int seek_fail = 0;
    int read_fail = 0;

    printf("[fs_perf] %d iterations of open/write/seek/read/close/unlink with size %d\n", ITERATIONS, 64);
    bench_start();
    for(int i = 0; i < ITERATIONS; i++){
        file = fopen(FILE_PATH, "w+");
        if(file == NULL){
            open_fail++;
            continue;
        }
        rc = fwrite(text_64, sizeof(char), 64, file);
        if(rc != 64){
            write_fail++;
            continue;
        }
        rc = fseek(file, 0, SEEK_SET);
        if(rc != 0){
            seek_fail++;
            continue;
        }
        rc = fread(buffer, sizeof(char), 64, file);
        if(rc != 64){
            read_fail++;
            continue;
        } 
        fclose(file);
        unlink(FILE_PATH);
    }
    bench_end();
    printf("[fs_perf] fopen failed %d times\n", open_fail);
    printf("[fs_perf] fwrite failed %d times\n", write_fail);
    printf("[fs_perf] fseek failed %d times\n", seek_fail);
    printf("[fs_perf] fread failed %d times\n", read_fail);

    return 0;
}
