#include "Arcanoid.h"
#include "ArcanoidHelper.h"
#include "Arcanoid_display.h"
#include "ArcanoidLevels.h"
#include "mtAudioEngine.h"

elapsedMillis perkTimer;
elapsedMillis gameRefresh;
elapsedMillis paddleTimer;

laser_bullet_t bullets[BULLETS_NUMBER];
game_params_t game;
round_params_t round_params;

paddle_t paddle;
ball_t ball[BALLS_NUM]={{BALL_CENTER,BALL_ABOVE_PADDLE_Y,{0,0,0},1,ballWaitingForRelease,0},
						{BALL_CENTER,BALL_ABOVE_PADDLE_Y,{0,0,0},0,ballWaitingForRelease,0},
						{BALL_CENTER,BALL_ABOVE_PADDLE_Y,{0,0,0},0,ballWaitingForRelease,0}};

block_t blocks[BLOCKS_NUM];
perk_t perk[PERKS_NUM];


audio_p play_audio = NULL;

static uint8_t paddleOneSoundFlag;
static uint8_t movementModifier;
static uint8_t paddleDirection;
static uint32_t lastSavedTime;
static uint8_t samplesLoaded[48];//samples loaded flags

static uint8_t isColliding(ball_t * ball, block_t *block , collision_type_t type);
static void handleLCDBorders(ball_t *handle_ball);
static void initializePerk(perk_t *perk , block_t *block);
static void handle_gameover();
static void resetBall(ball_t *ball);
static void handle_paddle();
static void handle_laser();
static void handle_perks();
static void handle_blocks();
static void handle_laserRelease(laser_bullet_t *bullet1, laser_bullet_t *bullet2);
static void prepareGameData();
static void randomizer();
static void gameCleanup();
static void randomStartVector(ball_t *ball_handle);
static uint8_t randomSampleNum();



void ARKANOID_updateSamplesLoaded(uint8_t sample,uint8_t value)
{
	samplesLoaded[sample]=value;
}

void ARKANOID_gameStart()
{
	if(game.gameStage == waitingForInit || game.gameStage == gameoverWaitingForReinit)
	{
		game.gameStage=initialized;
		game.lifes=3;
		game.level=1;

		for(int i=0;i<PERKS_NUM;i++)
		{
			perk[i].perkType=(block_perks_t)(i+1);
		}
	}

}

void ARKANOID_moveBarLeft()
{
	if(paddleDirection == 1)
	{
		paddleDirection=0;
		movementModifier=0;
		paddle.travel=0;
	}

	if(game.gameRunningFlag)
	{
		if(paddle.xLeftAnchor)
		{
			if((paddle.xLeftAnchor - PADDLE_VELOCITY)>0)
			{
				if(millis()-lastSavedTime < PADDLE_SPEEDUP_TIMEBASE_MS)
				{
					if(movementModifier<PADDLE_MAX_SPEED_MODIFIER)
					{
						movementModifier +=PADDLE_SPEED_MODIFIER;
					}
				}
				else
				{
					movementModifier=1;
				}

				lastSavedTime = millis();

				paddle.travel-= (movementModifier * PADDLE_VELOCITY);

			}
		}
	}
}

void ARKANOID_moveBarRight()
{
	if(paddleDirection == LEFT)
	{
		paddleDirection=RIGHT;
		movementModifier=0;
		paddle.travel=0;
	}

	if(game.gameRunningFlag)
	{
		if((paddle.xLeftAnchor+paddle.Length+PADDLE_VELOCITY) < PLAY_AREA_WIDTH)
		{
			if(millis()-lastSavedTime < PADDLE_SPEEDUP_TIMEBASE_MS)
			{
				if(movementModifier<PADDLE_MAX_SPEED_MODIFIER)
				{
					movementModifier +=PADDLE_SPEED_MODIFIER;
				}
			}
			else
			{
				movementModifier=1;
			}

			lastSavedTime = millis();

			paddle.travel += (movementModifier * PADDLE_VELOCITY);

		}
		else
		{
			/* If someone collected levelUP perk*/
			if(round_params.nextLevelEntry)
			{
				paddle.xLeftAnchor += 20;

				if(game.level < LEVELS)
				{
					game.gameStage=gameWaitingForNewLevel;
					game.level++;

					gameCleanup();

					round_params.roundStarted=FALSE;
					round_params.nextLevelEntry=FALSE;
				}
			}

		}
	}
}

void ARKANOID_releaseFromPaddle()
{
	if(game.gameRunningFlag)
	{
		handle_laserRelease(&bullets[0],&bullets[1]);

		for(int i=0;i<BALLS_NUM;i++)
		{
			if(ball[i].isActive)
			{
				if(ball[i].glue == ballWaitingForRelease)
				{
					if(ball[i].glue == ballWaitingForRelease)
					{
						ball[i].glue = ballRelease;
					}

					if(round_params.roundStarted)
					{
						if(ball[i].xAxisAnchor < (paddle.xLeftAnchor+(paddle.Length/2)))
						{
							ball[i].vector.x_vect = - round_params.ballSpeed;
							ball[i].vector.y_vect = - round_params.ballSpeed;
						}
						else
						{
							ball[i].vector.x_vect = round_params.ballSpeed;
							ball[i].vector.y_vect = - round_params.ballSpeed;
						}
					}
					else
					{
						randomStartVector(&ball[i]);
					}
				}
			}
		}

		round_params.roundStarted=TRUE;
	}
}

static void arcanoidGameInit()
{
	if((game.gameStage == initialized && !game.gameRunningFlag) || game.gameStage == gameWaitingForNewLevel)
	{
		uint8_t indestructibleBlocks=0;
		game.gameStage=initialized;

		round_params.blocksOnRound=load_level(game.level);

		if(game.gameStage == gameWaitingForNewLevel)
		{
			if(play_audio != NULL)
			{
				play_audio(bullet_block,30);
			}
		}

		for(int i=0;i<round_params.blocksOnRound;i++)
		{
			if(!blocks[i].isDestructible)
			{
				indestructibleBlocks++;
			}
		}

		round_params.blocksToGo = round_params.blocksOnRound - indestructibleBlocks;

		prepareGameData();

		game.gameRunningFlag=TRUE;
	}
}

void ARKANOID_gameLoop()
{
	arcanoidGameInit();

	if(game.gameStage == waitingForInit)
	{
		entryScreen();
	}

	if(game.gameRunningFlag)
	{
		if(gameRefresh>GAME_REFRESH_MS)
		{
			gameRefresh=0;

			gameDisplayBegin();

			/* Logic part of the game*/
			handle_perks();
			handle_paddle();
			handle_laser();

			for(int i=0;i<BALLS_NUM;i++)
			{
				handleLCDBorders(&ball[i]);
			}

			handle_blocks();

			/*Display part of the game*/
			for(int i=0;i<BULLETS_NUMBER;i++)
			{
				moveLaserBullet(&bullets[i],&paddle);
			}

			moveBar(&paddle);

			/* Ball moving with the paddle*/
			for(int i=0;i<BALLS_NUM;i++)
			{
				if(ball[i].isActive)
				{
					if((ball[i].glue == ballWaitingForRelease) || !round_params.roundStarted)
					{
						ball[i].xAxisAnchor = paddle.xLeftAnchor + paddle.ballCatchPoint[i];
					}
				}
			}

			for(int i=0;i<BALLS_NUM;i++)
			{
				moveBall(&ball[i]);
			}

			for(int i =0; i<round_params.blocksOnRound;i++)
			{
				showBlocks(&blocks[i]);
			}

			for(int i=0;i<PERKS_NUM;i++)
			{
				movePerk(&perk[i]);
			}

			updateSideTable(&game,&round_params);

			if(round_params.blocksToGo == 0)
			{
				if(game.level < LEVELS)
				{
					game.gameStage=gameWaitingForNewLevel;
					game.level++;
					gameCleanup();
					round_params.roundStarted=FALSE;
					round_params.nextLevelEntry=FALSE;
				}
			}

			gameDisplayFinish();
		}
	}

}

static void prepareGameData()
{
	//game.godmode=1;
	round_params.ballsActive=1;

	resetBall(&ball[0]);

	ball[1].isActive=0;
	ball[2].isActive=0;

	randomizer();

	for(int i=0;i<PERKS_NUM;i++)
	{
		perk[i].stage=perkActive;
	}

	/* Game parameters*/
	paddle.yAxis=PADDLE_Y_ANCHOR;
	paddle.Length=PADDLE_LENGTH;
	paddle.xLeftAnchor=PADDLE_X_ANCHOR;

	for(int i=0;i<BALLS_NUM;i++)
	{
		paddle.ballCatchPoint[i]=PADDLE_LENGTH/2;
	}

	bullets[0].leftRight=0;
	bullets[1].leftRight=1;
	bullets[0].allowRelease=0;
	bullets[1].allowRelease=0;

	paddle.visual=classic;

	round_params.ballSpeed= BALL_VELOCITY;

}

static void randomStartVector(ball_t *ball_handle)
{
	float vector=0;
	uint8_t diff;
	uint8_t sign=1;


	vector = get_randomNumber(BALL_VELOCITY);

	if(vector < BALL_SPEED_MODIFIER)
	{
		vector += BALL_SPEED_MODIFIER;
	}

	diff=round_params.ballSpeed-vector;

	sign = get_randomNumber(2);

	if(sign==1)
	{
		vector = invert_float(vector);
	}

	ball_handle->vector.x_vect = vector;

	vector = round_params.ballSpeed + diff;
	vector = invert_float(vector);

	ball_handle->vector.y_vect=vector;
}

static void randomizer()
{
	uint8_t randomBlockNumber=0;
	uint8_t randomPerkNum=0;
	uint8_t perksTaken=0;

	while(perksTaken < PERKS_NUM)
	{
		randomPerkNum=perksTaken;
		if(perksTaken == 6)
		{
			if(!(get_randomNumber(100) < LEVELUP_CHANCE))
			{
				randomPerkNum=NO_PERK;
				break;
			}
		}

		randomBlockNumber=get_randomNumber(round_params.blocksOnRound);

		blocks[randomBlockNumber].perkNum=randomPerkNum;
		perksTaken++;
	}
}

static uint8_t randomSampleNum()
{
	uint8_t randomNum;
	randomNum = get_randomNumber(47);

	while(samplesLoaded[randomNum] == 1)
	{
		randomNum++;

		if(randomNum==48)
		{
			randomNum=0;
		}
	}

	return randomNum;
}

static void handle_laserRelease(laser_bullet_t *bullet1, laser_bullet_t *bullet2)
{
	if(bullet1->allowRelease && bullet2->allowRelease)
	{
		if((bullet1->shoot==noBullet && bullet2 ->shoot == noBullet) || (bullet1->shoot==bulletHit && bullet2->shoot==bulletHit))
		{
			bullet1->isActive=TRUE;
			bullet2->isActive=TRUE;
			bullet1->shoot=bulletRelease;
			bullet2->shoot=bulletRelease;
		}
	}
}

static void handle_paddle()
{
	for(int i=0;i<BALLS_NUM;i++)
	{
		if(!ball[i].isActive) continue;

		if(ball[i].vector.x_vect == 0 || ball[i].vector.y_vect == 0)
		{
			ball[i].vector.y_vect=0;
			ball[i].vector.x_vect=0;
		}

		handle_angleModifier(&ball[i],&paddle,&round_params);

		/* Hitting the paddle */
		if(isOverlaping(&ball[i],&paddle,ball_paddle))
		{
			if(play_audio != NULL)
			{
				if(ball[i].glue != ballWaitingForRelease && !paddleOneSoundFlag)
				{
					play_audio(ball_paddle,2);
					paddleOneSoundFlag=1;
				}
			}

			if(ball[i].allowGlue == FALSE && round_params.roundStarted)
			{
				ball[i].vector.y_vect = -round_params.ballSpeed + ball[i].vector.angle_modifier;

				if(ball[i].xAxisAnchor <(paddle.xLeftAnchor+(paddle.Length/2)))
				{
					ball[i].vector.x_vect = - round_params.ballSpeed - ball[i].vector.angle_modifier;
				}
				else
				{
					ball[i].vector.x_vect = round_params.ballSpeed + ball[i].vector.angle_modifier;
				}
			}

			if(ball[i].allowGlue)
			{
				if(ball[i].glue == ballWithGlueMoving)
				{
					ball[i].vector.y_vect = 0;
					ball[i].vector.x_vect = 0;
					ball[i].glue=ballWaitingForRelease;
					paddle.ballCatchPoint[i]=ball[i].xAxisAnchor - paddle.xLeftAnchor;

				}
			}

		}
		else
		{
			if(ball[i].allowGlue)
			{
				if(ball[i].glue == ballRelease)
				{
					ball[i].glue = ballWithGlueMoving;
				}
			}
		}
	}
}

static void handle_laser()
{
	uint8_t hitCount=0;

	/* Bullet with block collision*/
	for(int j=0;j<round_params.blocksOnRound;j++)
	{
		for(int i=0;i<BULLETS_NUMBER;i++)
		{
			if(isOverlaping(&bullets[i],&blocks[j],bullet_block))
			{
				if(play_audio != NULL)
				{
					play_audio(bullet_block,randomSampleNum());
				}

				if(bullets[i].shoot==bulletOngoing)
				{
					if(handle_block_destroying(&blocks[j], &game, &round_params))
					{
						if(blocks[j].perkNum < VALID_PERK_NUM)
						{
							initializePerk(&perk[blocks[j].perkNum] , &blocks[j]);
						}
					}

					hitCount++;

					bullets[i].isActive=FALSE;
					bullets[i].shoot=bulletHit;

					if(hitCount >=2)
					{
						return;
					}
				}
			}
		}

	}
	/*Bullet with top LCD edge collision*/
	for(int i=0;i<BULLETS_NUMBER;i++)
	{
		if(getTopEdge(&bullets[i],tBullet)<0 && bullets[i].shoot == bulletOngoing)
		{
			bullets[i].isActive=FALSE;
			bullets[i].shoot=bulletHit;
		}
	}
}

static void handle_blocks()
{
	for(int i =0; i<round_params.blocksOnRound;i++)
	{
		for(int k=0;k<BALLS_NUM;k++)
		{
			if(isColliding(&ball[k],&blocks[i],ball_block))
			{
				if(play_audio != NULL)
				{
					play_audio(ball_block,randomSampleNum());
				}

				if(handle_block_destroying(&blocks[i], &game, &round_params))
				{
					if(blocks[i].perkNum < VALID_PERK_NUM)
					{
						initializePerk(&perk[blocks[i].perkNum] , &blocks[i]);
					}
				}
			}
		}
	}
}

static void handle_perks()
{
	for(int i =0;i<PERKS_NUM;i++)
	{
		if(isOverlaping(&perk[i], &paddle , perk_paddle))
		{
			if(perk[i].isActive)
			{
				if(play_audio != NULL)
				{
					play_audio(perk_paddle,randomSampleNum());
				}

				perk[i].isActive=FALSE;
				perk[i].stage=perkWaitingDeactivation;

				if(perk[i].perkType==paddleExtend)
				{
					if(paddle.xLeftAnchor + paddle.Length + PADDLE_EXTEND_VALUE > PLAY_AREA_WIDTH)
					{
						paddle.xLeftAnchor -= PADDLE_EXTEND_VALUE;
					}

					paddle.Length += PADDLE_EXTEND_VALUE;
					paddle.xLeftAnchor -= PADDLE_EXTEND_VALUE/2;
				}
				else if(perk[i].perkType==paddleShorten)
				{
					paddle.xLeftAnchor += PADDLE_SHORTEN_VALUE/2;
					paddle.Length -= PADDLE_SHORTEN_VALUE;
				}
				else if(perk[i].perkType==ballMultiplication)
				{
					round_params.ballsActive=3;

					perk[i].stage=perkNotActive;

					for(int j=0;j<BALLS_NUM;j++)
					{
						ball[j].isActive = TRUE;
					}

					/* Ball spawning, with copied settings from main ball*/
					memcpy(&ball[1],&ball[0],sizeof(ball[0]));
					memcpy(&ball[2],&ball[0],sizeof(ball[0]));

					ball[1].vector.x_vect = invert_float(ball[0].vector.x_vect);
					ball[1].vector.y_vect = invert_float(ball[0].vector.y_vect);

					ball[2].vector.x_vect = invert_float(ball[0].vector.x_vect);
					ball[2].vector.y_vect = ball[0].vector.y_vect;

				}
				else if(perk[i].perkType == ballGlue)
				{
					for(int j=0;j<BALLS_NUM;j++)
					{
						ball[j].allowGlue = TRUE;

						if(ball[j].vector.x_vect !=0)
						{
							ball[j].glue = ballWithGlueMoving;
						}
						else
						{
							ball[j].glue = ballWaitingForRelease;
						}
					}
				}
				else if(perk[i].perkType ==	ballSlowmo)
				{
					round_params.ballSpeed = BALL_SPEED_MODIFIER;

					for(int j=0;j<BALLS_NUM;j++)
					{
						if(ball[j].glue != ballWaitingForRelease)
						{
							change_vector(&ball[j],&round_params);
						}
					}
				}
				else if(perk[i].perkType == laser)
				{
					for(int j=0;j<BULLETS_NUMBER;j++)
					{
						bullets[j].allowRelease=TRUE;
						paddle.visual=armedClassic;
					}
				}
				else if(perk[i].perkType == levelUP)
				{
					if(game.level < LEVELS)
					{
						round_params.nextLevelEntry=TRUE;
					}
				}

				break;
			}

		}
		else
		{
			/*Player did not catch the perk*/
			if(getBottomEdge(&perk[i],tPerk) > PLAY_AREA_HEIGHT)
			{
				perk[i].isActive = FALSE;
			}
		}

		/*Perks timekeeping */
		if((perkTimer >= perk[i].timestamp + PERK_TIME) || game.forcePerkOff)
		{
			if(perk[i].stage==perkWaitingDeactivation)
			{
				perk[i].stage = perkNotActive;

				if(perk[i].perkType==paddleExtend)
				{
					paddle.Length -= PADDLE_EXTEND_VALUE;
					paddle.xLeftAnchor += PADDLE_EXTEND_VALUE/2;
				}
				else if(perk[i].perkType==paddleShorten)
				{
					if(paddle.xLeftAnchor + paddle.Length + PADDLE_SHORTEN_VALUE > PLAY_AREA_WIDTH)
					{
						paddle.xLeftAnchor -= PADDLE_SHORTEN_VALUE;
					}
					else
					{
						paddle.xLeftAnchor -= PADDLE_SHORTEN_VALUE/2;
					}

					paddle.Length += PADDLE_SHORTEN_VALUE;

				}
				else if(perk[i].perkType==ballGlue)
				{
					for(int j=0;j<BALLS_NUM;j++)
					{
						if(ball[j].glue == ballWithGlueMoving)
						{
							ball[j].glue=noGlue;
						}

						ball[j].allowGlue=FALSE;
					}
				}
				else if(perk[i].perkType ==	ballSlowmo)
				{
					round_params.ballSpeed = BALL_VELOCITY;

					for(int j=0;j<BALLS_NUM;j++)
					{
						if(ball[j].glue != ballWaitingForRelease)
						{
							change_vector(&ball[j],&round_params);
						}
					}
				}
				else if(perk[i].perkType == laser)
				{
					for(int j=0;j<BULLETS_NUMBER;j++)
					{
						bullets[j].isActive=FALSE;
						bullets[j].allowRelease=FALSE;

						paddle.visual=classic;
					}
				}
			}
		}
	}

	game.forcePerkOff=FALSE;
}

static void initializePerk(perk_t *perk , block_t *block)
{
	uint16_t block_center;
	if(perk->perkType != noPerk)
	{
		if(perk->stage < perkMoving)
		{
			perk->stage = perkMoving;

			perk->isActive=TRUE;
			perk->timestamp=perkTimer;
			perk->size=PERK_SQUARE_SIZE;

			block_center = block->xLeftAnchor + BLOCK_LENGTH/2;

			perk->xLeftAnchor = block_center - (perk->size/2);

			perk->yAxis=block->yAxis;
		}
	}
}

static void resetBall(ball_t *ball)
{
	ball->isActive=TRUE;
	ball->xAxisAnchor=BALL_CENTER;
	ball->yAxisAnchor=BALL_ABOVE_PADDLE_Y;
	ball->glue=ballWaitingForRelease;
	ball->allowGlue=FALSE;
	ball->vector.x_vect=0;
	ball->vector.y_vect=0;
}

static void handleBallConsuming(ball_t *handle_ball)
{
	if(!game.godmode)
	{
		if(handle_ball->isActive)
		{
			handle_ball->isActive=FALSE;
			round_params.ballsActive--;

			if(!round_params.ballsActive)
			{
				if(--game.lifes)
				{
					resetBall(&ball[0]);

					round_params.roundStarted=FALSE;

					paddle.yAxis=PADDLE_Y_ANCHOR;
					paddle.xLeftAnchor=PADDLE_X_ANCHOR;

					round_params.ballsActive=1;

					game.forcePerkOff=TRUE;
				}
				else
				{
					handle_gameover();
				}
			}
		}
	}
}

static void handleLCDBorders(ball_t *handle_ball)
{
	if(handle_ball->isActive)
	{
		if(getLeftEdge(handle_ball,tBall)<0)
		{
			handle_ball->vector.x_vect = round_params.ballSpeed;
			paddleOneSoundFlag=0;

			if(play_audio != NULL)
			{
				play_audio((collision_type_t)6,0);
			}
		}
		else if(getRightEdge(handle_ball,tBall) > PLAY_AREA_WIDTH)
		{
			handle_ball->vector.x_vect = - round_params.ballSpeed;

			if(play_audio != NULL)
			{
				play_audio((collision_type_t)6,0);
			}
		}

		if(getTopEdge(handle_ball,tBall)<0)
		{
			handle_ball->vector.y_vect = round_params.ballSpeed;

			if(play_audio != NULL)
			{
				play_audio((collision_type_t)6,0);
			}
		}
		else if(getBottomEdge(handle_ball,tBall) > PLAY_AREA_HEIGHT)
		{
			handle_ball->vector.y_vect = - round_params.ballSpeed;
			if(play_audio != NULL)
			{
				play_audio((collision_type_t)6,0);
			}
			handleBallConsuming(handle_ball);
			paddleOneSoundFlag=0;
		}
	}
}

static void gameCleanup()
{
	for(int i=0;i<PERKS_NUM;i++)
	{
		perk[i].isActive=FALSE;
		perk[i].stage=perkNotActive;
	}

	game.gameRunningFlag=FALSE;
}

static void handle_gameover()
{
	if(game.gameStage == initialized)
	{
		game.gameStage = gameoverWaitingForReinit;
		game.currentScore=0;
		round_params.roundStarted=FALSE;
		round_params.nextLevelEntry=0;
		game.level=1;
	}

	gameCleanup();
	gameoverScreen();
}

static uint8_t isColliding(ball_t * ball, block_t *block , collision_type_t type)
{
	if(!isOverlaping(ball,block,type)) return 0;

	if(block->isActive)
	{
		float overlapTop;
		float overlapBottom;
		float overlapLeft;
		float overlapRight;
		bool ballFromLeft;
		bool ballFromTop;

		float overlapX;
		float overlapY;

		overlapTop = (getBottomEdge(ball,tBall) - getTopEdge(block,tBlock));
		overlapBottom = (getBottomEdge(block,tBlock) - getTopEdge(ball,tBall));
		overlapLeft = (getRightEdge(ball,tBall) - getLeftEdge(block,tBlock));
		overlapRight = (getRightEdge(block,tBlock) - getLeftEdge(ball,tBall));

		ballFromLeft = (abs(overlapLeft) < abs(overlapRight));
		ballFromTop = (abs(overlapTop) < abs(overlapBottom));

		if(ballFromLeft)
		{
			overlapX=overlapLeft;
		}
		else
		{
			overlapX=overlapRight;
		}

		if(ballFromTop)
		{
			overlapY=overlapTop;
		}
		else
		{
			overlapY=overlapBottom;
		}

		if(abs(overlapX) < abs(overlapY))
		{
			if(ballFromLeft)
			{
				ball->vector.x_vect = -round_params.ballSpeed;
			}
			else
			{
				ball->vector.x_vect = round_params.ballSpeed;
			}
		}
		else
		{
			if(ballFromTop)
			{
				ball->vector.y_vect = -round_params.ballSpeed;
			}
			else
			{
				ball->vector.y_vect = round_params.ballSpeed;
			}
		}

		return 1;
	}

	return 0;
}

void ARKANOID_setAudioHandler(audio_p middlewarePointer)
{
	play_audio = middlewarePointer;
}

void ARKANOID_resetAudioHandler()
{
	play_audio=NULL;
}


