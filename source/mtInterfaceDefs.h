
#ifndef MT_INTERFACE_DEFS_H
#define MT_INTERFACE_DEFS_H


#define MT_INTERFACE_STARTUP_TIME 200




enum mtInterfaceModules
{
	mtModuleProjectEditor,
	mtModuleInstrumentEditor,



	mtModulesCount
};





enum enumMtOperatingMode
{
	mtOperatingModeNone,
	mtOperatingModeStartup,
	mtOperatingModeProjectEditor,
	mtOperatingModeSongEditor,
	mtOperatingModePaternEditor,
	mtOperatingModeInstrumentEditor,
	mtOperatingModeFileManager,
	mtOperatingModeRecorder,
	mtOperatingModeMixer,
	mtOperatingModeConfig,
};


#endif
