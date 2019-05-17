#ifndef NEWINTERFACE_MODULESBASE_H_
#define NEWINTERFACE_MODULESBASE_H_


#include <stdint.h>
#include <display.h>

class  cFunctionMachine;

class cModuleBase
{
	friend class cInterface;

public:

	virtual void update() = 0;
	virtual void start(uint32_t options) = 0;
	virtual void stop() = 0;



	void setEventFunct(void (*func)(uint8_t, void*, void*, void*)) { eventFunct = func; };



private:

	void (*eventFunct)(uint8_t, void*, void*, void*);

	uint8_t moduleRefresh = 0;
	cFunctionMachine* FM;


};




class  cFunctionMachine
{

public:

	void clearAll();
	void disable() {state = 0;};
	void enable() {state = 1;};




	void setPotObj(uint8_t objectID, void(*funct)(int16_t), hControl control);
	template <typename T>
	void setPotObj(uint8_t objectID, T param, T min, T max, T step, hControl control)
	{

	}

	void setButtonObj(uint8_t objectID, uint8_t state, void(*funct)(void), hControl control);
	void setButtonObj(uint8_t objectID, void(*funct)(uint8_t), hControl control);

	void setPadObj(uint8_t objectID, uint8_t state, void(*funct)(), hControl control);
	void setPadObj(uint8_t objectID, void(*funct)(uint8_t,int16_t), hControl control);




	void processButtonsInput(uint8_t button, uint8_t state);
	void processPotsInput(uint8_t pot, int16_t value);
	void processPadsInput(uint8_t pad, uint8_t state, int16_t velo);
	void processPadsInput(uint8_t pad, uint8_t state, int8_t x, int8_t y, int16_t velo);

private:

	struct strButtonObject
	{
		uint8_t isActive;

		void (*funct1)(void) = nullptr;
		void (*funct2)(uint8_t) = nullptr;
		uint8_t state;
		hControl control;
	};


	struct strPotObject
	{
		uint8_t isActive;

		void (*funct1)(void) = nullptr;
		hControl control;
	};

	struct strPadObject
	{
		uint8_t isActive;

		void (*funct1)(void) = nullptr;
		void (*funct2)(uint8_t,int16_t) = nullptr;
		uint8_t state;
		hControl control;

	};



	uint8_t state;

	static uint8_t potsCount;
	static uint8_t buttonsCount;
	static uint8_t padsCount;

	static strPotObject pots[];
	static strButtonObject buttons[];
	static strPadObject pads[];


};


template <uint8_t N>
class  cStateMachine
{
// tworzenie na start wsyzstkich stanow
// przechodzenie pomiedzy stanami
//
//

public:
	void addState(uint8_t id, void (stateBegin)(void) ,void (*stateUpdate)(void));
	void changeState();


private:

	const uint8_t statesCount = N;

	struct strSate
	{
		void (*stateBegin)(void);
		void (*stateUpdate)(void);

	};
	strSate sates[N];

};









#endif /* NEWINTERFACE_MODULESBASE_H_ */
