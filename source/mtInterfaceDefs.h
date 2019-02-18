
#ifndef MT_INTERFACE_DEFS_H
#define MT_INTERFACE_DEFS_H


#define MT_INTERFACE_STARTUP_TIME 200


enum mtInterfacePadButtonsFunctions
{
	interfacePadPlay,
	interfacePadStop,
	interfacePad2,
	interfacePadProjectEditor,
	interfacePadSampleBank,
	interfacePadInstrumentEditor,
	interfacePadConfig,



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
