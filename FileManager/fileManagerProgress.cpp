

#include "mtStructs.h"
#include "fileManagerDefs.h"
#include "SD.h"

#include "mtSequencer.h"

#include "debugLog.h"

#include "fileManager.h"
#include "fileTransfer.h"





//-----------------------------------------------------------------------------------------------------
//----------------------------------------   POSTEP   -------------------------------------------------
//-----------------------------------------------------------------------------------------------------


void cFileManager::calcTotalMemoryToTransfer()
{
	switch(status)
	{
	case fmSavingProjectToProjects:
	case fmLoadingProjectfromWorkspace:
	{
		totalMemoryToTranfser = 0;
		totalMemoryToTranfser += sizeof(strProjectFile);
		totalMemoryToTranfser += sizeof(Sequencer::strPattern);
		totalMemoryToTranfser += calcWorkspaceInstrumentsSize();
		totalMemoryToTranfser += calcWorkspaceSamplesSize(); //xxx moze troche trwac, mozna zoptymalizwac
		break;
	}
	case fmLoadingProjectFromProjects:
	{
		totalMemoryToTranfser = 0;
		totalMemoryToTranfser += sizeof(strProjectFile);
		totalMemoryToTranfser += calcProjectsInstrumentsSize();
		totalMemoryToTranfser += calcProjectSamplesSize(); //xxx moze troche trwac, mozna zoptymalizwac
		totalMemoryToTranfser *= 2;
		totalMemoryToTranfser += calcProjectPatternsSize();
		totalMemoryToTranfser += sizeof(Sequencer::strPattern);
		break;
	}

	case fmImportingSamplesToWorkspace:
	{
		totalMemoryToTranfser = 0;
		//totalMemoryToTranfser += (sizeof(strInstrumentFile)*importEndSlot-importStartSlot+1)*2;
		totalMemoryToTranfser += calcImportSamplesSize();
		break;
	}

	case fmCopyingInstrumentsInWorkspace:
	{
		totalMemoryToTranfser = 0;

		break;
	}

	default: break;
	}







}

void cFileManager::calcActualMemoryTransfered()
{
	switch(status)
	{
	case fmLoadingProjectfromWorkspace:
	{
		if(currentOperationStep == 0) actualMemoryTransfered = 0;
		if(currentOperationStep > 0) actualMemoryTransfered = sizeof(strProjectFile);
		if(currentOperationStep > 1) actualMemoryTransfered += sizeof(Sequencer::strPattern);
		if(currentOperationStep > 2) actualMemoryTransfered += sizeof(strInstrumentFile)*mtProject.instruments_count;
		if(currentOperationStep > 3) actualMemoryTransfered += getActualSampleMemoryLoaded();
		break;
	}
	case fmLoadingProjectFromProjects:
	{
		if(currentOperation == fmCopyProjectsToWorkspace)
		{
			if(currentOperationStep == 0) actualMemoryTransfered = 0;
			if(currentOperationStep > 0) actualMemoryTransfered = sizeof(strProjectFile);
			if(currentOperationStep > 1) actualMemoryTransfered += sizeof(Sequencer::strPattern)*copiedPatternsCount;
			if(currentOperationStep > 2) actualMemoryTransfered += sizeof(strInstrumentFile)*mtProject.instruments_count;
			if(currentOperationStep > 3) actualMemoryTransfered += getTotalSampleMemoryCopied()+fileTransfer.getBytesComplited();
			//if(currentOperationStep > 4) actualMemoryTransfered += getActualSampleMemoryLoaded();
		}
		else //fmLoadWorkspaceProject
		{
			actualMemoryTransfered = totalMemoryToTranfser/2;
			if(currentOperationStep > 0) actualMemoryTransfered += sizeof(strProjectFile);
			if(currentOperationStep > 1) actualMemoryTransfered += sizeof(Sequencer::strPattern);
			if(currentOperationStep > 2) actualMemoryTransfered += sizeof(strInstrumentFile)*mtProject.instruments_count;
			if(currentOperationStep > 3) actualMemoryTransfered += getActualSampleMemoryLoaded();
		}

		break;
	}
	case fmSavingProjectToProjects:
	{
		if(currentOperationStep == 0) actualMemoryTransfered = 0;
		if(currentOperationStep > 0) actualMemoryTransfered = sizeof(strProjectFile);
		if(currentOperationStep > 1) actualMemoryTransfered += sizeof(Sequencer::strPattern);
		if(currentOperationStep > 2) actualMemoryTransfered += sizeof(strInstrumentFile)*mtProject.instruments_count;
		if(currentOperationStep > 3) actualMemoryTransfered += getTotalSampleMemoryCopied();
		break;
	}
	case fmImportingSamplesToWorkspace:
	{
		actualMemoryTransfered = 0;
		if(currentOperationStep > 0 && currentOperationStep < 5)
		{
			actualMemoryTransfered = getTotalSampleMemoryCopied()+fileTransfer.getBytesComplited();

		}
		if(currentOperationStep > 4)
		{
			actualMemoryTransfered =  getTotalSampleMemoryCopied();
			actualMemoryTransfered += getActualSampleMemoryLoaded();
		}
		break;
	}

	case fmCopyingInstrumentsInWorkspace:
	{


		break;
	}

	default: actualMemoryTransfered = 0; break;
	}


}



uint8_t cFileManager::getProgress()
{
	if(status >= fmLoadEnd) return 100;

	calcActualMemoryTransfered();
	return (actualMemoryTransfered < totalMemoryToTranfser) ? (actualMemoryTransfered*100)/totalMemoryToTranfser: 100;
}
