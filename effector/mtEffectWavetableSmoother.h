#ifndef EFFECTOR_MTEFFECTWAVETABLESMOOTHER_H_
#define EFFECTOR_MTEFFECTWAVETABLESMOOTHER_H_
#include "mtEffect.h"

struct strEffectWavetableSmootherParams
{
	int windowSize;
	int windowsNumber;
};


class mtEffectWavetableSmoother : public mtEffect
{
public:
//***********VIRTUAL SET PARAMS
	void setParamiter(void * ptr, uint8_t n);
//***********
//***********VIRTUAL PROCESSING
	bool startProcess() override;
	int32_t updateProcess() override;
	bool getProcessState() override;
	uint32_t getExpectedProcessLength() override;
//***********
private:

	bool detectChangeSourceWindow(); //wykrywa kiedy probki zaczynaja byc pobierane z kolejnego okienka
	uint32_t getSmoothStepNumber(); // wykrywa ile "międzyokienek" ma miec wygladzanie miedzy oryginalami

	strEffectWavetableSmootherParams effectWavetableSmootherParams;
	uint32_t processedSamples = 0;
	uint32_t length;

	uint32_t sourceWindowMax; // maksymalny numer okienka zrodlowego

	float smoothWindowStep;	// krok o jaki ma sie zmienic zrodlowe okienko - gdy przekracza wartosci calkowite zmienia sie okienko

	float currentWindowNumber; // numer zrodlowego okienka wraz z zakumulowaną zmianą zmiennoprzecinkowa

	uint32_t iCurrentWindowNumber; //zrzutowana wartosc zrodlowego okienka
	uint32_t iLastWindowNumber;

	int16_t * dstAddr;
	int16_t * srcAddr;

	float currentCrossfadeCoef; // mnoznik biezacego okienka (odpowiada za srednia wazona - drugie okienko 1 - currentCrossfadeCoef)
	float crossfadeStep;	// zmiana currentCrossfadeCoef w kazdym kroku wygladzania

	bool state = false;
};

extern mtEffectWavetableSmoother sampleEditorWavetableSmoother;




#endif /* EFFECTOR_MTEFFECTWAVETABLESMOOTHER_H_ */
