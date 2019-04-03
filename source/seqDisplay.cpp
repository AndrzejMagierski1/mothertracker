#include "seqDisplay.h"

uint8_t blinkTab[8][20];
uint8_t beyondUp[8][12];
uint8_t beyondDown[8][12];

void SeqDisplay::init(Sequencer::strPattern * seq)
{
	sequencerPtr=seq;
	setMode(seqStop);
}

void SeqDisplay::update()
{
	uint8_t sequencerState=sequencer.getSeqState();
	static elapsedMillis blinkTimer;

	if((sequencerState == 1) && (state == seqStop) )
	{
		setMode(seqPlay);
	}
	else if((sequencerState == 2) && (state == seqPlay))
	{
		setMode(seqStop);
	}
	if(state == seqPlay) updatePlayMode();
	else
	{
		drawCurrentPosition();
		if(blinkTimer >= REFRESH_BLINK_TIME_MS)
		{
			blinkTimer=0;
			if(cleared) return;
			for(uint8_t i = 0; i< 8 ; i++)
			{
				for(uint8_t j=0;j<20; j++)
				{
					if(j==0) continue;
					if(blinkTab[i][j])
					{
						if(toggler)
						{
							if(j != TRACKER_LINE) leds.setLEDseq(i+1,j+1,1,31);
							else leds.setLEDseq(i+1,j+1,1,21);
						}
						else
						{
							if(j != TRACKER_LINE) leds.setLEDseq(i+1,j+1,0,31);
							else leds.setLEDseq(i+1,j+1,1,10);
						}
					}
				}
			}
			toggler = !toggler;

		}
	}

}

void SeqDisplay::setMode(uint8_t s)
{
	if(state == seqPlay && s == seqStop)
	{
		state=s;
		drawCurrentPosition();
	}
	else if (state == seqStop && s == seqPlay)
	{
		state=s;
		startPlayMode();
	}

}
void SeqDisplay::startPlayMode()
{
	setMode(seqPlay);
	setScroll(0);
	clearAllBlink();
	for(uint8_t i=0;i<8;i++)
	{
		if(sequencerPtr->track[i].isOn) leds.setLEDseq(i+1,1,1,31);
		else leds.setLEDseq(i+1,1,0,31);
	}
	for(uint8_t i=0;i<8;i++)
	{
		for(uint8_t j=1;j<19;j++)
		{
			if(j<(TRACKER_LINE))   leds.setLEDseq(i+1,j+1,0,31);
			else if(j==(TRACKER_LINE))
			{
				if(sequencerPtr->track[i].step[j - TRACKER_LINE].isOn) leds.setLEDseq(i+1,j+1,1,TRACKER_LINE_LIGHT_ON);
				else leds.setLEDseq(i+1,j+1,1,TRACKER_LINE_LIGHT_OFF);
			}
			else if(j>(TRACKER_LINE))
			{
				if(sequencerPtr->track[i].step[j - TRACKER_LINE].isOn) leds.setLEDseq(i+1,j+1,1,31);
				else leds.setLEDseq(i+1,j+1,0,31);
			}
		}
	}
}

void SeqDisplay::updatePlayMode()
{
	int8_t maxTrackLen=0;
	static elapsedMillis tim;
	if(tim>REFRESH_TIME)
	{
		tim=0;

		maxTrackLen=getMaxTrackLen();
		if(!maxTrackLen) return;

		for(uint8_t i=0;i<8;i++)
		{
			for(uint8_t j=1;j<19;j++)
			{
				if(j==TRACKER_LINE) leds.setLEDseq(i+1,j+1,1,TRACKER_LINE_LIGHT_OFF);
				else leds.setLEDseq(i+1,j+1,0,31);
			}
		}

		for(uint8_t i=0;i<8;i++)
		{
			if(sequencerPtr->track[i].isOn) leds.setLEDseq(i+1,1,1,31);
			else leds.setLEDseq(i+1,1,0,31);
		}
		for(uint8_t i=0;i<8;i++)
		{
			if(sequencerPtr->track[i].isOn)
			{
				for(uint8_t j=1;j<19;j++)
				{
					if(!(j-1-sequencer.ptrPlayer->row[i].actual_pos ))
					{
						if(sequencerPtr->track[i].step[j-1].isOn) leds.setLEDseq(i+1,TRACKER_LINE - sequencer.ptrPlayer->row[i].actual_pos +j,1,TRACKER_LINE_LIGHT_ON);
						else leds.setLEDseq(i+1,TRACKER_LINE  - sequencer.ptrPlayer->row[i].actual_pos + j,1,TRACKER_LINE_LIGHT_OFF);
					}
					else
					{
						if((TRACKER_LINE  - sequencer.ptrPlayer->row[i].actual_pos +j) > 1 )
						{
							if(sequencerPtr->track[i].step[j-1].isOn) leds.setLEDseq(i+1,TRACKER_LINE  - sequencer.ptrPlayer->row[i].actual_pos +j,1,31);
							else leds.setLEDseq(i+1,TRACKER_LINE - sequencer.ptrPlayer->row[i].actual_pos + j,0,31);
						}

					}

				}
			}
		}
	}
}

void SeqDisplay::drawCurrentPosition()
{
	int8_t maxTrackLen=0;
	static elapsedMillis tim;
	if(tim>REFRESH_TIME)
	{
		tim=0;

		maxTrackLen=getMaxTrackLen();
		if(!maxTrackLen) return;

		for(uint8_t i=0;i<8;i++)
		{
			for(uint8_t j=1;j<19;j++)
			{
				if(blinkTab[i][j]) continue;

				if(j==TRACKER_LINE) leds.setLEDseq(i+1,j+1,1,TRACKER_LINE_LIGHT_OFF);
				else leds.setLEDseq(i+1,j+1,0,31);

			}
		}

		for(uint8_t i=0;i<8;i++)
		{
			if(sequencerPtr->track[i].isOn) leds.setLEDseq(i+1,1,1,31);
			else leds.setLEDseq(i+1,1,0,31);
		}
		for(uint8_t i=0;i<8;i++)
		{
			if(sequencerPtr->track[i].isOn)
			{
				for(uint8_t j=1;j<19;j++)
				{
					if(blinkTab[i][TRACKER_LINE-(sequencer.ptrPlayer->row[i].actual_pos + scrollShift) + j -1] ) continue;

					if(!(j-1-(sequencer.ptrPlayer->row[i].actual_pos + scrollShift) ))
					{
						if(sequencerPtr->track[i].step[j-1].isOn) leds.setLEDseq(i+1,TRACKER_LINE - (sequencer.ptrPlayer->row[i].actual_pos+ scrollShift) +j,1,TRACKER_LINE_LIGHT_ON);
						else leds.setLEDseq(i+1,TRACKER_LINE  - (sequencer.ptrPlayer->row[i].actual_pos+ scrollShift) + j,1,TRACKER_LINE_LIGHT_OFF);
					}
					else
					{
						if((TRACKER_LINE  - (sequencer.ptrPlayer->row[i].actual_pos+scrollShift) +j) > 1 )
						{
							if(sequencerPtr->track[i].step[j-1].isOn) leds.setLEDseq(i+1,TRACKER_LINE  - (sequencer.ptrPlayer->row[i].actual_pos+scrollShift) +j,1,31);
							else leds.setLEDseq(i+1,TRACKER_LINE - (sequencer.ptrPlayer->row[i].actual_pos+scrollShift) + j,0,31);
						}

					}

				}
			}
		}
	}
}

//void SeqDisplay::updateStopMode()
//{
//	static elapsedMillis tim;
//	if(tim>REFRESH_TIME)
//	{
//	tim=0;
//
//	for(uint8_t i=0;i<8;i++)
//	{
//		if(sequencerPtr->track[i].isOn) leds.setLEDseq(i+1,1,1,31);
//		else leds.setLEDseq(i+1,1,0,31);
//	}
//	for(uint8_t i=0;i<8;i++)
//	{
//		for(uint8_t j=0;j<20;j++)
//		{
//			if(sequencerPtr->track[i].step[j].isOn) leds.setLEDseq(i+1,j+2,1,31);
//			else leds.setLEDseq(i+1,j+2,0,31);
//		}
//	}
//
//	}
//
//}

int8_t SeqDisplay::getMaxTrackLen()
{
	int8_t maxLen=0;
	for(uint8_t i=0;i<8;i++)
	{
		if(sequencerPtr->track[i].isOn)
		{
			if(maxLen < sequencerPtr->track[i].length) maxLen= sequencerPtr->track[i].length;
		}
	}

	return maxLen;

}
void SeqDisplay::setScroll(int8_t sc)
{
	scrollShift = sc;
}
void SeqDisplay::incScroll()
{
	uint8_t cnt=0;
	uint8_t temp[8];
	for(uint8_t i = 0; i<8 ; i++)
	{
		if(scrollShift + sequencer.ptrPlayer->row[i].actual_pos >= 32) cnt++;
	}

	if(!cnt)
	{
		if(!cleared)
		{
			for(uint8_t i=0;i<8;i++)
			{
				temp[i] = blinkTab[i][0];
			}
			for(uint8_t i=0;i<8;i++)
			{
				for(uint8_t j=0;j<20;j++)
				{
					if(j < 19)blinkTab[i][j]=blinkTab[i][j+1];
					if(j < 11) beyondDown[i][j] = beyondDown[i][j+1];
					if(j == 19) blinkTab[i][j] = beyondUp[i][0];
				}
			}
			for(uint8_t i=0;i<8;i++)
			{
				beyondDown[i][11] = temp[i];
			}
			for(uint8_t i=0;i<8;i++)
			{
				for(uint8_t j=0;j<11;j++)
				{
					beyondUp[i][j] = beyondUp[i][j+1];
				}
			}
		}
		scrollShift++;
	}

}

void SeqDisplay::decScroll()
{
	uint8_t cnt=0;
	uint8_t temp[8];
	for(uint8_t i = 0; i<8 ; i++)
	{
		if(scrollShift + sequencer.ptrPlayer->row[i].actual_pos <= 0) cnt++;
	}

	if(!cnt)
	{
		if(!cleared)
		{
			for(uint8_t i=0;i<8;i++)
			{
				temp[i] = blinkTab[i][19];
			}
			for(uint8_t i=0;i<8;i++)
			{
				for(int8_t j=19;j>=0;j--)
				{
					if(j > 0)
					{
						blinkTab[i][j] = blinkTab[i][j-1];
					}
					if( (j < 12) && (j > 0) )
					{
						beyondUp[i][j] = beyondUp[i][j-1];
					}
					if(j == 0)
					{
						blinkTab[i][j] = beyondDown[i][11];
					}
				}
			}
			for(uint8_t i=0;i<8;i++)
			{
				beyondUp[i][0] = temp[i];
			}
			for(uint8_t i=0;i<8;i++)
			{
				for(uint8_t j=11;j>=1;j--)
				{
					beyondDown[i][j] = beyondDown[i][j-1];
				}
			}
		}
		scrollShift--;
	}
}

uint8_t SeqDisplay::getStep(uint8_t x, uint8_t y)
{
	int8_t result = y  - TRACKER_LINE + sequencer.ptrPlayer->row[x].actual_pos + scrollShift;
	if(result<0) result=0;
	return  (uint8_t)result;
}

uint8_t SeqDisplay::getBlink(uint8_t track, uint8_t step)
{
	int8_t result = TRACKER_LINE - sequencer.ptrPlayer->row[track].actual_pos - scrollShift + step;
	if(result<0) result=0;
	if(result>19) result = 19;
	return  (uint8_t)result;
}

void SeqDisplay::setBlink(uint8_t x, uint8_t y)
{
	int8_t ymin,ymax;
	ymin=TRACKER_LINE-sequencer.ptrPlayer->row[x].actual_pos - scrollShift;
	if(ymin<1) ymin=1;
	if(ymin>19) ymin=19;
	ymax=19;
	if( (y >= ymin) && (y<= ymax) )
	{
		clearAllBlink();
		blinkTab[x][y]=1;
		cleared=0;
	}
}

void SeqDisplay::setMultiBlink(uint8_t x,uint8_t y)
{
	uint8_t xmin{0},ymin{0},xmax{0},ymax{0};

	if( (lastPoint.x != -1) && (lastPoint.y != - 1) )
	{
		if(x > lastPoint.x)
		{
			xmin = lastPoint.x;
			xmax = x;
		}
		else if (x == lastPoint.x)
		{
			xmin = x;
			xmax = x;
		}
		else
		{
			xmin = x;
			xmax = lastPoint.x;
		}

		if(y > lastPoint.y)
		{
			ymin = lastPoint.y;
			ymax = y;
		}
		else if (y == lastPoint.y)
		{
			ymin = y;
			ymax = y;
		}
		else
		{
			ymin = y;
			ymax = lastPoint.y;
		}
		for(uint8_t i = xmin; i<= xmax ; i++)
		{
			for(uint8_t j = ymin; j<= ymax ; j++)
			{
				blinkTab[i][j]=1;
			}
		}

	}
	lastPoint.x=x;
	lastPoint.y=y;
}

void SeqDisplay::clearLast()
{
	lastPoint.x=-1;
	lastPoint.y=-1;
}

void SeqDisplay::clearAllBlink()
{
	memset(blinkTab,0,160);
	cleared = 1;
}

SeqDisplay seqDisplay;
