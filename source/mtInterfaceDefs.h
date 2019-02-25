
#ifndef MT_INTERFACE_DEFS_H
#define MT_INTERFACE_DEFS_H


#define MT_INTERFACE_STARTUP_TIME 200


enum mtInterfacePadButtonsFunctions
{
	interfacePadPlay,
	interfacePadStop,
	empty1,
	interfacePadProjectEditor,
	interfacePadSampleBank,
	interfacePadInstrumentEditor,
	interfacePadConfig,
	interfacePadSettings,



};




enum mtInterfaceModules
{
	mtModuleProjectEditor,
	mtModuleInstrumentEditor,
	mtModuleStepEditor,
	mtModuleSampleBankEditor,



	mtModulesCount
};





enum enumMtOperatingMode
{
	mtOperatingModeNone,
	mtOperatingModeStartup,
	mtOperatingModeRun,
};


#endif
