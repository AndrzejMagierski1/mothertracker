#ifndef _MT_PADS_BACKLIGHT_H
#define _MT_PADS_BACKLIGHT_H







class cMtPadsBacklight
{
public:
	void init(uint16_t blink);
	void update();
	void startBlink(uint8_t gamma_pwm, uint8_t n);
	void stopBlink(uint8_t n);
	void setFrontLayer(uint8_t state, uint8_t gamma_pwm, uint8_t n );
	void setBackLayer(uint8_t state, uint8_t gamma_pwm, uint8_t n );
	void clearAllPads(uint8_t front, uint8_t back, uint8_t blink);

private:

	elapsedMillis blinkTimer;
	uint8_t frontLayer[48];
	uint8_t backLayer[48];
	uint8_t blinkLayer[48];
	uint16_t blinkConstrain;
	uint8_t blinkCounter;
	uint8_t toggler;
};











extern cMtPadsBacklight padsBacklight;


#endif
