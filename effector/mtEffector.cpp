#include "mtEffector.h"
#include "mtPadBoard.h"

//#include "mtFileManager.h"
#include "fileManager.h"

mtEffector effector;

void mtEffector::loadSample(const char *patch)
{
	//load sampel to memory efector //todo
	//fileByteSaved = 2 * fileManager.samplesLoader.waveLoader.start(patch,applyBuffer);

//	if(fileByteSaved == 0) return; //todo: obsluga bledu
	startAddress=applyBuffer;
}

void mtEffector::play(uint16_t start, uint16_t stop, uint8_t pad)
{
	uint32_t length;
	uint32_t addressShift;
	int8_t voiceToTake = mtPadBoard.getEmptyVoice();

	if(voiceToTake < 0) return;

	length =(uint32_t)((uint32_t)stop * (float)(bytesSaved/2)/MAX_16BIT);
	addressShift = (uint32_t)( (uint32_t)start * (float)(bytesSaved/2)/MAX_16BIT);

	mtPadBoard.startInstrument(pad, startAddress + addressShift, length - addressShift);
}

void mtEffector::playPrev(uint8_t pad)
{
	int8_t voiceToTake = mtPadBoard.getEmptyVoice();
	if(voiceToTake < 0) return;

	mtPadBoard.startInstrument(pad, startAddressEffect, affterEffectLength);
}
void mtEffector::stop(uint8_t pad)
{
	mtPadBoard.stopInstrument(pad);
}

void mtEffector::trim(uint16_t a, uint16_t b)
{
	uint32_t addressShift;
	uint32_t lengthShift;
	int16_t * localEffectAddress = previewBuffer;

	addressShift = (uint32_t)((uint32_t)a * (float)(bytesSaved/2)/MAX_16BIT);
	lengthShift = (uint32_t)((uint32_t)b * (float)(bytesSaved)/MAX_16BIT);

	undoCropLength = bytesSaved;
	undoCropStart = startAddress;

	startAddress += addressShift;
	bytesSaved = (lengthShift - 2*addressShift);

	affterEffectLength = bytesSaved;

	memcpy(localEffectAddress,startAddress,bytesSaved);
}

void mtEffector::clearMainBuffer()
{
	uint32_t bytesToClear = 0;

	bytesToClear = (previewBuffer - applyBuffer);

	memset(applyBuffer,0,bytesToClear);
}

void mtEffector::undoTrim()
{
	bytesSaved = undoCropLength;
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

	localStartShift = (uint32_t)((uint32_t)start * (float)(bytesSaved/2)/MAX_16BIT);
	localEndShift = (uint32_t)((uint32_t)end * (float)(bytesSaved/2)/MAX_16BIT);

	localStartAddress = startAddress + localStartShift;
	localEndAddress = startAddress + localEndShift;

	while((localEndAddress - localStartAddress) >= 4)
	{
		swap(localStartAddress,localEndAddress);
		localStartAddress++;
		localEndAddress--;
	}

	affterEffectLength = bytesSaved;
}

void mtEffector::save(const char *patch)
{
	if((saveStatus == enSaveStatus::waitingForSaveInit) || (saveStatus == enSaveStatus::saveDone))
	{
		if(SD.exists(patch)) SD.remove(patch);

		saveLength=bytesSaved;
		saveLengthMax=bytesSaved;

		file = SD.open(patch, FILE_WRITE);

		file.seek(44);
		currentAddress=startAddress;
		saveStatus = enSaveStatus::saving;
	}
}

uint8_t mtEffector::saveUpdate()
{
	uint8_t progress = 0;

	if(saveStatus == enSaveStatus::saving)
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
			saveStatus = enSaveStatus::saveDone;
			saveLength = 0;
		}

		progress = ((saveLengthMax - saveLength) * 100) / saveLengthMax;

	}

	return progress;
}

mtEffector::enSaveStatus mtEffector::getSaveStatus()
{
	return saveStatus;
}

void mtEffector::setSaveStatus(enSaveStatus status)
{
	saveStatus = status;
}

void mtEffector::setEffects()
{
	uint32_t localLength = affterEffectLength;

	int16_t * localAddress = startAddress ;
	int16_t * localEffectAddress = startAddressEffect;

	bytesSaved = 2*affterEffectLength;

	memcpy(localAddress,localEffectAddress,2*localLength);
}

void mtEffector::writeOutHeader()
{
	strWavFileHeader header;

	header.chunkId = 0x46464952; 																// "RIFF"
	header.chunkSize = bytesSaved + 36;
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
	header.subchunk2Size = bytesSaved;


	file.seek(0);
	file.write(&header,sizeof(header));
	file.close();
}

int32_t mtEffector::getLength()
{
	return bytesSaved;
}

int16_t * mtEffector:: getAddress()
{
	return startAddress;
}

int32_t mtEffector::getEffectLength()
{
	return affterEffectLength;
}

int16_t *mtEffector::getEffectAddress()
{
	return previewBuffer;
}

void mtEffector::swap(int16_t *p1, int16_t *p2)
{
	int16_t temp;

	temp = *p1;
	*p1 = *p2;
	*p2 = temp;
}
