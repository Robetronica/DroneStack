/**
 * mbedtls user config overrides for bare-metal STM32 target.
 * Included at the end of mbedtls/config.h via MBEDTLS_USER_CONFIG_FILE.
 */

#ifndef MBEDTLS_USER_CONFIG_H
#define MBEDTLS_USER_CONFIG_H

/* Disable platform entropy — no /dev/urandom on bare-metal */
#define MBEDTLS_NO_PLATFORM_ENTROPY

/* Disable BSD sockets networking module — not available on bare-metal */
#undef MBEDTLS_NET_C

/* Disable timing module — requires Unix/Windows system calls */
#undef MBEDTLS_TIMING_C

/* Disable filesystem I/O — no dirent.h / fopen on bare-metal */
#undef MBEDTLS_FS_IO

/* Disable PSA ITS file storage — depends on filesystem */
#undef MBEDTLS_PSA_ITS_FILE_C

/* Disable PSA crypto storage — psa/error.h not available in this mbedtls version */
#undef MBEDTLS_PSA_CRYPTO_STORAGE_C

#endif /* MBEDTLS_USER_CONFIG_H */
