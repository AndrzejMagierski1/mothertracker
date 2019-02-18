//#include "mtDisplay.h"
#include "mtProjectEditor.h"
#include "mtHardware.h"
#include "SD.h"
#include "sdram.h"
//#include "Arduino.h"
#include "spi_interrupt.h"
#include "sdram.h"
//#include "AudioStream.h"


//cr_section_macros.h

cMtProjectEditor mtProjectEditor;


strMtProject mtProject;

char * currentDirectoryOpen = "/Project001";
char * currentDirectory= "Project001";

__NOINIT(EXTERNAL_RAM) int16_t sdram_sampleBank[4*1024*1024];


void cMtProjectEditor::update()
{

	if(commandsToDo[ProjEditCommandOpenLastProject])
	{
		commandsToDo[ProjEditCommandOpenLastProject] = 0;

		uint8_t result = loadLastProject();
		if(result)
		{
			// jesli nie mozna zaladowac ostatneigo projektu
			// to poinformuj o tym interfejs
			//projectEditorEvent(ProjEditEventLoadLastProjFailed, &result,0,0);
		}
	}




}


void cMtProjectEditor::startProject()
{
	commandsToDo[ProjEditCommandOpenLastProject] = 1;




}

//------------------------------------------------------------------------------
uint8_t cMtProjectEditor::readProjectConfig()
{
	// parametry sampli ==============================================

	// trzeba czytac z pliku projektu zawierajacego opis banku sampli
	// pod jaki index tablicy sampli 0-32 zapisywac dany sampel
	// teraz domyslnie zajmowane 0-7

	for(uint8_t i = 0; i < 8; i++) // max do 9
	{
											// mtSampleTypeWaveFile
		mtProject.sampleBank.sample[i].type = mtSampleTypeWavetable;
		mtProject.sampleBank.sample[i].file_name[0] = i+49;
		mtProject.sampleBank.sample[i].file_name[1] = '.';
		mtProject.sampleBank.sample[i].file_name[2] = 'w';
		mtProject.sampleBank.sample[i].file_name[3] = 'a';
		mtProject.sampleBank.sample[i].file_name[4] = 'v';
		mtProject.sampleBank.sample[i].file_name[5] = 0;
		mtProject.sampleBank.sample[i].wavetable_window_size = 1024;
	}

	//mtProject.sampleBank.sample[1].wavetable_window_size = 1024;
	//mtProject.sampleBank.sample[1].type = mtSampleTypeWaveFile;

	// parametry instrumentow ========================================
	mtProject.instruments_count = 8;

	for(uint8_t i = 0; i < mtProject.instruments_count; i++)
	{
		mtProject.instrument[i].sampleIndex = i;

		mtProject.instrument[i].playMode = 1;

		mtProject.instrument[i].startPoint = 0;
		mtProject.instrument[i].loopPoint1 = 10000;
		mtProject.instrument[i].loopPoint2 = 20000;
		mtProject.instrument[i].endPoint = SAMPLE_POINT_POS_MAX;

		mtProject.instrument[i].envelope[envAmp].delay = 0;

		mtProject.instrument[i].envelope[envAmp].attack = 0;
		mtProject.instrument[i].envelope[envAmp].hold = 0;
		mtProject.instrument[i].envelope[envAmp].decay = 0;
		mtProject.instrument[i].envelope[envAmp].sustain = 1.0;
		mtProject.instrument[i].envelope[envAmp].release = 1000;
		mtProject.instrument[i].envelope[envAmp].amount = 1.0;
		mtProject.instrument[i].envelope[envAmp].enable = envelopeOn;

		mtProject.instrument[i].envelope[envFilter].delay = 0;
		mtProject.instrument[i].envelope[envFilter].attack = 3000;
		mtProject.instrument[i].envelope[envFilter].hold = 0;
		mtProject.instrument[i].envelope[envFilter].decay = 0;
		mtProject.instrument[i].envelope[envFilter].sustain = 1.0;
		mtProject.instrument[i].envelope[envFilter].release = 1000;
		mtProject.instrument[i].envelope[envFilter].amount = 1.0;
		mtProject.instrument[i].envelope[envFilter].enable = envelopeOff;

		mtProject.instrument[i].cutOff = 1.0;
		mtProject.instrument[i].filterEnable = filterOff;
		mtProject.instrument[i].filterType = lowPass;
		mtProject.instrument[i].resonance = 0;
		mtProject.instrument[i].panning = 50;
		mtProject.instrument[i].glide = 10000;
		mtProject.instrument[i].volume = 100;

		if(i >= 10)
		{
			mtProject.instrument[i].name[0] = (i+1)/10 + 48;
			mtProject.instrument[i].name[1] = (i+1)%10 + 48;
			mtProject.instrument[i].name[2] = 0;
		}
		else
		{
			mtProject.instrument[i].name[0] = (i+1)%10 + 48;
			mtProject.instrument[i].name[1] = 0;
		}

	}

	mtProject.instrument[0].startPoint = 19000;
	mtProject.instrument[0].loopPoint1 = 19000; //PRO
	mtProject.instrument[0].loopPoint2 = 29000;

	mtProject.instrument[0].lfo[lfoA].rate=600;
	mtProject.instrument[0].lfo[lfoA].wave=0;
	mtProject.instrument[0].lfo[lfoA].amount=4095;
	mtProject.instrument[0].lfo[lfoA].sync=0;
	mtProject.instrument[0].lfo[lfoA].enable=lfoOff;



	// parametry paternu ========================================



	return 0;
}

//-------------------------------------------------------------------------------
uint8_t cMtProjectEditor::loadSamplesBank()
{
	//zaladowanie banku sampli
	int32_t size;
	mtProject.sampleBank.sample[0].address = sdram_sampleBank;
	mtProject.sampleBank.samples_count = 0;

	for(uint8_t i = 0; i < SAMPLES_MAX; i++)
	{

		if(mtProject.sampleBank.sample[i].type == mtSampleTypeWavetable)
		{
			size = loadWavetable(mtProject.sampleBank.sample[i].file_name, mtProject.sampleBank.sample[i].address, &mtProject.sampleBank.sample[i].wavetable_window_size);

		}
		else
		{
			size = loadSample(mtProject.sampleBank.sample[i].file_name, mtProject.sampleBank.sample[i].address);
		}


		if(size > 0)
		{
			mtProject.sampleBank.used_memory += size*2;
			mtProject.sampleBank.sample[i].loaded = 1;
			mtProject.sampleBank.sample[i].length = size;

			mtProject.sampleBank.samples_count++;
		}
		else
		{
			mtProject.sampleBank.sample[i].loaded = 0;
			mtProject.sampleBank.sample[i].length = 0;
			mtProject.sampleBank.sample[i].file_name[0] = '-';
			mtProject.sampleBank.sample[i].file_name[1] = 'e';
			mtProject.sampleBank.sample[i].file_name[2] = 'm';
			mtProject.sampleBank.sample[i].file_name[3] = 'p';
			mtProject.sampleBank.sample[i].file_name[4] = 't';
			mtProject.sampleBank.sample[i].file_name[5] = 'y';
			mtProject.sampleBank.sample[i].file_name[6] = '-';
			mtProject.sampleBank.sample[i].file_name[7] = 0;
			size = 0;
			//return 2; // blad ladowania wave
		}

		if(i+1 < SAMPLES_MAX)
		{
			mtProject.sampleBank.sample[i+1].address = mtProject.sampleBank.sample[i].address+size;
		}
		if(mtProject.sampleBank.used_memory > mtProject.sampleBank.max_memory) return 1; // out of memory
	}

	return 0;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
uint8_t cMtProjectEditor::loadLastProject()
{
	if(readProjectConfig())
	{
		mtPrintln("loading config file failed!");
		return 1;
	}

	if(loadSamplesBank())
	{
		mtPrintln("loading samples failed!");
		return 2;
	}
	else
	{
//		mtPrint(mtProject.sampleBank.samples_count);
//		mtPrintln(" samples loaded successfully");
//		mtPrint("sample memory used: ");
//		mtPrint( int((mtProject.sampleBank.used_memory * 100 ) / mtProject.sampleBank.max_memory));
//		mtPrintln(" %");
	}





	return 0;
}


//-------------------------------------------------------------------------------
uint8_t cMtProjectEditor::isProjectLoaded()
{


	return 0;
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
uint8_t cMtProjectEditor::padsChange(uint8_t type, uint8_t n, uint8_t velo)
{
	eventFunct(mtPriojectEditorEventPadPress, &n, 0, 0);

	return 0;
}

void cMtProjectEditor::buttonChange(uint8_t button, uint8_t value)
{


}

void cMtProjectEditor::potChange(uint8_t pot, int16_t value)
{


}

void cMtProjectEditor::seqButtonChange(uint8_t type, uint8_t x, uint8_t y)
{


}

void cMtProjectEditor::writeInstrumentFile(char * name, strInstrument * instr)
{
	if(SD.exists(name)) return; //todo: do ustalenia czy nadpisujemy czy nie

	FsFile file;
	FastCRC32 crcCalc;

	strInstrumentFile instrumentFile;


	instrumentFile.instrumentDataAndHeader.instrument = * instr;


	instrumentFile.instrumentDataAndHeader.instrHeader.id_file[0]='I';
	instrumentFile.instrumentDataAndHeader.instrHeader.id_file[1]='D';
	instrumentFile.instrumentDataAndHeader.instrHeader.type = fileTypeInstrument;
	instrumentFile.instrumentDataAndHeader.instrHeader.version[0] = '0';
	instrumentFile.instrumentDataAndHeader.instrHeader.version[1] = '.';
	instrumentFile.instrumentDataAndHeader.instrHeader.version[2] = '0';
	instrumentFile.instrumentDataAndHeader.instrHeader.version[3] = '1';
	instrumentFile.instrumentDataAndHeader.instrHeader.id_data[0] = 'D';
	instrumentFile.instrumentDataAndHeader.instrHeader.id_data[1] = 'A';
	instrumentFile.instrumentDataAndHeader.instrHeader.id_data[2] = 'T';
	instrumentFile.instrumentDataAndHeader.instrHeader.id_data[3] = 'A';
	instrumentFile.instrumentDataAndHeader.instrHeader.size = sizeof(*instr);

	instrumentFile.crc = crcCalc.crc32((uint8_t *)&instrumentFile.instrumentDataAndHeader,sizeof(instrumentFile.instrumentDataAndHeader));

	file=SD.open(name,FILE_WRITE);
	file.write((uint8_t *)&instrumentFile,sizeof(instrumentFile));
	file.close();


}

void cMtProjectEditor::writePatternFile(char * name)
{
	if(SD.exists(name)) return; //todo: do ustalenia czy nadpisujemy czy nie

	FsFile file;
	FastCRC32 crcCalc;
	strPatternFile patternFile;
	Sequencer::strPattern * patt;
	patt=(Sequencer::strPattern *)sequencer.getPatternToSaveToFile();

	patternFile.patternDataAndHeader.pattern = * patt;


	patternFile.patternDataAndHeader.patternHeader.id_file[0]='I';
	patternFile.patternDataAndHeader.patternHeader.id_file[1]='D';
	patternFile.patternDataAndHeader.patternHeader.type = fileTypePattern;
	patternFile.patternDataAndHeader.patternHeader.version[0] = '0';
	patternFile.patternDataAndHeader.patternHeader.version[1] = '.';
	patternFile.patternDataAndHeader.patternHeader.version[2] = '0';
	patternFile.patternDataAndHeader.patternHeader.version[3] = '1';
	patternFile.patternDataAndHeader.patternHeader.id_data[0] = 'D';
	patternFile.patternDataAndHeader.patternHeader.id_data[1] = 'A';
	patternFile.patternDataAndHeader.patternHeader.id_data[2] = 'T';
	patternFile.patternDataAndHeader.patternHeader.id_data[3] = 'A';
	patternFile.patternDataAndHeader.patternHeader.size = sizeof(*patt);

	patternFile.crc = crcCalc.crc32((uint8_t *)&patternFile.patternDataAndHeader,sizeof(patternFile.patternDataAndHeader));

	file=SD.open(name,FILE_WRITE);
	file.write((uint8_t *)&patternFile,sizeof(patternFile));
	file.close();

	sequencer.saveToFileDone();
}


void cMtProjectEditor::writeProjectFile(char * name, strMtProjectRemote * proj)
{
	if(SD.exists(name)) SD.remove(name);

	FsFile file;
	FastCRC32 crcCalc;
	strProjectFile projectFile;

	projectFile.projectDataAndHeader.project = * proj;


	projectFile.projectDataAndHeader.projectHeader.id_file[0]='I';
	projectFile.projectDataAndHeader.projectHeader.id_file[1]='D';
	projectFile.projectDataAndHeader.projectHeader.type = fileTypeProject;
	projectFile.projectDataAndHeader.projectHeader.version[0] = '0';
	projectFile.projectDataAndHeader.projectHeader.version[1] = '.';
	projectFile.projectDataAndHeader.projectHeader.version[2] = '0';
	projectFile.projectDataAndHeader.projectHeader.version[3] = '1';
	projectFile.projectDataAndHeader.projectHeader.id_data[0] = 'D';
	projectFile.projectDataAndHeader.projectHeader.id_data[1] = 'A';
	projectFile.projectDataAndHeader.projectHeader.id_data[2] = 'T';
	projectFile.projectDataAndHeader.projectHeader.id_data[3] = 'A';
	projectFile.projectDataAndHeader.projectHeader.size = sizeof(*proj);

	projectFile.crc = crcCalc.crc32((uint8_t *)&projectFile.projectDataAndHeader,sizeof(projectFile.projectDataAndHeader));

	file=SD.open(name,FILE_WRITE);
	file.write((uint8_t *)&projectFile,sizeof(projectFile));
	file.close();


}

uint8_t cMtProjectEditor::readInstrumentFile(char * name, strInstrument * instr)
{
	if(!SD.exists(name)) return 0;
	FsFile file;
	FastCRC32 crcCalc;
	uint32_t checkCRC=0;

	strInstrumentFile instrumentFile;

	file=SD.open(name);
	file.read((uint8_t*)&instrumentFile, sizeof(instrumentFile));
	file.close();

	if(instrumentFile.instrumentDataAndHeader.instrHeader.type != fileTypeInstrument) return 0;

	checkCRC=crcCalc.crc32((uint8_t *)&instrumentFile.instrumentDataAndHeader,sizeof(instrumentFile.instrumentDataAndHeader));
	if(checkCRC == instrumentFile.crc)
	{
		*instr=instrumentFile.instrumentDataAndHeader.instrument;
		return 1;
	}
	else return 0;
}

uint8_t cMtProjectEditor::readPatternFile(char * name)
{
	if(!SD.exists(name))
	{
		return 0;
		sequencer.loadFromFileERROR();
	}
	FsFile file;
	FastCRC32 crcCalc;
	uint32_t checkCRC=0;


	strPatternFile patternFile;
	Sequencer::strPattern * patt;
	patt=(Sequencer::strPattern *)sequencer.getPatternToLoadFromFile();
	file=SD.open(name);
	file.read((uint8_t*)&patternFile, sizeof(patternFile));
	file.close();

	if(patternFile.patternDataAndHeader.patternHeader.type != fileTypePattern)
	{
		return 0;
		sequencer.loadFromFileERROR();
	}

	checkCRC=crcCalc.crc32((uint8_t *)&patternFile.patternDataAndHeader,sizeof(patternFile.patternDataAndHeader));
	if(checkCRC == patternFile.crc)
	{
		*patt=patternFile.patternDataAndHeader.pattern;
		sequencer.loadFromFileOK();
		return 1;
	}
	else
	{
		return 0;
		sequencer.loadFromFileERROR();
	}


}

uint8_t cMtProjectEditor::readProjectFile(char * name, strMtProjectRemote * proj)
{
	if(!SD.exists(name)) return 0;
	FsFile file;
	FastCRC32 crcCalc;
	uint32_t checkCRC=0;

	strProjectFile projectFile;

	file=SD.open(name);
	file.read((uint8_t*)&projectFile, sizeof(projectFile));
	file.close();

	if(projectFile.projectDataAndHeader.projectHeader.type != fileTypeProject) return 0;

	checkCRC=crcCalc.crc32((uint8_t *)&projectFile.projectDataAndHeader,sizeof(projectFile.projectDataAndHeader));
	if(checkCRC == projectFile.crc)
	{
		*proj=projectFile.projectDataAndHeader.project;
		return 1;
	}
	else return 0;
}

uint8_t cMtProjectEditor::openProject(char * name)
{
	uint8_t status;

	SD.open(currentDirectoryOpen);
	status = readProjectFile(name, &mtProject.mtProjectRemote);
	if(!status) return status;

	SD.open("/instruments");
	for(int i=0; i < INSTRUMENTS_COUNT; i++)
	{
		 if(mtProject.mtProjectRemote.instrumentFile[i].index != - 1)
		 {
			 status=readInstrumentFile(mtProject.mtProjectRemote.instrumentFile[i].name,&mtProject.instrument[mtProject.mtProjectRemote.instrumentFile[i].index]);
			 if(!status) return status;
		 }
	}
	SD.open("/patterns");
	for(int i=0; i< PATTERNS_COUNT;i++)
	{
		if(mtProject.mtProjectRemote.patternFile[i].index != - 1)
		{
			status=readPatternFile(mtProject.mtProjectRemote.patternFile[i].name);
			if(!status) return status;
			else break;
		}
	}

	for(int i=0; i< SAMPLES_COUNT ; i++)
	{
		if(mtProject.mtProjectRemote.sampleFile[i].index != - 1)
		{
			memcpy(mtProject.sampleBank.sample[mtProject.mtProjectRemote.sampleFile[i].index].file_name,mtProject.mtProjectRemote.sampleFile[i].name, SAMPLE_NAME_SIZE);
			mtProject.sampleBank.sample[mtProject.mtProjectRemote.sampleFile[i].index].type=mtProject.mtProjectRemote.sampleFile[i].type;
			mtProject.sampleBank.sample[mtProject.mtProjectRemote.sampleFile[i].index].wavetable_window_size=mtProject.mtProjectRemote.sampleFile[i].wavetable_window_size;
		}
	}

	return status;
}

void cMtProjectEditor::createNewProject(char* patch, char * name)
{
	char patchFolder[50];

	for(uint8_t i=0;i < INSTRUMENTS_COUNT;i++)
	{
		mtProject.mtProjectRemote.instrumentFile[i].index= -1;
	}
	for(uint8_t i=0;i < SAMPLES_COUNT;i++)
	{
		mtProject.mtProjectRemote.sampleFile[i].index= -1;
	}
	for(uint8_t i=0;i < PATTERNS_COUNT;i++)
	{
		mtProject.mtProjectRemote.patternFile[i].index= -1;
	}

	if(patch == NULL)
	{
		patchFolder[0]='/';
		patchFolder[1]=0;
		if(!SD.exists(name)) SD.mkdir(name);

		strcat(patchFolder,name);
		strcat(patchFolder,"/");
		strcat(patchFolder,"instruments");
		SD.mkdir(patchFolder);

		memset(patchFolder,0,50);
		patchFolder[0]='/';

		strcat(patchFolder,name);
		strcat(patchFolder,"/");
		strcat(patchFolder,"patterns");
		SD.mkdir(patchFolder);

		memset(patchFolder,0,50);
		patchFolder[0]='/';

		strcat(patchFolder,name);
		strcat(patchFolder,"/");
		strcat(patchFolder,"samples");
		SD.mkdir(patchFolder);

		memset(patchFolder,0,50);
		patchFolder[0]='/';

		strcat(patchFolder,name);
		strcat(patchFolder,"/");
		strcat(patchFolder,name);
		strcat(patchFolder,".bin");

		writeProjectFile(patchFolder, &mtProject.mtProjectRemote);

	}
	else
	{
		strcpy(patchFolder,patch);
		strcat(patchFolder,"/");
		strcat(patchFolder,name);
		if(!SD.exists(patchFolder)) SD.mkdir(patchFolder);

		memset(patchFolder,0,50);

		strcpy(patchFolder,patch);
		strcat(patchFolder,"/");
		strcat(patchFolder,name);
		strcat(patchFolder,"/");
		strcat(patchFolder,"instruments");
		if(!SD.exists(patchFolder)) SD.mkdir(patchFolder);

		memset(patchFolder,0,50);

		strcpy(patchFolder,patch);
		strcat(patchFolder,"/");
		strcat(patchFolder,name);
		strcat(patchFolder,"/");
		strcat(patchFolder,"patterns");
		if(!SD.exists(patchFolder)) SD.mkdir(patchFolder);

		memset(patchFolder,0,50);

		strcpy(patchFolder,patch);
		strcat(patchFolder,"/");
		strcat(patchFolder,name);
		strcat(patchFolder,"/");
		strcat(patchFolder,"samples");
		if(!SD.exists(patchFolder)) SD.mkdir(patchFolder);

		memset(patchFolder,0,50);

		strcpy(patchFolder,patch);
		strcat(patchFolder,"/");
		strcat(patchFolder,name);
		strcat(patchFolder,"/");
		strcat(patchFolder,name);
		strcat(patchFolder,".bin");

		writeProjectFile(patchFolder, &mtProject.mtProjectRemote);

	}

}

void cMtProjectEditor::importSampleToProject(char* projectPatch, char* filePatch, char* name, int8_t index, uint8_t type)
{
	char currentPatch[50];
	FsFile file;
	FsFile copy;
	uint8_t currentBuffor[1024];
	uint8_t cnt=0;
	while(mtProject.mtProjectRemote.sampleFile[cnt].index != -1)
	{
		cnt++;
	}

	mtProject.mtProjectRemote.sampleFile[cnt].index=index;
	mtProject.mtProjectRemote.sampleFile[cnt].type=type;
	strcpy(mtProject.mtProjectRemote.sampleFile[cnt].name,name);

	mtProject.sampleBank.sample[index].type=type;
	strcpy(mtProject.sampleBank.sample[index].file_name,name);

	if(filePatch!= NULL)
	{
		strcpy(currentPatch,filePatch);
		strcat(currentPatch,"/");
		strcat(currentPatch,name);
	}
	else
	{
		strcpy(currentPatch,name);
	}
	file = SD.open(currentPatch);

	memset(currentPatch,0,50);

	strcpy(currentPatch,projectPatch);
	strcat(currentPatch,"/");
	strcat(currentPatch,"samples");
	strcat(currentPatch,"/");
	strcat(currentPatch,name);
	copy= SD.open(currentPatch,FILE_WRITE);

	while(file.available())
	{
		file.read(currentBuffor,1024);
		copy.write(currentBuffor,1024);
	}
	file.close();
	copy.close();
}

/*void cMtProjectEditor::saveProject(char * name)
{
	if(!SD.exists(currentDirectory))
	{
		SD.mkdir(currentDirectory);
		SD.open(currentDirectoryOpen);
		SD.mkdir("instruments");
		SD.mkdir("patterns");
	}

}*/

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------



int32_t loadSample(const char *filename, int16_t * buf)
{
	strWavFileHeader sampleHead;
	uint16_t bufferLength=0;
	uint32_t accBufferLength=0;
//	uint32_t * bufStart;
	int16_t buf16[256];
	FsFile wavfile;

	//__disable_irq();

	//bufStart = (uint32_t*)buf;
	//buf+=2;

	wavfile = SD.open(filename);
	wavfile.read(&sampleHead, 44);

	if(sampleHead.format != 1163280727 || sampleHead.AudioFormat != 1  || sampleHead.bitsPerSample != 16  || sampleHead.sampleRate != 44100 )
	{
		wavfile.close();
//		__enable_irq();
		if(hardwareTest)
		{
			Serial.println("Bad WAV file or External RAM(if SD Card init is Correct");
			mtPrint("Bad WAV file or External RAM(if SD Card init is Correct");
		}
		return -1;
	}
	else
	{
		if(hardwareTest)
		{
			Serial.println("load WAV header to SDRAM succesfull");
			mtPrint("load WAV header to SDRAM succesfull");
		}
	}
/*
	if(sampleHead.numChannels == 1) sampleLength = sampleHead.subchunk2Size;
	else if(sampleHead.numChannels == 2) sampleLength = sampleHead.subchunk2Size/2;
	else
	{
		wavfile.close();
//		__enable_irq();
		return -2;
	}
*/
	if(sampleHead.numChannels == 1)
	{
		while ( wavfile.available() )
		{
			bufferLength = wavfile.read(buf16, 512);

			accBufferLength += bufferLength;

			for(int i=0; i< 256; i++)
			{
				if(bufferLength <= i ) *buf=0;
				else *buf=buf16[i];
				buf++;
			}
		}
	}
	else if (sampleHead.numChannels == 2)
	{
		while (wavfile.available() )
		{

			bufferLength = wavfile.read(buf16, 512);
			//Serial.println(bufferLength);
			accBufferLength += bufferLength;
			for(int i=0; i< 256; i+=2)
			{
				if(bufferLength <= i ) *buf=0;
				else *buf=buf16[i];
				buf++;
			}

		}
	}

	wavfile.close();

//	*bufStart = (accBufferLength/4);
	if(sampleHead.numChannels == 1)
	{
		accBufferLength = accBufferLength/2;
	}
	else if(sampleHead.numChannels == 2)
	{
		accBufferLength = accBufferLength/4;
	}


	return accBufferLength;
}

int32_t loadWavetableStandard(const char *filename, int16_t * buf)
{
	strWavFileHeader sampleHead;
	uint16_t bufferLength=0;
	int16_t buf16[256];
	int16_t buf16_1024[1024];
	int16_t windowDivider=0;
	int16_t currentWave[1024];
	int16_t nextWave[1024];

	FsFile wavfile;

	uint16_t numberOfWindows=0;
	uint16_t missingWindows=0;
	float windowsControl=0;
	float windowsCounter=0;
	uint16_t buforCounter=0;

	wavfile = SD.open(filename);
	wavfile.read(&sampleHead, 44);



	if(sampleHead.format != 1163280727 || sampleHead.AudioFormat != 1  || sampleHead.bitsPerSample != 16  || sampleHead.sampleRate != 44100 )
	{
		wavfile.close();
		if(hardwareTest)
		{
			Serial.println("Bad WAV file or External RAM(if SD Card init is Correct");
			mtPrint("Bad WAV file or External RAM(if SD Card init is Correct");
		}
		return -1;
	}
	else
	{
		if(hardwareTest)
		{
			Serial.println("load WAV header to SDRAM succesfull");
			mtPrint("load WAV header to SDRAM succesfull");
		}
	}

	numberOfWindows= (sampleHead.subchunk2Size/2)/STANDARD_WAVETABLE_WINDOW_LEN;
	if(numberOfWindows == STANDARD_WAVETABLE_WINDOWS_NUMBER)
	{
		if(sampleHead.numChannels == 1)
		{
			while ( wavfile.available() )
			{
				bufferLength = wavfile.read(buf16, 512);
				for(int i=0; i< 256; i++)
				{
					if(bufferLength <= i ) *buf=0;
					else *buf=buf16[i];
					buf++;
				}
			}
		}
		else if (sampleHead.numChannels == 2)
		{
			while (wavfile.available() )
			{
				bufferLength = wavfile.read(buf16, 512);
				for(int i=0; i< 256; i+=2)
				{
					if(bufferLength <= i ) *buf=0;
					else *buf=buf16[i];
					buf++;
				}
			}
		}

		wavfile.close();

	}

	else if(numberOfWindows > STANDARD_WAVETABLE_WINDOWS_NUMBER)
	{
		;
	}
	else if(numberOfWindows < STANDARD_WAVETABLE_WINDOWS_NUMBER)
	{
		missingWindows=STANDARD_WAVETABLE_WINDOWS_NUMBER-numberOfWindows;
		windowsControl=(float)STANDARD_WAVETABLE_WINDOWS_NUMBER/(missingWindows+1);

		if(sampleHead.numChannels == 1)
		{
			while ( wavfile.available() )
			{
				if((buforCounter < (uint16_t) windowsCounter) || ((!buforCounter)&&(windowsCounter == 0.0f)))
				{
					bufferLength=wavfile.read(buf16_1024, 2048);
					for(int i=0; i< 1024; i++)
					{
						if(bufferLength <= i ) *buf=0;
						else *buf=buf16_1024[i];
						buf++;
						currentWave[i]=buf16_1024[i];
					}
					buforCounter++;
				}
				else
				{
					bufferLength=wavfile.read(nextWave, 2048);
					windowDivider=getMiddleWindow(buforCounter,windowsCounter,windowsControl);

					for(uint8_t i=0;i<windowDivider;i++)
					{
						for(uint16_t j=0; j< 1024; j++)
						{
							*buf=currentWave[j]+(((nextWave[j]-currentWave[j])/(windowDivider+1))*i);
							buf++;
						}
					}
					for(int i=0; i< 1024; i++)
					{
						if(bufferLength <= i ) *buf=0;
						else *buf=nextWave[i];
						buf++;

						currentWave[i]=nextWave[i];
					}
					buforCounter++;
					buforCounter+=(windowDivider);
					windowsCounter+=(windowDivider*windowsControl);
				}
			}
		}
		else if (sampleHead.numChannels == 2)
		{
			// nie trzeba ale zostawiam jakieś bałagany
			/*while ( wavfile.available() )
			{
				if((buforCounter < (uint16_t) windowsCounter) || ((!buforCounter)&&(windowsCounter == 0.0f)))
				{
					bufferLength=wavfile.read(buf16_2048, 4096);
					for(int i=0; i< 2048; i+=2)
					{
						if(bufferLength <= i ) *buf=0;
						else *buf=buf16_2048[i];
						buf++;
						currentWave_2048[i]=buf16_2048[i];
					}
					buforCounter++;
				}
				else
				{
					bufferLength=wavfile.read(buf16_2048, 4096);
					for(int i=0; i< 2048; i+=2)
					{
						if(bufferLength <= i ) nextWave_2048[i]=0;
						else nextWave_2048[i]=buf16_2048[i];
					}
					windowDivider=getMiddleWindow(buforCounter,windowsCounter,windowsControl);

					for(uint8_t i=0;i<windowDivider;i++)
					{
						for(uint16_t j=0; j< 1024; j++)
						{
							*buf=currentWave_2048[j]+(((nextWave_2048[j]-currentWave_2048[j])/(windowDivider+1))*i);
							buf++;
						}
					}
					for(int i=0; i< 1024; i++)
					{
						if(bufferLength <= i ) *buf=0;
						else *buf=nextWave_2048[i];
						buf++;

						currentWave_2048[i]=nextWave_2048[i];
					}
					buforCounter++;
					buforCounter+=(windowDivider);
					windowsCounter+=(windowDivider*windowsControl);
				}
			}*/
		}

		wavfile.close();
	}

	return STANDARD_WAVETABLE_WINDOW_LEN*STANDARD_WAVETABLE_WINDOWS_NUMBER;
}


int32_t loadWavetableSerum(const char *filename, int16_t * buf)
{
	//strSerumWavetableFileHeader sampleHead;
	strWavFileHeader sampleHead;
	uint16_t bufferLength=0;

	float bufFloat[2048];
	float currentWave[2048];
	float nextWave[2048];

	int16_t windowDivider=0;
	uint16_t numberOfWindows=0;
	uint16_t missingWindows=0;
	float windowsControl=0;
	float windowsCounter=0;
	uint16_t buforCounter=0;


	int16_t currentInt=0;
	int16_t nextInt=0;
	FsFile wavfile;



	wavfile = SD.open(filename);
	readHeader(&sampleHead,&wavfile);



	if(sampleHead.format != 1163280727 || sampleHead.AudioFormat != 3  || sampleHead.bitsPerSample != 32  || sampleHead.sampleRate != 44100)
	{
		wavfile.close();
		if(hardwareTest)
		{
			Serial.println("Bad WAV file or External RAM(if SD Card init is Correct");
			mtPrint("Bad WAV file or External RAM(if SD Card init is Correct");
		}
		return -1;
	}
	else
	{
		if(hardwareTest)
		{
			Serial.println("load WAV header to SDRAM succesfull");
			mtPrint("load WAV header to SDRAM succesfull");
		}
	}

	numberOfWindows= (sampleHead.subchunk2Size/4)/SERUM_WAVETABLE_WINDOW_LEN;
	if(numberOfWindows == STANDARD_WAVETABLE_WINDOWS_NUMBER)
	{

		while ( wavfile.available() )
		{
			bufferLength = wavfile.read(bufFloat, 8192);

			for(int i=0; i< 2048; i++)
			{
				if(bufferLength <= i ) *buf=0;
				else *buf=fmap(bufFloat[i],MIN_WAVE_FLOAT,MAX_WAVE_FLOAT,MIN_SIGNED_16BIT,MAX_SIGNED_16BIT);
				buf++;
			}
		}

		wavfile.close();
	}

	else if(numberOfWindows > STANDARD_WAVETABLE_WINDOWS_NUMBER)
	{
		;
	}
	else if(numberOfWindows < STANDARD_WAVETABLE_WINDOWS_NUMBER)
	{
		missingWindows=STANDARD_WAVETABLE_WINDOWS_NUMBER-numberOfWindows;
		windowsControl=(float)STANDARD_WAVETABLE_WINDOWS_NUMBER/(missingWindows+1);


		while ( wavfile.available() )
		{
			if((buforCounter < (uint16_t) windowsCounter) || ((!buforCounter)&&(windowsCounter == 0.0f)))
			{
				bufferLength=wavfile.read(bufFloat, 8192);
				for(int i=0; i< 2048; i++)
				{
					if(bufferLength <= i ) *buf=0;
					else *buf=fmap(bufFloat[i],MIN_WAVE_FLOAT,MAX_WAVE_FLOAT,(const float)MIN_SIGNED_16BIT,(const float)MAX_SIGNED_16BIT);
					buf++;
					currentWave[i]=bufFloat[i];
				}
				buforCounter++;
			}
			else
			{
				bufferLength=wavfile.read(nextWave, 8192);
				windowDivider=getMiddleWindow(buforCounter,windowsCounter,windowsControl);

				for(uint8_t i=0;i<windowDivider;i++)
				{
					for(uint16_t j=0; j< 2048; j++)
					{
						currentInt=fmap(currentWave[j],MIN_WAVE_FLOAT,MAX_WAVE_FLOAT,(const float)MIN_SIGNED_16BIT,(const float)MAX_SIGNED_16BIT);
						nextInt=fmap(nextWave[j],MIN_WAVE_FLOAT,MAX_WAVE_FLOAT,(const float)MIN_SIGNED_16BIT,(const float)MAX_SIGNED_16BIT);
						*buf=currentInt+(((nextInt-currentInt)/(windowDivider+1))*i);
						buf++;
					}
				}
				for(int i=0; i< 2048; i++)
				{
					if(bufferLength <= i ) *buf=0;
					else *buf=fmap(nextWave[i],MIN_WAVE_FLOAT,MAX_WAVE_FLOAT,(const float)MIN_SIGNED_16BIT,(const float)MAX_SIGNED_16BIT);;
					buf++;

					currentWave[i]=nextWave[i];
				}
				buforCounter++;
				buforCounter+=(windowDivider);
				windowsCounter+=(windowDivider*windowsControl);
			}
		}
		wavfile.close();


	}

	return SERUM_WAVETABLE_WINDOW_LEN*STANDARD_WAVETABLE_WINDOWS_NUMBER;
}


uint8_t getMiddleWindow(uint16_t cnt, float windowsCnt, float windowsControl)
{
	uint8_t localCounter=0;
	while(cnt >= (uint16_t)windowsCnt)
	{
		localCounter++;
		cnt++;
		windowsCnt+=windowsControl;
	}
	return localCounter;

}

int16_t fmap(float x, float in_min, float in_max, float out_min, float out_max)
{
	  return (int16_t)( (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
}

void readHeader(strWavFileHeader* header, FsFile * wavfile)
{
	uint8_t tab[200];
	uint8_t headerSize=0;
	wavfile->read(tab,200);
	header->chunkId = *( (uint32_t *)(&tab[0]) );
	header->chunkSize = *( (uint32_t *)(&tab[4]) );
	header->format = *( (uint32_t *)(&tab[8]) );
	for(int i=12;i<200;i++)
	{
		if((tab[i] == 'f') && (tab[i+1] == 'm') && (tab[i+2] == 't') && (tab[i+3] == ' '))
		{
			header->subchunk1Id = *((uint32_t *)(&tab[i]));
			header->subchunk1Size = *((uint32_t *)(&tab[i+4]));
			header->AudioFormat = *((uint16_t *)(&tab[i+8]));
			header->numChannels = *((uint16_t *)(&tab[i+10]));
			header->sampleRate = *((uint32_t *)(&tab[i+12]));
			header->byteRate = *((uint32_t *)(&tab[i+16]));
			header->blockAlign = *((uint16_t *)(&tab[i+20]));
			header->bitsPerSample = *((uint16_t *)(&tab[i+22]));
		}

		if((tab[i] == 'd') && (tab[i+1] == 'a') && (tab[i+2] == 't') && (tab[i+3] == 'a'))
		{
			header->subchunk2Id = *((uint32_t *)(&tab[i]));
			header->subchunk2Size = *((uint32_t *)(&tab[i+4]));
			headerSize=i+8;
			wavfile->seek(headerSize);
			break;
		}

	}
}

int32_t loadWavetable(const char *filename, int16_t * buf ,uint16_t * windowSize)
{
	strWavFileHeader sampleHead;
	FsFile wavfile;
	int32_t size=-1;

	wavfile = SD.open(filename);
	readHeader(&sampleHead,&wavfile);
	wavfile.close();
	if(sampleHead.AudioFormat == 1)
	{
		size=loadWavetableStandard(filename,buf);
		*windowSize = STANDARD_WAVETABLE_WINDOW_LEN;
	}
	else if(sampleHead.AudioFormat == 3)
	{
		size=loadWavetableSerum(filename,buf);
		*windowSize = SERUM_WAVETABLE_WINDOW_LEN;
	}
	return size;

}
/*int32_t loadFullWavetableSerum(const char *baseName, int16_t * buf)
{
	strWavFileHeader sampleHead;
	uint8_t tabSize=0;
	uint16_t bufferLength=0;
	float currentWave[2048];
	FsFile wavfile;
	while( *(baseName+tabSize) != 0 )
	{
		tabSize++;
	}
	tabSize+=9;


	char name[tabSize];

	for(uint8_t i=0;i< tabSize; i++)
	{
		if(i< (tabSize-9)) name[i]=baseName[i];
		else if(i== (tabSize -1 ) ) name[i] = 0;
		else if(i== (tabSize -2 ) ) name[i] = 'V';
		else if(i== (tabSize -3 ) ) name[i] = 'A';
		else if(i== (tabSize -4 ) ) name[i] = 'W';
		else if(i== (tabSize -5 ) ) name[i] = '.';
		else if(i== (tabSize -6 ) ) name[i] = '1';
		else if(i== (tabSize -9 ) ) name[i] = '_';
		else name[i] ='0';
	}

	for(uint16_t a=0; a < STANDARD_WAVETABLE_WINDOWS_NUMBER;a++)
	{
		wavfile = SD.open(name);
		wavfile.read(&sampleHead,44);

		bufferLength = wavfile.read(currentWave, 8192);

		for(int j=0; j< SERUM_WAVETABLE_WINDOW_LEN; j++)
		{
			if(bufferLength <= j ) *buf=0;
			else *buf=fmap(currentWave[j],MIN_WAVE_FLOAT,MAX_WAVE_FLOAT,MIN_SIGNED_16BIT,MAX_SIGNED_16BIT);
			buf++;
		}
		wavfile.close();

		if(name[tabSize - 6 ] < '9') name[tabSize - 6 ]++;
		else if(name[tabSize - 6 ] == '9')
		{
			name[tabSize - 6 ] = '0';
			if( name[tabSize - 7 ] < '9') name[tabSize - 7 ]++;
			else if(name[tabSize - 7 ] == '9')
			{
				name[tabSize - 7 ]= '0';
				name[tabSize - 8]++;
			}
		}
	}

	return STANDARD_WAVETABLE_WINDOWS_NUMBER* SERUM_WAVETABLE_WINDOW_LEN;

}*/

void loadFullWavetableSerum(const char *baseName)
{
	strWavFileHeader sampleHead;
	uint8_t tabSize=0;
	uint8_t newTabSize=0;
	float currentWave[2048];
	FsFile wavfile;
	FsFile result;
	while( *(baseName+tabSize) != 0 )
	{
		tabSize++;
	}
	newTabSize=tabSize+5;
	tabSize+=9;

	char newName[newTabSize];
	char name[tabSize];

	for(uint8_t i=0;i< (newTabSize - 5); i++)
	{
		newName[i]=baseName[i];
	}
	newName[newTabSize -1] = 0;
	newName[newTabSize -2] = 'v';
	newName[newTabSize -3] = 'a';
	newName[newTabSize -4] = 'w';
	newName[newTabSize -5] = '.';

	if(SD.exists(newName)) return;
	for(uint8_t i=0;i< tabSize; i++)
	{
		if(i< (tabSize-9)) name[i]=baseName[i];
		else if(i== (tabSize -1 ) ) name[i] = 0;
		else if(i== (tabSize -2 ) ) name[i] = 'V';
		else if(i== (tabSize -3 ) ) name[i] = 'A';
		else if(i== (tabSize -4 ) ) name[i] = 'W';
		else if(i== (tabSize -5 ) ) name[i] = '.';
		else if(i== (tabSize -6 ) ) name[i] = '1';
		else if(i== (tabSize -9 ) ) name[i] = '_';
		else name[i] ='0';
	}


	result = SD.open(newName,FILE_WRITE);
	for(uint16_t a=0; a < STANDARD_WAVETABLE_WINDOWS_NUMBER;a++)
	{

		if(!a)
		{
			wavfile = SD.open(name);
			wavfile.read(&sampleHead,44);

			sampleHead.subchunk2Size=STANDARD_WAVETABLE_WINDOWS_NUMBER* SERUM_WAVETABLE_WINDOW_LEN*4;
			result.write(&sampleHead,44);
		}
		else
		{
			wavfile = SD.open(name);
			wavfile.read(&sampleHead,44);
		}


		wavfile.read(currentWave, 8192);
		result.write(currentWave, 8192);


		wavfile.close();

		if(name[tabSize - 6 ] < '9') name[tabSize - 6 ]++;
		else if(name[tabSize - 6 ] == '9')
		{
			name[tabSize - 6 ] = '0';
			if( name[tabSize - 7 ] < '9') name[tabSize - 7 ]++;
			else if(name[tabSize - 7 ] == '9')
			{
				name[tabSize - 7 ]= '0';
				name[tabSize - 8]++;
			}
		}
	}
	result.close();


}
