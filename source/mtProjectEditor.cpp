


//#include "mtDisplay.h"
#include "mtProjectEditor.h"

#include "sdram.h"
#include "Arduino.h"

cMtProjectEditor mtProjectEditor;


strMtProject mtProject;


__NOINIT(EXTERNAL_RAM) uint8_t sdram_writeBuffer[1024*1024];



extern uint32_t loadSdWavToMemory(const char *filename, uint16_t * buf);


//-------------------------------------------------------------------------------
uint8_t cMtProjectEditor::readProjectConfig()
{
	// parametry sampli ==============================================
	mtProject.sampleBank.samples_count = 8;

	for(uint8_t i = 0; i < mtProject.sampleBank.samples_count; i++)
	{
		mtProject.sampleBank.sample[i].index = i+1;
		//mtProject.sampleBank.sample[i].address = i+1;
	}

	mtProject.sampleBank.sample[0].file_name = (char*)"1.wav";
	mtProject.sampleBank.sample[1].file_name = (char*)"2.wav";
	mtProject.sampleBank.sample[2].file_name = (char*)"3.wav";
	mtProject.sampleBank.sample[3].file_name = (char*)"4.wav";
	mtProject.sampleBank.sample[4].file_name = (char*)"5.wav";
	mtProject.sampleBank.sample[5].file_name = (char*)"6.wav";
	mtProject.sampleBank.sample[6].file_name = (char*)"7.wav";
	mtProject.sampleBank.sample[7].file_name = (char*)"8.wav";

	// parametry instrumentow ========================================
	mtProject.instruments_count = 8;




	return 0;
}

//-------------------------------------------------------------------------------
uint8_t cMtProjectEditor::loadSamples()
{
	//zaladowanie banku sampli

	int32_t size;

	for(uint8_t i = 0; i < mtProject.sampleBank.samples_count; i++)
	{
		size = loadSdWavToMemory(mtProject.sampleBank.sample[i].file_name, mtProject.sampleBank.sample[i].address);

		if(size > 0)
		{
			mtProject.sampleBank.used_memory += size;
			mtProject.sampleBank.sample[i].loaded = 1;
		}
		else return 2; // blad ladowania wave

		if(mtProject.sampleBank.used_memory > mtProject.sampleBank.max_memory) return 1; // out of mem
	}

	 return 0;
}

//-------------------------------------------------------------------------------
uint8_t cMtProjectEditor::loadInstruments()
{




	return 0;
}

//-------------------------------------------------------------------------------
uint8_t cMtProjectEditor::loadProject()
{

	Serial.println(sdram_writeBuffer[0]);


	sdram_writeBuffer[0] = 1;

	Serial.println(sdram_writeBuffer[0]);

	readProjectConfig();

	if(loadSamples()) return 1;
	if(loadInstruments()) return 1;


	return 0;
}


//-------------------------------------------------------------------------------
uint8_t cMtProjectEditor::isProjectLoaded()
{


	return 0;
}
