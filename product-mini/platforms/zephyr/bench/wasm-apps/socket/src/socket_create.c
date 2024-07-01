#include <stdio.h>
#include <stdlib.h>
#include "../../benchmark.h"
#ifdef __wasi__
#include <sys/socket.h>
#include <arpa/inet.h>
#include "../../inc/wasi_socket_ext.h"
#else
#include  <zephyr/net/socket.h>
#endif /* __wasi__ */

int bench_socket_creation() {
    int sock;

    bench_start(); 
    sock = socket(AF_INET, SOCK_STREAM, 0);
    bench_end();

    printf("Socket value: %d\n", sock);
    return 0;
}