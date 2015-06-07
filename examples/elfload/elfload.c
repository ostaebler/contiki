#include "contiki.h"
#include "cfs.h"
#include "elfloader.h"
#include <stdio.h>

PROCESS(elfload_process, "elfload process");
AUTOSTART_PROCESSES(&elfload_process);

PROCESS_THREAD(elfload_process, ev, data)
{
	static int fd;
	int ret;
	PROCESS_BEGIN();

	elfloader_init();
	
	fd = cfs_open("/tmp/hello-world.elf", CFS_WRITE | CFS_APPEND | CFS_READ);

	if (fd > 0) {
		ret = elfloader_load(fd);
		cfs_close(fd);
		if (ret == ELFLOADER_OK) {
			printf("Starting program\n");
			autostart_start(elfloader_autostart_processes);
		} else {
			printf("Error loading ELF: %d\n", ret);
		}
	} else {
		printf("Error opening file\n");
	}

	PROCESS_END();
}
