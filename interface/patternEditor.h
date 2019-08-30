#ifndef INTERFACE_PATTERNEDITOR_H_
#define INTERFACE_PATTERNEDITOR_H_


#include <modulesBase.h>
#include "SD.h"

#include "mtSequencer.h"

uint8_t isMultiSelection();
void sendSelection();
void sendPasteSelection();

const uint8_t fillTypeListCount = 3;

const char fillTypeListLabels[fillTypeListCount][11]=
{
		"Constant",
		"From-To",
		"Random",

};

const uint8_t fillScaleFilterCount = 2;

const char fillScaleFilterLabels[fillScaleFilterCount][5]=
{
		"Yes",
		"No",

};

const uint8_t fillFxTypeCount = 10;

const char fillFxTypeLabels[fillFxTypeCount][20]=
{
		"A - Aaaaaaaa",
		"B - Bbbbbbbb",
		"C - Cccccccc",
		"D - Ddddddd",
		"E - Eeeeeee",
		"F - Fffff",
		"G - Gggggggg",
		"H - Hhhhhhhhhh",
		"I - Iiiiiiii",
		"J - Jjjjjjjj",

};

class cPatternEditor: public cModuleBase
{

public:

	virtual void update();
	virtual void start(uint32_t options);
	virtual void stop();

	virtual void initDisplayControls();
	virtual void destroyDisplayControls();

	cPatternEditor()
	{
		seq = nullptr;
		lastPatternPosition = 0;

		patternControl = nullptr;
	}
	virtual ~cPatternEditor() {}

//----------------------------------

	void showDefaultScreen();
	void setDefaultScreenFunct();

	void focusOnActual();

	void moveCursorByStep();

	void refreshPattern();
	void readPatternState();

	void changeActualStepNote(int16_t value);
	void changeActualStepInstrument(int16_t value);
	void changeActualStepVolume(int16_t value);

	void focusOnPattern();
	void unfocusPattern();

	void changeActualTempo(int16_t value);
	void changeActualPattern(int16_t value);
	void changeActualPatternLength(int16_t value);
	void changeActualPatternEditStep(int16_t value);

	void changeFillData(int16_t value);
	void changeRandomiseData(int16_t value);

	void refreshEditState();

	void lightUpPadBoard();
	void clearPadBoard();
//----------------------------------
	void activateLabelsBorder();
	void activateFillPopupBorder();
	void activateRandomisePopupBorder();

	void showTempo();
	void showPattern();
	void showLength();
	void showStep();

	void refreshPatternParams();

	void showEditModeLabels();
	void hideEditModeLabels();

	void showFillPopup();
	void refreshFillType();
	void refreshFillFrom();
	void refreshFillTo();
	void refreshFillParam();
	void refreshFillStep();
	void hideFillPopup();


	void showRandomisePopup();
	void refreshRandomiseFrom();
	void refreshRandomiseTo();
	void refreshRandomiseParam();
	void refreshRandomiseStep();
	void hideRandomisePopup();

//----------------------------------
// Funct
	uint16_t lastPatternPosition;
	uint8_t isPleyheadOnScreen();

	uint16_t lastPlayedPattern = 0;

	uint8_t isCursorInSelection();

	Sequencer::strPattern * seq;

	strTrackerPattern trackerPattern;

	uint8_t editMode = 0;
	uint8_t editParam = 0;
	uint8_t isSelectingNow = 0;

	uint8_t padsTempData[48];

//----------------------------------
// GUI

	strFrameData frameData;

	hControl topLabel[8];
	hControl bottomLabel[8];
	hControl patternControl;
	hControl frameControl;

	// popups
	hControl patternPopupLabel;
	hControl param1PopupListControl;
	hControl val1PopupLabel;
	hControl val2PopupLabel;
	hControl val3PopupLabel;
	hControl param2PopupListControl;



	strLabelData bottomValuesConfig = { 40, 0, 2048UL | 256UL };

	int8_t selectedPlace = -1;

	// parametry
	char tempo[7];
	char pattern[5];
	char length[5];
	char step[5];


//------------------------------------------------------
// fill
	uint8_t fillState = 0;
	uint8_t fillPlace = 0;
	//
	strList fillTypeList;
	char *fillTypeListNames[fillTypeListCount];
	//
	strList fillScaleFilterList;
	char *fillScaleFilterNames[fillScaleFilterCount];
	//
	strList fillFxTypeList;
	char *fillFxTypeNames[fillFxTypeCount];
	//
	char fillText1[10];
	char fillText2[10];
	char fillText3[10];
	//
	struct strFill
	{
		uint16_t type;
		uint16_t from;
		uint16_t to;
		uint16_t param;

	} fillData[4];

	uint16_t fillStep = 1;

//------------------------------------------------------
// random
	uint8_t randomiseState = 0;
	uint8_t randomisePlace = 1;
	//
	// listy wziete z filla
	//
	// texty z fill
	//
	struct strRandomise
	{
		uint16_t from;
		uint16_t to;
		uint16_t param;

	} randomiseData[4];

	uint16_t randomiseStep = 1;



};

extern cPatternEditor patternEditor;



const char mtNotes[128][5] =
{
	"C0",	"C#0",	"D0",	"D#0",	"E0",	"F0",	"F#0",	"G0",	"G#0",	"A0",	"A#0",	"B0",
	"C1",	"C#1",	"D1",	"D#1",	"E1",	"F1",	"F#1",	"G1",	"G#1",	"A1",	"A#1",	"B1",
	"C2",	"C#2",	"D2",	"D#2",	"E2",	"F2",	"F#2",	"G2",	"G#2",	"A2",	"A#2",	"B2",
	"C3",	"C#3",	"D3",	"D#3",	"E3",	"F3",	"F#3",	"G3",	"G#3",	"A3",	"A#3",	"B3",
	"C4",	"C#4",	"D4",	"D#4",	"E4",	"F4",	"F#4",	"G4",	"G#4",	"A4",	"A#4",	"B4",
	"C5",	"C#5",	"D5",	"D#5",	"E5",	"F5",	"F#5",	"G5",	"G#5",	"A5",	"A#5",	"B5",
	"C6",	"C#6",	"D6",	"D#6",	"E6",	"F6",	"F#6",	"G6",	"G#6",	"A6",	"A#6",	"B6",
	"C7",	"C#7",	"D7",	"D#7",	"E7",	"F7",	"F#7",	"G7",	"G#7",	"A7",	"A#7",	"B7",
	"C8",	"C#8",	"D8",	"D#8",	"E8",	"F8",	"F#8",	"G8",	"G#8",	"A8",	"A#8",	"B8",
	"C9",	"C#9",	"D9",	"D#9",	"E9",	"F9",	"F#9",	"G9",	"G#9",	"A9",	"A#9",	"B9",
	"C10",	"C#10",	"D10",	"D#10",	"E10",	"F10",	"F#10",	"G10"
};

#endif /* INTERFACE_PATTERNEDITOR_H_ */




