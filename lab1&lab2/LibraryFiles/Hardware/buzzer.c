#include "buzzer.h"
#include "FreeRTOS.h"
#include "task.h"
#include "delay.h" // Microsecond delays[cite: 1]

// Appendix B Tone Frequencies[cite: 1]
static u32 notes[] = {
    2272, 2024, 3822, 3401, 3030, 2865, 2551,
    1136, 1012, 1912, 1703, 1517, 1432, 1275
};

// Appendix B Song String[cite: 1]
uint8_t default_song[] = "e2,d2,e2,d2,e2,B2,d2,c2,A2_C2,E2,A2,B2_E2,G2,B2,c4_E2,e2,d2,e2,d2,e2,B2,d2,c2,A2_C2,E2,A2,B2_E2,c2,B2,A4";

void Buzzer_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(BUZZER_CLK, ENABLE);

    GPIO_InitStructure.GPIO_Pin = BUZZER_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_Init(BUZZER_PORT, &GPIO_InitStructure);

    BUZZER_OFF();
}

uint32_t getNote(uint8_t ch) {
    if (ch >= 'A' && ch <= 'G') return notes[ch - 'A'];
    if (ch >= 'a' && ch <= 'g') return notes[ch - 'a' + 7];
    return 0;
}

uint32_t getDuration(uint8_t ch) {
    if (ch < '0' || ch > '9') return 500;
    return (ch - '0') * 250;
}

uint32_t getPause(uint8_t ch) {
    switch (ch) {
        case '+': return 0;
        case ',': return 5;
        case '.': return 20;
        case ' ': return 30;
        default: return 5;
    }
}

void playNote(uint32_t note, uint32_t durationMs) {
    uint32_t t = 0;
    if (note > 0) {
        while (t < (durationMs * 1000)) {
            BUZZER_ON();
            delay_us(note / 2);
            BUZZER_OFF();
            delay_us(note / 2);
            t += note;
        }
    } else {
        delay_xms(durationMs);
    }
}

void playSong(uint8_t *song) {
    uint32_t note = 0, dur = 0, pause = 0;
    while (*song != '\0') {
        note = getNote(*song++);
        if (*song == '\0') break;
        dur = getDuration(*song++);
        if (*song == '\0') break;
        pause = getPause(*song++);
        
        playNote(note, dur);
        delay_us(pause);
    }
}