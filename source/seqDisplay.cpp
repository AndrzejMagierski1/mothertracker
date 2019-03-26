#include "seqDisplay.h"

void SeqDisplay::init(Sequencer::strPattern * seq)
{
	sequencerPtr=seq;
	setMode(seqStop);
}

void SeqDisplay::update()
{
	if(state == seqPlay) updatePlayMode();
	else drawCurrentPosition();
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
			if(sequencerPtr->track[i].isOn)
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
			if(sequencerPtr->track[i].isOn)
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

SeqDisplay seqDisplay;
