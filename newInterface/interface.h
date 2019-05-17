

#ifndef NEWINTERFACE_INTERFACE_H_
#define NEWINTERFACE_INTERFACE_H_



#include "interfaceDefs.h"
#include <stdint.h>
#include "elapsedMillis.h"



#include "mtHardware.h"
#include "modulesBase.h"




typedef cModuleBase* hModule;


class cInterface
{
public:

	// mtInterface.cpp -----------------------------
	void begin();
	void update();
	void setOperatingMode(uint8_t mode);



	// mtInterface.cpp -----------------------------



	// mtInterfaceInputs.cpp -----------------------------
	void potChange(uint8_t n, int16_t value);
	void buttonChange(uint8_t n, uint8_t value);
	void powerButtonChange(uint8_t value);
	void padPressed(uint8_t n, int8_t x, int8_t y, uint8_t velo);
	void padReleased(uint8_t n);
	void seqButtonPressed(uint8_t x, uint8_t y);
	void seqButtonReleased(uint8_t x, uint8_t y);
	void seqButtonHold(uint8_t x, uint8_t y);
	void seqButtonDouble(uint8_t x, uint8_t y);
	// mtInterfaceInputs.cpp -----------------------------

	uint8_t activeModules[mtModulesCount] = {0};

	void activateModule(uint8_t module);
	void deactivateModule(uint8_t module);

private:

	void processOperatingMode();

	static const uint8_t modulesCount;
	static const hModule modules[];


	cFunctionMachine uiFM;


	uint8_t operatingMode = 0;

	elapsedMillis startupTimer;


	uint8_t onScreenModule = 0;


};


extern cInterface mtInterface;

extern void interfaceEnvents(uint8_t event, void* param1, void* param2, void* param3);






#endif /* NEWINTERFACE_INTERFACE_H_ */
