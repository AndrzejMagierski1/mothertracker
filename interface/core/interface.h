
#ifndef INTERFACE_INTERFACE_H_
#define INTERFACE_INTERFACE_H_


#include "core/interfaceDefs.h"
#include "core/modulesBase.h"
#include <stdint.h>
#include "elapsedMillis.h"

#include "mtHardware.h"

#include "core/interfacePopups.h"


typedef cModuleBase* hModule;
class cInterfacePopups;

//Higher number = higher priority
enum action_priorities
{
	noSdCardPriority = 0x01U,
	powerButtonActionPriority = 0x02U,
};


class cInterface
{
public:

	// interface.cpp -----------------------------
	void begin();
	void update();
	void setOperatingMode(uint8_t mode);

	void activateModule(hModule module, uint32_t options);
	void deactivateModule(hModule module);
	void switchModuleByButton(hModule module, uint8_t button, uint8_t options);
	void switchModuleToPrevious(hModule module);
	int8_t getButtonIndex(uint8_t button);
	void toggleActiveModule(uint8_t state);

	void handleCardSlotAction(uint8_t state);
	void handlePowerButtonAction(uint8_t state);

	friend void interfaceEnvents(uint8_t event, void* param1, void* param2, void* param3);

	// interfaceMisc.cpp -----------------------------
	void openStartupProject();
	uint8_t detectProjectLoadState();

	void initStartScreen();
	void refreshStartScreen();
	void hideStartScreen();
	void destroyStartScreen();


	void activateInterface();
	void deactivateInterfaceNoSd();
	void deactivateInterfaceShutdown();

	void showDisplayNoSdCard();
	void hideDisplayNoSdCard();


	void showDisplayShutdown();
	void refreshDisplayShutdown();
	void hideDisplayShutdown();

	void commonThingsUpdate();

	// interfaceInputs.cpp -----------------------------
	void potChange(uint8_t n, int16_t value);
	void buttonChange(uint8_t n, uint8_t value);
	void powerButtonChange(uint8_t state);
	void padPressed(uint8_t n, int8_t x, int8_t y, uint8_t velo);
	void padReleased(uint8_t n);
	void padHold(uint8_t n);
	void SDCardChange(uint8_t state);

private:
	friend cInterfacePopups;

	void processOperatingMode();
	void doStartTasks();
	void processStartScreen();
	void processPowerOffSequence();


	static const uint8_t modulesCount;
	static const hModule modules[];

	static const uint8_t modulesButtonsCount;
	static const uint32_t modulesButtons[][3];

	cFunctionMachine uiFM;


	uint8_t operatingMode = 0;


	hModule onScreenModule = nullptr;
	uint32_t lastOptions = 0;

	uint8_t toggledState = 0; //blokuje toggl(1) kiedy nie bylo toggle(0) a modol tez nie jest aktywny

	hModule previousModule = nullptr;
	uint32_t previousModuleOptions = 0;

	// interfaceMisc.cpp -----------------------------
	uint8_t minStartTimeCounter = 0;
	uint8_t startProjectLoadingProgress = 0;
	hControl startScreenControl = nullptr;
	strStartScreenData startScreenData;

	hControl turnOffProgressBar = nullptr;
	char turnOffText[20];

	hControl noSdTextControl;
	const char *noSdText = "Please insert SD card to continue";
	const char *projectLoadText = "Opening last project...";

	//styl popupu interfejsu
	strPopupStyleConfig popupConfig;

	uint8_t openFromWorkspaceFlag = 0;

	uint8_t isBooted = 0;
	uint32_t globalActionPriority;

	uint8_t fileManagerPopupEnabled = 0;
	uint8_t fileManagerPopupState = 0;
};


extern cInterface mtInterface;

extern void interfaceEnvents(uint8_t event, void* param1, void* param2, void* param3);






#endif /* INTERFACE_INTERFACE_H_ */
