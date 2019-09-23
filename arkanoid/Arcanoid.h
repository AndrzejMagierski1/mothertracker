#ifndef SOURCE_ARCANOID_H_
#define SOURCE_ARCANOID_H_

#include "Arduino.h"

#define TRUE							1
#define FALSE							0

/*LCD constants*/
#define LCD_WIDTH						800
#define LCD_HEIGTH						480

/* Block constants*/
#define BLOCKS_NUM 						44
#define BLOCK_LINE_WIDTH 				48
#define BLOCK_HEIGHT 					16
#define BLOCK_LENGTH					47
#define INDESTRUCTIBLE_BLOCK_LIFE 	 	6
#define BLOCK_EXTEND_PIXELS 			(BLOCK_LINE_WIDTH/SUBPIXELS_PER_PIXEL) * 2
#define BLOCK_MINIMUM_SPACING 			(BLOCK_EXTEND_PIXELS + 2)
#define BLOCK_EDGE_SPACING 				(BLOCK_EXTEND_PIXELS/2 + 2)


/*Perks constants*/
#define PERKS_NUM 					 	7
#define PERK_SQUARE_SIZE 				14
#define PERK_TIME 						15000
#define PERK_VELOCITY 					2
#define PERK_LINE_WIDTH 				96
#define VALID_PERK_NUM					7

/*Balls constants*/
#define BALLS_NUM 						3
#define BALL_RADIUS						20
#define BALL_VELOCITY 					4
#define BALL_SPEED_MODIFIER 			2
#define BALL_CENTER 					PADDLE_CENTER
#define BALL_ABOVE_PADDLE_Y 			(PADDLE_Y_ANCHOR  - BALL_RADIUS) + 3
#define MAGIC_DYNAMIC_NUMBER 			0.05f
#define ANGLE_MODIFIER_LIMIT 			1



/*Paddle constants*/
#define PADDLE_VELOCITY 				8
#define PADDLE_HEIGTH 					16
#define PADDLE_EXTEND_VALUE 			50
#define PADDLE_SHORTEN_VALUE 			50
#define PADDLE_CENTER 					305
#define PADDLE_LENGTH 					200
#define PADDLE_X_ANCHOR 				(PADDLE_CENTER - (PADDLE_LENGTH/2))
#define PADDLE_Y_ANCHOR 				445
#define PADDLE_LINE_WIDTH 				48
#define PADDLE_MAX_TRAVEL				200


/*Bullet constants*/
#define BULLET_VELOCITY 				20
#define EDGE_OFFSET 					24
#define BULLET_HEIGHT 					10
#define BULLET_LENGTH 					6
#define BULLETS_NUMBER					2
#define BULLET_LINE_WIDTH 				16

#define CANON_LENGTH 					(10 + BULLET_LENGTH + BULLET_LINE_WIDTH)
#define CANON_HEIGTH 					20
#define CANON_EDGE_OFFSET 				(EDGE_OFFSET-10)


/*Other constants*/
#define GAME_REFRESH_MS 				10
#define PLAY_AREA_HEIGHT 				LCD_HEIGTH
#define PLAY_AREA_WIDTH  				610
#define POINTS_PER_HIT 					5
#define SUBPIXELS_PER_PIXEL 			16
#define NEW_LEVEL_ENTRY_SIZE			80
#define SIDE_TABLE_LINE_WIDTH			2
#define SIDE_TABLE_CENTER				(LCD_WIDTH - ((LCD_WIDTH - PLAY_AREA_WIDTH - SIDE_TABLE_LINE_WIDTH)/2))


#define LEVELUP_CHANCE 					10

#define NO_PERK 						0xFF

#define LEVELS							4

#define RIGHT							1
#define LEFT							0



typedef enum
{
	waitingForInit,
	initialized,
	gameoverWaitingForReinit,
	gameWaitingForNewLevel,

}game_stage_t;

typedef enum
{
	noPerk,
	paddleExtend,
	paddleShorten,
	ballMultiplication,
	ballGlue,
	ballSlowmo,
	laser,
	levelUP,

}block_perks_t;

typedef enum
{
	noGlue,
	ballWithGlueMoving,
	ballRelease,
	ballWaitingForRelease,

}glue_stage_t;

typedef enum
{
	perkActive,
	perkMoving,
	perkWaitingDeactivation,
	perkNotActive,

}perk_stage_t;

typedef enum
{
	tBall,
	tPaddle,
	tBlock,
	tPerk,
	tBullet,
}basic_type_t;

typedef enum
{
	ball_paddle,
	ball_block,
	perk_paddle,
	bullet_block,

}collision_type_t;

typedef struct
{
	float x_vect;
	float y_vect;

	float angle_modifier;

}vector_t;

typedef enum
{
	classic,
	armedClassic,

}bar_visual_t;

typedef enum
{
	running,
	pause,
}pause_stage_t;

typedef struct
{
	uint16_t yAxis;
	uint16_t Length;
	int16_t xLeftAnchor;

	uint16_t prevXanchor;

	bar_visual_t visual;
	int16_t ballCatchPoint[3];
	int16_t travel;

}paddle_t;

typedef struct
{
	float xAxisAnchor;
	float yAxisAnchor;

	vector_t vector;

	uint8_t isActive;

	glue_stage_t glue;
	uint8_t allowGlue;
}ball_t;

typedef struct
{
	uint16_t xLeftAnchor;
	uint16_t yAxis;
	uint8_t life;
	uint8_t perkNum;
	uint8_t isActive;

	uint8_t isDestructible;

}block_t;

typedef struct
{
	perk_stage_t stage;
	block_perks_t perkType;
	uint16_t xLeftAnchor;
	uint16_t yAxis;
	uint16_t size;
	uint32_t timestamp;

	uint8_t isActive;

}perk_t;


typedef struct
{
	uint16_t highestScore;
	uint16_t currentScore;
	uint8_t level;
	uint8_t lifes;

	game_stage_t gameStage;
	uint8_t gameRunningFlag;
	pause_stage_t pause;

	uint8_t forcePerkOff;

	uint8_t godmode;

}game_params_t;

typedef struct
{
	uint8_t blocksToGo;
	uint8_t ballSpeed;
	uint8_t roundStarted;
	uint8_t blocksOnRound;
	uint8_t ballsActive;

	uint8_t nextLevelEntry;

}round_params_t;

typedef enum
{
	noBullet,
	bulletRelease,
	bulletOngoing,
	bulletHit,
}shoot_stage_t;

typedef struct
{
	uint16_t xAnchor;
	uint16_t yAnchor;

	shoot_stage_t shoot;
	uint8_t leftRight;

	uint8_t isActive;
	uint8_t allowRelease;

}laser_bullet_t;

typedef void (*audio_p)(uint8_t pitch, uint8_t sampleNum);


void ARKANOID_moveBarLeft(uint8_t movement);
void ARKANOID_moveBarRight(uint8_t movement);
void ARKANOID_releaseFromPaddle();
void ARKANOID_gameLoop();
void ARKANOID_gameStart();
void ARKANOID_pauseControl(pause_stage_t pauseControl);

void ARKANOID_setAudioHandler(audio_p middlewarePointer);
void ARKANOID_resetAudioHandler();

void ARKANOID_updateSamplesLoaded(uint8_t sample,uint8_t value);

extern block_t blocks[BLOCKS_NUM];


#endif
