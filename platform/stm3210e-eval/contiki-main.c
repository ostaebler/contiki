#include <stm32f10x.h>
#include <stm32f10x_dma.h>
#include <gpio.h>
#include <nvic.h>
#include <stdint.h>
#include <stdio.h>
#include <debug-uart.h>
#include <sys/process.h>
#include <sys/procinit.h>
#include <etimer.h>
#include <sys/autostart.h>
#include <clock.h>
#include "uart1.h"

unsigned int idle_count = 0;

int
main()
{
  uart1_init(115200);
  leds_init();
  printf("Initialising\n\r");
  
  clock_init();
  process_init();
  process_start(&etimer_process, NULL);
  autostart_start(autostart_processes);
  printf("Processes running\n\r");
  while(1) {
    do {
    } while(process_run() > 0);
    idle_count++;
    /* Idle! */
    /* Stop processor clock */
    /* asm("wfi"::); */ 
  }
  return 0;
}

void assert_param(int b)
{
}
