#include "seqDisplay.h"

void SeqDisplay::init(Sequencer::strBank * seq)
{
	sequencerPtr=seq;
	refreshTimer=0;
	setMode(seqStop);
	setMode(seqPlay);
}

void SeqDisplay::update()
{
	if(refreshTimer >= REFRESH_TIME)
	{
	if(state == seqPlay) updatePlayMode();
	else updateStopMode();
	refreshTimer=0;
	}
}

void SeqDisplay::setMode(uint8_t s)
{
	if(state == seqPlay && s == seqStop)
	{
		state=s;
		updateStopMode();
	}
	else if (state == seqStop && s == seqPlay)
	{
		state=s;
		startPlayMode();
	}

}
void SeqDisplay::startPlayMode()
{
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
				if(sequencerPtr->track[i].step[j - TRACKER_LINE].isOn) leds.setLEDseq(i+1,j+1,1,28);
				else leds.setLEDseq(i+1,j+1,1,3);
			}
			else if(j>(TRACKER_LINE))
			{
				if(sequencerPtr->track[i].step[j - TRACKER_LINE].isOn) leds.setLEDseq(i+1,j+1,1,31);
				else leds.setLEDseq(i+1,j+1,0,31);
			}
		}
	}
	leds.updateSeq();
}

/*
void SeqDisplay::updatePlayMode()
{
	int8_t stepOverZero=0;
	int8_t maxTrackLen=0;

	maxTrackLen=getMaxTrackLen();
	if(!maxTrackLen) return;



	for(uint8_t i=0;i<8;i++)
	{
		if(sequencerPtr->track[i].isOn) leds.setLEDseq(i+1,1,1,31);
		else leds.setLEDseq(i+1,1,0,31);
	}
	for(uint8_t i=0;i<8;i++)
	{
		if( (TRACKER_LINE - sequencer.ptrPlayer->row[i].actual_pos) < 0)  stepOverZero=TRACKER_LINE - sequencer.ptrPlayer->row[i].actual_pos;

		for(uint8_t j=1;j<19;j++)
		{
			if((j<(TRACKER_LINE - sequencer.ptrPlayer->row[i].actual_pos)) && ((TRACKER_LINE - sequencer.ptrPlayer->row[i].actual_pos) > 0)) leds.setLEDseq(i+1,j+1,0,31);

			else if(j>(TRACKER_LINE - sequencer.ptrPlayer->row[i].actual_pos - stepOverZero))
			{
				if(j==TRACKER_LINE)
				{
					if(sequencerPtr->track[i].step[j - TRACKER_LINE - sequencer.ptrPlayer->row[i].actual_pos - stepOverZero].isOn) leds.setLEDseq(i+1,j+1,1,23);
					else leds.setLEDseq(i+1,j+1,1,8);
				}
				else
				{
					if(sequencerPtr->track[i].step[j - TRACKER_LINE - sequencer.ptrPlayer->row[i].actual_pos - stepOverZero].isOn) leds.setLEDseq(i+1,j+1,1,31);
					else leds.setLEDseq(i+1,j+1,0,31);
				}
			}
		}
	}
	leds.updateSeq();
}
*/

void SeqDisplay::updatePlayMode()
{
	int8_t maxTrackLen=0;

	maxTrackLen=getMaxTrackLen();
	if(!maxTrackLen) return;

	for(uint8_t i=0;i<8;i++)
	{
		for(uint8_t j=1;j<19;j++)
		{
			if(j==TRACKER_LINE) leds.setLEDseq(i+1,j+1,1,3);
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

		for(uint8_t j=1;j<19;j++)
		{
			if(!(j-1-sequencer.ptrPlayer->row[i].actual_pos ))
			{
				if(sequencerPtr->track[i].step[j-1].isOn) leds.setLEDseq(i+1,TRACKER_LINE - sequencer.ptrPlayer->row[i].actual_pos +j,1,28);
				else leds.setLEDseq(i+1,TRACKER_LINE  - sequencer.ptrPlayer->row[i].actual_pos + j,1,3);
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
	leds.updateSeq();

}


void SeqDisplay::updateStopMode()
{
	for(uint8_t i=0;i<8;i++)
	{
		if(sequencerPtr->track[i].isOn) leds.setLEDseq(i+1,1,1,31);
		else leds.setLEDseq(i+1,1,0,31);
	}
	for(uint8_t i=1;i<8;i++)
	{
		for(uint8_t j=0;j<19;j++)
		{
			if(sequencerPtr->track[i].step[j].isOn) leds.setLEDseq(i+1,j+1,1,31);
			else leds.setLEDseq(i+1,j+1,0,31);
		}
	}

}

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

SeqDisplay seqDisplay;
