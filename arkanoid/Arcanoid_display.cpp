#include "Arcanoid_display.h"
#include "FT812.h"
#include "FT8xx.h"
#include "ArcanoidHelper.h"

const uint32_t block_colors[] ={0xFFFFFF,0x00FF00,0xFFFF00,0xFF0000,0x0000FF,0x696969};
const uint32_t perks_colors[] ={0x00FF00,0xFF0000,0x0000FF,0xFFFF00,0x00FFFF,0xFF00FF,0xAABBCC};

const char perks_letter[PERKS_NUM][2] ={{"E"},{"R"},{"M"},{"G"},{"S"},{"L"},{"X"}};

extern game_params_t game;


void gameDisplayBegin()
{
	API_LIB_BeginCoProList();
	API_CMD_DLSTART();
	API_VERTEX_FORMAT(0);
	API_CLEAR_COLOR(0);
	API_CLEAR(1,1,1);
}

void gameDisplayFinish()
{
	API_DISPLAY();
	API_CMD_SWAP();
	API_LIB_EndCoProList();
}

void entryScreen()
{
	gameDisplayBegin();

	API_LINE_WIDTH(40);
	API_COLOR(0xFFFFFF);

	API_BEGIN(RECTS);

	API_LINE_WIDTH(BLOCK_LINE_WIDTH);
	API_COLOR(0xFFFFFF);

	API_VERTEX2F(70, 240);
	API_VERTEX2F(118, 256);
	API_VERTEX2F(126, 240);
	API_VERTEX2F(174, 256);
	API_VERTEX2F(126, 216);
	API_VERTEX2F(174, 232);

	API_VERTEX2F(626, 240);
	API_VERTEX2F(674, 256);
	API_VERTEX2F(626, 216);
	API_VERTEX2F(674, 232);
	API_VERTEX2F(682, 240);
	API_VERTEX2F(730, 256);

	API_END();

	API_CMD_TEXT(400, 210 , 30, OPT_CENTERX, "POLYEND ARKANOID GAME");
	API_CMD_TEXT(400, 245 , 27, OPT_CENTERX, "(Press SHIFT to begin)");

	gameDisplayFinish();

}

void gameoverScreen()
{
	gameDisplayBegin();

	API_COLOR(0xFFFFFF);
	API_CMD_TEXT(400, 210 , 30, OPT_CENTERX, "GAME OVER");
	API_CMD_TEXT(400, 245 , 27, OPT_CENTERX, "(Press SHIFT to start over)");

	gameDisplayFinish();
}


void showBlocks(block_t *block_handle)
{
	if(block_handle->isActive)
	{
		API_BEGIN(RECTS);

		API_LINE_WIDTH(BLOCK_LINE_WIDTH);
		API_COLOR(block_colors[(block_handle->life -1)]);

		API_VERTEX2F(block_handle->xLeftAnchor, block_handle->yAxis);
		API_VERTEX2F(block_handle->xLeftAnchor + BLOCK_LENGTH, block_handle->yAxis + BLOCK_HEIGHT);

		API_END();
	}
}

void moveBall(ball_t *handle_ball)
{
	if(handle_ball->isActive)
	{
		API_COLOR_RGB(128, 0, 0);
		API_POINT_SIZE(BALL_RADIUS*10);

		API_BEGIN(FTPOINTS);

		if(game.pause == running)
		{
			handle_ball->xAxisAnchor += handle_ball->vector.x_vect;
			handle_ball->yAxisAnchor += handle_ball->vector.y_vect;
		}

		API_VERTEX2F(handle_ball->xAxisAnchor,handle_ball->yAxisAnchor);

		API_END();
	}
}

void moveBar(paddle_t *handle_bar)
{
	API_COLOR(0xFFFFFF);

	API_BEGIN(RECTS);

	API_LINE_WIDTH(PADDLE_LINE_WIDTH);

	if(game.pause == running)
	{
		if(handle_bar->travel >= 16 || handle_bar->travel <= -16)
		{
			if(handle_bar->travel>0)
			{
				handle_bar->xLeftAnchor += 16;
				handle_bar->travel -= 16;
				if(handle_bar->xLeftAnchor + handle_bar->Length > PLAY_AREA_WIDTH)
				{
					handle_bar->xLeftAnchor = PLAY_AREA_WIDTH - handle_bar->Length;
					handle_bar->travel=0;
				}

			}
			else
			{
				handle_bar->xLeftAnchor -=16;
				handle_bar->travel +=16;
				if(handle_bar->xLeftAnchor < 0)
				{
					handle_bar->xLeftAnchor = 0;
					handle_bar->travel=0;
				}
			}
		}
	}

	API_VERTEX2F(handle_bar->xLeftAnchor, handle_bar->yAxis);
	API_VERTEX2F(handle_bar->xLeftAnchor + handle_bar->Length, handle_bar->yAxis + PADDLE_HEIGTH);

	if(handle_bar->visual==armedClassic)
	{
		uint16_t xLeftCanonPosition = handle_bar->xLeftAnchor + CANON_EDGE_OFFSET;
		uint16_t xRightCanonPosition = (handle_bar->xLeftAnchor + handle_bar->Length) - CANON_EDGE_OFFSET;

		API_VERTEX2F(xLeftCanonPosition, handle_bar->yAxis+1);
		API_VERTEX2F(xLeftCanonPosition + CANON_LENGTH, handle_bar->yAxis - CANON_HEIGTH);

		API_VERTEX2F(xRightCanonPosition - CANON_LENGTH, handle_bar->yAxis+1);
		API_VERTEX2F(xRightCanonPosition, handle_bar->yAxis - CANON_HEIGTH);
	}



	API_END();
}

void movePerk(perk_t *handle_perk)
{
	if(handle_perk->isActive)
	{
		uint16_t xMiddle;
		uint16_t yMiddle;

		API_BEGIN(RECTS);
		API_COLOR(perks_colors[handle_perk->perkType]);

		API_LINE_WIDTH(PERK_LINE_WIDTH);

		xMiddle= handle_perk->xLeftAnchor + (handle_perk->size/2) ;
		yMiddle= handle_perk->yAxis - (PERK_LINE_WIDTH/SUBPIXELS_PER_PIXEL)/2;

		API_VERTEX2F(handle_perk->xLeftAnchor, handle_perk->yAxis);
		API_VERTEX2F(handle_perk->xLeftAnchor + handle_perk->size, handle_perk->yAxis + handle_perk->size);

		if(game.pause == running)
		{
			handle_perk->yAxis += PERK_VELOCITY;
		}

		API_END();

		API_COLOR_RGB(255, 255, 255);
		API_CMD_TEXT(xMiddle, yMiddle, 27, OPT_CENTERX, &perks_letter[handle_perk->perkType-1][0]);
	}
}

void updateSideTable(game_params_t *params, round_params_t *roundparams)
{
	char value[20];

	API_COLOR(0xFFFFFF);

	API_BEGIN(RECTS);

	API_LINE_WIDTH(40);
	API_COLOR(0xFFFFFF);

	if(roundparams->nextLevelEntry)
	{
		API_VERTEX2F(PLAY_AREA_WIDTH, 0);
		API_VERTEX2F(PLAY_AREA_WIDTH+SIDE_TABLE_LINE_WIDTH, PLAY_AREA_HEIGHT - NEW_LEVEL_ENTRY_SIZE);
	}
	else
	{
		API_VERTEX2F(PLAY_AREA_WIDTH, 0);
		API_VERTEX2F(PLAY_AREA_WIDTH+SIDE_TABLE_LINE_WIDTH, PLAY_AREA_HEIGHT);
	}

	if(params->lifes >= 1)
	{
		API_VERTEX2F(624, 460);
		API_VERTEX2F(674, 470);

		if(params->lifes >=2)
		{
			API_VERTEX2F(682, 460);
			API_VERTEX2F(732, 470);

			if(params->lifes >=3)
			{
				API_VERTEX2F(740, 460);
				API_VERTEX2F(790, 470);
			}

		}
	}

	API_END();

	itoa(params->level, value, 10);

	API_CMD_TEXT(SIDE_TABLE_CENTER, 190 , 29, OPT_CENTERX, "LEVEL:");
	API_CMD_TEXT(SIDE_TABLE_CENTER, 220 , 29, OPT_CENTERX, value);

	itoa(params->currentScore, value, 10);

	API_CMD_TEXT(SIDE_TABLE_CENTER, 310 , 27, OPT_CENTERX, "Score:");
	API_CMD_TEXT(SIDE_TABLE_CENTER, 330 , 27, OPT_CENTERX, value);

	itoa(params->highestScore, value, 10);

	API_CMD_TEXT(SIDE_TABLE_CENTER, 360 , 27, OPT_CENTERX, "Highest Score:");
	API_CMD_TEXT(SIDE_TABLE_CENTER, 380 , 27, OPT_CENTERX, value);

	if(params->pause == pause)
	{
		API_COLOR(0xFF0000);
		API_CMD_TEXT(SIDE_TABLE_CENTER, 100 , 29, OPT_CENTERX, "PAUSED");
	}
}

void moveLaserBullet(laser_bullet_t *bullet_handler,paddle_t *handle_bar)
{
	if(bullet_handler->isActive)
	{
		API_BEGIN(RECTS);

		API_COLOR(0xFFFF00);

		API_LINE_WIDTH(BULLET_LINE_WIDTH);

		if(bullet_handler->shoot == bulletRelease)
		{
			bullet_handler->shoot=bulletOngoing;

			if(bullet_handler->leftRight==0)
			{
				bullet_handler->xAnchor = handle_bar->xLeftAnchor + EDGE_OFFSET;
			}
			else
			{
				bullet_handler->xAnchor = handle_bar->xLeftAnchor + handle_bar->Length - EDGE_OFFSET;
				bullet_handler->xAnchor -= BULLET_LENGTH;
			}

			bullet_handler->yAnchor = handle_bar->yAxis;
		}

		API_VERTEX2F(bullet_handler->xAnchor, bullet_handler->yAnchor);
		API_VERTEX2F(bullet_handler->xAnchor+BULLET_LENGTH, bullet_handler->yAnchor - BULLET_HEIGHT);

		if(game.pause == running)
		{
			bullet_handler->yAnchor -= BULLET_VELOCITY;
		}

		API_END();
	}
}
