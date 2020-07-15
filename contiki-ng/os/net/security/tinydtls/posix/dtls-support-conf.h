#ifndef DTLS_SUPPORT_CONF_H_
#define DTLS_SUPPORT_CONF_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

typedef struct {
  socklen_t size;		/**< size of addr */
  union {
    struct sockaddr     sa;
    struct sockaddr_storage st;
    struct sockaddr_in  sin;
    struct sockaddr_in6 sin6;
  } addr;
  uint8_t ifindex;
} session_t;

#define DTLS_TICKS_PER_SECOND 1000

typedef uint64_t dtls_tick_t;

#define LOG_CONF_OUTPUT_PREFIX(level, level_str, module) \
  dtls_support_log_prefix(level, level_str, module)

void dtls_support_log_prefix(int level, const char *level_str, const char *module);

#endif /* DTLS_SUPPORT_CONF_H_ */
