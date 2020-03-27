#include "mtFileManagerOldVersionRecovery.h"
#include "fileManagerDefs.h"

static void recoveryProjectVersion1();

mtVersionRecovery versionRecovery;


//przepisanie funkcji do wskaznika
mtVersionRecovery::mtVersionRecovery()
{
	recoveryProjectFunction[0] = recoveryProjectVersion1;
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
	receivedProject->values.delayPanning = 0;

	memcpy(receivedProject->projectName,(uint8_t *)(((uint32_t)&tmp.projectName) - 4) , PROJECT_NAME_SIZE );
	// shift 4 zostal dobrany metoda prob i bledow, teoretycznie dodano 1 byte ale prawdopdobnie kompilator to ulozyl w jakis inny sposob

}
