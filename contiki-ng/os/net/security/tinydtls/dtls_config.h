
/* Define if building universal (internal helper macro) */
/* #undef AC_APPLE_UNIVERSAL_BUILD */

/* Define to 1 if building with ECC support. */
#define DTLS_ECC 1

/* Define to 1 if building with PSK support */
#define DTLS_PSK 1

/* Define to 1 if you have the <arpa/inet.h> header file. */
/* #undef HAVE_ARPA_INET_H */

/* Define to 1 if you have the <assert.h> header file. */
#define HAVE_ASSERT_H 1

/* Define to 1 if you have the <fcntl.h> header file. */
/* #undef HAVE_FCNTL_H */

/* Define to 1 if you have the `fls' function. */
#define HAVE_FLS 1

/* Define to 1 if you have the <inttypes.h> header file. */
/* #undef HAVE_INTTYPES_H */

/* Define to 1 if your system has a GNU libc compatible `malloc' function, and
   to 0 otherwise. */
/* #undef HAVE_MALLOC */

/* Define to 1 if you have the <memory.h> header file. */
/* #undef HAVE_MEMORY_H */

/* Define to 1 if you have the `memset' function. */
/* #undef HAVE_MEMSET */

/* Define to 1 if you have the <netdb.h> header file. */
/* #undef HAVE_NETDB_H */

/* Define to 1 if you have the <netinet/in.h> header file. */
/* #undef HAVE_NETINET_IN_H */

/* Define to 1 if you have the `select' function. */
/* #undef HAVE_SELECT */

/* Define to 1 if struct sockaddr_in6 has a member sin6_len. */
/* #undef HAVE_SOCKADDR_IN6_SIN6_LEN */

/* Define to 1 if you have the `socket' function. */
/* #undef HAVE_SOCKET */

/* Define to 1 if you have the <stddef.h> header file. */
/* #undef HAVE_STDDEF_H */

/* Define to 1 if you have the <stdint.h> header file. */
#ifndef HAVE_STDINT_H
#define HAVE_STDINT_H 1
#endif /* HAVE_STDINT_H */

/* Define to 1 if you have the <stdlib.h> header file. */
/* #undef HAVE_STDLIB_H */

/* Define to 1 if you have the `strdup' function. */
/* #undef HAVE_STRDUP */

/* Define to 1 if you have the `strerror' function. */
/* #undef HAVE_STRERROR */

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the `strnlen' function. */
#define HAVE_STRNLEN 1

/* Define to 1 if you have the <sys/param.h> header file. */
/* #undef HAVE_SYS_PARAM_H */

/* Define to 1 if you have the <sys/socket.h> header file. */
/* #undef HAVE_SYS_SOCKET_H */

/* Define to 1 if you have the <sys/stat.h> header file. */
/* #undef HAVE_SYS_STAT_H */

/* Define to 1 if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H

/* Define to 1 if you have the <sys/types.h> header file. */
/* #undef HAVE_SYS_TYPES_H */

/* Define to 1 if you have the <time.h> header file. */
#define HAVE_TIME_H 1

/* Define to 1 if you have the <unistd.h> header file. */
/* #undef HAVE_UNISTD_H */

/* Define to 1 if you have the `vprintf' function. */
#define HAVE_VPRINTF 1

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME "tinydtls"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "tinydtls 0.8.6"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "tinydtls"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "0.8.6"

/* Define to 1 if you have the ANSI C header files. */
/* #undef STDC_HEADERS */

#define SHA2_USE_INTTYPES_H 1

/* global constants for constrained devices */
#ifndef DTLS_PEER_MAX
/** The maximum number DTLS peers (i.e. sessions). */
#define DTLS_PEER_MAX 1
#endif

#ifndef DTLS_HANDSHAKE_MAX
/** The maximum number of concurrent DTLS handshakes. */
#define DTLS_HANDSHAKE_MAX 1
#endif

#ifndef DTLS_SECURITY_MAX
/** The maximum number of concurrently used cipher keys */
#define DTLS_SECURITY_MAX (DTLS_PEER_MAX + DTLS_HANDSHAKE_MAX)
#endif

#ifndef DTLS_HASH_MAX
/** The maximum number of hash functions that can be used in parallel. */
#define DTLS_HASH_MAX (3 * DTLS_PEER_MAX)
#endif

/** Defined to 1 if tinydtls is built with support for ECC */
#define DTLS_ECC 1

/** Defined to 1 if tinydtls is built with support for PSK */
#define DTLS_PSK 1

/** do not use uthash hash tables */
#define DTLS_PEERS_NOHASH 1

#define DTLS_CHECK_CONTENTTYPE 1
