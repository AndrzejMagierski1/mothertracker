#include "ArcanoidRoundCreator.h"





void placeBlock(uint8_t blocknum,uint16_t x_axis,uint16_t y_axis,uint16_t blockLength, uint8_t blocklife)
{
	blocks[blocknum].isActive=1;
	blocks[blocknum].xLeftAnchor=x_axis;
	blocks[blocknum].yAxis=y_axis;
	blocks[blocknum].life=blocklife;

	if(blocks[blocknum].life >= INDESTRUCTIBLE_BLOCK_LIFE)
	{
		blocks[blocknum].isDestructible=0;
	}
	else
	{
		blocks[blocknum].isDestructible=1;
	}

	blocks[blocknum].perkNum=NO_PERK;
	blocks[blocknum].isDestructible=1;
}

void placeBlockRow(uint8_t startBlock,uint8_t blocksToCreate,uint16_t x_axis,uint16_t y_axis,uint16_t blockLength,uint16_t blockslife)
{
	for(int i=0;i<blocksToCreate;i++)
	{
		blocks[startBlock+i].isActive=1;

		if(x_axis<BLOCK_EDGE_SPACING)
		{
			blocks[startBlock+i].xLeftAnchor=x_axis + (i*blockLength) + (i*BLOCK_MINIMUM_SPACING) + BLOCK_EDGE_SPACING;
		}
		else
		{
			blocks[startBlock+i].xLeftAnchor=x_axis + (i*blockLength) + (i*BLOCK_MINIMUM_SPACING);
		}

		blocks[startBlock+i].yAxis=y_axis;
		blocks[startBlock+i].life=blockslife;

		if(blocks[startBlock+i].life >= INDESTRUCTIBLE_BLOCK_LIFE)
		{
			blocks[startBlock+i].isDestructible=0;
		}
		else
		{
			blocks[startBlock+i].isDestructible=1;
		}

		blocks[startBlock+i].perkNum=NO_PERK;

	}
}
