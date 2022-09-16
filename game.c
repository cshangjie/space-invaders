/*
 * game.c
 *
 *  Created on: Apr 30, 2022
 *      Author: Ziyang_Cai
 */

#include "game.h"

TaskHandle_t task_game_handle;
QueueHandle_t queue_game_actions;
SemaphoreHandle_t Sem_LCD;
bool lightMode = 1;
bool force_next_refresh = 0;

bool game_started = false;
struct Entity enemies[16];
struct Entity player;
struct Entity bullets[10];

int enemeyArrLen = sizeof enemies / sizeof enemies[0];

int player_speed = 6;
int enemy_speed = 1;
int curr_lvl = 0;
void initialize_enemies();
void initialize_player();
void initialize_bullets();
void shootBullet();
/******************************************************************************
* Task used to print out messages to the console
******************************************************************************/
void task_game(void *pvParameters) {
    BaseType_t xHigherPriorityTaskWoken;
    while (1) {
        game_actions_t action;
        xSemaphoreTake(Sem_LCD, portMAX_DELAY);
        xQueueReceive(queue_game_actions, &action, portMAX_DELAY);
        int i;
        switch (action) {
            case SHAKE:
//                printf("shake");
                if (game_started && player.lives < 1) {
                    player.lives += 1;
                    force_next_refresh = true;
                }
                if (game_started){
                    for(i = 0; i < enemeyArrLen; i++){
                        if(enemies[i].deleted == false){
                            enemies[i].y_loc -= 30;
                            drawEntity(&enemies[i]);
                        }
                    }
                }
                break;
            case JOYSTICK_DIR_LEFT:
//                printf("left");
                if (player.x_loc > (shipWidthPixels >> 1))
                    player.x_loc -= player_speed;
                break;
            case JOYSTICK_DIR_RIGHT:
//                printf("right");
                if (player.x_loc < 129 - (shipWidthPixels >> 1))
                    player.x_loc += player_speed;
                break;
            case JOYSTICK_DIR_DOWN:
//                printf("down");
                if (player.y_loc < 129 - (shipHeightPixels >> 1))
                    player.y_loc += player_speed;
                break;
            case JOYSTICK_DIR_UP:
//                printf("up");
                if (player.y_loc > (shipHeightPixels >> 1))
                    player.y_loc -= player_speed;
                break;
            case JOYSTICK_DIR_CENTER:
//                printf("center");
                break;
            case IS_DARK: // set to dark mode
//                printf("dark");
                lightMode = 0;
                if (game_started) {
                    lcd_Clear(lightMode);
                    force_next_refresh = 1;
                } else {
                    lcd_drawStartingScreen(lightMode);
                }
                break;
            case IS_LIGHT: // set to light mode
//                printf("light");
                lightMode = 1;
                if (game_started) {
                    lcd_Clear(lightMode);
                    force_next_refresh = 1;
                } else {
                    lcd_drawStartingScreen(lightMode);
                }
                break;
            case S2_PRESSED:
//                printf("S2 pressed");
                if (game_started) {
                    shootBullet();
                    curr_song = PEW;
                    xTaskNotifyGive(task_music_play_song_handle);
                    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
                }
                break;
            case S2_RELEASED:
//                printf("S2 released");
                break;
            case S1_PRESSED:
//                printf("S1 pressed");
                if (!game_started) {
                    game_actions_t action = GAME_START;
                    xQueueSendToBack(queue_game_actions, &action, portMAX_DELAY);
                } else {
                    game_actions_t action = GAME_END;
                    xQueueSendToBack(queue_game_actions, &action, portMAX_DELAY);
                }
                break;
            case S1_RELEASED:
//                printf("S1 released");
                break;
            case GAME_TICK:
                if (game_started) {
//                    printf("Tick");

                    // update any existing bullet entity locations
                    uint8_t b = 0;
                    for (b = 0; b < 10; b++) {
                        if(bullets[b].deleted == false){
                            bullets[b].x_prev_loc = bullets[b].x_loc;
                            bullets[b].y_prev_loc = bullets[b].y_loc;
                            if (bullets[b].y_loc - 8 <= (pewHeightPixels >> 1))
                                deleteEntity(&bullets[b]);
                            else {
                                bullets[b].y_loc -= 8;
                                drawEntity(&bullets[b]);
                            }
                        }
                    }
                    // update enemy entity locations
                    for (i = 0; i < enemeyArrLen; i++) {
                        if (enemies[i].deleted == false) {
                            enemies[i].y_prev_loc = enemies[i].y_loc;
                            enemies[i].y_loc += enemy_speed;
                            drawEntity(&enemies[i]);
                        }
                    }
                    drawEntity(&player);
                    force_next_refresh = false;
                    // collision checks for bullets & enemies
                    for(b = 0; b < 10; b++){
                        if(bullets[b].deleted == false){
                            for(i = 0; i < enemeyArrLen; i++){
                                // check if bullet is within x bounds of the current enemy entity
                                if(enemies[i].deleted == false){
                                    if((bullets[b].x_loc >= enemies[i].x_loc - 10) && (bullets[b].x_loc <= enemies[i].x_loc + 10)){
                                        // check if the bullet is colliding with the bottom of the alien
                                        if(abs((int)(bullets[b].y_loc - enemies[i].y_loc)) < 9){
                                            if(enemies[i].lives == 0){
                                                deleteEntity(&enemies[i]);
                                            }else{
                                                enemies[i].lives -= 1;
                                            }
                                            // delete both entities
                                            deleteEntity(&bullets[b]);
                                            force_next_refresh = true;
                                            curr_song = HIT;
                                            xTaskNotifyGive(task_music_play_song_handle);
                                            break;
                                        }
                                    }
                                }
                            }
                            continue;
                        }
                    }
                    // collision checks for enemies and player
                    for(i = 0; i < enemeyArrLen; i++){
                        if(enemies[i].deleted == false){
                            // check if enemy below threshold
                            if((enemies[i].y_loc >= 122)){
                                // End game
                                game_actions_t action = GAME_END;
                                xQueueSendToBack(queue_game_actions, &action, portMAX_DELAY);
                                // Play losing sound queue
                                curr_song = LOSE;
                                xTaskNotifyGive(task_music_play_song_handle);
                                portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
                            }
                            // if colliding
                            if((abs(player.y_loc - enemies[i].y_loc) <= 9) && (abs(player.x_loc - enemies[i].x_loc) <= 20)){
                                // check lives
                                if(player.lives == 0){
                                    // end game
                                    game_actions_t action = GAME_END;
                                    xQueueSendToBack(queue_game_actions, &action, portMAX_DELAY);
                                    // play losing sound
                                    curr_song = LOSE;
                                    xTaskNotifyGive(task_music_play_song_handle);
                                    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
                                }
                                else{
                                    // update live count
                                    player.lives -= 1;
                                    // delete enemy
                                    deleteEntity(&enemies[i]);
                                    force_next_refresh = true;
                                }
                            }
                        }
                    }
                }
                break;
            case GAME_START:
                // start game and default all values of entities
                // clear screen and draw the initialized entities
                game_started = true;
                lcd_Clear(lightMode);
                initialize_player(&player);
                curr_lvl = 0;
                drawEntity(&player);
                initialize_bullets();
                game_actions_t action = NEXT_LVL;
                xQueueSendToBack(queue_game_actions, &action, portMAX_DELAY);
                break;
            case GAME_END:
                // set game state to ended and reset the screen
                game_started = false;
                lcd_drawStartingScreen(lightMode);
                break;
            case NEXT_LVL:
                if (curr_lvl >= 3) {
                    // If at the last level, check if all enemies are killed
                    bool enemies_died = true;
                    int i;
                    for (i = 11 ;i < 16; i++) {
                        if (enemies[i].deleted == false)
                            enemies_died = false;
                    }
                    if (enemies_died) {
                        // Game win if all enemies are killed
                        curr_song = WIN;
                        // play winning sound
                        xTaskNotifyGive(task_music_play_song_handle);
                        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
                    } else {
                        // if enemies are all killed, wait a bit longer
                        break;
                    }
                    game_actions_t action = GAME_END;
                    xQueueSendToBack(queue_game_actions, &action, portMAX_DELAY);
                    break;
                }
                // Spawn enemies
                initialize_enemies(curr_lvl);
                for (i = 0; i < 5; i++) {
                    drawEntity(&enemies[i]);
                }
                // Set next level
                if (curr_lvl <= 2) curr_lvl += 1;
                break;
        }
//        printf("\r\n");
        xSemaphoreGive(Sem_LCD);
    }
}

void initialize_player(struct Entity *ent) {
    // Initialize player entity at bottom center of screen
    ent->height = 10;
    ent->width = 10;
    ent->type = PLAYER;
    ent->x_loc = 63;
    ent->y_loc = 127;
    ent->x_prev_loc = -1;
    ent->y_prev_loc = -1;
    ent->deleted = false;
    ent->lives = 0;
    int i;
    // Delete the enemies at game start
    for (i = 0; i < 16; i++) {
        enemies[i].deleted = true;
    }
}

void initialize_enemies(int lvl) {
    //Place enemies on the screen based on different levels
    uint8_t i = 0;
    switch (lvl) {
    case 0:
        for (i = 0; i < 5; i++) {
            enemies[i].height = enemyHeightPixels;
            enemies[i].width = enemyWidthPixels;
            enemies[i].type = ENEMY;
            enemies[i].x_loc = (enemyWidthPixels >> 1) + i * (20+2) + 10;
            enemies[i].y_loc = 30;
            enemies[i].x_prev_loc = -1;
            enemies[i].y_prev_loc = -1;
            enemies[i].deleted = false;
            enemies[i].lives = 0;
        }
        break;
    case 1:
        for (i = 5; i < 11; i++) {
            enemies[i].height = enemyHeightPixels;
            enemies[i].width = enemyWidthPixels;
            enemies[i].type = ENEMY;
            enemies[i].x_loc = 30+ ((i-5) % 2) * (60);
            enemies[i].y_loc = ((i-5)>>1) * (20);
            enemies[i].x_prev_loc = -1;
            enemies[i].y_prev_loc = -1;
            enemies[i].deleted = false;
            enemies[i].lives = 1;
        }
        break;
    case 2:
        for (i = 11; i < 16; i++) {
            enemies[i].height = enemyHeightPixels;
            enemies[i].width = enemyWidthPixels;
            enemies[i].type = ENEMY;
            enemies[i].x_loc = (enemyWidthPixels >> 1) + (i-11) * (20+2) + 10;
            if ((i-11) <= 2)
                enemies[i].y_loc = (i-11) * (20) - 20;
            else
                enemies[i].y_loc = 100 - (i-11) * (20)-20;
            enemies[i].x_prev_loc = -1;
            enemies[i].y_prev_loc = -1;
            enemies[i].deleted = false;
            enemies[i].lives = 2;
        }
        break;
    }
}

void initialize_bullets() {
    // Initailize the button (hidden at the start)
    uint8_t i = 0;
    for (i = 0; i < 10; i++) {
        bullets[i].height = 20;
        bullets[i].width = 20;
        bullets[i].type = BULLET;
        bullets[i].x_loc = 0;
        bullets[i].y_loc = 0;
        bullets[i].x_prev_loc = -1;
        bullets[i].y_prev_loc = -1;
        bullets[i].deleted = true;
    }
}

TaskHandle_t task_game_tick_handle;
int lvl_timer = 0;

void task_game_tick(void *pvParameter) {
    // create a 50ms clock for game rerenders and a 4 second clock for new level spawn
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    const TickType_t freq = pdMS_TO_TICKS(50);

    while(1) {
        if (game_started)
            lvl_timer += 1;
        else
            lvl_timer =0;
        if (lvl_timer == 80) {
            lvl_timer = 0;
            game_actions_t action = NEXT_LVL;
            xQueueSendToBack(queue_game_actions, &action, portMAX_DELAY);
        }
        game_actions_t action = GAME_TICK;
        xQueueSendToBack(queue_game_actions, &action, portMAX_DELAY);
        xTaskDelayUntil(&xLastWakeTime, freq);
    }
}

/*******************************************************************************
 * Function Name: drawEntity
 ********************************************************************************
 * Summary: draws the entity and updates the previous location after drawing
 * Returns: Nothing
 *******************************************************************************/
void drawEntity(struct Entity *ent){
    uint16_t imageColor;
    uint16_t backgroundColor;
    const uint8_t *image;
    uint8_t imageWidth;
    uint8_t imageHeight;
    // choose the appropriate colors based on lightMode flag and models
    // choose the appropriate bitmap
    if(ent->type == PLAYER){
        image = shipBitmaps;
        imageWidth = shipWidthPixels;
        imageHeight = shipHeightPixels;
        int colors[] = {LCD_COLOR_GREEN, LCD_COLOR_BLUE};
        imageColor = colors[ent->lives];
        if(lightMode){
            backgroundColor = LCD_COLOR_WHITE;
        }else{
            backgroundColor = LCD_COLOR_BLACK;
        }
    }
    else if(ent->type == ENEMY){
        image = enemyBitmaps;
        imageWidth = enemyWidthPixels;
        imageHeight = enemyHeightPixels;
        int colors[] = {LCD_COLOR_RED, LCD_COLOR_ORANGE, LCD_COLOR_MAGENTA};
        imageColor = colors[ent->lives];
        if(lightMode){
            backgroundColor = LCD_COLOR_WHITE;
        }else{
            backgroundColor = LCD_COLOR_BLACK;
        }
    }
    else if(ent->type == BULLET){
        image = pewBitmaps;
        imageWidth = pewWidthPixels;
        imageHeight = pewHeightPixels;
        imageColor = LCD_COLOR_YELLOW;
        if(lightMode){
            backgroundColor = LCD_COLOR_WHITE;
        }else{
            backgroundColor = LCD_COLOR_BLACK;
        }
    }
    // Entity safety checks
    if((ent->x_loc < 0) || (ent->y_loc < 0)){
        printf("Invalid current position.");
        return;
    }
    // check if entity is newly instantiated and needs to be drawn
    if((ent->x_prev_loc == -1) & (ent->y_prev_loc == -1) & (ent->deleted == false)){
        // don't mask anything except the backgrounds
        lcd_draw_image((uint16_t) ent->x_loc, (uint16_t) ent->y_loc, imageWidth, imageHeight, image, imageColor, backgroundColor);
        ent->x_prev_loc = ent->x_loc;
        ent->y_prev_loc = ent->y_loc;
    }
    // check if entity has moved needs to be redrawn
    if((force_next_refresh || (ent->x_loc != ent->x_prev_loc) || (ent->y_loc != ent->y_prev_loc)) & (ent->deleted == false)){
        //mask old position - draw rectangle at old position or draw old entity in background color
        lcd_draw_rectangle((uint16_t)ent->x_prev_loc, (uint16_t)ent->y_prev_loc, imageWidth, imageHeight, backgroundColor);
        // draw new position
        lcd_draw_image((uint16_t) ent->x_loc, (uint16_t) ent->y_loc, imageWidth, imageHeight, image, imageColor, backgroundColor);
        // update prev loc to current loc
        ent->x_prev_loc = ent->x_loc;
        ent->y_prev_loc = ent->y_loc;
    }
}
/*******************************************************************************
 * Function Name: deleteEntity
 ********************************************************************************
 * Summary: masks over the deleted entity and updates the deleted status
 * Returns: Nothing
 *******************************************************************************/
void deleteEntity(struct Entity *ent){
    uint16_t backgroundColor;
    uint8_t imageWidth;
    uint8_t imageHeight;
    // set the image values
    switch(ent->type){
    case ENEMY:
        imageWidth = enemyWidthPixels;
        imageHeight = enemyHeightPixels;
        if(lightMode){
            backgroundColor = LCD_COLOR_WHITE;
        }else{
            backgroundColor = LCD_COLOR_BLACK;
        }
        break;
    case PLAYER:
        imageWidth = shipWidthPixels;
        imageHeight = shipHeightPixels;
        if(lightMode){
            backgroundColor = LCD_COLOR_WHITE;
        }else{
            backgroundColor = LCD_COLOR_BLACK;
        }
        break;
    case BULLET:
        imageWidth = pewWidthPixels;
        imageHeight = pewHeightPixels;
        if(lightMode){
            backgroundColor = LCD_COLOR_WHITE;
        }else{
            backgroundColor = LCD_COLOR_BLACK;
        }
        break;
    }
    // mask the entity on lcd
    lcd_draw_rectangle((uint16_t) ent->x_loc, (uint16_t) ent->y_loc, imageWidth, imageHeight, backgroundColor);
    // update the entity's deleted status
    ent->deleted = true;
}
void shootBullet(){
    // iterate over list of bullet entities
    uint8_t i = 0;
    for(i = 0; i < 10; i++){
        // check for deleted entities
        if(bullets[i].deleted == true){
            // update bullet attributes and draw bullet right above player
            bullets[i].x_loc = player.x_loc;
            bullets[i].y_loc = player.y_loc - (int8_t)10;
            bullets[i].deleted = false;
            drawEntity(&bullets[i]);
            return;
        }
    }
}
