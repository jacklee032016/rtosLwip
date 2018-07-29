/**
 *
 * \brief This file contains the functions to take care of tftp client
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
#include <string.h>
#include "udp.h"
#include "tftp_client/tftp_client.h"
#include "config/conf_tftp_client.h" 
#include "config/conf_custom_log.h"
#include "general_use_timers/general_use_timers.h"
#include "config/conf_general_use_timers.h"

static void tftp_client_send_options(void);
static void tftp_client_send_ack(void);
static void tftp_client_recv_handler(void *arg, struct udp_pcb *pcb, struct pbuf *p, struct ip_addr *addr, u16_t port);
static void tftp_client_trigger_udp_send(struct pbuf *p);
static void tftp_client_parse_options_ack(struct pbuf *p);
static void tftp_client_process_data(struct pbuf *p);
static void tftp_client_process_tsize_value(char* pu8_options,  U8 u8_no_of_digits);
static void tftp_client_process_blksize_value(char* pu8_options, U8 u8_no_of_digits);
static void tftp_client_process_timeout_value(char* pu8_options, U8 u8_no_of_digits);
static void tftp_client_close_connection(void);
static void tftp_client_send_error(U16 u16_error_code);
static void tftp_client_handle_server_connection_timeout(void);

/** Global/Static Variables*/
struct udp_pcb *tftp_pcb;

static const tftp_options as_tftp_options[] = 
{
	{"tsize", tftp_client_process_tsize_value},
	{"blksize", tftp_client_process_blksize_value},
	{"timeout", tftp_client_process_timeout_value},
};

static tftp_client_options tftp_client_requested_options;
static tftp_client_options tftp_client_accepted_options;

static pf_packet_handler mpf_received_packet_handler;
static pf_last_packet_handler mpf_received_last_packet_handler;
static pf_no_response_timeout_handler mpf_no_response_handler;
static char* pc_flash_file_name;

U16 u16_block_number;


/**
 * \brief This function connects to server using UDP
 */
void tftp_client_connect_to_server(void)
{
	err_t response;
	ip_addr_t tftp_server_ip;
	
	IP4_ADDR(&tftp_server_ip,
		TFTP_SERVER_IP_ADDRESS0, TFTP_SERVER_IP_ADDRESS1, TFTP_SERVER_IP_ADDRESS2, TFTP_SERVER_IP_ADDRESS3);

	/* create a new UDP PCB structure  */
	tftp_pcb = udp_new();
	if (!tftp_pcb)
	{
		/* Error creating PCB. Out of Memory  */
		CUSTOM_LOG(("Can not create pcb"));
		return;
	}
	
	response = udp_connect(tftp_pcb, &tftp_server_ip, TFTP_SERVER_DEFAULT_PORT);
	if (response == ERR_OK)
	{
		puts("TFTP Client Started.....\r");

		/* Initialize receive callback function  */
		udp_recv(tftp_pcb, tftp_client_recv_handler, NULL);
		
		/* Initiate TFTP Options */
		tftp_client_send_options();

		/* recent state variables */
		u16_block_number=0;
	}
}
/**
 * \brief This function terminates current UDP connection.
 */
void tftp_client_terminate_connection(void)
{
	udp_recv(tftp_pcb, NULL, NULL);
	udp_remove(tftp_pcb);
	stop_general_timer(etimer_server_connection);
	puts(".....TFTP Client Closed\r");
}
/**
 * \brief This function sends options to server
 */
static void tftp_client_send_options(void)
{
	U8 u8_tftp_client_options[100];	/** Space holder to accommodate Client options */
	U8 u8_options_len;

	tftp_client_requested_options.u16_block_size = TFTP_CLIENT_REQUESTING_BLOCK_SIZE;
	tftp_client_requested_options.u8_timeout = TFTP_CLIENT_REQUESTING_TIMEOUT;
	
	u8_options_len = tftp_client_prepare_options_packet(u8_tftp_client_options, pc_flash_file_name, tftp_client_requested_options.u32_file_size);

	struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, u8_options_len, PBUF_RAM);
	memcpy(p->payload, u8_tftp_client_options, u8_options_len);
	tftp_client_trigger_udp_send(p);
}
/**
 * \brief This function initiates udp transfer
 */
static void tftp_client_trigger_udp_send(struct pbuf *p)
{
	err_t response;

	response = udp_send(tftp_pcb, p);
	pbuf_free(p);	
	
	if(response)
	{
		/* Set to close connection */
		tftp_client_terminate_connection();
		CUSTOM_LOG(("Resp:%03d, BlockNo:%04d",response, u16_block_number));
	}
	else
	{
		/* start server connection timer... It expects at least one packet with in time configured timeout else assumes */
		start_general_timer(etimer_server_connection, TFTP_CLIENT_REQUESTING_TIMEOUT*1000, tftp_client_handle_server_connection_timeout, eTimerOneshot);
	}

}
/**
 * \brief This is a call back and gets called on receiving a message on TFTP port
 * \param[in] arg  Pointer to arguments received from udp layer
 * \param[in] pcb  Current UDP pcb pointer
 * \param[in] p    pointer to data received
 * \param[in] addr Source address of the packet
 * \param[in] port Source port of the packet
 */
static void tftp_client_recv_handler(void *arg, struct udp_pcb *pcb, struct pbuf *p, struct ip_addr *addr, u16_t port)
{
	U8* pu8_data;
	
	/* Stop server connection timer as we received response... */
	stop_general_timer(etimer_server_connection);

	/* Update remote port with received port number */
	pcb->remote_port = port;
	
	pu8_data = (U8*)(p->payload);
	CUSTOM_LOG(("Port:%05d, OPC:%03d, Len:%04d", port, pu8_data[1], p->len));
	
	switch(pu8_data[1])
	{
		case TFTP_OPCODE_OACK:
			tftp_client_parse_options_ack(p);
			break;
		
		case TFTP_OPCODE_DATA:
			tftp_client_process_data(p);
			break;

		case TFTP_OPCODE_ERROR:
			CUSTOM_LOG(("Error:%d, %s", (((U16)pu8_data[2]*256) + (U16)pu8_data[3]), &pu8_data[4]));
			tftp_client_close_connection();
			break;
			
		case TFTP_OPCODE_RRQ:
		case TFTP_OPCODE_WRQ:
		case TFTP_OPCODE_ACK:
		default:
			/* ignore unsupported packets */
			CUSTOM_LOG(("Unsupported packet received"));
			tftp_client_close_connection();
			break;
	}
	
	/* Free up pbuf as it is processed */
	pbuf_free(p);
}
/**
 * \brief This function parses options and sends ack on accepting it.
 */
static void tftp_client_parse_options_ack(struct pbuf *p)
{
	char au8_options[50]; /** Space holder to accommodate option acked by Server for further processing */
	U8 u8_options_string_index;
	U8 u8_options_index;
	U16 u16_payload_index;
	U8* pu8_payload = (U8*)p->payload;
	pf_tftp_options_handler pf_tftp_server_options_handler;
	
	pf_tftp_server_options_handler = NULL;
	
	/* Load options with TFTP defaults... these will be used in case server rejects options */
	tftp_client_accepted_options.u16_block_size = TFTP_CLIENT_DEFAULT_BLOCK_SIZE;
	tftp_client_accepted_options.u8_timeout = TFTP_CLIENT_DEFAULT_TIMEOUT;
	
	/* Read options received from server*/
	for(u16_payload_index=2, u8_options_string_index=0; u16_payload_index < p->len; u16_payload_index++)
	{
		/* Copy string... be it be option param or value */
		au8_options[u8_options_string_index++] = pu8_payload[u16_payload_index];
		
		/* Received first string in the options */
		if(pu8_payload[u16_payload_index] == '\0')	
		{
			/* Check if it is a param or value */
			if(pf_tftp_server_options_handler)
			{
				pf_tftp_server_options_handler(au8_options, u8_options_string_index);
				pf_tftp_server_options_handler=NULL;
			}
			else
			{
				/* Get param handler from received params*/
				for(u8_options_index=0; u8_options_index < NUMBER_OF_ELEMENTS(as_tftp_options); u8_options_index++)
				{
					if(!strncmp((char*)au8_options, as_tftp_options[u8_options_index].pc_option_param, u8_options_string_index))
					{
						pf_tftp_server_options_handler = as_tftp_options[u8_options_index].pf_option_handler;
						break;
					}
				}
			}
			u8_options_string_index = 0;
		}
	}
	
	tftp_client_send_ack();
}
/**
 * \brief This function parses data and sends ack on accepting it.
 */
static void tftp_client_process_data(struct pbuf *p)
{
	U8 u8_return_status=0;
	U8* pu8_data = (U8*)p->payload;
	U16 u16_current_packet_block_number;
	
	u16_current_packet_block_number = ((U16)pu8_data[2]*256) + (U16)pu8_data[3];
	
	if(u16_current_packet_block_number > u16_block_number)
	{
		u16_block_number = u16_current_packet_block_number;
	
		/* Check for last data packet */
		if(p->len < (tftp_client_accepted_options.u16_block_size + TFTP_PAY_LOAD_HDR_SIZE))
		{
			if(mpf_received_packet_handler)
				u8_return_status = mpf_received_packet_handler(&pu8_data[4], (U16)(p->len - TFTP_PAY_LOAD_HDR_SIZE));
			
			tftp_client_close_connection();
		}
		else if(p->len == (tftp_client_accepted_options.u16_block_size + TFTP_PAY_LOAD_HDR_SIZE))
		{
			/* Process received data, Received complete packet process it */
			if(mpf_received_packet_handler)
				u8_return_status = mpf_received_packet_handler(&pu8_data[4], (U16)(p->len - TFTP_PAY_LOAD_HDR_SIZE));
		}
	}
	
	if(u8_return_status)
	{
		tftp_client_send_error(u8_return_status);
	}
	else
	{
		/* Send ack to received DATA packet */
		tftp_client_send_ack();
	}
}
/**
 * \brief This function sends ack to server
 */
static void tftp_client_send_ack(void)
{
	U8 u8TFTP_ACK_Packet[] = {0, TFTP_OPCODE_ACK, 0, 0};

	u8TFTP_ACK_Packet[2] = (U8)(u16_block_number >> 8);
	u8TFTP_ACK_Packet[3] = (U8)(u16_block_number);
	
	struct pbuf *p_ack = pbuf_alloc(PBUF_TRANSPORT, sizeof(u8TFTP_ACK_Packet), PBUF_RAM);
	memcpy(p_ack->payload, u8TFTP_ACK_Packet, sizeof(u8TFTP_ACK_Packet));
	tftp_client_trigger_udp_send(p_ack);
}
/**
 * \brief This function sends ack to server
 */
static void tftp_client_send_error(U16 u16_error_code)
{
	U8 u8TFTP_ACK_Packet[] = {0, TFTP_OPCODE_ERROR, 0, 0};

	u8TFTP_ACK_Packet[2] = (U8)(u16_error_code >> 8);
	u8TFTP_ACK_Packet[3] = (U8)(u16_error_code);
	struct pbuf *p_ack = pbuf_alloc(PBUF_TRANSPORT, sizeof(u8TFTP_ACK_Packet), PBUF_RAM);
	memcpy(p_ack->payload, u8TFTP_ACK_Packet, sizeof(u8TFTP_ACK_Packet));
	tftp_client_trigger_udp_send(p_ack);
}
/**
 * \brief This function executes last packet handler and initiates connection closure
 */
static void tftp_client_close_connection(void)
{
	if(mpf_received_last_packet_handler)
		(void)mpf_received_last_packet_handler();

	/* Disconnect */
	tftp_client_terminate_connection();
};
/**
 * \brief This function checks received tsize value
 */
static void tftp_client_process_tsize_value(char* pu8_options, U8 u8_no_of_digits)
{
	/* Record receive file size */
	tftp_client_accepted_options.u32_file_size = atol(pu8_options);
}
/**
 * \brief This function checks received blksize value
 */
static void tftp_client_process_blksize_value(char* pu8_options, U8 u8_no_of_digits)
{
	/* Record receive file size */
	tftp_client_accepted_options.u16_block_size = (U16)atol(pu8_options);
}
/**
 * \brief This function checks received timeout value
 */
static void tftp_client_process_timeout_value(char* pu8_options, U8 u8_no_of_digits)
{
	/* Record receive file size */
	tftp_client_accepted_options.u8_timeout = (U8)atol(pu8_options);
}
/**
 * \brief This function assigns flash call back handler
 */
void tftp_client_register_received_packet_handler(pf_packet_handler pf_recvd_handler)
{
	mpf_received_packet_handler = pf_recvd_handler;
}
/**
 * \brief This function assigns flash last packet handler
 */
void tftp_client_register_received_last_packet_handler(pf_last_packet_handler pf_recvd_handler)
{
	mpf_received_last_packet_handler = pf_recvd_handler;
}
/**
 * \brief This function assigns flash file info
 */
void tftp_client_register_requesting_file_info(const char* pc_file_name)
{
	pc_flash_file_name = (char*)pc_file_name;
}
/**
 * \brief This function assigns flash file info
 */
void tftp_client_register_server_no_response_handler(pf_no_response_timeout_handler pf_recvd_handler)
{
	mpf_no_response_handler = pf_recvd_handler;
}
/**
 * \brief This function prepares options packet to negotiate with Server
 * \param[in] pu8_tftp_options pointer to load client options
 * \param[in] pu8_file_name file name to request from Server
 * \param[in] u32_file_size file size to read from Server
 */
U8 tftp_client_prepare_options_packet(U8* pu8_tftp_options, char* pu8_file_name, U32 u32_file_size)
{
	char const* p_tftp_Mode = "octet";
	char const* p_tftp_tsize = "tsize";
	char const* p_tftp_blksize = "blksize";
	char const* p_tftp_timeout = "timeout";
	U8 a_option_value[10];	/** Space holder to accommodate option value, Maximum of 999999999 supported  */
	char* p_option_value;
	U8* pu8_tftp_options_start_address = pu8_tftp_options;
	
	*pu8_tftp_options++ = 0;
	*pu8_tftp_options++ = TFTP_OPCODE_RRQ;
	
	/* Append tftp file name */
	while(*pu8_file_name != '\0')
	{
		*pu8_tftp_options++ = *pu8_file_name++;
	}
	*pu8_tftp_options++=0;
	
	/* Append tftp mode */
	while(*p_tftp_Mode != '\0')
	{
		*pu8_tftp_options++ = *p_tftp_Mode++;
	}
	*pu8_tftp_options++=0;
	
	
	/* Append block size and value */
	while(*p_tftp_blksize != '\0')
	{
		*pu8_tftp_options++ = *p_tftp_blksize++;
	}
	*pu8_tftp_options++=0;
	p_option_value = (char*)a_option_value;
	utoa(tftp_client_requested_options.u16_block_size, p_option_value, 10);
	while(*p_option_value != '\0')
	{
		*pu8_tftp_options++ = *p_option_value++;
	}
	*pu8_tftp_options++=0;
	

	/* Append timeout and value */
	while(*p_tftp_timeout != '\0')
	{
		*pu8_tftp_options++ = *p_tftp_timeout++;
	}
	*pu8_tftp_options++=0;
	p_option_value = (char*)a_option_value;
	utoa((U16)tftp_client_requested_options.u8_timeout, p_option_value, 10);
	while(*p_option_value != 0x00)
	{
		*pu8_tftp_options++ = *p_option_value++;
	}
	*pu8_tftp_options++=0;
	
	
	/* Append tsize and value */
	while(*p_tftp_tsize != '\0')
	{
		*pu8_tftp_options++ = *p_tftp_tsize++;
	}
	*pu8_tftp_options++=0;
	p_option_value = (char*)a_option_value;
	utoa(0, p_option_value, 10);
	while(*p_option_value != '\0')
	{
		*pu8_tftp_options++ = *p_option_value++;
	}
	*pu8_tftp_options++=0;

	return((U8)(pu8_tftp_options - pu8_tftp_options_start_address));
}
/**
 * \brief This function terminates connection and re starts based on handler
 */
static void tftp_client_handle_server_connection_timeout(void)
{
	tftp_client_terminate_connection();
	
	if(mpf_no_response_handler)
		mpf_no_response_handler();
}