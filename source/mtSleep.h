#ifndef SOURCE_MTSLEEP_H_
#define SOURCE_MTSLEEP_H_
#include "Snooze.h"
#include "mtHardware.h"
#include "modulesBase.h"


#define CM4_SCB_AIRCR 					(*(uint32_t*)0xE000ED0C)
#define CM4_SCB_AIRCR_VECTKEY_POS 		16
#define CM4_SCB_AIRCR_SYSRESETREQ_MASK	0x04U


#define TURN_OFF_TIME_MS					2000


enum powerType
{
	powerTypeLow,
	powerTypeNormal
};

class mtSleep
{
public:
	mtSleep(){};
	~mtSleep(){};

	void update();

	uint8_t powerState = powerTypeNormal;

	void handlePowerState(uint8_t value);
	uint8_t isLowPower();
private:

	uint8_t state = 0;

	hControl turnOffProgressBar = nullptr;
	char turnOffText[20];

	uint8_t firstPress;
	uint32_t firstPressTimestamp;
	elapsedMillis shutdownTimer;
	uint8_t lastValue;

	void goLowPower(uint8_t value);
	void wakeUp(uint8_t value);
	void disableAll();

	void initDisplayCountDown();
	void refreshDisplayCountDown(uint16_t timeLeft_ms);
	void deinitDisplayCountDown();

	void resetMCU();

};

extern mtSleep lowPower;



#endif /* SOURCE_MTSLEEP_H_ */
