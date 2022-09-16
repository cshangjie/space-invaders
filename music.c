/*
 * music.c
 *
 * Author: Ziyang Cai (zcai75@wisc.edu)
 *
 */

#include "music.h"


Note_t Song_pew[] =
{
    {PEW_NOTE, ONE_EIGTH, false},
};

Note_t Song_win[] =
{
     {NOTE_A5, ONE_HALF, true},
     {NOTE_C6, ONE_HALF, true},
     {NOTE_E6, ONE_HALF, true},
     {NOTE_A6, ONE_HALF, true},
     {NOTE_G6, ONE_HALF, true}
};

Note_t Song_lose[] =
{
    {NOTE_E7, ONE_QUARTER, true},
    {NOTE_D6, ONE_QUARTER, true},
    {NOTE_C6, ONE_QUARTER, true},
    {NOTE_B5, ONE_QUARTER, true},
    {NOTE_A5, ONE_HALF, false},
    {NOTE_A5, ONE_HALF, false}
};

Note_t Song_hit[] =
{
    {NOTE_A4, ONE_EIGTH, false},
    {NOTE_A5, ONE_EIGTH, false}
};


//***************************************************************
// This function returns how long an individual  notes is played
//***************************************************************
uint32_t music_get_time_delay(measure_time_t time)
{
    uint32_t time_return = 0;

    time_return  = MEASURE_DURATION * MEASURE_RATIO;

    switch(time)
    {
        case ONE_QUARTER:
        {
            time_return  = time_return / 4;
            break;
        }
        case ONE_HALF:
        {
            time_return  = time_return / 2;
            break;
        }
        case ONE_EIGTH:
        {
            time_return  = time_return / 8;
            break;
        }
        case THREE_EIGTH:
        {
            time_return = time_return * 3;
            time_return  = time_return / 8;
            break;
        }
        default:
        {
            break;
        }
    }

    return time_return - DELAY_AMOUNT;

}


void setup_buzzer() {
    // P2.7 is pwm output for buzzer
    // Set direction to output
    P2->DIR |= BIT7;
    // Set primary mode (pwm)
    P2->SEL0 |= BIT7;
    P2->SEL1 &= ~BIT7;

    // Disable and stop timer_A
    TIMER_A0->CTL = 0;
    // Set/Reset mode
    TIMER_A0->CCTL[4] = TIMER_A_CCTLN_OUTMOD_7;
    // Use master clock (24MHz is set using __SYSTEM_CLOCK variable defined in the startup file system_msp432p401r.c)
    TIMER_A0->CTL |= TIMER_A_CTL_SSEL__SMCLK;
}

void stop_buzzer() {
    // Disable and stop timer_A
    TIMER_A0->CTL = 0;
}

//***************************************************************************
// Plays a single note of the song based on note_index.  After
// the note is played, there is an optional delay in between
// notes.
//
// Examples
// Song[note_index].period       -- Used to determine the period of the
//                                  PWM pulse.
//
// Song[note_index].time         -- 1/4 or 1/2 time note.  Call
//                                  music_get_time_delay() to determine how
//                                  long to play the note
//
// Song[note_index].delay        -- If true, add a period of silence for
//                                  DELAY_AMOUNT
//
//                                  If false, return without delaying.
//*************************************************************************
void music_play_note(Note_t Song[], uint16_t note_index)
{
    // Set period of timer A
    TIMER_A0->CCR[0] = Song[note_index].period - 1;
    // Set pwm to be 50%
    TIMER_A0->CCR[4] = Song[note_index].period / 2 - 1;
    // make sure timer is disabled
    TIMER_A0->CTL &= ~TIMER_A_CTL_MC_MASK;
    // enable timer and clear current count value
    TIMER_A0->CTL |= TIMER_A_CTL_MC__UP | TIMER_A_CTL_CLR;
    // Wait specified clock cycles (note current playing)
    vTaskDelay(music_get_time_delay(Song[note_index].time));
    // disable timer
    TIMER_A0->CTL &= ~TIMER_A_CTL_MC_MASK;
    if (Song[note_index].delay) {
        // Delay specified cycles
        vTaskDelay(DELAY_AMOUNT);
//        ece353_T32_1_wait_100mS();
    }
}


//***************************************************************
// Plays the song (loop through, playing each note) 
// and then returns
//***************************************************************
TaskHandle_t task_music_play_song_handle;
bool buzzer_on = false;
sound_t curr_song;
void task_music_play_song(void *pvParameters)
{
    while (1) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (buzzer_on) stop_buzzer();
        setup_buzzer();
        buzzer_on = true;
        // Iterate through each note and play
//        music_play_note(3);
//        for (i = 0; i < 2; i++) {
//            music_play_note(i);
//        }
        music_play(curr_song);
        stop_buzzer();
        buzzer_on = false;
    }
}

// Play the requested song
void music_play(sound_t sound) {
    Note_t *song;
    int len;
    switch (sound) {
    case PEW:
        song = Song_pew;
        len = sizeof(Song_pew) / sizeof(Note_t);
        break;
    case WIN:
        song = Song_win;
        len = sizeof(Song_win) / sizeof(Note_t);
        break;
    case LOSE:
        song = Song_lose;
        len = sizeof(Song_lose) / sizeof(Note_t);
        break;
    case HIT:
        song = Song_hit;
        len = sizeof(Song_hit) / sizeof(Note_t);
        break;
    default:
        return;
    }
    int i;
    for (i = 0; i < len; i++) {
        music_play_note(song, i);
    }
}
