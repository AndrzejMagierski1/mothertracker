
#ifndef INTERFACE_INTERFACE_H_
#define INTERFACE_INTERFACE_H_


#include <interfaceDefs.h>
#include <modulesBase.h>
#include <stdint.h>
#include "elapsedMillis.h"

#include "mtHardware.h"

#include "interfacePopups.h"


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

	friend void interfaceEnvents(uint8_t event, void* param1, void* param2, void* param3);

	// interfaceMisc.cpp -----------------------------
	void initStartScreen();
	void showStartScreen();
	void hideStartScreen();
	void destroyStartScreen();

	void hideAllGlobalActions();

	void initDisplayCountDown();
	void refreshDisplayCountDown(uint16_t timeLeft_ms, uint8_t progress);
	void deinitDisplayCountDown();
	uint8_t shutdownScreenInitFlag = 0;
	uint32_t shutdownRequestTimestamp;
	elapsedMillis shutdownTimer;


	void initDisplayNoSdCard();
	void refreshDsiplayNoSdCard();
	void deinitDisplayNoSdCard();
	uint8_t noSdCardInitFlag = 0;


	uint8_t detectStartState();
	void openStartupProject();

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

	void handleGlobalActions();
	void handleShutdown();
	void handleNoSdCard();

	static const uint8_t modulesCount;
	static const hModule modules[];

	static const uint8_t modulesButtonsCount;
	static const uint32_t modulesButtons[][3];

	cFunctionMachine uiFM;


	uint8_t operatingMode = 0;

	elapsedMillis startupTimer;


	hModule onScreenModule = nullptr;
	uint32_t lastOptions = 0;

	uint8_t toggledState = 0; //blokuje toggl(1) kiedy nie bylo toggle(0) a modol tez nie jest aktywny

	hModule previousModule = nullptr;
	uint32_t previousModuleOptions = 0;

	uint8_t doOnStart = 0;
	uint8_t isCardOnBoot = 0;
	uint8_t lastBootCardState = 0;


	// interfaceMisc.cpp -----------------------------
	uint8_t startSampleLoadingFlag = 0;
	uint8_t startSampleLoadingProgress = 0;
	hControl startScreenControl = nullptr;
	strStartScreenData startScreenData;


	hControl turnOffProgressBar = nullptr;
	char turnOffText[20];

	hControl noSdTextControl;
	const char *noSdText = "Please insert SD card to continue";


	//styl popupu interfejsu
	strPopupStyleConfig popupConfig;

	uint8_t openFromWorkspaceFlag = 0;

	uint8_t isBooted = 0;
	uint32_t globalActionPriority;


};


extern cInterface mtInterface;

extern void interfaceEnvents(uint8_t event, void* param1, void* param2, void* param3);






#endif /* INTERFACE_INTERFACE_H_ */
