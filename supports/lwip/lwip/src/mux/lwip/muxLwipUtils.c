
#include "lwipMux.h"
#include "lwip/inet.h"
//#include "lwip/prot/ethernet.h"

void extLwipDhcpDebug(	ip4_addr_t *ip, ip4_addr_t *mask, 	ip4_addr_t *gw)
{
	if(!MUX_DEBUG_IS_ENABLE(MUX_DEBUG_FLAG_DHCP) )
	{
		return;
	}
	
	printf( "DHCP Configure: IP:%3"U16_F".%3"U16_F".%3"U16_F"%3"U16_F"; MASK:%3"U16_F".%3"U16_F".%3"U16_F"%3"U16_F"; GATEWAY %3"U16_F".%3"U16_F".%3"U16_F"%3"U16_F"; "LWIP_NEW_LINE, 
		ip4_addr1_16(ip),ip4_addr2_16(ip), ip4_addr3_16(ip), ip4_addr4_16(ip), 
		ip4_addr1_16(mask),ip4_addr2_16(mask), ip4_addr3_16(mask), ip4_addr4_16(mask), 
		ip4_addr1_16(gw),ip4_addr2_16(gw), ip4_addr3_16(gw), ip4_addr4_16(gw) 
		);
}


void muxLwipIp4DebugPrint(struct pbuf *p, const char *prompt)
{
	struct ip_hdr *iphdr = (struct ip_hdr *)p->payload;

	if(!MUX_DEBUG_IS_ENABLE(MUX_DEBUG_FLAG_IP_IN|MUX_DEBUG_FLAG_IP_OUT))
	{
		return;
	}
	
	printf( "%s IP header:" LWIP_NEW_LINE, prompt );
	printf( "+-------------------------------+" LWIP_NEW_LINE);
	printf( "|%2"S16_F" |%2"S16_F" |  0x%02"X16_F" |     %5"U16_F"     | (v, hl, tos, len)" LWIP_NEW_LINE, (u16_t)IPH_V(iphdr), (u16_t)IPH_HL(iphdr),  (u16_t)IPH_TOS(iphdr),lwip_ntohs(IPH_LEN(iphdr)));
	printf( "+-------------------------------+" LWIP_NEW_LINE);
	printf( "|    %5"U16_F"      |%"U16_F"%"U16_F"%"U16_F"|    %4"U16_F"   | (id, flags, offset)" LWIP_NEW_LINE,
		lwip_ntohs(IPH_ID(iphdr)), (u16_t)(lwip_ntohs(IPH_OFFSET(iphdr)) >> 15 & 1), (u16_t)(lwip_ntohs(IPH_OFFSET(iphdr)) >> 14 & 1), (u16_t)(lwip_ntohs(IPH_OFFSET(iphdr)) >> 13 & 1),
		(u16_t)(lwip_ntohs(IPH_OFFSET(iphdr)) & IP_OFFMASK));
	printf( "+-------------------------------+" LWIP_NEW_LINE);
	printf( "|  %3"U16_F"  |  %3"U16_F"  |    0x%04"X16_F"     | (ttl, proto, chksum)" LWIP_NEW_LINE, (u16_t)IPH_TTL(iphdr), (u16_t)IPH_PROTO(iphdr),lwip_ntohs(IPH_CHKSUM(iphdr)));
	printf( "+-------------------------------+" LWIP_NEW_LINE);
	printf( "|  %3"U16_F"  |  %3"U16_F"  |  %3"U16_F"  |  %3"U16_F"  | (src)" LWIP_NEW_LINE, 	ip4_addr1_16(&iphdr->src),ip4_addr2_16(&iphdr->src), ip4_addr3_16(&iphdr->src), ip4_addr4_16(&iphdr->src));
	printf( "+-------------------------------+" LWIP_NEW_LINE);
	printf( "|  %3"U16_F"  |  %3"U16_F"  |  %3"U16_F"  |  %3"U16_F"  | (dest)" LWIP_NEW_LINE, 
		ip4_addr1_16(&iphdr->dest), ip4_addr2_16(&iphdr->dest), ip4_addr3_16(&iphdr->dest), ip4_addr4_16(&iphdr->dest));
	printf( "+-------------------------------+" LWIP_NEW_LINE);
}


void muxLwipIgmpDebugPrint(const ip4_addr_t *groupaddr, const char isJoin)
{
	if(!MUX_DEBUG_IS_ENABLE(MUX_DEBUG_FLAG_IGMP) )
	{
		return;
	}
	
	printf("IGMP %s group '%s'"LWIP_NEW_LINE, (isJoin==0)?"leave from":"join to",  inet_ntoa(*(ip_addr_t *)groupaddr) );
	
}


char	muxNetIsGroupAddress(unsigned int	*ipAddress)
{
	const ip4_addr_t *mcIpAddr = (ip4_addr_t *)ipAddress;
	if( ip4_addr_ismulticast(mcIpAddr))
	{
		return 1;
	}

	return 0;
}

char muxNetMulticastIP4Mac(unsigned int	*ipAddress, MUX_MAC_ADDRESS *macAddress)
{
#if 1
	const ip4_addr_t *mcIpAddr = (ip4_addr_t *)ipAddress;
	if( ip4_addr_ismulticast(mcIpAddr))
#else
	if(IP_ADDR_IS_MULTICAST(ipAddress) )
#endif
	{/* Hash IP multicast address to MAC address.*/
		macAddress->address[0] = LL_IP4_MULTICAST_ADDR_0;
		macAddress->address[1] = LL_IP4_MULTICAST_ADDR_1;
		macAddress->address[2] = LL_IP4_MULTICAST_ADDR_2;
		macAddress->address[3] = ip4_addr2(mcIpAddr) & 0x7f;
		macAddress->address[4] = ip4_addr3(mcIpAddr);
		macAddress->address[5] = ip4_addr4(mcIpAddr);

		return EXIT_SUCCESS;
	}

	return EXIT_FAILURE;
}

char muxTxMulticastIP2Mac(MUX_RUNTIME_CFG *runCfg)
{
	const ip4_addr_t *mcIpAddr = (ip4_addr_t *)&runCfg->dest.ip;
	if( ip4_addr_ismulticast(mcIpAddr) )
	{/* Hash IP multicast address to MAC address.*/
		runCfg->dest.mac.address[0] = LL_IP4_MULTICAST_ADDR_0;
		runCfg->dest.mac.address[1] = LL_IP4_MULTICAST_ADDR_1;
		runCfg->dest.mac.address[2] = LL_IP4_MULTICAST_ADDR_2;
		runCfg->dest.mac.address[3] = ip4_addr2(mcIpAddr) & 0x7f;
		runCfg->dest.mac.address[4] = ip4_addr3(mcIpAddr);
		runCfg->dest.mac.address[5] = ip4_addr4(mcIpAddr);

		return EXIT_SUCCESS;
	}

	return EXIT_FAILURE;
}


