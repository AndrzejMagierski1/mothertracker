
#ifndef MT_INTERFACE_DEFS_H
#define MT_INTERFACE_DEFS_H


#define MT_INTERFACE_STARTUP_TIME 200




enum mtInterfaceModules
{
	mtModuleProjectEditor,
	mtModuleInstrumentEditor,
	mtModuleStepEditor,



	mtModulesCount
};





enum enumMtOperatingMode
{
	mtOperatingModeNone,
	mtOperatingModeStartup,
	mtOperatingModeRun,
};


#endif
