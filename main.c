#include "stm32f4xx.h"

#include "FreeRTOS.h"
#include "task.h"

#include "stm32f4x7_eth_bsp.h"



#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/dhcp.h"
#include "ethernetif.h"
#include "main.h"
#include "tcpip.h"
#include "netif.h"
#include "http_server.h"
#include <stdio.h>
#include "mbinit.h"

struct netif xnetif;

void LwIP_Init(void)
{
  ip_addr_t ipaddr;
  ip_addr_t netmask;
  ip_addr_t gw;

  tcpip_init( NULL, NULL );	

#ifdef USE_DHCP
  ipaddr.addr = 0;
  netmask.addr = 0;
  gw.addr = 0;
#else
  IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
  IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1 , NETMASK_ADDR2, NETMASK_ADDR3);
  IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
#endif

  netif_add(&xnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);
  netif_set_default(&xnetif);
  netif_set_up(&xnetif); 
}


int main()
{
    /* configure Ethernet (GPIOs, clocks, MAC, DMA) */ 
    ETH_BSP_Config();

    /* Initilaize the LwIP stack */
    LwIP_Init();
    MB_TCP_Init();

    xTaskCreate(http_server_netconn_thread, "HTTP", 512, NULL, 2, ( TaskHandle_t * ) NULL);

    vTaskStartScheduler();
    return 0;
}