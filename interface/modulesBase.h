#ifndef INTERFACE_MODULESBASE_H_
#define INTERFACE_MODULESBASE_H_


#include <stdint.h>
#include <display.h>

#include <interfaceDefs.h>

class  cFunctionMachine;

//#############################################################################################################
//#############################################################################################################
class cModuleBase
{
	friend class cInterface;

public:
	cModuleBase()
	{

	}
	~cModuleBase(){	}

	virtual void update() = 0;
	virtual void start(uint32_t options) = 0;
	virtual void stop() = 0;

	virtual void initDisplayControls() = 0;
	virtual void destroyDisplayControls() = 0;


	void setEventFunct(void (*func)(uint8_t, void*, void*, void*)) { eventFunct = func; };

	cFunctionMachine* FM;

	void (*eventFunct)(uint8_t, void*, void*, void*);


protected:


	uint8_t moduleRefresh = 0;


};


//#############################################################################################################
//#############################################################################################################
template<typename T>
struct paramChange
{
   T* ptr;
   T min;
   T max;
   T step;
};


class  cFunctionMachine
{

public:

	void clearAll();
	void disable() {state = 0;};
	void enable() {state = 1;};

	void clearAllButtons();
	void clearAllPots();
	void clearAllPads();
	void clearButtonsRange(uint8_t from, uint8_t to);



	void setPotObj(uint8_t objectID, uint8_t(*funct)(int16_t), hControl control);
	void setPotObj(int8_t objectID, uint8_t* param, uint8_t min, uint8_t max, uint8_t step, hControl control);
	void setPotObj(int8_t objectID, uint16_t* param, uint16_t min, uint16_t max, uint16_t step, hControl control);
/*
	template <typename T>
	void setPotObj(uint8_t objectID, T* param, T min, T max, T step, hControl control)
	{

	}
*/
	template <typename T>
	void setPotObjControlValue(uint8_t objectID, T* param, T min, T max, T step, hControl control)
	{
/*
		<T>paramChange* struct =  new <T>paramChange;

		pots[objectID].funct1 = <T>changeParam;
		pots[objectID].control = control;

		pots[objectID].mode = 1;
*/
	}



	void setButtonObj(uint8_t objectID, uint8_t state, uint8_t(*funct)(void));
	void setButtonObj(uint8_t objectID, uint8_t state, uint8_t(*funct)(uint8_t));
	void setButtonObj(uint8_t objectID, uint8_t(*funct)(uint8_t));



	void setPadObj(uint8_t objectID, uint8_t state, uint8_t(*funct)(), hControl control);
	void setPadObj(uint8_t objectID, uint8_t(*funct)(uint8_t,int16_t), hControl control);




	void processButtonsInput(uint8_t button, uint8_t state);
	void processPotsInput(uint8_t pot, int16_t value);
	void processPadsInput(uint8_t pad, uint8_t state, int16_t velo);
	void processPadsInput(uint8_t pad, uint8_t state, int8_t x, int8_t y, int16_t velo);

private:

	struct strButtonObject
	{
		uint8_t mode = 0;

		uint8_t (*funct1)(void) = nullptr;
		uint8_t (*funct2)(uint8_t) = nullptr;
		uint8_t state = 0;
		hControl control = nullptr;
	};



	template <typename T>
	void changeParam(paramChange<T>* param, int16_t value)
	{
		if(*param->ptr + value > param->max) *param->ptr = param->max;
		else if(*param->ptr + value < param->min) *param->ptr = param->min;
		else *param->ptr += value;
	}


	struct strPotObject
	{
		uint8_t mode;

		uint8_t (*funct1)(int16_t) = nullptr;
		void * paramStruct;
		hControl control;
	};

	struct strPadObject
	{
		uint8_t mode;

		uint8_t (*funct1)(void) = nullptr;
		uint8_t (*funct2)(uint8_t,int16_t) = nullptr;
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


	uint8_t potsCleared = 1;
	uint8_t buttonsCleared = 1;
	uint8_t padsCleared = 1;
};









#endif /* INTERFACE_MODULESBASE_H_ */
