#ifndef ZEPHYR_INTERFACE_H
#define ZEPHYR_INTERFACE_H

/* Provide interfaces and types to link at runtime  
 * The definitions are taken from: 
 *  <zephyr/net/net-ip.h> 
 *  <zephyr/net/socket.h>
 * Which can't be included due to missing Kconfig dependencies.
 */

#define BSWAP_16(x) ((uint16_t) ((((x) >> 8) & 0xff) | (((x) & 0xff) << 8)))
#define BSWAP_32(x) ((uint32_t) ((((x) >> 24) & 0xff) | \
				   (((x) >> 8) & 0xff00) | \
				   (((x) & 0xff00) << 8) | \
				   (((x) & 0xff) << 24)))

#define sys_cpu_to_be16(val) BSWAP_16(val)
#define sys_cpu_to_be32(val) BSWAP_32(val)

#define htons(x) sys_cpu_to_be16(x)
#define htonl(x) sys_cpu_to_be32(x)

/* From Zephyr: lib/libc/minimal/include/stdio.h */
// typedef int FILE;

FILE *fopen(const char *pathname, const char *mode);
size_t fwrite(const void *ptr, size_t size, size_t nitems,
           FILE *stream);
size_t fread(void *ptr, size_t size, size_t nmemb,
                    FILE *stream);
int fseek(FILE *stream, long offset, int whence);
int unlink(const char *pathname);
int mkdir(const char *pathname, mode_t mode);


#endif /* ZEPHYR_INTERFACE_H */