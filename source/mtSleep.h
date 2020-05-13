#ifndef SOURCE_MTSLEEP_H_
#define SOURCE_MTSLEEP_H_
#include "Snooze.h"
#include "mtHardware.h"



#define CM4_SCB_AIRCR 					(*(uint32_t*)0xE000ED0C)
#define CM4_SCB_AIRCR_VECTKEY_POS 		16
#define CM4_SCB_AIRCR_SYSRESETREQ_MASK	0x04U


const uint16_t  TURN_OFF_TIME =			2000;


enum enShutdownState
{
	shutdownStateNone,
	shutdownStateStart,
	shutdownStateSleep,
};

class mtSleep
{
public:
	void startPowerOffSequence();
	void stopPowerOffSequence();

	uint16_t getTimeLeft();
	uint8_t getShutdownProgress();

	inline uint8_t getLowPowerState() { return shutdownState; }
	inline void setLowPowerState(uint8_t state) { shutdownState =  state; }

	void goLowPower(uint8_t isMCUInicialized = 1);
	void wakeUp();

	void resetMCU();
private:
	uint8_t shutdownState = shutdownStateNone;



	void disableAll();
};

extern mtSleep lowPower;



#endif /* SOURCE_MTSLEEP_H_ */
