#include "dev/button-sensor.h"
#include "lib/sensors.h"
#include "misc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_exti.h"

EXTI_InitTypeDef   EXTI_InitStructure;
GPIO_InitTypeDef   GPIO_InitStructure;
NVIC_InitTypeDef   NVIC_InitStructure;

const struct sensors_sensor button_sensor;
const struct sensors_sensor *sensors[] = {
	&button_sensor,
	0
};
unsigned char sensors_flags[(sizeof(sensors) / sizeof(struct sensors_sensor *))];

int _status;

void EXTI9_5_IRQHandler(void)
{
	sensors_changed(&button_sensor);
	EXTI_ClearITPendingBit(EXTI_Line8);
}

static int value(int type)
{
	return GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_8);
}

static int configure(int type, int value)
{
	switch (type) {
		case SENSORS_HW_INIT:

			RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG|RCC_APB2Periph_AFIO,ENABLE);

			GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
			GPIO_Init(GPIOG, &GPIO_InitStructure);

			GPIO_EXTILineConfig(GPIO_PortSourceGPIOG, GPIO_PinSource8);

			EXTI_InitStructure.EXTI_Line=EXTI_Line8;
			EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
			EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
			EXTI_InitStructure.EXTI_LineCmd = ENABLE;
			EXTI_Init(&EXTI_InitStructure);

			NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
			return 1;

		case SENSORS_ACTIVE:
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
			NVIC_Init(&NVIC_InitStructure);
			_status = 1;
			return 1;
	}

	return 0;
}

static int status(int type)
{
	return _status;
}

SENSORS_SENSOR(button_sensor, BUTTON_SENSOR, value, configure, status); 
