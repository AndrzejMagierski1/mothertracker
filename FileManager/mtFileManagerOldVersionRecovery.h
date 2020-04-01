#ifndef FILEMANAGER_MTFILEMANAGEROLDVERSIONRECOVERY_H_
#define FILEMANAGER_MTFILEMANAGEROLDVERSIONRECOVERY_H_
#include "mtStructs.h"

/*Aby dodac nowa translacje wystarczy zwiekszyc PROJECT_FILE_VERSION w mtStructs.h,
 w pliku mtFileManagerOldVersionRecovery.cpp utworzyc void'a wykonującego procedure,
przypisac adres utworzonego voida w konstruktorze mtVersionRecovery do wskaznika
 z odpowiednim indeksem(jezeli numeracja jest poprawna to z najwiekszym mozliwym)*/


constexpr uint8_t PROJECT_VERSION_RECOVERY_COUNTS = PROJECT_FILE_VERSION - 1;

class mtVersionRecovery
{
public:
	mtVersionRecovery();
	strMtProjectRemote * getReadedProject();
	void translateProject(uint8_t firstVersion, uint8_t lastVersion, strMtProjectRemote* currentProject);
private:
	void (*recoveryProjectFunction[PROJECT_VERSION_RECOVERY_COUNTS])(void);
	strMtProjectRemote * receivedProject;
};

extern mtVersionRecovery versionRecovery;

#endif /* FILEMANAGER_MTFILEMANAGEROLDVERSIONRECOVERY_H_ */
