/*
* 
*/

#ifndef __HTTP_CLIENT_H__
#define __HTTP_CLIENT_H__

#include "utils/lwiplib.h"

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "ethernet.h"


// You can replace this enum for saving MEMORY (replace with define's)
typedef enum
{
	OK,
	OUT_MEM,
	TIMEOUT,
	NOT_FOUND,
	GEN_ERROR
}hc_errormsg;

struct hc_state
{
	u8_t Num;
	char *Page;
	char *PostVars;
	char *RecvData;
	u16_t Len;
	u8_t ConnectionTimeout;
	void (* ReturnPage)(u8_t num, hc_errormsg, char *data, u16_t len);
};

// Public function
int hc_open(struct ip_addr,  char *, char *, void (*)(u8_t, hc_errormsg, char *, u16_t) );

#endif

