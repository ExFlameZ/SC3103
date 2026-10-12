#include "system.h"
#include "buzzer.h"
#include "button.h"
#include "uart.h"
#include "led.h"
#include "show.h"
#include "timers.h" // FreeRTOS Software Timer header[cite: 1, 2]

// Task Priorities
#define START_TASK_PRIO   4
#define BUTTON_TASK_PRIO  2
#define UART_TASK_PRIO    2
#define SHOW_TASK_PRIO    1

// Task Stack Sizes
#define START_STK_SIZE    256
#define BUTTON_STK_SIZE   256
#define UART_STK_SIZE     256
#define SHOW_STK_SIZE     128

// Handles
TaskHandle_t StartTask_Handler;
TimerHandle_t led_timer_handle = NULL;

// Function Prototypes
void start_task(void *pvParameters);
void uart_task(void *pvParameters);
void vLEDTimerCallback(TimerHandle_t xTimer);

int main(void) {
    systemInit();   // Initialize system clocks[cite: 2]
    LED_Init();     // Initialize LED GPIOs[cite: 2]
    Buzzer_Init();  // Initialize Buzzer GPIO[cite: 1]
    Button_Init();  // Initialize USER Button GPIO[cite: 1]
    
    // Create initial startup task[cite: 2]
    xTaskCreate((TaskFunction_t ) start_task,
                (const char*    ) "start_task",
                (uint16_t       ) START_STK_SIZE,
                (void*          ) NULL,
                (UBaseType_t    ) START_TASK_PRIO,
                (TaskHandle_t*  ) &StartTask_Handler);
                
    vTaskStartScheduler(); // Launch FreeRTOS[cite: 2]
}

// Practice B: LED Software Timer Callback[cite: 1]
void vLEDTimerCallback(TimerHandle_t xTimer) {
    LED = !LED; // Toggle LED pin state[cite: 2]
}

// Practice C: UART Task[cite: 1]
void uart_task(void *pvParameters) {
    uart3_init(115200); // Initialize USART3 at 115200 baud[cite: 1]
    
    while (1) {
        // Send name to PC every 1 second[cite: 1]
        char *name = "Student Name\r\n";
        while (*name) {
            usart3_send(*name++);
        }
        
        // Receive number character from PC and dynamically update LED timer period[cite: 1]
        if (rx_char >= '1' && rx_char <= '9') {
            uint32_t new_period = (rx_char - '0') * 200; // e.g., '5' -> 1000ms
            xTimerChangePeriod(led_timer_handle, pdMS_TO_TICKS(new_period), 0); // Practice C requirement[cite: 1]
            rx_char = 0; // Clear received buffer
        }

        vTaskDelay(pdMS_TO_TICKS(1000)); // 1-second interval[cite: 1]
    }
}

void start_task(void *pvParameters) {
    taskENTER_CRITICAL(); // Enter critical section[cite: 2]

    // 1. Practice A: Create Musical Button Task[cite: 1]
    xTaskCreate(button_task, "button_task", BUTTON_STK_SIZE, NULL, BUTTON_TASK_PRIO, NULL);

    // 2. Practice C: Create UART Task[cite: 1]
    xTaskCreate(uart_task, "uart_task", UART_STK_SIZE, NULL, UART_TASK_PRIO, NULL);

    // 3. OLED Display Task from Exercise #1[cite: 1, 2]
    xTaskCreate(show_task, "show_task", SHOW_STK_SIZE, NULL, SHOW_TASK_PRIO, NULL);

    // 4. Practice B: Create and start LED Software Timer (1 sec period)[cite: 1]
    led_timer_handle = xTimerCreate("LED_Timer",
                                    pdMS_TO_TICKS(1000),
                                    pdTRUE,
                                    (void*)0,
                                    vLEDTimerCallback);

    if (led_timer_handle != NULL) {
        xTimerStart(led_timer_handle, 0); // Start timer daemon[cite: 1]
    }

    vTaskDelete(StartTask_Handler); // Delete startup task[cite: 2]
    taskEXIT_CRITICAL(); // Exit critical section[cite: 2]
}