#ifndef SOURCE_ARCANOID_DISPLAY_H_
#define SOURCE_ARCANOID_DISPLAY_H_

#include "Arcanoid.h"

void gameDisplayBegin();
void gameDisplayFinish();

void entryScreen();
void gameoverScreen();


void showBlocks(block_t *block_handle);
void moveBall(ball_t *handle_ball);
void moveBar(paddle_t *handle_bar);
void movePerk(perk_t *handle_perk);
void moveLaserBullet(laser_bullet_t *bullet_handler,paddle_t *handle_bar);


void updateSideTable(game_params_t *params, round_params_t *roundparams);




#endif
