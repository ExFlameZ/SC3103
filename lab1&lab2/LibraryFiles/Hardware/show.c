#include "show.h"
#include <stdio.h>

// Global elapsed time counter (in seconds)
static uint32_t total_seconds = 0;

void show_task(void *pvParameters)
{
   u32 lastWakeTime = getSysTickCnt();
   
   // Initialize OLED display hardware once when task starts
   OLED_Init();
   OLED_Clear();

   while(1)
   {	
       // Maintain precise 50 Hz task rate (20 ms period)
       vTaskDelayUntil(&lastWakeTime, F2T(RATE_50_HZ));

       // Update OLED buffer and refresh hardware display
       oled_show();
   }
}  

void oled_show(void)
{  
   static uint16_t tick_counter = 0;
   uint32_t minutes = 0;
   uint32_t seconds = 0;
   char octal_str[20];

   // Display Header Text
   OLED_ShowString(0, 0, (const uint8_t *)"Octal Clock");

   // Increment total elapsed time by 1 second every 50 task ticks (50 * 20 ms = 1000 ms)
   tick_counter++;
   if(tick_counter >= 50)
   {
       total_seconds++;
       tick_counter = 0;
   }

   // Calculate real-time minutes and seconds
   minutes = total_seconds / 60;
   seconds = total_seconds % 60;

   // Format as Base-8 (Octal) using %02lo specifier
   sprintf(octal_str, "%02lo:%02lo", minutes, seconds);

   // Render elapsed Octal time on line 2
   OLED_ShowString(0, 20, (const uint8_t *)octal_str);

   // Flush display memory buffer to OLED screen
   OLED_Refresh_Gram();
}