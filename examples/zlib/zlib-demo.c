#include "contiki.h"
#include "zlib.h"
#include <stdio.h>

PROCESS(zlib_process, "zlib process");
AUTOSTART_PROCESSES(&zlib_process);

const unsigned char data_array[] = {
		0x78, 0x9c, 0xf3, 0x48, 0xcd, 0xc9, 0xc9, 0x57, 0x08, 0xcf, 0x2f, 0xca,
		0x49, 0x51, 0x64, 0x00, 0x00, 0x20, 0x87, 0x04, 0x3e};


PROCESS_THREAD(zlib_process, ev ,data)
{
	static char dst[20];
	unsigned long dst_len = sizeof(dst);

	PROCESS_BEGIN();

	uncompress(dst, &dst_len, data_array, sizeof(data_array));
	printf("Uncompressed: %s\n", dst);

	PROCESS_END();
}
