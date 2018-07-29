/**
 * \file
 *
 * \brief Include file for tftp client
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
#ifndef TFTP_CLIENT_INTERFACE_H
#define TFTP_CLIENT_INTERFACE_H

#include <asf.h>

#define TFTP_OPCODE_RRQ					1
#define TFTP_OPCODE_WRQ					2
#define TFTP_OPCODE_DATA				3
#define TFTP_OPCODE_ACK					4
#define TFTP_OPCODE_ERROR				5
#define TFTP_OPCODE_OACK				6

#define TFTP_PAY_LOAD_HDR_SIZE			4

#define NUMBER_OF_ELEMENTS(x)			sizeof(x)/sizeof(x[0])

typedef void (*pf_tftp_options_handler)(char*, U8);
typedef U8 (*pf_packet_handler)(U8*, U16);
typedef U8 (*pf_last_packet_handler)(void);
typedef void (*pf_no_response_timeout_handler)(void);

typedef struct
{
	U32 u32_file_size;
	U16 u16_block_size;
	U8 u8_timeout;
}tftp_client_options;

typedef struct  
{
	char const* pc_option_param;
	pf_tftp_options_handler pf_option_handler;
}tftp_options;

void tftp_client_terminate_connection(void);
void tftp_client_connect_to_server(void);
void tftp_client_register_received_packet_handler(pf_packet_handler pf_recvd_handler);
void tftp_client_register_received_last_packet_handler(pf_last_packet_handler pf_recvd_handler);
void tftp_client_register_server_no_response_handler(pf_no_response_timeout_handler pf_recvd_handler);
void tftp_client_register_requesting_file_info(const char* pc_file_name);
U8 tftp_client_prepare_options_packet(U8* tftp_options, char* pu8_file_name, U32 u32_file_size);


#endif	/* TFTP_CLIENT_INTERFACE_H */

