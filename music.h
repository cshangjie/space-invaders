/*
 * music.h
 *
 *      Author: Ziyang Cai (zcai75@wisc.edu)
 */

#ifndef MUSIC_H_
#define MUSIC_H_

#include "main.h"

//*****************************************************************************
// You will need to determine the number of SMCLKs it will
// take for the following notes and thier associated frequencies.
//
// Assume that your MCU is running at 24MHz.
//*****************************************************************************
#define NEW_NOTE       50000
#define LOSE_NOTE1     22368
#define LOSE_NOTE2     22368
#define LOSE_NOTE3     27272
#define PEW_NOTE       21641
#define NOTE_A4        54545 // 440 Hz
#define NOTE_A5        27272 // 880 Hz
#define NOTE_A5S       22368 // 932 Hz
#define NOTE_B5        24291 // 988 Hz
#define NOTE_C6        22944 // 1046 Hz
#define NOTE_C6S       21641 // 1109 Hz
#define NOTE_D6        20425 // 1175 Hz
#define NOTE_D6S       19277 // 1245 Hz
#define NOTE_E6        18195 // 1319 Hz
#define NOTE_F6        17179 // 1397 Hz
#define NOTE_F6S       16216 // 1480 Hz
#define NOTE_G6        15306 // 1568 Hz
#define NOTE_G6S       14449 // 1661 Hz
#define NOTE_A6        13636 // 1760 Hz
#define NOTE_A6S       12868 // 1865 Hz
#define NOTE_B6        12145 // 1976 Hz
#define NOTE_C7        11466 // 2093 Hz
#define NOTE_C7S       10825 // 2217 Hz
#define NOTE_D7        10217 // 2349 Hz
#define NOTE_D7S       9642 // 2489 Hz
#define NOTE_E7        9101 // 2637 Hz
#define NOTE_F7        8589 // 2794 Hz
#define NOTE_F7S       8108 // 2960 Hz
#define NOTE_G7        7653 // 3136 Hz
#define NOTE_G7S       7224 // 3322 Hz

//*****************************************************************************
// DO NOT MODIFY ANYTHING BELOW
//*****************************************************************************
#define MEASURE_DURATION pdMS_TO_TICKS(300)    // 500 milliseconds
#define MEASURE_RATIO           2    // 2/4 time
#define DELAY_AMOUNT     pdMS_TO_TICKS(10)    // 10  milliseconds
#define SONG_NUM_NOTES 28

typedef enum measure_time_t {
    ONE_QUARTER,
    ONE_HALF,
    ONE_EIGTH,
    THREE_EIGTH
} measure_time_t;

typedef struct{
    uint32_t period;
    measure_time_t time;
    bool delay;
}Note_t;

void setup_buzzer(void);

//***************************************************************
//***************************************************************
void music_init(void);

//***************************************************************
//***************************************************************
extern TaskHandle_t task_music_play_song_handle;
void task_music_play_song(void *pvParameters);
typedef enum {
    PEW,
    WIN,
    LOSE,
    HIT
} sound_t;
extern sound_t curr_song;
void music_play(sound_t sound);

#endif /* MUSIC_H_ */
