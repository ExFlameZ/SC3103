#ifndef __BUTTON_H
#define __BUTTON_H

#include "stm32f4xx.h"

#define BUTTON_PORT     GPIOA
#define BUTTON_PIN      GPIO_Pin_0
#define BUTTON_CLK      RCC_AHB1Periph_GPIOA

void Button_Init(void);
void button_task(void *pvParameters);

#endif