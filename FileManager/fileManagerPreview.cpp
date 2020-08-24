

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


	if(explorerList[index][0] == '/') return false;

	char filePath[255];
	strcpy(filePath, explorerCurrentPath);
	strcat(filePath, "/");
	strcat(filePath, explorerList[index]);

	if(!SD.exists(filePath)) return false;

	if(wavfile.isOpen())
	{
		wavfile.close();
	}

	wavfile.open(filePath);

	if(!wavfile)
	{
		wavfile.close();
		return false;
	}

	strWavFileHeader header;
	readHeader(&header,&wavfile);

	wavfile.close();

	if(header.AudioFormat == 3)
	{
		playerFloatWavFromSD.play(filePath);
		//debugLog.addLine("preview float wav");
	}
	else
	{
		if(header.bitsPerSample == 16)
		{
			player16BitWavFromSD.play(filePath);
			//debugLog.addLine("preview 16bit wav");
		}
		else if (header.bitsPerSample == 24)
		{
			player24BitWavFromSD.play(filePath);
			//debugLog.addLine("preview 24bit wav");
		}
	}


	status = fmPreviewSampleFromSd;
	currentOperationStep = 0;
	currentOperation = fmPreviewSamplesFromSD;

	return true;
}


bool cFileManager::stopPrevievSamplefromSD()
{
	if(status != fmPreviewSampleFromSd && currentOperation != fmPreviewSamplesFromSD) return false;

	player16BitWavFromSD.stop();
	playerFloatWavFromSD.stop();
	player24BitWavFromSD.stop();


	status = fmIdle;
	currentOperationStep = 0;
	currentOperation = fmNoOperation;


	return true;
}




