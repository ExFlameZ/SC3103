#ifndef __UART_H
#define __UART_H

#include "stm32f4xx.h"
#include "stdio.h"

// External variable to store received character from PC[cite: 1]
extern volatile uint8_t rx_char;

// Function Prototypes
void uart3_init(u32 bound);
void usart3_send(u8 data);

#endif