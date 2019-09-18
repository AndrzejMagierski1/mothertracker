

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


class cInterfacePopups
{
public:

	// gui constrols
	void initPopupsDisplayControls();
	void destroyPopupsDisplayControls();

	// step popups funct
	uint8_t getStepPopupState();
	void showStepPopup(uint8_t stepPopupNone, int8_t initVal);
	void refreshPopups();
	void hideStepPopups();

	void changeStepPopupValue(int16_t value);
	void setStepPopupValue(int16_t value);
	int16_t getStepPopupValue();


	// global popups functs
	void showGlobalPopup();






private:

	void showNotesPopup();
	void showInstrumentsPopup();
	void showVolumesPopup();
	void showFxesPopup();


	void showActualInstrument();
	void listInstruments();

	void refreshAllList();
	void refreshList(uint8_t n);

	void selectPadOnPopup(int8_t pad);

	//
	hControl listControl[4];
	hControl keyboardControl;


	hControl bgLabel;
	hControl textLabel1;
	hControl textLabel2;


	strList popupList[4];
	strPadNames padNamesStruct;



	int8_t selectedActualItem = 0;
	char** ptrActualItemsList = nullptr;



	uint8_t stepPopupState = 0;





};

extern cInterfacePopups mtPopups;




#endif /* INTERFACE_INTERFACEPOPUPS_H_ */
