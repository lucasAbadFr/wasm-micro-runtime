#ifndef ZEPHYR_INTERFACE_H
#define ZEPHYR_INTERFACE_H

/* Provide interfaces and types to link at runtime  
 * The definitions are taken from: 
 *  <zephyr/net/net-ip.h> 
 *  <zephyr/net/socket.h>
 * Which can't be included due to missing Kconfig dependencies.
 */

#define AF_INET 1 // On zephyr 
#define SOCK_STREAM 1 
#define IPPROTO_TCP 6

#define BSWAP_16(x) ((uint16_t) ((((x) >> 8) & 0xff) | (((x) & 0xff) << 8)))
#define BSWAP_32(x) ((uint32_t) ((((x) >> 24) & 0xff) | \
				   (((x) >> 8) & 0xff00) | \
				   (((x) & 0xff00) << 8) | \
				   (((x) & 0xff) << 24)))

#define sys_cpu_to_be16(val) BSWAP_16(val)
#define sys_cpu_to_be32(val) BSWAP_32(val)

#define htons(x) sys_cpu_to_be16(x)
#define htonl(x) sys_cpu_to_be32(x)

typedef size_t socklen_t;
typedef unsigned short int sa_family_t;

/* Use interfaces to compile */
struct in_addr {
    union {
        uint8_t s4_addr[4];    /**< IPv4 address buffer */
        uint16_t s4_addr16[2]; /**< In big endian */
        uint32_t s4_addr32[1]; /**< In big endian */
        uint32_t s_addr; /**< In big endian, for POSIX compatibility. */
    };
};

/** IPv6 address struct */
struct in6_addr {
    union {
        uint8_t s6_addr[16];   /**< IPv6 address buffer */
        uint16_t s6_addr16[8]; /**< In big endian */
        uint32_t s6_addr32[4]; /**< In big endian */
    };
};

/** Socket address struct for IPv4. */
struct sockaddr_in {
    sa_family_t		sin_family;    /**< AF_INET      */
    uint16_t		sin_port;      /**< Port number  */
    struct in_addr		sin_addr;      /**< IPv4 address */
};

/** Socket address struct for IPv6. */
struct sockaddr_in6 {
    sa_family_t		sin6_family;   /**< AF_INET6               */
    uint16_t		sin6_port;     /**< Port number            */
    struct in6_addr		sin6_addr;     /**< IPv6 address           */
    uint8_t			sin6_scope_id; /**< Interfaces for a scope */
};

#define NET_SOCKADDR_MAX_SIZE (sizeof(struct sockaddr_in6))

struct sockaddr {
    sa_family_t sa_family; /**< Address family */
/** @cond INTERNAL_HIDDEN */
    char data[NET_SOCKADDR_MAX_SIZE - sizeof(sa_family_t)];
/** @endcond */
};


int socket(int family, int type, int proto);
int connect(int sock, const struct sockaddr *addr, socklen_t addrlen);
int bind(int sock, const struct sockaddr *addr, socklen_t addrlen);
ssize_t sendto(int sock, const void *buf, size_t len, int flags,
			     const struct sockaddr *dest_addr,
			     socklen_t addrlen);
ssize_t recvfrom(int sock, void *buf, size_t max_len, int flags,
                   struct sockaddr *src_addr,
                   socklen_t *addrlen);
int close(int sock);

#endif /* ZEPHYR_INTERFACE_H */