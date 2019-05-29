#ifndef NEWINTERFACE_PATTERNEDITOR_H_
#define NEWINTERFACE_PATTERNEDITOR_H_


#include "modulesBase.h"

#include "SD.h"

#include "mtSequencer.h"



class cPatternEditor: public cModuleBase
{

public:

	virtual void update();
	virtual void start(uint32_t options);
	virtual void stop();

	cPatternEditor() {}
	~cPatternEditor() {}

	void showDefaultScreen();

	void focusOnSelected();

//----------------------------------
	void refreshPattern();
	void readPatternState();

	void changeActualStepNote(int16_t value);
	void changeActualStepInstrument(int16_t value);
	void changeActualStepVolume(int16_t value);

	uint16_t lastPatternPosition;

	Sequencer::strPattern * seq;

	strTrackerPattern trackerPattern;

	uint8_t editMode = 0;

	//uint8_t last_selected_instrument = 0;
	uint8_t last_selected_length = 100;
//----------------------------------



	hControl topLabel[8];
	hControl bottomLabel[8];
	hControl patternControl;





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

#endif /* NEWINTERFACE_PATTERNEDITOR_H_ */