#include <stdio.h>
#include <stdlib.h>
#include "../../benchmark.h"
#ifdef __wasi__
	#include <sys/socket.h>
	#include <arpa/inet.h>
    #include <unistd.h> // For close()
	#include "../../inc/wasi_socket_ext.h"
#else /* zephyr */
	#include "zephyr_interface.h"
#endif /* __wasi__ */

int bench_socket_bind() {
	int st, sock;
	struct sockaddr_in addr;
	int rc = 0;
    
    // IP address 192.0.2.1
	addr.sin_family = AF_INET; //AF_INET;
#ifdef __wasi__
    addr.sin_port = htons(8000);
#else
    addr.sin_port = htons(12345);
#endif
	addr.sin_addr.s_addr = htonl(3221225985); 

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    printf("[socket_bind] sock value: %d\n", sock);

	bench_start();
	rc = bind(sock, (struct sockaddr*)&addr, sizeof(addr));
	bench_end();

    printf("[socket_bind] bind value: %d\n", rc);

    return 0;
}