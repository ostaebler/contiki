#include "contiki.h"
#include "cfs.h"

/*---------------------------------------------------------------------------*/
PROCESS(touch_process, "touch process");
AUTOSTART_PROCESSES(&touch_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(touch_process, ev, data)
{

	static int fd;
	PROCESS_BEGIN();

	fd = cfs_open("/tmp/touched", CFS_WRITE);

	if (fd > 0) {
		cfs_write(fd, "foo", 3);
		cfs_close(fd);
	} 

	PROCESS_END();
}
/*---------------------------------------------------------------------------*/
