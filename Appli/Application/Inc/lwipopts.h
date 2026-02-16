/**
 ******************************************************************************
 * @file    lwipopts.h
 * @brief   lwIP 2.1.2 configuration for STM32N657 + CYW43439 WiFi (FreeRTOS)
 ******************************************************************************
 */

#ifndef LWIPOPTS_H
#define LWIPOPTS_H

/* ---------- errno ---------- */
#define LWIP_ERRNO_STDINCLUDE           1       /* Use picolibc's <errno.h> */

/* ---------- RNG ---------- */
#include <stdlib.h>
#define LWIP_RAND()                     ((u32_t)rand())

/* ---------- Platform / OS ---------- */
#define NO_SYS                          0       /* Use FreeRTOS (OS mode) */
#define LWIP_FREERTOS_CHECK_CORE_LOCKING 1
#define LWIP_TCPIP_CORE_LOCKING         1
#define SYS_LIGHTWEIGHT_PROT            1
#define LWIP_COMPAT_MUTEX               0       /* Use real mutexes from sys_arch */

/* ---------- Thread config ---------- */
#define TCPIP_THREAD_NAME               "tcpip"
#define TCPIP_THREAD_STACKSIZE          (4 * 1024)
#define TCPIP_THREAD_PRIO               32      /* osPriorityAboveNormal */
#define DEFAULT_THREAD_STACKSIZE        (2 * 1024)
#define DEFAULT_THREAD_PRIO             24      /* osPriorityNormal */
#define TCPIP_MBOX_SIZE                 16
#define DEFAULT_TCP_RECVMBOX_SIZE       16
#define DEFAULT_UDP_RECVMBOX_SIZE       16
#define DEFAULT_ACCEPTMBOX_SIZE         8
#define DEFAULT_RAW_RECVMBOX_SIZE       16

/* ---------- Memory ---------- */
#define MEM_ALIGNMENT                   4
#define MEM_SIZE                        (32 * 1024)
#define MEMP_NUM_PBUF                   24
#define MEMP_NUM_UDP_PCB                8
#define MEMP_NUM_TCP_PCB                8
#define MEMP_NUM_TCP_PCB_LISTEN         4
#define MEMP_NUM_TCP_SEG                32
#define MEMP_NUM_NETBUF                 8
#define MEMP_NUM_NETCONN                12
#define MEMP_NUM_SYS_TIMEOUT            16
#define PBUF_POOL_SIZE                  24
#define PBUF_POOL_BUFSIZE               1536    /* Full Ethernet MTU + headers */

/* ---------- IPv4 ---------- */
#define LWIP_IPV4                       1
#define LWIP_IPV6                       0

/* ---------- DHCP ---------- */
#define LWIP_DHCP                       1
#define LWIP_AUTOIP                     0
#define LWIP_DHCP_CHECK_LINK_UP         1

/* ---------- DNS ---------- */
#define LWIP_DNS                        1
#define DNS_MAX_SERVERS                 2

/* ---------- IGMP ---------- */
#define LWIP_IGMP                       1

/* ---------- UDP ---------- */
#define LWIP_UDP                        1

/* ---------- TCP ---------- */
#define LWIP_TCP                        1
#define TCP_MSS                         1460
#define TCP_WND                         (4 * TCP_MSS)
#define TCP_SND_BUF                     (4 * TCP_MSS)
#define TCP_SND_QUEUELEN                (2 * TCP_SND_BUF / TCP_MSS)
#define TCP_QUEUE_OOSEQ                 1
#define LWIP_TCP_KEEPALIVE              1
#define LWIP_TCP_TIMESTAMPS             0

/* ---------- Network interface ---------- */
#define LWIP_NETIF_STATUS_CALLBACK      1
#define LWIP_NETIF_LINK_CALLBACK        1
#define LWIP_NETIF_HOSTNAME             1
#define LWIP_NETIF_API                  1
#define LWIP_NETIF_REMOVE_CALLBACK      1

/* ---------- Socket API ---------- */
#define LWIP_SOCKET                     1
#define LWIP_NETCONN                    1
#define LWIP_SO_RCVTIMEO                1
#define LWIP_SO_SNDTIMEO                1
#define SO_REUSE                        1
#define LWIP_NETCONN_SEM_PER_THREAD     1

/* ---------- ICMP ---------- */
#define LWIP_ICMP                       1
#define LWIP_RAW                        1

/* ---------- Stats ---------- */
#define LWIP_STATS                      0
#define LWIP_STATS_DISPLAY              0

/* ---------- Checksum ---------- */
/* No hardware checksum offload — use software checksums (defaults) */

/* ---------- Debug ---------- */
#define LWIP_DBG_MIN_LEVEL              LWIP_DBG_LEVEL_ALL
#define LWIP_DBG_TYPES_ON               LWIP_DBG_OFF

/* Uncomment individual modules to enable debug output:
#define ETHARP_DEBUG                    LWIP_DBG_ON
#define NETIF_DEBUG                     LWIP_DBG_ON
#define PBUF_DEBUG                      LWIP_DBG_ON
#define API_LIB_DEBUG                   LWIP_DBG_ON
#define API_MSG_DEBUG                   LWIP_DBG_ON
#define SOCKETS_DEBUG                   LWIP_DBG_ON
#define ICMP_DEBUG                      LWIP_DBG_ON
#define INET_DEBUG                      LWIP_DBG_ON
#define IP_DEBUG                        LWIP_DBG_ON
#define IP_REASS_DEBUG                  LWIP_DBG_ON
#define MEM_DEBUG                       LWIP_DBG_ON
#define MEMP_DEBUG                      LWIP_DBG_ON
#define SYS_DEBUG                       LWIP_DBG_ON
#define TCP_DEBUG                       LWIP_DBG_ON
#define TCP_INPUT_DEBUG                 LWIP_DBG_ON
#define TCP_OUTPUT_DEBUG                LWIP_DBG_ON
#define TCP_RTO_DEBUG                   LWIP_DBG_ON
#define TCP_CWND_DEBUG                  LWIP_DBG_ON
#define TCP_WND_DEBUG                   LWIP_DBG_ON
#define TCP_FR_DEBUG                    LWIP_DBG_ON
#define TCP_QLEN_DEBUG                  LWIP_DBG_ON
#define TCP_RST_DEBUG                   LWIP_DBG_ON
#define UDP_DEBUG                       LWIP_DBG_ON
#define TCPIP_DEBUG                     LWIP_DBG_ON
#define DHCP_DEBUG                      LWIP_DBG_ON
#define DNS_DEBUG                       LWIP_DBG_ON
*/

#endif /* LWIPOPTS_H */
