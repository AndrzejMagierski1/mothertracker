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
	hControl notePopoutControl = nullptr;
	hControl titleBar;
	hControl titleLabel;



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

	strPadNames padNamesStruct;
	char *padNamesPointer[48];
	char padNames[48][5];
	void showNotePopout();
	void hideNotePopout();

	uint8_t noteButtonHoldFlag;
};

extern cPatternEditor patternEditor;





#endif /* INTERFACE_PATTERNEDITOR_H_ */




