/**************************************************************************
*                                                                         *
*   PROJECT     : TMON (Transparent monitor)                              *
*                                                                         *
*   MODULE      : LWIP.c                                                  *
*                                                                         *
*   AUTHOR      : Michael Anburaj                                         *
*                 URL  : http://geocities.com/michaelanburaj/             *
*                 EMAIL: michaelanburaj@hotmail.com                       *
*                                                                         *
*   PROCESSOR   : Any                                                     *
*                                                                         *
*   Tool-chain  : gcc                                                     *
*                                                                         *
*   DESCRIPTION :                                                         *
*   LwIP master source file.                                              *
*                                                                         *
**************************************************************************/

#include "tcpip.h"
#include "ethernetif.h"
#include "netconf.h"

/* ********************************************************************* */
/* Global definitions */


/* ********************************************************************* */
/* File local definitions */

netif_t     main_net;

/* ********************************************************************* */
/* Local functions */

static void LwIP_SendSem(void)
{
 // sys_sem_signal(&lwip_eth_sem);
  while(BSP_ETH_IsRxPktValid()) 
   ethernetif_input(&main_net);
}
/* ********************************************************************* */
/* Global functions */


/**
  * @brief  LwIP periodic tasks
  * @param  localtime the current LocalTime value
  * @retval None
  */
static int static_ip_timeout=0;
static void LwIP_DHCP_task(void *arg)
{
  static struct ip_addr ip={0};
  bool static_ip = arg? true:false;
  if((!ip_addr_isany(&main_net.ip_addr)&&
     !ip_addr_cmp(&main_net.ip_addr,&ip)) || (++static_ip_timeout > 60 && static_ip)){
       if(!static_ip){   
         ip_addr_set(&ip,&main_net.ip_addr);
         SetLocalhost(&main_net.ip_addr);
         SetNetMask(&main_net.netmask);
         SetGateway(&main_net.gw);
/*** LCD Display DHCP Address ****/ 
#include "stm32_eval.h" 
#include "stm322xg_eval_lcd.h"         
         uint32_t IPaddress = main_net.ip_addr.addr;
         uint8_t iptab[4];
         uint8_t iptxt[20];
         iptab[0] = (uint8_t)(IPaddress >> 24);
         iptab[1] = (uint8_t)(IPaddress >> 16);
         iptab[2] = (uint8_t)(IPaddress >> 8);
         iptab[3] = (uint8_t)(IPaddress);
          
         sprintf((char*)iptxt, "  %d.%d.%d.%d", iptab[3], iptab[2], iptab[1], iptab[0]);  
          
          /* Display the IP address */
         LCD_DisplayStringLine(Line7, (uint8_t*)"IP address assigned ");
         LCD_DisplayStringLine(Line8, (uint8_t*)"  by a DHCP server  ");
         LCD_DisplayStringLine(Line9, iptxt);         
/************************************/           
       }else{
         ip_addr_set(&ip,Localhost());
         netif_set_addr(&main_net, Localhost(),GetNetMask(),GetGateway());
         dhcp_stop(&main_net);
         return;
       }
  }
  tcpip_timeout(1000, LwIP_DHCP_task, arg); 
}  
/**
  * @brief  Initializes the lwIP stack
  * @param  None
  * @retval None
  */
void LwIP_Init(bool static_ip)
{
  struct ip_addr ipaddr;
  struct ip_addr netmask;
  struct ip_addr gw;

    
  tcpip_init(NULL, NULL);


  ipaddr.addr = 0;
  netmask.addr = 0;
  gw.addr = 0;
  
  MEMCPY(main_net.hwaddr,BSP_ETH_GetMacAddress(),ETHARP_HWADDR_LEN);
  main_net.hwaddr_len=ETHARP_HWADDR_LEN;
  MEMCPY(main_net.name,"eth0",4);
#if LWIP_NETIF_HOSTNAME  
  main_net.hostname = GetHostName();
#endif
  /* - netif_add(struct netif *netif, struct ip_addr *ipaddr,
            struct ip_addr *netmask, struct ip_addr *gw,
            void *state, err_t (* init)(struct netif *netif),
            err_t (* input)(struct pbuf *p, struct netif *netif))
    
   Adds your network interface to the netif_list. Allocate a struct
  netif and pass a pointer to this structure as the first argument.
  Give pointers to cleared ip_addr structures when using DHCP,
  or fill them with sane numbers otherwise. The state pointer may be NULL.

  The init function pointer must point to a initialization function for
  your ethernet netif interface. The following code illustrates it's use.*/
  
  
  netif_add(&main_net, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);
  /*  Registers the default network interface.*/
  netif_set_default(&main_net);

  BSP_ETH_IRQHandler = LwIP_SendSem;
  /*  Creates a new DHCP client for this interface on the first call.
  Note: you must call dhcp_fine_tmr() and dhcp_coarse_tmr() at
  the predefined regular intervals after starting the client.
  You can peek in the netif->dhcp struct for the actual DHCP status.*/
  dhcp_start(&main_net);
  /*  When the netif is fully configured this function must be called.*/
  netif_set_up(&main_net);

  tcpip_timeout(1000, LwIP_DHCP_task, (void*)static_ip);   
  SetHostName("STM322xG");
  SsdpInit(&main_net);
  LlmnrInit(&main_net);  

}
void LwIP_DhcpStart(bool static_ip)
{
  dhcp_stop(&main_net);
  netif_set_addr(&main_net, 0,0,0);
  dhcp_start(&main_net);
  static_ip_timeout=0;
  tcpip_timeout(1000, LwIP_DHCP_task, (void*)static_ip);  
}
void LwIP_DhcpStop(void)
{
  dhcp_stop(&main_net);
}



/* ********************************************************************* */
