#include  "mtRecorder.h"

void Recorder:: startRecording(char * name, int16_t * addr)
{
	strcpy(currentName,name);
	currentAddress = addr;
	startAddress = addr;
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

	mode = recorderModeRec;
	recByteSaved = 0;
}

void Recorder::update()
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
		if(recByteSaved >= SAMPLE_MEMORY_MAX) stopRecording();
	}
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
}


void Recorder::play(uint16_t start, uint16_t stop)
{
uint32_t length;
uint32_t addressShift;
length =(uint32_t)((uint32_t)stop * (float)(recByteSaved/2)/MAX_16BIT);

addressShift = (uint32_t)( (uint32_t)start * (float)(recByteSaved/2)/MAX_16BIT);

instrumentPlayer[0].noteOnforPrev(startAddress + addressShift,length);
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
	lengthShift =(uint32_t)((uint32_t)b * (float)(recByteSaved/2)/MAX_16BIT);
	startAddress+=addressShift;
	recByteSaved -= 2*lengthShift; //zamieniam probki na bajty
}

void Recorder::save(char * patch, char * name)
{
	char currentPatch[PATCH_SIZE];
	uint32_t length;
	if(patch[0] != 0)
	{
		strcpy(currentPatch,patch);
		strcat(currentPatch,"/");
		strcat(currentPatch,name);
	}
	else strcpy(currentPatch,name);

	length=recByteSaved;

	if (SD.exists(currentPatch)) SD.remove(currentPatch);

	rec = SD.open(currentPatch, FILE_WRITE);


	rec.seek(44);
	currentAddress=startAddress;
	while(length)
	{
		if(length >= 2048)
		{
			rec.write(currentAddress,2048);
			currentAddress+=1024;
			length-=2048;
		}
		else
		{
			rec.write(currentAddress,length);
			length=0;
		}
	}
	writeOutHeader();
}

int16_t * Recorder::getAddress()
{
	return currentAddress;
}

uint32_t Recorder::getLength()
{
	return recByteSaved/2;
}

void Recorder::writeOutHeader()
{
	Subchunk2Size = recByteSaved;
	ChunkSize = Subchunk2Size + 36;
	rec.seek(0);
	rec.write("RIFF",4);
	byte1 = ChunkSize & 0xff;
	byte2 = (ChunkSize >> 8) & 0xff;
	byte3 = (ChunkSize >> 16) & 0xff;
	byte4 = (ChunkSize >> 24) & 0xff;
	rec.write(byte1);  rec.write(byte2);  rec.write(byte3);  rec.write(byte4);
	rec.write("WAVE",4);
	rec.write("fmt ",4);
	byte1 = Subchunk1Size & 0xff;
	byte2 = (Subchunk1Size >> 8) & 0xff;
	byte3 = (Subchunk1Size >> 16) & 0xff;
	byte4 = (Subchunk1Size >> 24) & 0xff;
	rec.write(byte1);  rec.write(byte2);  rec.write(byte3);  rec.write(byte4);
	byte1 = AudioFormat & 0xff;
	byte2 = (AudioFormat >> 8) & 0xff;
	rec.write(byte1);  rec.write(byte2);
	byte1 = numChannels & 0xff;
	byte2 = (numChannels >> 8) & 0xff;
	rec.write(byte1);  rec.write(byte2);
	byte1 = sampleRate & 0xff;
	byte2 = (sampleRate >> 8) & 0xff;
	byte3 = (sampleRate >> 16) & 0xff;
	byte4 = (sampleRate >> 24) & 0xff;
	rec.write(byte1);  rec.write(byte2);  rec.write(byte3);  rec.write(byte4);
	byte1 = byteRate & 0xff;
	byte2 = (byteRate >> 8) & 0xff;
	byte3 = (byteRate >> 16) & 0xff;
	byte4 = (byteRate >> 24) & 0xff;
	rec.write(byte1);  rec.write(byte2);  rec.write(byte3);  rec.write(byte4);
	byte1 = blockAlign & 0xff;
	byte2 = (blockAlign >> 8) & 0xff;
	rec.write(byte1);  rec.write(byte2);
	byte1 = bitsPerSample & 0xff;
	byte2 = (bitsPerSample >> 8) & 0xff;
	rec.write(byte1);  rec.write(byte2);
	rec.write("data",4);
	byte1 = Subchunk2Size & 0xff;
	byte2 = (Subchunk2Size >> 8) & 0xff;
	byte3 = (Subchunk2Size >> 16) & 0xff;
	byte4 = (Subchunk2Size >> 24) & 0xff;
	rec.write(byte1);  rec.write(byte2);  rec.write(byte3);  rec.write(byte4);
	rec.close();
}

Recorder recorder;
