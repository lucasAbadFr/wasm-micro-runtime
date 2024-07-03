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

int bench_socket_connect() {
	int st, sock;
	struct sockaddr_in addr;
	int rc = 0;
    
    // IP address 192.0.2.10 
	addr.sin_family = AF_INET; //AF_INET;
	addr.sin_port = htons(8000);
	addr.sin_addr.s_addr = htonl(3221225994); 

	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    printf("[socket_connect] sock value: %d\n", sock);

	bench_start();
	rc = connect(sock, (struct sockaddr*)&addr, sizeof(addr));
	bench_end();

    printf("[socket_connect] connect value: %d\n", rc);
	close(sock);

    return 0;
}