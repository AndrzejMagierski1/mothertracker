#include "seqDisplay.h"

uint8_t blinkTab[8][20];

void SeqDisplay::init(Sequencer::strPattern * seq)
{
	sequencerPtr=seq;
	setMode(seqStop);
}

void SeqDisplay::update()
{
	uint8_t sequencerState=sequencer.getSeqState();
	static elapsedMillis blinkTimer;
	static uint8_t toggler;
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
				for(uint8_t j=0;j<19;j++)
				{
					blinkTab[i][j]=blinkTab[i][j+1];
				}
			}
		}
		scrollShift++;
	}

}

void SeqDisplay::decScroll()
{
	uint8_t cnt=0;
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
				for(uint8_t j=20;j>=1;j--)
				{
					blinkTab[i][j]=blinkTab[i][j-1];
				}
			}
		}
		scrollShift--;
	}
}

uint8_t SeqDisplay::getStep(uint8_t x, uint8_t y)
{
	uint8_t result = y  - TRACKER_LINE + sequencer.ptrPlayer->row[x].actual_pos + scrollShift;
	if(result<0) result=0;
	return  result;
}

void SeqDisplay::setBlink(uint8_t x, uint8_t y)
{
	blinkTab[x][y]=1;
	cleared=0;
}

void SeqDisplay::clearAllBlink()
{
	memset(blinkTab,0,160);
	cleared = 1;
}

SeqDisplay seqDisplay;
