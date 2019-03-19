#ifndef SOURCE_MTSLEEP_H_
#define SOURCE_MTSLEEP_H_
#include "Snooze.h"
#include "mtHardware.h"

void goLowPower();
void wakeUp();
void changePowerState();
void disableAll();
void powerModeUpdate();

enum powerType
{
	powerTypeLow,
	powerTypeNormal
};
extern uint8_t powerState;
extern uint8_t powerChanged;

#endif /* SOURCE_MTSLEEP_H_ */
