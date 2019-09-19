

#ifndef INTERFACE_INTERFACEPOPUPS_H_
#define INTERFACE_INTERFACEPOPUPS_H_


#include <modulesBase.h>


enum enumStepPopups
{
	stepPopupNone,
	stepPopupNote,
	stepPopupInstr,
	stepPopupVol,
	stepPopupFx
};

struct strPopupStyleConfig
{
	uint8_t line1_font;
	uint8_t line1_style;
	uint8_t line1_color;

	uint8_t line2_font;
	uint8_t line2_style;
	uint8_t line2_color;

	uint8_t line3_font;
	uint8_t line3_style;
	uint8_t line3_color;

	uint8_t line4_font;
	uint8_t line4_style;
	uint8_t line4_color;
};


class cInterfacePopups
{
public:

	// gui constrols
	void initPopupsDisplayControls();
	void destroyPopupsDisplayControls();


	//-------------------------------------
	// step popups funct
	void showStepPopup(uint8_t stepPopupNone, int8_t initVal);
	void hideStepPopups();

	void changeStepPopupValue(int16_t value, uint8_t dir = 0);
	void setStepPopupValue(int16_t value);

	uint8_t getStepPopupState();
	int16_t getStepPopupValue();


	//-------------------------------------
	// global popups functs
	void show(uint8_t config_slot, char* line1);
	void show(uint8_t config_slot, char* line1, char* line2);
	void show(uint8_t config_slot, char* line1, char* line2, char* line3);
	void show(uint8_t config_slot, char* line1, char* line2, char* line3, char* line4);

	void show(uint8_t config_slot, char** multiLineText, uint8_t lines_count);

	void config(uint8_t slot, strPopupStyleConfig* config);
	void setup(uint8_t time);



private:

	//-------------------------------------
	// step
	void setPopupFunct();

	void showNotesPopup();
	void showInstrumentsPopup();
	void showVolumesPopup();
	void showFxesPopup();

	void showActualInstrument();
	void listInstruments();

	void refreshAllList();
	void refreshList(uint8_t n);

	void selectPadOnPopup(int8_t pad);

	hControl listControl[4];
	hControl keyboardControl;

	hControl bgLabel;
	hControl textLabel1;
	hControl textLabel2;

	hControl textPopup;

	strList popupList[4];
	strPadNames padNamesStruct;

	int8_t selectedActualItem = 0;
	char** ptrActualItemsList = nullptr;

	uint8_t stepPopupState = 0;

	//-------------------------------------
	// global

	static const uint8_t configsCount = 5;
	strPopupStyleConfig configs[configsCount];

	char* textlines[4];

	uint16_t displayTime = 1000;

};

extern cInterfacePopups mtPopups;




#endif /* INTERFACE_INTERFACEPOPUPS_H_ */
