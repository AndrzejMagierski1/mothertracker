
#ifndef INTERFACE_INTERFACE_H_
#define INTERFACE_INTERFACE_H_


#include <interfaceDefs.h>
#include <modulesBase.h>
#include <stdint.h>
#include "elapsedMillis.h"

#include "mtHardware.h"



typedef cModuleBase* hModule;


class cInterface
{
public:

	// interface.cpp -----------------------------
	void begin();
	void update();
	void setOperatingMode(uint8_t mode);

	void activateModule(hModule module, uint32_t options);
	void deactivateModule(hModule module);
	void switchModuleByButton(hModule module, uint8_t button);
	void switchModuleToPrevious(hModule module);
	int8_t getButtonIndex(uint8_t button);

	// interfaceMisc.cpp -----------------------------
	void initStartScreen();
	void showStartScreen();
	void destroyStartScreen();

	uint8_t detectStartState();
	void openStartupProject();

	// interfaceInputs.cpp -----------------------------
	void potChange(uint8_t n, int16_t value);
	void buttonChange(uint8_t n, uint8_t value);
	void powerButtonChange(uint8_t value);
	void padPressed(uint8_t n, int8_t x, int8_t y, uint8_t velo);
	void padReleased(uint8_t n);
	void padHold(uint8_t n);
	void seqButtonPressed(uint8_t x, uint8_t y);
	void seqButtonReleased(uint8_t x, uint8_t y);
	void seqButtonHold(uint8_t x, uint8_t y);
	void seqButtonDouble(uint8_t x, uint8_t y);


private:

	void processOperatingMode();

	static const uint8_t modulesCount;
	static const hModule modules[];

	static const uint8_t modulesButtonsCount;
	static const uint32_t modulesButtons[][3];

	cFunctionMachine uiFM;


	uint8_t operatingMode = 0;

	elapsedMillis startupTimer;


	hModule onScreenModule = nullptr;
	uint32_t lastOptions = 0;

	hModule previousModule = nullptr;
	uint32_t previousModuleOptions = 0;

	uint8_t doOnStart = 1;


	// interfaceMisc.cpp -----------------------------
	uint8_t startSampleLoadingFlag = 0;
	uint8_t startSampleLoadingProgress = 0;
	hControl startScreenControl = nullptr;
	strStartScreenData startScreenData;






};


extern cInterface mtInterface;

extern void interfaceEnvents(uint8_t event, void* param1, void* param2, void* param3);



class cInterfacePopups
{
public:

	// interfacePopups.cpp -----------------------------
	void initPopupsDisplayControls();
	void destroyPopupsDisplayControls();

	void showNotesPopup(int8_t startVal);
	void showInstrumentsPopup(int8_t startVal);
	void showVolumesPopup(int8_t startVal);
	void showFxesPopup(int8_t startVal);

	void showGlobalPopup();



private:

	void showActualInstrument();
	void listInstruments();

	void refreshAllList();
	void refreshList(uint8_t n);


	// interfacePopups.cpp -----------------------------
	hControl listControl[4];
	hControl keyboardControl;


	hControl titleBar;
	hControl titleLabel;
	hControl instrumentLabel;



	strList popupList[4];
	strPadNames padNamesStruct;



	int8_t selectedActualItem = 0;
	char** ptrActualItemsList = nullptr;



	uint8_t notePopupState = 0;
	uint8_t intrPopupState = 0;
	uint8_t volPopupState = 0;
	uint8_t fxPopupState = 0;




};

extern cInterfacePopups mtPopups;




#endif /* INTERFACE_INTERFACE_H_ */
