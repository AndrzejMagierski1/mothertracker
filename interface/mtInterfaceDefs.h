
#ifndef MT_INTERFACE_DEFS_H
#define MT_INTERFACE_DEFS_H


#define MT_INTERFACE_STARTUP_TIME 200


enum mtInterfacePadButtonsFunctions
{
//1
	interfacePadPlay,
	interfacePadStop,
	interfacePadRec,
	interfacePadProjectEditor,
	interfacePadSampleBank,
	interfacePadInstrumentEditor,
	interfacePadConfig,
	interfacePadSettings,
//2
	empty8,
	empty9,
	empty10,
	interfacePadRecorder,
	interfacePadParams,
	interfacePadEnvelopes,
	empty14,
	empty15,
//3
	empty16,
	empty17,
	empty18,
	empty19,
	empty20,
	empty21,
	interfacePadUp,
	empty23,
//4
	interfacePadNotes,
	interfacePadInstr,
	interfacePadVelocity,
	interfacePadFx,
	interfacePadFxParams,
	interfacePadLeft,
	interfacePadDown,
	interfacePadRight,
//5
	interfacePadKeybord0,


};




enum mtInterfaceModules
{
	mtModuleProjectEditor,
	mtModuleInstrumentEditor,
	mtModuleStepEditor,
	mtModuleSampleBankEditor,
	mtModuleConfigEditor,



	mtModulesCount
};





enum enumMtOperatingMode
{
	mtOperatingModeNone,
	mtOperatingModeStartup,
	mtOperatingModeRun,
};


#endif
