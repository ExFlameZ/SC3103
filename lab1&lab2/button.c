#include "button.h"
#include "buzzer.h"
#include "FreeRTOS.h"
#include "task.h"

void Button_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(BUTTON_CLK, ENABLE);

    GPIO_InitStructure.GPIO_Pin = BUTTON_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; // Pull-down for active-high USER button
    GPIO_Init(BUTTON_PORT, &GPIO_InitStructure);
}

// Practice A Task Implementation[cite: 1]
void button_task(void *pvParameters) {
    while (1) {
        // Poll USER button state[cite: 1]
        if (GPIO_ReadInputDataBit(BUTTON_PORT, BUTTON_PIN) == 1) {
            playSong(default_song); // Play song on buzzer when pressed[cite: 1]
        }
        vTaskDelay(pdMS_TO_TICKS(50)); // Poll every 50ms[cite: 1]
    }
}