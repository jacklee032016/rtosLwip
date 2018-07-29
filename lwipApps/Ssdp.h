#ifndef _SSDP_H
#define _SSDP_H


int   uuid_bin2str(const BYTE *bin, char *str, size_t max_len);
void  SsdpInit(struct netif *net);
void  SsdpDown(void);
#endif