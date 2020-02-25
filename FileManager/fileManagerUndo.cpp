

#include "mtStructs.h"
#include "fileManagerDefs.h"

#include "SD.h"
#include "debugLog.h"
#include "core/interfacePopups.h"

#include "fileManager.h"

#define UNDO_CAPACITY 20
__NOINIT(EXTERNAL_RAM) Sequencer::strPattern undoPatternBuffer[UNDO_CAPACITY] { 0 };
__NOINIT(EXTERNAL_RAM) uint8_t undoPatternBufferIndexes[UNDO_CAPACITY] { 0 };

struct strUndo
{
	uint8_t actualIndex = 0; // gdzie jesteśmy w tablicy
	uint8_t storedCount = 0; // ile razy zrzuciliśmy
	uint8_t redoPossibility = 0; // ile razy zrzuciliśmy
	strPopupStyleConfig popupConfig {
			2,					// time
			800 / 2 - 150,		// x
			480 / 2 - 50,		// y
			300,				// w
			100,				// h
			0xffffff,			// lineColor[4];
			0xffffff,
			0xffffff,
			0xffffff,
			controlStyleCenterX,//lineStyle[4];
			controlStyleCenterX,
			controlStyleCenterX,
			controlStyleCenterX };

} static undo;

#define UNDO_SONG_CAPACITY 5
__NOINIT(EXTERNAL_RAM) Sequencer::strPattern undoSongBuffer[UNDO_SONG_CAPACITY] { 0 };
__NOINIT(EXTERNAL_RAM) uint8_t undoSongBufferIndexes[UNDO_SONG_CAPACITY] { 0 };

struct strUndoSong
{
	uint8_t actualIndex = 0; // gdzie jesteśmy w tablicy
	uint8_t storedCount = 0; // ile razy zrzuciliśmy
	uint8_t redoPossibility = 0; // ile razy zrzuciliśmy


} static undoSong;



void cFileManager::storePatternUndoRevision()
{
	undoPatternBuffer[undo.actualIndex] = *sequencer.getActualPattern();
	undoPatternBufferIndexes[undo.actualIndex] = mtProject.values.actualPattern;


	undo.redoPossibility = 0;
	if (undo.actualIndex >= UNDO_CAPACITY)
	{
		undo.actualIndex = 0;
		undo.storedCount++;
	}
	else
	{
		undo.actualIndex++;
		undo.storedCount++;
	}
	if (undo.storedCount > UNDO_CAPACITY) undo.storedCount = UNDO_CAPACITY;

//	Serial.printf(
//			">>>pattern stored\nactualIndex: %d, storedCount: %d, redoPossibility: %d\n",
//			undo.actualIndex,
//			undo.storedCount,
//			undo.redoPossibility);

}

void cFileManager::storeSongUndoRevision(uint8_t index)
{
//	undoSongBuffer[undoSong.actualIndex] = *sequencer.getActualPattern(); //todo: pobrać z pliku o indexie




	char patternToLoad[PATCH_SIZE] { 0 };
	sprintf(patternToLoad, "Workspace/patterns/pattern_%02d.mtp", index);
	readPatternFile(patternToLoad, (uint8_t*) &undoSongBuffer[undoSong.actualIndex]);

	undoSongBufferIndexes[undoSong.actualIndex] = index;





	undoSong.redoPossibility = 0;
	if (undoSong.actualIndex >= UNDO_SONG_CAPACITY)
	{
		undoSong.actualIndex = 0;
		undoSong.storedCount++;
	}
	else
	{
		undoSong.actualIndex++;
		undoSong.storedCount++;
	}
	if (undoSong.storedCount > UNDO_SONG_CAPACITY) undoSong.storedCount = UNDO_SONG_CAPACITY;


	char line[80];
	sprintf(line,
			">>>pattern %d stored: actualIndex: %d, storedCount: %d, redoPossibility: %d",
			index,
			undoSong.actualIndex,
			undoSong.storedCount,
			undoSong.redoPossibility);
	debugLog.addLine(line);
//	Serial.printf(
//			">>>pattern stored\nactualIndex: %d, storedCount: %d, redoPossibility: %d\n",
//			undoSong.actualIndex,
//			undoSong.storedCount,
//			undoSong.redoPossibility);

}



void cFileManager::undoSongPattern()
{
	bool doUndo = 0;
//	uint8_t oldIndex = undoSong.actualIndex;
	if (undoSong.actualIndex > 0 && undoSong.storedCount > 0)
	{
		undoSong.actualIndex--;
		undoSong.storedCount--;
		doUndo = 1;
	}
	else if (undoSong.actualIndex == 0 && undoSong.storedCount > 0)
	{
		undoSong.actualIndex = UNDO_SONG_CAPACITY - 1;
		doUndo = 1;
	}
	else
	{
		mtPopups.config(0, &undo.popupConfig);
		mtPopups.show(0, "No undo data :(");
	}

	if (doUndo)
	{

		char filePath[PATCH_SIZE] { 0 };
		uint8_t bufferedPatternNumber = undoSongBufferIndexes[undoSong.actualIndex];
		sprintf(filePath, "Workspace/patterns/pattern_%02d.mtp", bufferedPatternNumber);

		writePatternFile(filePath, (uint8_t*) &undoSongBuffer[undoSong.actualIndex]);


		updatePatternBitmask(bufferedPatternNumber-1, (uint8_t*)&undoSongBuffer[undoSong.actualIndex]);

		undoSong.redoPossibility++;


		char line[82];
		sprintf(line,
				"<<<pattern %d restored: actualIndex: %d, storedCount: %d, redoPossibility: %d",
				bufferedPatternNumber,
				undoSong.actualIndex,
				undoSong.storedCount,
				undoSong.redoPossibility);
		debugLog.addLine(line);

//		Serial.printf(
//				"<<<pattern restored\nactualIndex: %d, storedCount: %d, redoPossibility: %d\n",
//				undoSong.actualIndex,
//				undoSong.storedCount,
//				undoSong.redoPossibility);
	}
}
void cFileManager::undoPattern()
{
	bool doUndo = 0;
	uint8_t oldIndex = undo.actualIndex;
	if (undo.actualIndex > 0 && undo.storedCount > 0)
	{
		undo.actualIndex--;
		undo.storedCount--;
		doUndo = 1;
	}
	else if (undo.actualIndex == 0 && undo.storedCount > 0)
	{
		undo.actualIndex = UNDO_CAPACITY - 1;
		doUndo = 1;
	}
	else
	{
//		Serial.printf("no more undo data\n");

		mtPopups.config(0, &undo.popupConfig);
		mtPopups.show(0, "No undo data :(");
	}

	if (doUndo)
	{

		undoPatternBuffer[oldIndex] = *sequencer.getActualPattern();

		*sequencer.getActualPattern() = undoPatternBuffer[undo.actualIndex];
		mtProject.values.actualPattern = undoPatternBufferIndexes[undo.actualIndex];
		setPatternStructChanged(mtProject.values.actualPattern);

		undo.redoPossibility++;
//		Serial.printf(
//				"<<<pattern restored\nactualIndex: %d, storedCount: %d, redoPossibility: %d\n",
//				undo.actualIndex,
//				undo.storedCount,
//				undo.redoPossibility);
	}
}
void cFileManager::redoPattern()
{
	bool doRedo = 0;
	if (undo.actualIndex < UNDO_CAPACITY - 1 && undo.redoPossibility > 0)
	{
		undo.actualIndex++;
		undo.storedCount++;
		doRedo = 1;
	}
	else if (undo.actualIndex == UNDO_CAPACITY - 1 && undo.redoPossibility > 0)
	{
		undo.actualIndex = 0;
		doRedo = 1;
	}
	else
	{
//		Serial.printf("no more redo possibility\n");
		mtPopups.config(0, &undo.popupConfig);
		mtPopups.show(0, "No redo data :(");
	}

	if (doRedo)
	{
		undo.redoPossibility--;
		*sequencer.getActualPattern() = undoPatternBuffer[undo.actualIndex];
		mtProject.values.actualPattern = undoPatternBufferIndexes[undo.actualIndex];
		setPatternStructChanged(mtProject.values.actualPattern);

//		Serial.printf(
//				">>>pattern redo\nactualIndex: %d, storedCount: %d, redoPossibility: %d\n",
//				undo.actualIndex,
//				undo.storedCount,
//				undo.redoPossibility);
	}

}
