#include "ArcanoidHelper.h"

elapsedMillis paddleDynamicsTimer;
elapsedMicros random_factor;

static uint8_t initFlag;

uint32_t get_randomNumber(uint32_t maxvalue)
{
	maxvalue +=1;

	if(!initFlag)
	{
		srand(random_factor);
		initFlag=1;
	}

	return rand() % maxvalue;
}

void handle_angleModifier(ball_t *ball, paddle_t *paddle, round_params_t *roundparam)
{
	if(paddleDynamicsTimer > 100)
	{
		ball->vector.angle_modifier=paddle->travel* MAGIC_DYNAMIC_NUMBER;

		if(roundparam->ballSpeed == BALL_SPEED_MODIFIER)
		{
			if(ball->vector.angle_modifier>ANGLE_MODIFIER_LIMIT)
			{
				ball->vector.angle_modifier=ANGLE_MODIFIER_LIMIT;
			}
		}
		else if(roundparam->ballSpeed == BALL_VELOCITY)
		{
			if(ball->vector.angle_modifier > ANGLE_MODIFIER_LIMIT)
			{
				ball->vector.angle_modifier = ANGLE_MODIFIER_LIMIT;
			}
		}

		paddleDynamicsTimer =0;
	}
}

void change_vector(ball_t *ball_handle, round_params_t *round)
{
	if(ball_handle->vector.x_vect > 0)
	{
		ball_handle->vector.x_vect = round->ballSpeed;
	}
	else
	{
		ball_handle->vector.x_vect = -round->ballSpeed;
	}

	if(ball_handle->vector.y_vect > 0)
	{
		ball_handle->vector.y_vect = round->ballSpeed;
	}
	else
	{
		ball_handle->vector.y_vect = -round->ballSpeed;
	}
}

uint8_t handle_block_destroying(block_t *block_handle, game_params_t *game, round_params_t *round)
{
	if(block_handle->isDestructible)
	{
		game->currentScore +=POINTS_PER_HIT;

		if(game->currentScore > game->highestScore)
		{
			game->highestScore = game->currentScore;
		}

		block_handle->life--;
	}

	if(!block_handle->life)
	{
		block_handle->isActive=0;
		round->blocksToGo -= 1;

		return 1;
	}

	return 0;
}

float make_positive(float num)
{
	union
	{
		uint32_t ulValue;
		float fValue;
	}x;

	x.fValue=num;
	x.ulValue &= ~(0x80000000);

	return x.fValue;
}

float make_negative(float num)
{
	union
	{
		uint32_t ulValue;
		float fValue;
	}x;

	x.fValue=num;
	x.ulValue |= 0x80000000;

	return x.fValue;
}

float invert_float(float num)
{
	union
	{
		uint32_t ulValue;
		float fValue;
	}x;

	x.fValue=num;
	x.ulValue ^= 0x80000000;

	return x.fValue;
}

/*! Returns right edge point of called type*/
int16_t getRightEdge(const void *p, const basic_type_t type)
{
	if(type == tBall)
	{
		ball_t *ball = (ball_t*)p;

		return ball->xAxisAnchor + BALL_RADIUS -5;
	}
	else if(type == tPaddle)
	{
		paddle_t *paddle = (paddle_t*)p;

		return paddle->xLeftAnchor + paddle->Length + (PADDLE_LINE_WIDTH/SUBPIXELS_PER_PIXEL);
	}
	else if(type == tBlock)
	{
		block_t *block = (block_t*)p;

		return block->xLeftAnchor + BLOCK_LENGTH + (BLOCK_LINE_WIDTH/SUBPIXELS_PER_PIXEL);
	}
	else if(type == tPerk)
	{
		perk_t *perk = (perk_t*)p;

		return perk->xLeftAnchor + perk->size + (PERK_LINE_WIDTH/SUBPIXELS_PER_PIXEL);
	}
	else if(type == tBullet)
	{
		laser_bullet_t *bullet = (laser_bullet_t*)p;

		return bullet->xAnchor + BULLET_HEIGHT + (BULLET_LINE_WIDTH/SUBPIXELS_PER_PIXEL);
	}

	return 0;
}

/*! Returns left edge point of called type*/
int16_t getLeftEdge(const void *p,const  basic_type_t type)
{
	if(type == tBall)
	{
		ball_t *ball = (ball_t*)p;

		return ball->xAxisAnchor - BALL_RADIUS +5;
	}
	else if(type == tPaddle)
	{
		paddle_t *paddle = (paddle_t*)p;

		return paddle->xLeftAnchor - (PADDLE_LINE_WIDTH/SUBPIXELS_PER_PIXEL);
	}
	else if(type == tBlock)
	{
		block_t *block = (block_t*)p;

		return block->xLeftAnchor - (BLOCK_LINE_WIDTH/SUBPIXELS_PER_PIXEL);
	}
	else if(type == tPerk)
	{
		perk_t *perk = (perk_t*)p;

		return perk->xLeftAnchor - (PERK_LINE_WIDTH/SUBPIXELS_PER_PIXEL);
	}
	else if(type == tBullet)
	{
		laser_bullet_t *bullet = (laser_bullet_t*)p;

		return bullet->xAnchor - (BULLET_LINE_WIDTH/SUBPIXELS_PER_PIXEL);
	}

	return 0;
}

/*! Returns top edge point of called type*/
int16_t getTopEdge(const void *p,const basic_type_t type)
{
	if(type == tBall)
	{
		ball_t *ball = (ball_t*)p;

		return (ball->yAxisAnchor - BALL_RADIUS +5);
	}
	else if(type == tPaddle)
	{
		paddle_t *paddle = (paddle_t*)p;

		return paddle->yAxis;
	}
	else if(type == tBlock)
	{
		block_t *block = (block_t*)p;

		return block->yAxis;
	}
	else if(type == tPerk)
	{
		perk_t *perk = (perk_t*)p;

		return perk->yAxis;
	}
	else if(type == tBullet)
	{
		laser_bullet_t *bullet = (laser_bullet_t*)p;

		return bullet->yAnchor - BULLET_LENGTH;
	}

	return 0;
}

/*! Returns bottom edge point of called type*/
int16_t getBottomEdge(const void *p, const basic_type_t type)
{
	if(type == tBall)
	{
		ball_t *ball = (ball_t*)p;

		return (ball->yAxisAnchor + BALL_RADIUS - 5);
	}
	else if(type == tPaddle)
	{
		paddle_t *paddle = (paddle_t*)p;

		return paddle->yAxis + PADDLE_HEIGTH;
	}
	else if(type == tBlock)
	{
		block_t *block = (block_t*)p;

		return block->yAxis + BLOCK_HEIGHT;
	}
	else if(type == tPerk)
	{
		perk_t *perk = (perk_t*)p;

		return perk->yAxis + perk->size;
	}
	else if(type == tBullet)
	{
		laser_bullet_t *bullet = (laser_bullet_t*)p;

		return bullet->yAnchor;
	}

	return 0;
}

static corner_t getLeftDownCorner(ball_t *ball)
{
	corner_t localCorner;

	localCorner.x_axis = (ball->xAxisAnchor - BALL_RADIUS*cos(((45*M_PI)/180)));
	localCorner.y_axis = (ball->yAxisAnchor + BALL_RADIUS*sin(((45*M_PI)/180)));

	return localCorner;
}

static corner_t getRightDownCorner(ball_t *ball)
{
	corner_t localCorner;

	localCorner.x_axis = (ball->xAxisAnchor + BALL_RADIUS*cos(((45*M_PI)/180)));
	localCorner.y_axis = (ball->yAxisAnchor + BALL_RADIUS*sin(((45*M_PI)/180)));

	return localCorner;
}

/*! Check if 2 objests specified by collistion type are overlaping on each other*/
uint8_t isOverlaping(const void *p1,const  void *p2 , const collision_type_t type)
{
	if(type == ball_paddle)
	{
		ball_t *ball = (ball_t*)p1;
		paddle_t *paddle = (paddle_t*)p2;

		if(ball->isActive)
		{
			uint8_t firstCheck = 0;

			firstCheck = (getRightEdge(paddle,tPaddle) >= getLeftEdge(ball,tBall)) && (getLeftEdge(paddle,tPaddle) <= getRightEdge(ball,tBall))
							&& (getTopEdge(paddle, tPaddle) <= getBottomEdge(ball, tBall));
			if(firstCheck)
			{
				uint8_t secondCheck = 0;
				corner_t localLeftDownCorner;
				corner_t localRightDownCorner;

				localLeftDownCorner = getLeftDownCorner(ball);
				localRightDownCorner = getRightDownCorner(ball);

				//Left down corner check
				//
				if((getRightEdge(paddle,tPaddle) > localLeftDownCorner.x_axis) && (getLeftEdge(paddle,tPaddle) <= localLeftDownCorner.x_axis)
						&& (getTopEdge(paddle, tPaddle) <= localLeftDownCorner.y_axis))
				{
					secondCheck = 1;
				}

				//Right down corner check
				//
				if((getRightEdge(paddle,tPaddle) > localRightDownCorner.x_axis) && (getLeftEdge(paddle,tPaddle) <= localRightDownCorner.x_axis)
						&& (getTopEdge(paddle, tPaddle) <= localRightDownCorner.y_axis))
				{
					secondCheck = 1;
				}

				firstCheck = secondCheck;

			}

			return firstCheck;
		}
	}
	else if(type == ball_block)
	{
		ball_t *ball = (ball_t*)p1;
		block_t *block = (block_t*)p2;

		if(ball->isActive && block->isActive)
		{

			return (getRightEdge(block,tBlock) >= getLeftEdge(ball,tBall)) && (getLeftEdge(block,tBlock) <= getRightEdge(ball,tBall))
					&& (getBottomEdge(block, tBlock) >= getTopEdge(ball, tBall)) && (getTopEdge(block, tBlock) <= getBottomEdge(ball, tBall));
		}
	}
	else if(type == perk_paddle)
	{
		perk_t *perk = (perk_t*)p1;
		paddle_t *paddle = (paddle_t*)p2;

		if(perk->isActive)
		{
			return (getRightEdge(paddle,tPaddle) >= getLeftEdge(perk,tPerk)) && (getLeftEdge(paddle,tPaddle) <= getRightEdge(perk,tPerk))
					&& (getBottomEdge(paddle, tPaddle) >= getTopEdge(perk, tPerk)) && (getTopEdge(paddle, tPaddle) <= getBottomEdge(perk, tPerk));
		}
	}
	else if(type == bullet_block)
	{
		laser_bullet_t *bullet = (laser_bullet_t*)p1;
		block_t *block = (block_t*)p2;

		if(block->isActive && bullet->isActive)
		{
			return (getRightEdge(block,tBlock) >= getLeftEdge(bullet,tBullet)) && (getLeftEdge(block,tBlock) <= getRightEdge(bullet,tBullet))
						&& (getBottomEdge(block, tBlock) >= getTopEdge(bullet, tBullet)) && (getTopEdge(block, tBlock) <= getBottomEdge(bullet, tBullet));
		}
	}
	return 0;
}




