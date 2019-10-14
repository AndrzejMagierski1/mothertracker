#ifndef SOURCE_MTSLEEP_H_
#define SOURCE_MTSLEEP_H_
#include "Snooze.h"
#include "mtHardware.h"


#define CM4_SCB_AIRCR 					(*(uint32_t*)0xE000ED0C)
#define CM4_SCB_AIRCR_VECTKEY_POS 		16
#define CM4_SCB_AIRCR_SYSRESETREQ_MASK	0x04U


#define TURN_OFF_TIME_MS					2000

void goLowPower(uint8_t value);
void wakeUp(uint8_t value);
void changePowerState(uint8_t value);
void disableAll();
void powerModeUpdate();

uint8_t isLowPower();

enum powerType
{
	powerTypeLow,
	powerTypeNormal
};
extern uint8_t powerState;
extern uint8_t powerChanged;




#endif /* SOURCE_MTSLEEP_H_ */
