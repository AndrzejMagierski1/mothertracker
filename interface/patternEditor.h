
#ifndef INTERFACE_PATTERNEDITOR_H_
#define INTERFACE_PATTERNEDITOR_H_

#include <modulesBase.h>
#include "SD.h"

#include "mtSequencer.h"



uint8_t isMultiSelection();
void sendSelection();
void sendCopySelection();
void sendPasteSelection();




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

	void setNotePopupFunct();
	void setFxListPopupFunct();

	void focusOnActual();

	void moveCursorByStep();

	void refreshPattern();
	void readPatternState();


	void focusOnPattern();
	void unfocusPattern();

	void changeActualTempo(int16_t value);
	void changeActualPattern(int16_t value);
	void setActualPattern(int16_t value);
	void changeActualPatternLength(int16_t value);
	void setActualPatternLength(int16_t value);
	void changeActualPatternEditStep(int16_t value);
	void setActualPatternEditStep(int16_t value);

	void changeFillData(int16_t value);
	void changeRandomiseData(int16_t value);

	void changeSelectedFx(int16_t value, uint8_t type = 0);

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

	void showFxListPopup();
	void refreshFxListPopup();
	void showFxList(uint8_t n);
	void hideFxListPopup();
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
	uint8_t shiftAction = 0; // ustawić na 1 jeśli była akcja shift + cokolwiek

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
	hControl instrumentLabel;

	hControl fxListControl[4];


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
	//
	strList fillScaleFilterList;
	//
	strList fillFxTypeList;
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
// randomise
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

	strPadNames padNamesStruct;


//-------------------------------------------------------
// fx popup
	void readStepFx();
	void setStepFx();

	strList fxList[4];
	uint8_t selectedFx = 0;
	uint8_t fxListPopupState = 0;

//-------------------------------------------------------
// note popup
	void showNotePopout();
	void hideNotePopout();
	void selectNoteOnPopout(uint8_t pad);
	void showActualInstrument();

	uint8_t noteButtonHoldFlag;

//-------------------------------------------------------
//



};

extern cPatternEditor patternEditor;





#endif /* INTERFACE_PATTERNEDITOR_H_ */




