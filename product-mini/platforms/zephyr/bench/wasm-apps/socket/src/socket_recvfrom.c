#include <stdio.h>
#include <stdlib.h>
#include "../../benchmark.h"
#ifdef __wasi__
    #include <sys/socket.h>
    #include <arpa/inet.h>
    #include "../../inc/wasi_socket_ext.h"
#else
    #include "zephyr_interface.h"
#endif /* __wasi__ */

/* Ensure that were you open the HTTP server there is a hello.txt file */
#define REQUEST "GET /hello.txt HTTP/1.0\r\nHost: 192.0.2.10\r\n\r\n"
#define REQUEST_SIZE 47

int bench_socket_recvfrom() {
    int st, sock;
	struct sockaddr_in addr;
	int rc = 0;
    char response[1024];
    socklen_t socklen = sizeof(addr);
    
    // IP address 192.0.2.10 
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8000);
	addr.sin_addr.s_addr = htonl(3221225994); 

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    printf("[socket_recvfrom] sock value: %d\n", sock);

	rc = connect(sock, (struct sockaddr*)&addr, sizeof(addr));
    printf("[socket_recvfrom] connect value: %d\n", rc);

    rc = sendto(sock, (const void *)REQUEST, REQUEST_SIZE, 0,
                      (struct sockaddr *)&addr, sizeof(addr));
    printf("[socket_recvfrom] sendto value: %d\n", rc);
    
    bench_start();
    rc = recvfrom(sock, (void *)response, sizeof(response) - 1, 0,
                      (struct sockaddr *)&addr, &socklen);
    bench_end();
    printf("[socket_recvfrom] recvfrom value: %d\n", rc);
    
    return 0;
}