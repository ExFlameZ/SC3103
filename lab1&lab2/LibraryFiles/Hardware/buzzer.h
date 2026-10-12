#ifndef __BUZZER_H
#define __BUZZER_H

#include "stm32f4xx.h"

#define BUZZER_PORT     GPIOB
#define BUZZER_PIN      GPIO_Pin_10
#define BUZZER_CLK      RCC_AHB1Periph_GPIOB

#define BUZZER_ON()     GPIO_SetBits(BUZZER_PORT, BUZZER_PIN)
#define BUZZER_OFF()    GPIO_ResetBits(BUZZER_PORT, BUZZER_PIN)

void Buzzer_Init(void);
void playSong(uint8_t *song);

extern uint8_t default_song[];

#endif