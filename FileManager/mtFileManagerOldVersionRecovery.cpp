#include "mtFileManagerOldVersionRecovery.h"
#include "fileManagerDefs.h"

static void recoveryProjectVersion1();
static void recoveryProjectVersion2();
static void recoveryProjectVersion3();
static void recoveryProjectVersion4();
static void recoveryInstrumentVersion1();
static void recoveryInstrumentVersion2();
static void recoveryInstrumentVersion3();

mtVersionRecovery versionRecovery;

//***************************************************************PROJECT
//przepisanie funkcji do wskaznika
mtVersionRecovery::mtVersionRecovery()
{
	//PROJECT
	recoveryProjectFunction[0] = recoveryProjectVersion1;
	recoveryProjectFunction[1] = recoveryProjectVersion2;
	recoveryProjectFunction[2] = recoveryProjectVersion3;
	recoveryProjectFunction[3] = recoveryProjectVersion4;
	//INSTRUMENT
	recoveryInstrumentFunction[0] = recoveryInstrumentVersion1;
	recoveryInstrumentFunction[1] = recoveryInstrumentVersion2;
	recoveryInstrumentFunction[2] = recoveryInstrumentVersion3;
}
//zwraca wskaznik do aktywnego odczytanego projektu
strMtProjectRemote * mtVersionRecovery::getReadedProject()
{
	return receivedProject;
}

// wykonuje serie translacji od odczytanej wersji do najnowszej
void mtVersionRecovery::translateProject(uint8_t firstVersion, uint8_t lastVersion, strMtProjectRemote* currentProject)
{
	receivedProject = currentProject; // odebrany projekt

	for(uint8_t i = firstVersion; i < lastVersion; i++ ) // wykonanie odpowiedniej ilosci etapow translacji
	{
		recoveryProjectFunction[i-1](); //wywolanie wskaznika funkcyjnego - obslugujacego dany etap
	}
}


// odzyskiwanie danych z projektu Versja 1
static void recoveryProjectVersion1()
{
	strMtProjectRemote * receivedProject = versionRecovery.getReadedProject(); //wskaznik do projektu odczytanego z pliku
	strMtProjectRemote tmp;

	memcpy(&tmp,receivedProject, sizeof(tmp));



	uint32_t bytesShift = (uint32_t)&receivedProject->values.limiterAttack - (uint32_t)&receivedProject->values;
	uint32_t bytesToCopy = sizeof(strMtValues) - bytesShift - 1;

	memcpy((uint8_t*)&receivedProject->values.limiterAttack, (uint8_t *)( ((uint32_t)&tmp.values.limiterAttack) - 4), bytesToCopy);
	receivedProject->values.delayFeedback = 50;
	receivedProject->values.delayTime = 500;
	receivedProject->values.delayParams = 0;

	memcpy(receivedProject->projectName,(uint8_t *)(((uint32_t)&tmp.projectName) - 4) , PROJECT_NAME_SIZE );
	// shift 4 zostal dobrany metoda prob i bledow, teoretycznie dodano 1 byte ale prawdopdobnie kompilator to ulozyl w jakis inny sposob

}
// odzyskiwanie danych z projektu Versja 2
static void recoveryProjectVersion2()
{
	strMtProjectRemote * receivedProject = versionRecovery.getReadedProject(); //wskaznik do projektu odczytanego z pliku
	strMtProjectRemote tmp;

	memcpy(&tmp,receivedProject, sizeof(tmp));

	receivedProject->values.reverb.size = DEFAULT_REVERB_SIZE;
	receivedProject->values.reverb.damp = DEFAULT_REVERB_DAMP;
	receivedProject->values.reverb.predelay = DEFAULT_REVERB_PREDELAY;
	receivedProject->values.reverb.diffusion = DEFAULT_REVERB_DIFFUSION;

	strcpy(receivedProject->projectName,tmp.projectName - sizeof(strMtValues::strReverbParams));

}

// odzyskiwanie danych z projektu Versja 3
static void recoveryProjectVersion3()
{
	strMtProjectRemote * receivedProject = versionRecovery.getReadedProject(); //wskaznik do projektu odczytanego z pliku
	strMtProjectRemote tmp;

	memcpy(&tmp,receivedProject, sizeof(tmp));

	for(uint8_t track = 0; track < 8; track++)
	{
		sprintf(&receivedProject->values.TrackNames[track][0],"Track %d",track+1);
	}

	strcpy(receivedProject->projectName, tmp.projectName - (8*21));

}

static void recoveryProjectVersion4()
{
	strMtProjectRemote * receivedProject = versionRecovery.getReadedProject(); //wskaznik do projektu odczytanego z pliku
	strMtProjectRemote tmp;

	memcpy(&tmp,receivedProject, sizeof(tmp));

	for(uint8_t track = 0; track < 8; track++)
	{
		receivedProject->values.trackVolume[track] = 100;
	}
	receivedProject->values.reverbVolume = 100;
	receivedProject->values.delayVolume = 100;


	strcpy(receivedProject->projectName, tmp.projectName - (10 * sizeof(uint8_t)));
}


//*************************************************************************INSTRUMENT

strInstrumentFile * mtVersionRecovery::getReadedInstrument()
{
	return receivedInstrument;
}

void mtVersionRecovery::translateinstrument(uint8_t firstVersion, uint8_t lastVersion, strInstrumentFile* currentInstrument)
{
	receivedInstrument = currentInstrument; // odebrany projekt

	for(uint8_t i = firstVersion; i < lastVersion; i++ ) // wykonanie odpowiedniej ilosci etapow translacji
	{
		recoveryInstrumentFunction[i-1](); //wywolanie wskaznika funkcyjnego - obslugujacego dany etap
	}
}

static void recoveryInstrumentVersion1()
{
	strInstrumentFile * receivedInstrument = versionRecovery.getReadedInstrument(); //wskaznik do instrumentu odczytanego z pliku

	if(receivedInstrument->instrumentDataAndHeader.instrument.playMode > 4)
	{
		receivedInstrument->instrumentDataAndHeader.instrument.playMode++;
	}
}

static void recoveryInstrumentVersion2()
{
	strInstrumentFile * receivedInstrument = versionRecovery.getReadedInstrument(); //wskaznik do instrumentu odczytanego z pliku

	receivedInstrument->instrumentDataAndHeader.instrument.envelope[envFinetune].delay = 0;
	receivedInstrument->instrumentDataAndHeader.instrument.envelope[envFinetune].attack = 3000;
	receivedInstrument->instrumentDataAndHeader.instrument.envelope[envFinetune].hold = 0;
	receivedInstrument->instrumentDataAndHeader.instrument.envelope[envFinetune].decay = 0;
	receivedInstrument->instrumentDataAndHeader.instrument.envelope[envFinetune].sustain = 1.0;
	receivedInstrument->instrumentDataAndHeader.instrument.envelope[envFinetune].release = 1000;
	receivedInstrument->instrumentDataAndHeader.instrument.envelope[envFinetune].amount = 1.0;
	receivedInstrument->instrumentDataAndHeader.instrument.envelope[envFinetune].loop = 0;
	receivedInstrument->instrumentDataAndHeader.instrument.envelope[envFinetune].enable = 0;
}

static void recoveryInstrumentVersion3()
{
	strInstrumentFile * receivedInstrument = versionRecovery.getReadedInstrument(); //wskaznik do instrumentu odczytanego z pliku

	receivedInstrument->instrumentDataAndHeader.instrument.reverbSend = 0;
}
