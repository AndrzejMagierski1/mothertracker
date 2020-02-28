

#include "mtStructs.h"
#include "fileManagerDefs.h"

#include "SD.h"
#include "debugLog.h"

#include "fileTransfer.h"
#include "fileManager.h"


#include "mtAudioEngine.h"


bool cFileManager::previevSamplefromSD(uint8_t index)
{
	if(status != fmIdle && status != fmSavingProjectToWorkspace) return false;
	if(currentOperation != fmNoOperation && currentOperation != fmSaveWorkspaceProject) return false;


	char filePath[255];
	strcpy(filePath, explorerCurrentPath);
	strcat(filePath, "/");
	strcat(filePath, explorerList[index]);

	if(!SD.exists(filePath)) return false;

	wavfile.open(filePath);

	if(!wavfile)
	{
		wavfile.close();
		return false;
	}

	strWavFileHeader header;
	readHeader(&header,&wavfile);

	wavfile.close();

	if(header.AudioFormat == 3) playSdWavFloat.play(filePath);
	else
	{
		if(header.bitsPerSample == 16) playSdWav.play(filePath);
		else if (header.bitsPerSample == 24) playSdWav24Bit.play(filePath);
	}


	status = fmPreviewSampleFromSd;
	currentOperationStep = 0;
	currentOperation = fmPreviewSamplesFromSD;

	return true;
}


bool cFileManager::stopPrevievSamplefromSD()
{
	playSdWav.stop();
	playSdWavFloat.stop();
	playSdWav24Bit.stop();


	status = fmIdle;
	currentOperationStep = 0;
	currentOperation = fmNoOperation;


	return true;
}




