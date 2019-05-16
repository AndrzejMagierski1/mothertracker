#ifndef NEWINTERFACE_MODULESBASE_H_
#define NEWINTERFACE_MODULESBASE_H_


#include <stdint.h>
#include <display.h>



class cModuleBase
{
public:

	virtual void update() = 0;
	virtual void start(uint32_t options) = 0;
	virtual void stop() = 0;



	void setEventFunct(void (*func)(uint8_t, void*, void*, void*)) { eventFunct = func; };



private:

	void (*eventFunct)(uint8_t, void*, void*, void*);




};




class  cFunctionMachine
{

// funkcja wykonywujaca sie
// kontrolka
// dane do kontrolki
//
	template <typename T>
	void setFunctObject(uint8_t objectID, T* param, T min, T max, T step, hControl control)
	{

	}

	void setFunctObject(uint8_t objectID, void(*funct)(void), hControl control);
	void setFunctObject(uint8_t objectID, void(*funct)(int16_t), hControl control);
	void setFunctObject(uint8_t objectID, void(*funct)(uint8_t), hControl control);


	void processButtonsInput(uint8_t button, uint8_t state);
	void processPotsInput(uint8_t pot, int16_t value);
	void processPadInput(uint8_t pad, int16_t value);

	struct strValueChange
	{

	};


};


template <uint8_t N>
class  cStateMachine
{
// tworzenie na start wsyzstkich stanow
// przechodzenie pomiedzy stanami
//
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
