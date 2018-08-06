/**
 *  RFC 1521 base64 encoding/decoding
 */
#ifndef MBEDTLS_BASE64_H
#define MBEDTLS_BASE64_H

#include <stddef.h>


#define	MD_OUT_LENGTH_MD5			16
#define	MD_OUT_LENGTH_RIPEMD		20
#define	MD_OUT_LENGTH_SHA1			20
#define	MD_OUT_LENGTH_SHA256		32
#define	MD_OUT_LENGTH_SHA512		64

#define	MD_DATA_LENGTH_MD5			64
#define	MD_DATA_LENGTH_RIPEMD		64
#define	MD_DATA_LENGTH_SHA1			64
#define	MD_DATA_LENGTH_SHA256		64
#define	MD_DATA_LENGTH_SHA512		128


#define MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL               -0x002A  /**< Output buffer too small. */
#define MBEDTLS_ERR_BASE64_INVALID_CHARACTER              -0x002C  /**< Invalid character in input. */

#ifdef __cplusplus
extern "C" {
#endif

/**
 *           Encode a buffer into base64 format
 *
 * \param dst      destination buffer
 * \param dlen     size of the destination buffer
 * \param olen     number of bytes written
 * \param src      source buffer
 * \param slen     amount of data to be encoded
 *
 * \return         0 if successful, or MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL.
 *                 *olen is always updated to reflect the amount
 *                 of data that has (or would have) been written.
 *                 If that length cannot be represented, then no data is
 *                 written to the buffer and *olen is set to the maximum
 *                 length representable as a size_t.
 *
 * \note           Call this function with dlen = 0 to obtain the
 *                 required buffer size in *olen
 */
int mbedtls_base64_encode( unsigned char *dst, size_t dlen, size_t *olen,
                   const unsigned char *src, size_t slen );

/**
 *           Decode a base64-formatted buffer
 *
 * \param dst      destination buffer (can be NULL for checking size)
 * \param dlen     size of the destination buffer
 * \param olen     number of bytes written
 * \param src      source buffer
 * \param slen     amount of data to be decoded
 *
 * \return         0 if successful, MBEDTLS_ERR_BASE64_BUFFER_TOO_SMALL, or
 *                 MBEDTLS_ERR_BASE64_INVALID_CHARACTER if the input data is
 *                 not correct. *olen is always updated to reflect the amount
 *                 of data that has (or would have) been written.
 *
 * \note           Call this function with *dst = NULL or dlen = 0 to obtain
 *                 the required buffer size in *olen
 */
int mbedtls_base64_decode( unsigned char *dst, size_t dlen, size_t *olen,
                   const unsigned char *src, size_t slen );


#ifdef __cplusplus
}
#endif

#endif /* base64.h */
