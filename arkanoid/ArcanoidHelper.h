#ifndef SOURCE_ARCANOIDHELPER_H_
#define SOURCE_ARCANOIDHELPER_H_

#include "Arduino.h"
#include "Arcanoid.h"

uint32_t get_randomNumber(uint32_t maxvalue);

void handle_angleModifier(ball_t *ball, paddle_t *paddle, round_params_t *roundparam);

void change_vector(ball_t *ball_handle, round_params_t *round);

uint8_t handle_block_destroying(block_t *block_handle, game_params_t *game, round_params_t *round);

float invert_float(float num);
float make_positive(float num);
float make_negative(float num);

int16_t getRightEdge(const void *p, const basic_type_t type);
int16_t getLeftEdge(const void *p, const basic_type_t type);
int16_t getBottomEdge(const void *p, const basic_type_t type);
int16_t getTopEdge(const void *p, const basic_type_t type);

uint8_t isOverlaping(const void *p1, const void *p2 , const collision_type_t type);


#endif
