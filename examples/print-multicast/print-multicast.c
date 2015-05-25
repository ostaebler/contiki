#include "contiki.h"
#include "net/ip/uip.h"

#include <stdio.h>

PROCESS(print_multicast_process, "print-multicast process");
AUTOSTART_PROCESSES(&print_multicast_process);

static struct uip_udp_conn *udpconn;

static void udphandler(void)
{
	/* terminate payload */
	int len = uip_datalen();
	((char *)uip_appdata)[len] = 0;

	printf("Received: %s\n", (char*)uip_appdata);
}

PROCESS_THREAD(print_multicast_process, ev, data)
{
	PROCESS_BEGIN();

	udpconn = udp_new(NULL, UIP_HTONS(0), NULL);
	udp_bind(udpconn, UIP_HTONS(1234));

	while(1) {
		PROCESS_WAIT_EVENT_UNTIL(ev == tcpip_event);
		udphandler();
	}

	PROCESS_END();
}
