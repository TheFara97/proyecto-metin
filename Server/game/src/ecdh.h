/*
 * Interface of curve P-256 (ECDH and ECDSA)
 *
 * Author: Manuel Pégourié-Gonnard.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef P256_M_H
#define P256_M_H

#include <stdint.h>
#include <stddef.h>

 /* Status codes */
#define P256_SUCCESS            0
#define P256_RANDOM_FAILED      -1
#define P256_INVALID_PUBKEY     -2
#define P256_INVALID_PRIVKEY    -3
#define P256_INVALID_SIGNATURE  -4

#ifdef __cplusplus
extern "C" {
#endif

    /*
     * ECDH compute shared secret
     *
     * [out] secret: on success, holds the shared secret, as a big-endian integer
     * [in] priv: our private key as a big-endian integer
     * [in] pub: the peer's public key, as two big-endian integers
     *
     * return:  P256_SUCCESS on success
     *          P256_INVALID_PRIVKEY if priv is invalid
     *          P256_INVALID_PUBKEY if pub is invalid
     */
    int p256_ecdh_shared_secret(uint8_t secret[32],
        const uint8_t priv[32], const uint8_t pub[64]);

#ifdef __cplusplus
}
#endif

#endif /* P256_M_H */
