/*
 *
 * @file tftp_server.h
 *
 * @author   Logan Gunthorpe <logang@deltatee.com>
 *
 * @brief    Trivial File Transfer Protocol (RFC 1350)
 *
 * Copyright (c) Deltatee Enterprises Ltd. 2013
 * All rights reserved.
 *
*/


#ifndef LWIP_HDR_APPS_TFTP_SERVER_H
#define LWIP_HDR_APPS_TFTP_SERVER_H

#include "lwip/apps/tftp_opts.h"
#include "lwip/err.h"
#include "lwip/pbuf.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @ingroup tftp
 * TFTP context containing callback functions for TFTP transfers
 */
struct tftp_context
{
  /**
   * Open file for read/write.
   * @param fname Filename
   * @param mode Mode string from TFTP RFC 1350 (netascii, octet, mail)
   * @param write Flag indicating read (0) or write (!= 0) access
   * @returns File handle supplied to other functions
   */
	void *(*open)(void *handle, const char* fname, const char* mode, char write);

  /**
   * Close file handle
   * @param handle File handle returned by open()
   */
	void (*close)(void* handle);
  /**
   * Read from file 
   * @param handle File handle returned by open()
   * @param buf Target buffer to copy read data to
   * @param bytes Number of bytes to copy to buf
   * @returns &gt;= 0: Success; &lt; 0: Error
   */
	int (*read)(void* handle, void* buf, int bytes);
  /**
   * Write to file
   * @param handle File handle returned by open()
   * @param pbuf PBUF adjusted such that payload pointer points
   *             to the beginning of write data. In other words,
   *             TFTP headers are stripped off.
   * @returns &gt;= 0: Success; &lt; 0: Error
   */
	int (*write)(void* handle, struct pbuf* p);

  	void		*priv;
};

err_t tftp_init(const struct tftp_context* ctx);

#define	MUX_TFTP_IMAGE_OS_NAME			"muxOs"	
#define	MUX_TFTP_IMAGE_FPGA_NAME		"muxFpga"	

#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_APPS_TFTP_SERVER_H */
