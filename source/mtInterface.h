

#ifndef MTINTERFACE_H
#define MTINTERFACE_H


enum enumMtOperatingMode
{
	mtOperatingModeNone,
	mtOperatingModeStartup,
	mtOperatingModeSongEditor,
	mtOperatingModeFileManager,
	mtOperatingModeSampleEditor,
	mtOperatingModePrint,
	mtOperatingModePolyLogo,
};



class cMtInterface
{
public:
	void begin();
	void update();

private:

	uint8_t operatingMode;




};










extern cMtInterface mtInterface;



#endif
