#include "contiki-net.h"
#include "webclient.h"
#include "cfs/cfs.h"
#include <stdio.h>

#define HOST "172.18.0.1"

PROCESS(packagemanager_process, "packagemanager process");
AUTOSTART_PROCESSES(&packagemanager_process);

static int file = -1;

static void app_quit(void)
{
  if(file != -1) {
    cfs_close(file);
  }
}

static void install_package(char *name)
{
	file = cfs_open(name, CFS_WRITE);
	if(file == -1) {
		printf("Open error with '%s'\n", name);
	} else {
		if(webclient_get(HOST, 80, name) == 0) {
			puts("Out of memory error");
		} else {
			puts("Connecting...");
		}
	}
}

/*-----------------------------------------------------------------------------------*/
/* Callback function. Called from the webclient when the HTTP
 * connection was abruptly aborted.
 */
void
webclient_aborted(void)
{
	puts("Connection reset by peer");
	app_quit();
}
/*-----------------------------------------------------------------------------------*/
/* Callback function. Called from the webclient when the HTTP
 * connection timed out.
 */
void
webclient_timedout(void)
{
	puts("Connection timed out");
	app_quit();
}
/*-----------------------------------------------------------------------------------*/
/* Callback function. Called from the webclient when the HTTP
 * connection was closed after a request from the "webclient_close()"
 * function. .
 */
void
webclient_closed(void)
{  
	puts("Done.");
	app_quit();
}
/*-----------------------------------------------------------------------------------*/
/* Callback function. Called from the webclient when the HTTP
 * connection is connected.
 */
void
webclient_connected(void)
{    
	puts("Request sent...");
}
/*-----------------------------------------------------------------------------------*/
/* Callback function. Called from the webclient module when HTTP data
 * has arrived.
 */
void
webclient_datahandler(char *data, uint16_t len)
{
	static unsigned long dload_bytes;
	int ret;

	if(len > 0) {
		dload_bytes += len;
		printf("Downloading (%lu bytes)\n", dload_bytes);
		if(file != -1) {
			ret = cfs_write(file, data, len);
			if(ret != len) {
				printf("Wrote only %d bytes\n", ret);
			}
		}
	}

	if(data == NULL) {
		printf("Finished downloading %lu bytes.\n", dload_bytes);
		printf("Loading new App\n");
#if 0
		elfloader_load(file);
#endif
		app_quit();
	}
}
/*-----------------------------------------------------------------------------------*/


PROCESS_THREAD(packagemanager_process, ev ,data)
{
	static char name[] = "hello-world.elf";
	static int i;
	PROCESS_BEGIN();

	/* Allow other processes to initialize properly. */
	for(i = 0; i < 1000; ++i)
		PROCESS_PAUSE();

	install_package(name);

	while(1) {
		PROCESS_WAIT_EVENT();

		if(ev == tcpip_event)
			webclient_appcall(data);
	}

	PROCESS_END();
}
