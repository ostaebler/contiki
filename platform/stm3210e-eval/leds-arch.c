#include "leds.h"
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

#define LED1_PIN  GPIO_Pin_9
#define LED1_PORT GPIOF
#define LED1_RCC  RCC_APB2Periph_GPIOF

void leds_arch_init(void)
{
    /* Enable GPIOF clock */
    RCC_APB2PeriphClockCmd(LED1_RCC, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    /* Configure PA.8 as Output push-pull */
    GPIO_InitStructure.GPIO_Pin = LED1_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(LED1_PORT, &GPIO_InitStructure);
}

unsigned char leds_arch_get(void)
{
    return (unsigned char) GPIO_ReadOutputDataBit(LED1_PORT, LED1_PIN);
}

void leds_arch_set(unsigned char leds)
{
    GPIO_WriteBit(LED1_PORT, LED1_PIN, leds & 0x01);
}
