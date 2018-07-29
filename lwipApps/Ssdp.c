/***************************************************************************** 
* Ssdp.c - Simple Service Discovery Protocol.
*
*
* portions Copyright (c) 2006 by Michael Vysotsky.
*
******************************************************************************/
#include <includes.h>
#include "netconf.h"
#include "Ssdp.h"
#include "UpnpWeb.h"
/*----------------------------------------------------------------------------*/
/*                      DEFINITIONS                                           */
/*----------------------------------------------------------------------------*/


#define   UPNP_CACHE_SEC (UPNP_CACHE_SEC_MIN + 1) /* cache time we use */
#define   UPNP_CACHE_SEC_MIN    1800  /* min cachable time per UPnP standard */
#define   UPNP_ADVERTISE_REPEAT 2     /* no more than 3 */
#define   MAX_MSEARCH           20    /* max simultaneous M-SEARCH replies ongoing */
#define   UPNP_MULTICAST_ADDRESS  "239.255.255.250" /* for UPnP multicasting */
#define   UPNP_MULTICAST_PORT   1900 /* UDP port to monitor for UPnP */

#define   UPNP_RANDOM_TIME      UPNP_CACHE_SEC / 4 + (((UPNP_CACHE_SEC / 4) * (rand()&0xFFFFF)) >> 16)      

typedef enum {
  ADVERTISE_UP = 0,
  ADVERTISE_DOWN = 1,
  MSEARCH_REPLY = 2
}SSDP_TYPES;
/*----------------------------------------------------------------------------*/
/*                      VARIABLES                                             */
/*----------------------------------------------------------------------------*/

struct udp_pcb  * udp_ssdp=NULL;
const BYTE        SSDPMAC[] = {0x01,0x00,0x5E,0x7F,0xFF,0xFA};
SSDP_TYPES        ssdp_type = ADVERTISE_UP;
BYTE              ssdp_state=0;
struct ip_addr    ssdp_ip_reply;
WORD              ssdp_port_reply;


/*----------------------------------------------------------------------------*/
/*                      PROTOTYPES                                            */
/*----------------------------------------------------------------------------*/
static  void  Ssdp_Udp_Listner(void *arg, struct udp_pcb *upcb, struct pbuf *p, struct ip_addr *addr, u16_t port);
static  int   token_eq(const char *s1, const char *s2);
static  int   line_length(const char *l);
static  int   line_length_stripped(const char *l);
static  int   token_eq(const char *s1, const char *s2);
static  int   token_length(const char *s);
static  int   word_separation_length(const char *s);
static  int   str_starts(const char *str, const char *start);
static void   SsdpRun(void * arg);
static void   upnp_printf(char *buf, char *fmt, ...);


void SsdpInit(struct netif *net)
{
    /* Internet Broadcast SSDP MAC address config */
  ETH_MACAddressConfig(ETH_MAC_Address2,(BYTE*)SSDPMAC);
  ETH_MACAddressPerfectFilterCmd(ETH_MAC_Address2,ENABLE);
  ETH_MACAddressFilterConfig(ETH_MAC_Address2,ETH_MAC_AddressFilter_DA);
  net->flags |= NETIF_FLAG_IGMP;
  if((udp_ssdp = udp_new())){
    udp_recv(udp_ssdp, Ssdp_Udp_Listner, NULL);
    udp_bind(udp_ssdp, IP_ADDR_ANY, 1900);
  }
  tcpip_timeout((UPNP_RANDOM_TIME),SsdpRun,NULL);
  UpnpWebInit();  
}

void SsdpDown(void)
{
  ssdp_type = ADVERTISE_DOWN;
  udp_remove(udp_ssdp);
  udp_ssdp = NULL;
  tcpip_untimeout(SsdpRun,NULL);
  ETH_MACAddressPerfectFilterCmd(ETH_MAC_Address2,DISABLE);
#if LWIP_IGMP     
  struct ip_addr ip;
  IP4_ADDR(&ip,239,255,255,250); 
  igmp_leavegroup(Localhost(),&ip);
#endif  
}     
static  void  Ssdp_Udp_Listner(void *arg, struct udp_pcb *upcb, struct pbuf *p, struct ip_addr *addr, u16_t port)
{
  BYTE *buff=p->payload;
  if (strstr((const char*)buff, "NOTIFY ")) {
/*
* Silently ignore NOTIFYs to avoid filling debug log with
* unwanted messages.
*/
    pbuf_free(p);
    return;
  }
/* Parse first line */
  if (strncmp((const char*)buff, "M-SEARCH", strlen("M-SEARCH")) == 0 &&
          !isgraph(buff[strlen("M-SEARCH")])) {
      const char *buf= (const char*)buff;
      bool got_host = false;
      bool got_st = false, st_match = false;
      bool got_man = false;
      bool got_mx = false;
      long ssdp_time_interval=0;
/*
* Skip first line M-SEARCH * HTTP/1.1
* (perhaps we should check remainder of the line for syntax)
*/
      buf += line_length(buf);
/* Parse remaining lines */
      for (; *buf != '\0'; buf += line_length(buf)) {
        const char *	end = buf + line_length_stripped(buf);
        if (token_eq(buf, "host")) {
/* The host line indicates who the packet
* is addressed to... but do we really care?
* Note that Microsoft sometimes does funny
* stuff with the HOST: line.
*/
          got_host = true;
          continue;
        } else if (token_eq(buf, "st")) {
/* There are a number of forms; we look
* for one that matches our case.
*/
          got_st = true;
          buf += token_length(buf);
          buf += word_separation_length(buf);
          if (*buf != ':')
            continue;
          buf++;
          buf += word_separation_length(buf);
          if (str_starts(buf, "ssdp:all")) {
            st_match = true;
            continue;
          }
          if (str_starts(buf, "upnp:rootdevice")) {
            st_match = true;
            continue;
          }
          if (str_starts(buf, "uuid:")) {
            char uuid_string[80];
            buff += strlen("uuid:");
            uuid_bin2str(UUID_DEVICE, uuid_string,
                        sizeof(uuid_string));
            if (str_starts(buf, uuid_string))
              st_match = true;
              continue;
          }
          continue;
      } else if (token_eq(buf, "man")) {
        buf += token_length(buf);
        buf += word_separation_length(buf);
        if (*buf != ':')
          continue;
        buf++;
        buf += word_separation_length(buf);
        if (!str_starts(buf, "\"ssdp:discover\"")) {
/*  WPS UPnP: Unexpected. "M-SEARCH man-field */
          pbuf_free(p);
          return;
        }
        got_man = true;
        continue;
      } else if (token_eq(buf, "mx")) {
        buf += token_length(buf);
        buf += word_separation_length(buf);
        if (*buf != ':')
          continue;
        buf++;
        buf += word_separation_length(buf);
        ssdp_time_interval = atol(buf);
        got_mx = true;
        continue;
      }
            /* ignore anything else */
    }
    if (!got_host || !got_st || !got_man || !got_mx || ssdp_time_interval < 0) {
    /* WPS UPnP: Invalid M-SEARCH: */
      return ;
    }
    if (!st_match) {
    /* WPS UPnP: Ignored M-SEARCH */
      pbuf_free(p);
      return;
    }
    if (ssdp_time_interval > 120)
              ssdp_time_interval = 120; /* UPnP-arch-DeviceArchitecture, 1.2.3 */
      ssdp_time_interval *= OS_CFG_TICK_RATE_HZ;   /* tick per second */
      ssdp_type = MSEARCH_REPLY;
      ssdp_state = 0;
      tcpip_timeout(ssdp_time_interval,SsdpRun,NULL);
      ip_addr_set(&ssdp_ip_reply,addr);
      ssdp_port_reply = port;
      pbuf_free(p);
      return;
  }
  pbuf_free(p);
}

/*
 *      SSDP State mashine
 */
static void SsdpRun(void * arg)
{
  if( Localhost()){
    struct ip_addr reply_ip;
    IP4_ADDR(&reply_ip,239,255,255,250);
#if LWIP_IGMP    
    igmp_joingroup(Localhost(),&reply_ip);
#endif    
    char *msg;
    char *NTString = "";
    char uuid_string[80];
    WORD  reply_port = UPNP_MULTICAST_PORT;
    uuid_bin2str(UUID_DEVICE, uuid_string, sizeof(uuid_string));
    msg = mem_malloc(400); /* more than big enough */
    if (msg == NULL)
		return;
    memset(msg,0,400);
    switch (ssdp_type) {
    case ADVERTISE_UP:
    case ADVERTISE_DOWN:
      NTString = "NT";
      strcpy(msg, "NOTIFY * HTTP/1.1\r\n");
      upnp_printf(msg, "HOST: %s:%d\r\n",
                    UPNP_MULTICAST_ADDRESS, UPNP_MULTICAST_PORT);
      upnp_printf(msg, "CACHE-CONTROL: max-age=%d\r\n",
                    UPNP_CACHE_SEC);
      upnp_printf(msg, "NTS: %s\r\n",
                    (ssdp_type == ADVERTISE_UP ?
                     "ssdp:alive" : "ssdp:byebye"));
      break;
    case MSEARCH_REPLY:
      ip_addr_set(&reply_ip,&ssdp_ip_reply);
      reply_port = ssdp_port_reply;
      NTString = "ST";
      strcpy(msg, "HTTP/1.1 200 OK\r\n");
      upnp_printf(msg, "CACHE-CONTROL: max-age=%d\r\n",
                    UPNP_CACHE_SEC);
      strcat(msg, "EXT:\r\n");
      break;
    }
    if (ssdp_type != ADVERTISE_DOWN) {
/* Where others may get our XML files from */
      upnp_printf(msg, "LOCATION: http://%s:%d/%s\r\n",
                  htoa((struct in_addr *)Localhost()), UPNP_HTTP_PORT,
                  UPNP_DEVICE_XML_FILE);
    }

/* The SERVER line has three comma-separated fields:
 *      operating system / version
 *      upnp version
 *      software package / version
 * However, only the UPnP version is really required, the
 * others can be place holders... for security reasons
 * it is better to NOT provide extra information.
 */
    strcat(msg, "SERVER: unspecified, UPnP/1.0, unspecified\r\n");
    switch (ssdp_state / UPNP_ADVERTISE_REPEAT) {
    case 0:
      upnp_printf(msg, "%s: upnp:rootdevice\r\n", NTString);
      upnp_printf(msg, "USN: uuid:%s::upnp:rootdevice\r\n",
                    uuid_string);
      break;
    case 1:
      upnp_printf(msg, "%s: uuid:%s\r\n", NTString, uuid_string);
      upnp_printf(msg, "USN: uuid:%s\r\n", uuid_string);
      break;
    }
    strcat(msg, "\r\n");
    struct pbuf * udpbuf = pbuf_alloc(PBUF_TRANSPORT, 0, PBUF_REF);
    if(udpbuf == NULL){
      mem_free(msg);
      return;
    }    
    udpbuf->payload = (void*)msg;
    udpbuf->len = udpbuf->tot_len = strlen(msg);
    udp_sendto(udp_ssdp,udpbuf,&reply_ip,reply_port);
    mem_free(msg);
    udpbuf->payload = NULL;
    pbuf_free(udpbuf);
    if (ssdp_state + 1 >= 2 * UPNP_ADVERTISE_REPEAT){
      ssdp_state =0;
    }
    ssdp_type =  ADVERTISE_UP;   
  }
  tcpip_timeout((UPNP_RANDOM_TIME),SsdpRun,NULL);
}

/*********************************************************************/
/**               SSDP Utilities                                    **/
/*********************************************************************/

/* Check tokens for equality, where tokens consist of letters, digits,
 * underscore and hyphen, and are matched case insensitive.
 */
static int token_eq(const char *s1, const char *s2)
{
  int c1;
  int c2;
  int end1 = 0;
  int end2 = 0;
  for (;;) {
    c1 = *s1++;
    c2 = *s2++;
    if (isalpha(c1) && isupper(c1))
            c1 = tolower(c1);
    if (isalpha(c2) && isupper(c2))
            c2 = tolower(c2);
    end1 = !(isalnum(c1) || c1 == '_' || c1 == '-');
    end2 = !(isalnum(c2) || c2 == '_' || c2 == '-');
    if (end1 || end2 || c1 != c2)
            break;
  }
  return end1 && end2; /* reached end of both words? */
}
/* No. of chars through (including) end of line */
static int line_length(const char *l)
{
  const char *lp = l;
  while (*lp && *lp != '\n')
          lp++;
  if (*lp == '\n')
          lp++;
  return lp - l;
}
/* No. of chars excluding trailing whitespace */
static int line_length_stripped(const char *l)
{
  const char *lp = l + line_length(l);
  while (lp > l && !isgraph(lp[-1]))
          lp--;
  return lp - l;
}

/* Return length of token (see above for definition of token) */
static int token_length(const char *s)
{
  const char *begin = s;
  for (;; s++) {
    int c = *s;
    int end = !(isalnum(c) || c == '_' || c == '-');
    if (end)
      break;
  }
  return s - begin;
}
/* return length of interword separation.
 * This accepts only spaces/tabs and thus will not traverse a line
 * or buffer ending.
 */
static int word_separation_length(const char *s)
{
  const char *begin = s;
  for (;; s++) {
    int c = *s;
    if (c == ' ' || c == '\t')
      continue;
    break;
  }
  return s - begin;
}
static int str_starts(const char *str, const char *start)
{
  return strncmp(str, start, strlen(start)) == 0;
}
int uuid_bin2str(const BYTE *bin, char *str, size_t max_len)
{
  int len;
  len = snprintf(str, max_len, "%02x%02x%02x%02x-%02x%02x-%02x%02x-"
    "%02x%02x-%02x%02x%02x%02x%02x%02x",
    bin[0], bin[1], bin[2], bin[3],
    bin[4], bin[5], bin[6], bin[7],
    bin[8], bin[9], bin[10], bin[11],
    bin[12], bin[13], bin[14], bin[15]);
  if (len < 0 || (size_t) len >= max_len)
    return -1;
  return 0;
}
static void upnp_printf(char *buf, char *fmt, ...)
{
  va_list ap;
  char *tmp = buf + strlen(buf);
  va_start(ap, fmt);
  vsprintf(tmp, fmt, ap);
  va_end(ap);
}
