#include  "mtRecorder.h"
//#include "mtFileManager.h"
void Recorder:: startRecording(int16_t * addr)
{
	currentAddress = addr;
	startAddress = addr;

	updateTimer.update(1000);

	queue.begin();
	if(mtConfig.audioCodecConfig.inSelect == inputSelectMic )
	{
		mixerRec.gain(0,1.0);
		mixerRec.gain(1,0.0);
	}
	else if(mtConfig.audioCodecConfig.inSelect == inputSelectLineIn)
	{
		mixerRec.gain(0,0.5);
		mixerRec.gain(1,0.5);
	}

	queue.begin();
	mode = recorderModeRec;
	recByteSaved = 0;
}

uint8_t Recorder::update()
{
	if(mode == recorderModeRec )
	{
		if ((queue.available() >= 1))
		{
			memcpy(currentAddress,queue.readBuffer(),256);
			queue.freeBuffer();
			currentAddress += 128;
			recByteSaved += 256;
		}
		if(recByteSaved >= SAMPLE_MEMORY_MAX)
		{
			queue.end();
			mode = recorderModeStop;
			return 0;
		}
	}
	return 1;
}


void Recorder::stopRecording()
{
	queue.end();
	if (mode == recorderModeRec)
	{
		while ((queue.available() > 0) )
		{
			update();
		}
	}
	mode = recorderModeStop;
	updateTimer.update(4500);
}


void Recorder::play(uint16_t start, uint16_t stop)
{
	uint32_t length;
	uint32_t addressShift;
	length =(uint32_t)((uint32_t)stop * (float)(recByteSaved/2)/MAX_16BIT);

	addressShift = (uint32_t)( (uint32_t)start * (float)(recByteSaved/2)/MAX_16BIT);

	instrumentPlayer[0].noteOnforPrev(startAddress + addressShift,length - addressShift,mtSampleTypeWaveFile);
}

void Recorder::stop()
{
	instrumentPlayer[0].noteOff();
}

void Recorder::trim(uint16_t a, uint16_t b)
{
	uint32_t addressShift;
	uint32_t lengthShift;

	addressShift = (uint32_t)( (uint32_t)a * (float)(recByteSaved/2)/MAX_16BIT);
	lengthShift =(uint32_t)((uint32_t)b * (float)(recByteSaved)/MAX_16BIT);
	startAddress+=addressShift;
	recByteSaved = lengthShift - 2*addressShift; //zamieniam probki na bajty
}

void Recorder::undo(int16_t * address, uint32_t length)
{
	startAddress=address;
	recByteSaved = 2*length; //zamieniam probki na bajty
}

uint8_t Recorder::startSave(char * name, uint8_t type)
{
	char currentPatch[PATCH_SIZE];


	if(!SD.exists("Recorded")) SD.mkdir(0,"Recorded");

	strcpy(currentPatch,"Recorded/");
	strcat(currentPatch,name);
	strcat(currentPatch,".wav");

	saveLength=recByteSaved;

	if(type == 0)
	{
		if (SD.exists(currentPatch)) return 0;
	}
	else if(type == 1)
	{
		if (SD.exists(currentPatch)) SD.remove(currentPatch);
	}

	saveInProgressFlag = 1;
	rec = SD.open(currentPatch, SD_FILE_WRITE);

	rec.write(currentPatch,sizeof(header)); //tablica ktora byla pod reka aby ustawic seek na 44 - funkcja seek powodowala blad
	currentAddress=startAddress;

	return 1;
}

uint8_t Recorder::startSaveLoad(char * name, uint8_t idx, uint8_t type)
{
	uint8_t status = startSave(name,type);


	if(!status) return 0;

	sprintf(currentName,"%s.wav",name);
	currentIndex = idx;
	loadAfterSaveFlag = 1;

	return 1;
}

void Recorder::updateSave()
{
	if(!saveInProgressFlag) return;
	if(saveLength)
	{
		if(saveLength > 2048)
		{
			rec.write(currentAddress,2048);
			currentAddress+=1024;
			saveLength-=2048;

		}
		else
		{
			rec.write(currentAddress,saveLength);
			saveLength=0;
			stopSave();
		}
	}
}
void Recorder::stopSave()
{
	saveInProgressFlag = 0;
	//writeOutHeader();
	if(loadAfterSaveFlag)
	{
		loadAfterSaveFlag = 0;
		//fileManager.setAutoLoadFlag();
		//fileManager.setStart(currentIndex);
		//fileManager.assignSampleToInstrument((char*)"Recorded", currentName, currentIndex);
		//todo
	}
}
uint8_t Recorder::getSaveProgress()
{
	return recByteSaved > 0 ? ((recByteSaved - saveLength)*100)/recByteSaved : 0;
}
uint8_t Recorder::getSaveState()
{
	return saveLength > 0 ? 1:0;
}


int16_t * Recorder::getAddress()
{
	return currentAddress;
}

int16_t * Recorder::getStartAddress()
{
	return startAddress;
}
uint32_t Recorder::getLength()
{
	return recByteSaved/2;
}

void Recorder::writeOutHeader()
{

	header.chunkId = 0x46464952; 																// "RIFF"
	header.chunkSize = recByteSaved + 36;
	header.format = 0x45564157;																	// "WAVE"
	header.subchunk1Id = 0x20746d66;															// "fmt "
	header.subchunk1Size = 16;
	header.AudioFormat = 1;
	header.numChannels = 1;
	header.sampleRate = 44100;
	header.bitsPerSample = 16;
	header.byteRate = header.sampleRate * header.numChannels * (header.bitsPerSample/8);
	header.blockAlign = header.numChannels * (header.bitsPerSample/8);
	header.subchunk2Id = 0x61746164;															// "data"
	header.subchunk2Size = recByteSaved;


	rec.seek(0);
	rec.write(&header,sizeof(header));
	rec.close();
}

Recorder recorder;

