
#ifndef INTERFACE_PATTERNEDITOR_H_
#define INTERFACE_PATTERNEDITOR_H_

#include <modulesBase.h>
#include "SD.h"

#include "mtSequencer.h"



uint8_t isMultiSelection();
uint8_t isEditMode();
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
	void setInstrPopupFunct();
	void setVolPopupFunct();
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
	void setFillData(uint16_t value);
	void setRandomiseData(uint16_t value);

	void changneFillDataByPad(uint8_t pad);
	void changneRandomiseDataByPad(uint8_t pad);

	void refreshEditState();

	void lightUpPadBoard();
	void clearPadBoard();

	void setMuteFunct(uint8_t state);
	void setProjectSaveFlags();
//----------------------------------

	void activateLabelsBorder();
	void activateFillPopupBorder();
	void activateRandomisePopupBorder();
	void setFillPlace(uint8_t place, int8_t dir = 0);
	void changeFillPlace(int8_t diff);

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

	void activateSelection();
	void deactivateSelection();
	void playheadRecMode();
	void playheadNormalMode();

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


	hControl label[8];
	hControl bgLabel;
	hControl patternControl;
	hControl frameControl;

	// popups
	hControl patternPopupLabel;
	hControl patternPopupTitleLabel;
	hControl param1PopupListControl;
	hControl val1PopupBar;
	hControl val2PopupBar;
//	hControl val1PopupLabel;
//	hControl val2PopupLabel;
	hControl param3PopupListControl;
	hControl param2PopupListControl;
	hControl notePopoutControl = nullptr;
//	hControl titleBar;
//	hControl titleLabel;
//	hControl instrumentLabel;

	strFrameData frameData;

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
	strList fillStepList;
	//
	char fillText1[10];
	char fillText2[10];
	//
	struct strFill
	{
		int16_t type;
		int16_t from;
		int16_t to;
		int16_t param;

	} fillData[4];

	int16_t fillStep = 1;


//-------------------------------------------------------
// popups
	void cancelPopups();

	uint8_t getStepFx();
	int8_t getStepVol();

	uint8_t dontShowPopupsUntilButtonRelease = 0;
	uint8_t insertOnPopupHideDisabled = 0;

	uint8_t wasNotesEditBefore = 0;


//-------------------------------------------------------
// pojedyncze parametry / 8 trackow
	uint8_t patternViewMode = 0;  // przechowuje bitowo pokazywane parametry dla 8 trackow
	uint8_t patternViewChanged = 0;
	uint8_t selectedParams[2];
	uint8_t patternButtonReleaseActive = 0;

	void setPatternViewMode(uint8_t param);


};

extern cPatternEditor patternEditor;





#endif /* INTERFACE_PATTERNEDITOR_H_ */




