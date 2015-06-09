#include "contiki-net.h"
#include "webclient.h"
#include "cfs/cfs.h"
#include <stdio.h>
#include <string.h>

#define HOST "172.18.0.1"

#define CHROOT "/tmp/contiki/"

#define APP_STATUS_STOPPED 0
#define APP_STATUS_INIT 1
#define APP_STATUS_INSTALLED 2
#define APP_STATUS_LIST 3

PROCESS(packagemanager_process, "packagemanager process");
AUTOSTART_PROCESSES(&packagemanager_process);

static int file = -1;
static int app_status = 0;

const char poc_process_name[] = "hello-world";
struct process poc_process = {};

static void app_quit(void)
{
  if(file != -1) {
    cfs_close(file);
  }
}

static void list_installed_packages(void)
{
	struct cfs_dir dir;
	struct cfs_dirent dirent;
	char process_name[32];
	char process_version[16];

	bzero(process_name, sizeof(process_name));
	bzero(process_version, sizeof(process_version));

	if(cfs_opendir(&dir, CHROOT) == 0) {
		printf("Installed Packages:\n");
		printf("Name\t\tVersion\n");
		while(cfs_readdir(&dir, &dirent) != -1) {
			char *dot = strrchr(dirent.name, '.');
			if (dot && !strcmp(dot, ".elf")) {
				char *underscore = strchr(dirent.name, '_');
				strncpy(process_name, dirent.name, underscore-dirent.name);
				strncpy(process_version, underscore + 1, dot-underscore-1);
				printf("%s\t%s\n", process_name, process_version);
			}
		}
	cfs_closedir(&dir);
	printf("\n");
	}
}

static void remove_package(char *name)
{
	struct process *p;
	struct cfs_dir dir;
	struct cfs_dirent dirent;
	char newname[16] = "";
	char newname_path[64] = CHROOT;

	/* find and stop process */
	for(p = PROCESS_LIST(); p != NULL; p = p->next) {
		if (!strcmp(name, p->name))  {
			printf("Stopping process %s\n", name);
			process_exit(p);
		}
	}

	/* Remove app file in cfs */
	if(cfs_opendir(&dir, CHROOT) == 0) {
		strcat(newname, name);
		strcat(newname, ".elf");
		while(cfs_readdir(&dir, &dirent) != -1) {
			if (!strncmp(name, dirent.name, strlen(name))) {
				char *dot = strrchr(dirent.name, '.');
				if (dot && !strcmp(dot, ".elf")) {
					strcat(newname_path, dirent.name);
					printf("Removing %s\n", newname_path);
					cfs_remove(newname_path);
					break;
				}
			}
		}
		cfs_closedir(&dir);
	}
}

static void install_package(char *name)
{
	char newname[64] = "";

	strcat(newname, CHROOT);
	strcat(newname, name);
	file = cfs_open(newname, CFS_WRITE);
	if(file == -1) {
		printf("Open error with '%s'\n", newname);
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
#else
		poc_process.name = poc_process_name;
		process_start(&poc_process, NULL);
#endif
		app_quit();
		app_status += 1;
	}
}
/*-----------------------------------------------------------------------------------*/

void print_processes(void)
{
	struct process *p;

	printf("process list:\n");
	for(p = PROCESS_LIST(); p != NULL; p = p->next) {
		char namebuf[30];
		strncpy(namebuf, PROCESS_NAME_STRING(p), sizeof(namebuf));
		printf("%s\n", namebuf);
	}
	printf("\n");
}

PROCESS_THREAD(packagemanager_process, ev ,data)
{
	static char name[] = "hello-world_0.1.elf";
	static int i;
	PROCESS_BEGIN();

	/* Allow other processes to initialize properly. */
	for (i = 0; i < 1000; ++i)
		PROCESS_PAUSE();
	app_status = APP_STATUS_INIT;

	/* List installed packages first */
	list_installed_packages();
	print_processes();

	printf("=================\n");
	/* Install new package */
	install_package(name);

	while (app_status < APP_STATUS_INSTALLED) {
		PROCESS_WAIT_EVENT();

		if(ev == tcpip_event)
			webclient_appcall(data);
	}

	/* List installed packages again */
	list_installed_packages();
	print_processes();
	app_status += 1;
	printf("=================\n");

	remove_package(poc_process_name);

	list_installed_packages();
	print_processes();

	PROCESS_END();
}
