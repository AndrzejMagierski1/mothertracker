
/*
//	wywolywanie popupu	- max 4 linie tekstu
void show(config_slot, linia_1_textu);
void show(config_slot, linia_1_textu,linia_2_textu);
void show(config_slot, linia_1_textu,linia_1_textu...);

//	 ukrywanie jesli potrzba szybciej niz ustawiony czas
void hide();

//	configuracja wygladu popupu
void config(slot, struktura_konfigutracji);
*/


#ifndef INTERFACE_INTERFACEPOPUPS_H_
#define INTERFACE_INTERFACEPOPUPS_H_


#include <modulesBase.h>
#include <elapsedMillis.h>



enum enumStepPopups
{
	stepPopupNone,
	stepPopupNote,
	stepPopupInstr,
	stepPopupFx
};

struct strPopupStyleConfig
{
	uint8_t time;

	uint16_t x;
	uint16_t y;
	uint16_t w;
	uint16_t h;

	uint32_t lineColor[4];
	uint32_t lineStyle[4];

};


class cInterfacePopups
{
public:

	void update();

	// gui constrols
	void initPopupsDisplayControls();
	void destroyPopupsDisplayControls();


	//-------------------------------------
	// step popups funct
	void showStepPopup(uint8_t stepPopupType, int8_t initVal);
	void hideStepPopups();

	void changeStepPopupValue(int16_t value, uint8_t dir = 0);
	void setStepPopupValue(int16_t value);

	uint8_t getStepPopupState();
	int16_t getStepPopupValue();

	void lightUpPads();


	void toggleStepPopupDescription();

	//-------------------------------------
	// global popups functs
	void show(uint8_t config_slot, char* line1);
	void show(uint8_t config_slot, const char* line1)
	{ show(config_slot, (char*) line1); }

	void show(uint8_t config_slot, char* line1, char* line2);
	void show(uint8_t config_slot, const char* line1, const char* line2)
	{ show(config_slot, (char*) line1, (char*) line2); }

	void show(uint8_t config_slot, char* line1, char* line2, char* line3);
	void show(uint8_t config_slot, const char* line1, const char* line2, const char* line3)
	{ show(config_slot, (char*) line1, (char*) line2, (char*) line3); }

	void show(uint8_t config_slot, char* line1, char* line2, char* line3, char* line4);
	void show(uint8_t config_slot, const char* line1, const char* line2, const char* line3, const char* line4)
	{ show(config_slot, (char*) line1, (char*) line2, (char*) line3, (char*) line4); }

	void show(uint8_t config_slot, char** multiLineText, uint8_t lines_count);
	void hide();

	void config(uint8_t slot, strPopupStyleConfig* config);



private:

	//-------------------------------------
	// step
	void setPopupFunct();

	void showNotesPopup();
	void showInstrumentsPopup();
	void showFxesPopup();

	void showActualInstrument();
	void listInstruments();

	void refreshAllList();
	void refreshList(uint8_t n);

	void selectPadOnPopup(int8_t pad);

	void refreshStepPopupDescription();

	hControl listControl;
	hControl keyboardControl;

	hControl bgLabel;
	hControl textLabel1;
	hControl textLabel2;

	strList popupList;
	strList instrList;
	strPadNames padNamesStruct;
	strLabelData labelDoubleArrow;

	int8_t selectedActualItem = 0;
	char** ptrActualItemsList = nullptr;

	uint8_t stepPopupState = 0;
	uint8_t stepPopupDescriptionState = 0;

	//-------------------------------------
	// global

	hControl textPopup;
	strTextPopupData popupData;


	static const uint8_t textLinesCount = 4;
	static const uint8_t configsCount = 5;
	strPopupStyleConfig globalConfig[configsCount] =
	{
		{1,800/2-50,480/2-50,100,100,0xffffff,0xffffff,0xffffff,0xffffff,0,0,0,0,},
		{1,800/2-50,480/2-50,100,100,0xffffff,0xffffff,0xffffff,0xffffff,0,0,0,0,},
		{1,800/2-50,480/2-50,100,100,0xffffff,0xffffff,0xffffff,0xffffff,0,0,0,0,},
		{1,800/2-50,480/2-50,100,100,0xffffff,0xffffff,0xffffff,0xffffff,0,0,0,0,},
		{1,800/2-50,480/2-50,100,100,0xffffff,0xffffff,0xffffff,0xffffff,0,0,0,0,},
	};

	char* textLines[textLinesCount];
	uint32_t* textStyles[textLinesCount];
	uint32_t* textColors[textLinesCount];

	elapsedMillis popupTimer;
	uint8_t globalPopupDisplayTime = 0;

};

extern cInterfacePopups mtPopups;




#endif /* INTERFACE_INTERFACEPOPUPS_H_ */
