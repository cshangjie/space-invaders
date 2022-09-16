/*
 * game.h
 *
 *  Created on: Apr 30, 2022
 *      Author: Ziyang_Cai
 */

#ifndef GAME_H_
#define GAME_H_
#include "main.h"

extern QueueHandle_t queue_game_actions;
extern TaskHandle_t task_game_handle;
extern TaskHandle_t task_game_tick_handle;
extern int gameState;
typedef enum {
    GAME_TICK,
    GAME_START,
    GAME_END,
    SHAKE,
    JOYSTICK_DIR_CENTER,
    JOYSTICK_DIR_LEFT,
    JOYSTICK_DIR_RIGHT,
    JOYSTICK_DIR_UP,
    JOYSTICK_DIR_DOWN,
    IS_DARK,
    IS_LIGHT,
    S2_PRESSED,
    S2_RELEASED,
    S1_PRESSED,
    S1_RELEASED,
    NEXT_LVL
} game_actions_t;

typedef enum {
    PLAYER,
    ENEMY,
    BULLET
} entity_t;

struct Entity {
    int8_t x_loc;
    int8_t y_loc;
    int8_t x_prev_loc;
    int8_t y_prev_loc;
    int8_t width;
    int8_t height;
    entity_t type;
    bool deleted;
    int8_t lives;
};

void task_game(void *pvParameters);
void task_game_tick(void *pvParameter);

void drawEntity(struct Entity *ent);
void deleteEntity(struct Entity *ent);

#endif /* GAME_H_ */
