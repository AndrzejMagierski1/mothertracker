#include "mtWavetableLoader.h"
#include "mtHardware.h"


uint8_t WavetableLoader::isWavetable(const char *filename)
{
	if(filename == nullptr) return 0;
	if( !(wavfile = SD.open(filename)) ) return 0;

	readHeader(&sampleHead,&wavfile);

	if( (sampleHead.format != 1163280727) || (sampleHead.AudioFormat != 3)  || (sampleHead.bitsPerSample != 32)
	|| (sampleHead.sampleRate != 44100) || (sampleHead.numChannels != 1)
	|| (sampleHead.subchunk2Size != 4 * SERUM_WAVETABLE_WINDOW_LEN * STANDARD_WAVETABLE_WINDOWS_NUMBER) ) return 0;

	return 1;
}

//void WavetableLoader::update()
//{
//	if(state == loadState::loadEnded) return;
//	if(wavfile.available())
//	{
//		bufferLength = wavfile.read(bufFloat, 8192) / 4;
//
//		for(int i = 0; i < bufferLength; i++)
//		{
//			*(destBuf++)=fmap(*(bufFloat+i),MIN_WAVE_FLOAT,MAX_WAVE_FLOAT,MIN_SIGNED_16BIT,MAX_SIGNED_16BIT);
//		}
//		sampleCounter+=bufferLength;
//	}
//	else
//	{
//		stop();
//	}
//}
//uint32_t WavetableLoader::start(const char *filename, int16_t * buf)
//{
//	if(buf == nullptr) return 0;
//
//	if ( (wavfile = SD.open(filename)) == nullptr )
//	{
//		stop();
//		return 0;
//	}
//
//	sampleCounter = 0;
//
//	destBuf = buf;
//
//	readHeader(&sampleHead,&wavfile);
//
//	if( (sampleHead.format != 1163280727) || (sampleHead.AudioFormat != 3)  || (sampleHead.bitsPerSample != 32)
//			|| (sampleHead.sampleRate != 44100) || (sampleHead.numChannels != 1)
//			|| (sampleHead.subchunk2Size != 4*SERUM_WAVETABLE_WINDOW_LEN*STANDARD_WAVETABLE_WINDOWS_NUMBER) )
//	{
//		stop();
//		return 0;
//	}
//	state = loadState::loadInProgress;
//
//	return SERUM_WAVETABLE_WINDOW_LEN*STANDARD_WAVETABLE_WINDOWS_NUMBER;
//}
//void WavetableLoader::stop()
//{
//
//}
//uint8_t WavetableLoader::getState()
//{
//
//}
//uint8_t WavetableLoader::getProgress()
//{
//
//}
//
////***********************************WAVETABLE STANDARD LOADER
//uint32_t WavetableLoader::WavetableStandardLoader::start(const char *filename, int16_t * buf)
//{
//	if(buf == nullptr)
//	{
//		return 0;
//	}
//
//	bufferLength=0;
//	windowDivider=0;
//	numberOfWindows=0;
//	windowsCounter=0;
//	bufferCounter=0;
//	middleWindowFlag = 0;
//	sampleCounter = 0;
//
//	destBuf = buf;
//	wavfile = SD.open(filename);
//
//	if(!wavfile)
//	{
//		stop();
//		return 0;
//	}
//
//	readHeader(&sampleHead,&wavfile);
//
//	if(sampleHead.format != 1163280727 || sampleHead.AudioFormat != 1  || sampleHead.bitsPerSample != 16
//			|| sampleHead.sampleRate != 44100 || sampleHead.numChannels != 1 )
//	{
//		stop();
//		return 0;
//	}
//
//	numberOfWindows = (sampleHead.subchunk2Size/sampleHead.blockAlign)/STANDARD_WAVETABLE_WINDOW_LEN;
//
//	if(numberOfWindows < STANDARD_WAVETABLE_WINDOWS_NUMBER)
//	{
//		missingWindows = STANDARD_WAVETABLE_WINDOWS_NUMBER - numberOfWindows;
//		windowsControl = (float) STANDARD_WAVETABLE_WINDOWS_NUMBER / (missingWindows + 1);
//	}
//
//	state = loadState::loadInProgress;
//	return STANDARD_WAVETABLE_WINDOW_LEN*STANDARD_WAVETABLE_WINDOWS_NUMBER;
//}
//
//void WavetableLoader::WavetableStandardLoader::update()
//{
//	if(state == loadState::loadEnded) return;
//
//	if(numberOfWindows == STANDARD_WAVETABLE_WINDOWS_NUMBER)
//	{
//		if(wavfile.available())
//		{
//			if(sampleHead.numChannels == 1)
//			{
//				if( wavfile.available() )
//				{
//					bufferLength = wavfile.read(destBuf, 1024);
//					destBuf += (bufferLength/2);
//					sampleCounter += bufferLength/2;
//				}
//			}
//		}
//		else
//		{
//			stop();
//		}
//	}
//	else if(numberOfWindows < STANDARD_WAVETABLE_WINDOWS_NUMBER)
//	{
//		if(middleWindowFlag)
//		{
//			if(currentMiddleWindow < windowDivider)
//			{
//				for(uint16_t j = 0; j< bufferLength; j++)
//				{
//					*(destBuf++) = *(currentWave + j) + ( ( ( *(nextWave + j ) - *(currentWave + j) ) / (windowDivider + 1) ) * currentMiddleWindow );
//					sampleCounter++;
//				}
//				currentMiddleWindow++;
//			}
//			else if(currentMiddleWindow == windowDivider)
//			{
//				memcpy(destBuf,nextWave,2*bufferLength);
//				memcpy(currentWave,nextWave, 2*bufferLength);
//
//				destBuf += bufferLength;
//				sampleCounter += bufferLength;
//				bufferCounter += (windowDivider + 1);
//				windowsCounter += (windowDivider * windowsControl);
//				middleWindowFlag = 0;
//				currentMiddleWindow = 0;
//			}
//		}
//		else
//		{
//			if(wavfile.available() )
//			{
//				if(sampleHead.numChannels == 1)
//				{
//					if( (bufferCounter < (uint16_t) windowsCounter) || ( (!bufferCounter) && (windowsCounter == 0.0f) ))
//					{
//						bufferLength = wavfile.read(destBuf, 2048);
//						memcpy(currentWave,destBuf,bufferLength);
//
//						destBuf += (bufferLength/2);
//						sampleCounter += (bufferLength/2);
//						bufferCounter++;
//					}
//					else
//					{
//						bufferLength = wavfile.read(nextWave, 2048) / 2;
//						windowDivider = getMiddleWindow(bufferCounter,windowsCounter,windowsControl);
//						middleWindowFlag = 1;
//						currentMiddleWindow = 0;
//					}
//				}
//			}
//			else
//			{
//				stop();
//			}
//		}
//	}
//}
//uint8_t WavetableLoader::WavetableStandardLoader::getCurrentState()
//{
//	return (uint8_t) state;
//}
//void WavetableLoader::WavetableStandardLoader::stop()
//{
//	wavfile.close();
//	state = loadState::loadEnded;
//}
//uint8_t WavetableLoader::WavetableStandardLoader::getProgress()
//{
//	return map(sampleCounter,0,STANDARD_WAVETABLE_WINDOW_LEN*STANDARD_WAVETABLE_WINDOWS_NUMBER,0,100);
//}
////*********************************************************
////***********************************WAVETABLE SERUM LOADER
//uint32_t WavetableLoader::WavetableSerumLoader::start(const char *filename, int16_t * buf)
//{
//	if(buf == nullptr)
//	{
//		return 0;
//	}
//
//	bufferLength=0;
//	windowDivider=0;
//	numberOfWindows=0;
//	windowsCounter=0;
//	bufferCounter=0;
//	middleWindowFlag = 0;
//	sampleCounter = 0;
//
//	destBuf = buf;
//	wavfile = SD.open(filename);
//
//	if(!wavfile)
//	{
//		stop();
//		return 0;
//	}
//
//	readHeader(&sampleHead,&wavfile);
//
//	if(sampleHead.format != 1163280727 || sampleHead.AudioFormat != 1  || sampleHead.bitsPerSample != 16
//			|| sampleHead.sampleRate != 44100 || sampleHead.numChannels != 1 )
//	{
//		stop();
//		return 0;
//	}
//
//	numberOfWindows = (sampleHead.subchunk2Size/sampleHead.blockAlign)/SERUM_WAVETABLE_WINDOW_LEN;
//
//	if(numberOfWindows < STANDARD_WAVETABLE_WINDOWS_NUMBER)
//	{
//		missingWindows = STANDARD_WAVETABLE_WINDOWS_NUMBER - numberOfWindows;
//		windowsControl = (float) STANDARD_WAVETABLE_WINDOWS_NUMBER / (missingWindows + 1);
//	}
//
//	state = loadState::loadInProgress;
//	return SERUM_WAVETABLE_WINDOW_LEN*STANDARD_WAVETABLE_WINDOWS_NUMBER;
//}
//void WavetableLoader::WavetableSerumLoader::update()
//{
//
//}
//uint8_t WavetableLoader::WavetableSerumLoader::getCurrentState()
//{
//	return (uint8_t) state;
//}
//void WavetableLoader::WavetableSerumLoader::stop()
//{
//	wavfile.close();
//	state = loadState::loadEnded;
//}
//
//uint8_t  WavetableLoader::WavetableSerumLoader::getProgress()
//{
//	return map(sampleCounter,0,SERUM_WAVETABLE_WINDOW_LEN*STANDARD_WAVETABLE_WINDOWS_NUMBER,0,100);
//}
//
////*********************************************************
//
//
//int32_t WavetableLoader::loadWavetable(const char *filename, int16_t * buf ,uint16_t * windowSize)
//{
//	strWavFileHeader sampleHead;
//	FsFile wavfile;
//	int32_t size=-1;
//
//	wavfile = SD.open(filename);
//	readHeader(&sampleHead,&wavfile);
//	wavfile.close();
//	if(sampleHead.AudioFormat == 1)
//	{
//		size=loadWavetableStandard(filename,buf);
//		*windowSize = STANDARD_WAVETABLE_WINDOW_LEN;
//	}
//	else if(sampleHead.AudioFormat == 3)
//	{
//		size=loadWavetableSerum(filename,buf);
//		*windowSize = SERUM_WAVETABLE_WINDOW_LEN;
//	}
//	return size;
//
//}
//
//int32_t loadWavetableStandard(const char *filename, int16_t * buf)
//{
//	strWavFileHeader sampleHead;
//	uint16_t bufferLength=0;
//	int16_t buf16[256];
//	int16_t buf16_1024[1024];
//	int16_t windowDivider=0;
//	int16_t currentWave[1024];
//	int16_t nextWave[1024];
//
//	FsFile wavfile;
//
//	uint16_t numberOfWindows=0;
//	uint16_t missingWindows=0;
//	float windowsControl=0;
//	float windowsCounter=0;
//	uint16_t buforCounter=0;
//
//	wavfile = SD.open(filename);
//	wavfile.read(&sampleHead, 44);
//
//
//
//	if(sampleHead.format != 1163280727 || sampleHead.AudioFormat != 1  || sampleHead.bitsPerSample != 16  || sampleHead.sampleRate != 44100 )
//	{
//		wavfile.close();
//		if(hardwareTest)
//		{
//			Serial.println("Bad WAV file or External RAM(if SD Card init is Correct");
//		}
//		return -1;
//	}
//	else
//	{
//		if(hardwareTest)
//		{
//			Serial.println("load WAV header succesfull");
//		}
//	}
//
//	numberOfWindows= (sampleHead.subchunk2Size/2)/STANDARD_WAVETABLE_WINDOW_LEN;
//	if(numberOfWindows == STANDARD_WAVETABLE_WINDOWS_NUMBER)
//	{
//		if(sampleHead.numChannels == 1)
//		{
//			while ( wavfile.available() )
//			{
//				bufferLength = wavfile.read(buf16, 512);
//				for(int i=0; i< 256; i++)
//				{
//					if(bufferLength <= i ) *buf=0;
//					else *buf=buf16[i];
//					buf++;
//				}
//			}
//		}
//		else if (sampleHead.numChannels == 2)
//		{
//			while (wavfile.available() )
//			{
//				bufferLength = wavfile.read(buf16, 512);
//				for(int i=0; i< 256; i+=2)
//				{
//					if(bufferLength <= i ) *buf=0;
//					else *buf=buf16[i];
//					buf++;
//				}
//			}
//		}
//
//		wavfile.close();
//
//	}
//
//	else if(numberOfWindows > STANDARD_WAVETABLE_WINDOWS_NUMBER)
//	{
//		;
//	}
//	else if(numberOfWindows < STANDARD_WAVETABLE_WINDOWS_NUMBER)
//	{
//		missingWindows=STANDARD_WAVETABLE_WINDOWS_NUMBER-numberOfWindows;
//		windowsControl=(float)STANDARD_WAVETABLE_WINDOWS_NUMBER/(missingWindows+1);
//
//		if(sampleHead.numChannels == 1)
//		{
//			while ( wavfile.available() )
//			{
//				if((buforCounter < (uint16_t) windowsCounter) || ((!buforCounter)&&(windowsCounter == 0.0f)))
//				{
//					bufferLength=wavfile.read(buf16_1024, 2048);
//					for(int i=0; i< 1024; i++)
//					{
//						if(bufferLength <= i ) *buf=0;
//						else *buf=buf16_1024[i];
//						buf++;
//						currentWave[i]=buf16_1024[i];
//					}
//					buforCounter++;
//				}
//				else
//				{
//					bufferLength=wavfile.read(nextWave, 2048);
//					windowDivider=getMiddleWindow(buforCounter,windowsCounter,windowsControl);
//
//					for(uint8_t i=0;i<windowDivider;i++)
//					{
//						for(uint16_t j=0; j< 1024; j++)
//						{
//							*buf=currentWave[j]+(((nextWave[j]-currentWave[j])/(windowDivider+1))*i);
//							buf++;
//						}
//					}
//					for(int i=0; i< 1024; i++)
//					{
//						if(bufferLength <= i ) *buf=0;
//						else *buf=nextWave[i];
//						buf++;
//
//						currentWave[i]=nextWave[i];
//					}
//					buforCounter++;
//					buforCounter+=(windowDivider);
//					windowsCounter+=(windowDivider*windowsControl);
//				}
//			}
//		}
//		wavfile.close();
//	}
//
//	return STANDARD_WAVETABLE_WINDOW_LEN*STANDARD_WAVETABLE_WINDOWS_NUMBER;
//}
//
//
//int32_t loadWavetableSerum(const char *filename, int16_t * buf)
//{
//	//strSerumWavetableFileHeader sampleHead;
//	strWavFileHeader sampleHead;
//	uint16_t bufferLength=0;
//
//	float bufFloat[2048];
//	float currentWave[2048];
//	float nextWave[2048];
//
//	int16_t windowDivider=0;
//	uint16_t numberOfWindows=0;
//	uint16_t missingWindows=0;
//	float windowsControl=0;
//	float windowsCounter=0;
//	uint16_t buforCounter=0;
//
//
//	int16_t currentInt=0;
//	int16_t nextInt=0;
//	FsFile wavfile;
//
//
//
//	wavfile = SD.open(filename);
//	readHeader(&sampleHead,&wavfile);
//
//
//
//	if(sampleHead.format != 1163280727 || sampleHead.AudioFormat != 3  || sampleHead.bitsPerSample != 32  || sampleHead.sampleRate != 44100)
//	{
//		wavfile.close();
//		if(hardwareTest)
//		{
//			Serial.println("Bad WAV file or External RAM(if SD Card init is Correct");
//			//mtPrint("Bad WAV file or External RAM(if SD Card init is Correct");
//		}
//		return -1;
//	}
//	else
//	{
//		if(hardwareTest)
//		{
//			Serial.println("load WAV header to SDRAM succesfull");
//			//mtPrint("load WAV header to SDRAM succesfull");
//		}
//	}
//
//	numberOfWindows= (sampleHead.subchunk2Size/4)/SERUM_WAVETABLE_WINDOW_LEN;
//	if(numberOfWindows == STANDARD_WAVETABLE_WINDOWS_NUMBER)
//	{
//
//		while ( wavfile.available() )
//		{
//			bufferLength = wavfile.read(bufFloat, 8192);
//
//			for(int i=0; i< 2048; i++)
//			{
//				if(bufferLength <= i ) *buf=0;
//				else *buf=fmap(bufFloat[i],MIN_WAVE_FLOAT,MAX_WAVE_FLOAT,MIN_SIGNED_16BIT,MAX_SIGNED_16BIT);
//				buf++;
//			}
//		}
//
//		wavfile.close();
//	}
//
//	else if(numberOfWindows > STANDARD_WAVETABLE_WINDOWS_NUMBER)
//	{
//		;
//	}
//	else if(numberOfWindows < STANDARD_WAVETABLE_WINDOWS_NUMBER)
//	{
//		missingWindows=STANDARD_WAVETABLE_WINDOWS_NUMBER-numberOfWindows;
//		windowsControl=(float)STANDARD_WAVETABLE_WINDOWS_NUMBER/(missingWindows+1);
//
//
//		while ( wavfile.available() )
//		{
//			if((buforCounter < (uint16_t) windowsCounter) || ((!buforCounter)&&(windowsCounter == 0.0f)))
//			{
//				bufferLength=wavfile.read(bufFloat, 8192);
//				for(int i=0; i< 2048; i++)
//				{
//					if(bufferLength <= i ) *buf=0;
//					else *buf=fmap(bufFloat[i],MIN_WAVE_FLOAT,MAX_WAVE_FLOAT,(const float)MIN_SIGNED_16BIT,(const float)MAX_SIGNED_16BIT);
//					buf++;
//					currentWave[i]=bufFloat[i];
//				}
//				buforCounter++;
//			}
//			else
//			{
//				bufferLength=wavfile.read(nextWave, 8192);
//				windowDivider=getMiddleWindow(buforCounter,windowsCounter,windowsControl);
//
//				for(uint8_t i=0;i<windowDivider;i++)
//				{
//					for(uint16_t j=0; j< 2048; j++)
//					{
//						currentInt=fmap(currentWave[j],MIN_WAVE_FLOAT,MAX_WAVE_FLOAT,(const float)MIN_SIGNED_16BIT,(const float)MAX_SIGNED_16BIT);
//						nextInt=fmap(nextWave[j],MIN_WAVE_FLOAT,MAX_WAVE_FLOAT,(const float)MIN_SIGNED_16BIT,(const float)MAX_SIGNED_16BIT);
//						*buf=currentInt+(((nextInt-currentInt)/(windowDivider+1))*i);
//						buf++;
//					}
//				}
//				for(int i=0; i< 2048; i++)
//				{
//					if(bufferLength <= i ) *buf=0;
//					else *buf=fmap(nextWave[i],MIN_WAVE_FLOAT,MAX_WAVE_FLOAT,(const float)MIN_SIGNED_16BIT,(const float)MAX_SIGNED_16BIT);;
//					buf++;
//
//					currentWave[i]=nextWave[i];
//				}
//				buforCounter++;
//				buforCounter+=(windowDivider);
//				windowsCounter+=(windowDivider*windowsControl);
//			}
//		}
//		wavfile.close();
//
//
//	}
//
//	return SERUM_WAVETABLE_WINDOW_LEN*STANDARD_WAVETABLE_WINDOWS_NUMBER;
//}
//
//
//uint8_t getMiddleWindow(uint16_t cnt, float windowsCnt, float windowsControl)
//{
//	uint8_t localCounter=0;
//	while(cnt >= (uint16_t)windowsCnt)
//	{
//		localCounter++;
//		cnt++;
//		windowsCnt+=windowsControl;
//	}
//	return localCounter;
//
//}
//
//int16_t fmap(float x, float in_min, float in_max, float out_min, float out_max)
//{
//	  return (int16_t)( (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
//}
//
//void loadFullWavetableSerum(const char *baseName)
//{
//	strWavFileHeader sampleHead;
//	uint8_t tabSize=0;
//	uint8_t newTabSize=0;
//	float currentWave[2048];
//	FsFile wavfile;
//	FsFile result;
//	while( *(baseName+tabSize) != 0 )
//	{
//		tabSize++;
//	}
//	newTabSize=tabSize+5;
//	tabSize+=9;
//
//	char newName[newTabSize];
//	char name[tabSize];
//
//	for(uint8_t i=0;i< (newTabSize - 5); i++)
//	{
//		newName[i]=baseName[i];
//	}
//	newName[newTabSize -1] = 0;
//	newName[newTabSize -2] = 'v';
//	newName[newTabSize -3] = 'a';
//	newName[newTabSize -4] = 'w';
//	newName[newTabSize -5] = '.';
//
//	if(SD.exists(newName)) return;
//	for(uint8_t i=0;i< tabSize; i++)
//	{
//		if(i< (tabSize-9)) name[i]=baseName[i];
//		else if(i== (tabSize -1 ) ) name[i] = 0;
//		else if(i== (tabSize -2 ) ) name[i] = 'V';
//		else if(i== (tabSize -3 ) ) name[i] = 'A';
//		else if(i== (tabSize -4 ) ) name[i] = 'W';
//		else if(i== (tabSize -5 ) ) name[i] = '.';
//		else if(i== (tabSize -6 ) ) name[i] = '1';
//		else if(i== (tabSize -9 ) ) name[i] = '_';
//		else name[i] ='0';
//	}
//
//
//	result = SD.open(newName,FILE_WRITE);
//	for(uint16_t a=0; a < STANDARD_WAVETABLE_WINDOWS_NUMBER;a++)
//	{
//
//		if(!a)
//		{
//			wavfile = SD.open(name);
//			wavfile.read(&sampleHead,44);
//
//			sampleHead.subchunk2Size=STANDARD_WAVETABLE_WINDOWS_NUMBER* SERUM_WAVETABLE_WINDOW_LEN*4;
//			result.write(&sampleHead,44);
//		}
//		else
//		{
//			wavfile = SD.open(name);
//			wavfile.read(&sampleHead,44);
//		}
//
//
//		wavfile.read(currentWave, 8192);
//		result.write(currentWave, 8192);
//
//
//		wavfile.close();
//
//		if(name[tabSize - 6 ] < '9') name[tabSize - 6 ]++;
//		else if(name[tabSize - 6 ] == '9')
//		{
//			name[tabSize - 6 ] = '0';
//			if( name[tabSize - 7 ] < '9') name[tabSize - 7 ]++;
//			else if(name[tabSize - 7 ] == '9')
//			{
//				name[tabSize - 7 ]= '0';
//				name[tabSize - 8]++;
//			}
//		}
//	}
//	result.close();
//}

