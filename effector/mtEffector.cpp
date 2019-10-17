#include "mtEffector.h"

mtEffector effector;

void mtEffector::loadSample(const char *patch)
{
	fileByteSaved = 2 * fileManager.samplesLoader.waveLoader.start(patch,applyBuffer);
	if(fileByteSaved == 0) return; //todo: obsluga bledu
	startAddress=applyBuffer;
}

void mtEffector::play(uint16_t start, uint16_t stop)
{
	uint32_t length;
	uint32_t addressShift;
	length =(uint32_t)((uint32_t)stop * (float)(fileByteSaved/2)/MAX_16BIT);

	addressShift = (uint32_t)( (uint32_t)start * (float)(fileByteSaved/2)/MAX_16BIT);

	instrumentPlayer[0].noteOnforPrev(startAddress + addressShift,length - addressShift);
}

void mtEffector::playPrev()
{
	startAddressEffect = previewBuffer;

	instrumentPlayer[0].noteOnforPrev(startAddressEffect,affterEffectLength);
}
void mtEffector::stop()
{
	instrumentPlayer[0].noteOff();
}

void mtEffector::trim(uint16_t a, uint16_t b)
{
	uint32_t addressShift;
	uint32_t lengthShift;
	int16_t * localEffectAddress = previewBuffer;

	addressShift = (uint32_t)((uint32_t)a * (float)(fileByteSaved/2)/MAX_16BIT);
	lengthShift = (uint32_t)((uint32_t)b * (float)(fileByteSaved)/MAX_16BIT);

	undoCropLength = fileByteSaved;
	undoCropStart = startAddress;

	startAddress += addressShift;
	fileByteSaved = (lengthShift - 2*addressShift);

	affterEffectLength = fileByteSaved;

	memcpy(localEffectAddress,startAddress,fileByteSaved);
}

void mtEffector::clearMainBuffer()
{
	uint32_t bytesToClear = 0;

	bytesToClear = (previewBuffer - applyBuffer);

	memset(applyBuffer,0,bytesToClear);
}

void mtEffector::undoTrim()
{
	fileByteSaved = undoCropLength;
	startAddress = undoCropStart;
}

void mtEffector::undoReverse()
{
	reverse(undoReverseStart,undoReverseEnd);
}

void mtEffector::reverse(uint16_t start, uint16_t end)
{
	int16_t *localStartAddress;
	int16_t *localEndAddress;

	uint32_t localStartShift;
	uint32_t localEndShift;

	undoReverseStart = start;
	undoReverseEnd = end;

	localStartShift = (uint32_t)((uint32_t)start * (float)(fileByteSaved/2)/MAX_16BIT);
	localEndShift = (uint32_t)((uint32_t)end * (float)(fileByteSaved/2)/MAX_16BIT);

	localStartAddress = startAddress + localStartShift;
	localEndAddress = startAddress + localEndShift;

	while((localEndAddress - localStartAddress) >= 4)
	{
		swap(localStartAddress,localEndAddress);
		localStartAddress++;
		localEndAddress--;
	}

	affterEffectLength = fileByteSaved;
}

void mtEffector::save(const char *patch)
{
	if((saveStage == waitingForSaveInit) || (saveStage == saveDone))
	{
		if(SD.exists(patch)) SD.remove(patch);

		saveLength=fileByteSaved;
		saveLengthMax=fileByteSaved;

		file = SD.open(patch, FILE_WRITE);

		file.seek(44);
		currentAddress=startAddress;
		saveStage = saving;
	}
}

uint8_t mtEffector::saveUpdate()
{
	uint8_t progress = 0;

	if(saveStage == saving)
	{
		if(saveLength)
		{
			if(saveLength >= 2048)
			{
				file.write(currentAddress,2048);
				currentAddress+=1024;
				saveLength-=2048;
			}
			else
			{
				file.write(currentAddress,saveLength);
				writeOutHeader();
				saveStage = saveDone;
				saveLength = 0;
			}

			progress = ((saveLengthMax - saveLength) * 100) / saveLengthMax;
		}
	}

	return progress;
}

save_stages_t mtEffector::getSaveStatus()
{
	return saveStage;
}

void mtEffector::setSaveStatus(save_stages_t status)
{
	saveStage = status;
}

void mtEffector::setEffects()
{
	uint32_t localLength = affterEffectLength;

	int16_t * localAddress = startAddress ;
	int16_t * localEffectAddress = startAddressEffect;

	fileByteSaved = 2*affterEffectLength;

	memcpy(localAddress,localEffectAddress,2*localLength);
}

void mtEffector::writeOutHeader()
{
	Subchunk2Size = fileByteSaved;
	ChunkSize = Subchunk2Size + 36;
	file.seek(0);
	file.write("RIFF",4);
	byte1 = ChunkSize & 0xff;
	byte2 = (ChunkSize >> 8) & 0xff;
	byte3 = (ChunkSize >> 16) & 0xff;
	byte4 = (ChunkSize >> 24) & 0xff;
	file.write(byte1);  file.write(byte2);  file.write(byte3);  file.write(byte4);
	file.write("WAVE",4);
	file.write("fmt ",4);
	byte1 = Subchunk1Size & 0xff;
	byte2 = (Subchunk1Size >> 8) & 0xff;
	byte3 = (Subchunk1Size >> 16) & 0xff;
	byte4 = (Subchunk1Size >> 24) & 0xff;
	file.write(byte1);  file.write(byte2);  file.write(byte3);  file.write(byte4);
	byte1 = AudioFormat & 0xff;
	byte2 = (AudioFormat >> 8) & 0xff;
	file.write(byte1);  file.write(byte2);
	byte1 = numChannels & 0xff;
	byte2 = (numChannels >> 8) & 0xff;
	file.write(byte1);  file.write(byte2);
	byte1 = sampleRate & 0xff;
	byte2 = (sampleRate >> 8) & 0xff;
	byte3 = (sampleRate >> 16) & 0xff;
	byte4 = (sampleRate >> 24) & 0xff;
	file.write(byte1);  file.write(byte2);  file.write(byte3);  file.write(byte4);
	byte1 = byteRate & 0xff;
	byte2 = (byteRate >> 8) & 0xff;
	byte3 = (byteRate >> 16) & 0xff;
	byte4 = (byteRate >> 24) & 0xff;
	file.write(byte1);  file.write(byte2);  file.write(byte3);  file.write(byte4);
	byte1 = blockAlign & 0xff;
	byte2 = (blockAlign >> 8) & 0xff;
	file.write(byte1);  file.write(byte2);
	byte1 = bitsPerSample & 0xff;
	byte2 = (bitsPerSample >> 8) & 0xff;
	file.write(byte1);  file.write(byte2);
	file.write("data",4);
	byte1 = Subchunk2Size & 0xff;
	byte2 = (Subchunk2Size >> 8) & 0xff;
	byte3 = (Subchunk2Size >> 16) & 0xff;
	byte4 = (Subchunk2Size >> 24) & 0xff;
	file.write(byte1);  file.write(byte2);  file.write(byte3);  file.write(byte4);
	file.close();
}

int32_t mtEffector::getLength()
{
	return fileByteSaved;
}

int16_t * mtEffector:: getAddress()
{
	return startAddress;
}

void mtEffector::swap(int16_t *p1, int16_t *p2)
{
	int16_t temp;

	temp = *p1;
	*p1 = *p2;
	*p2 = temp;
}
