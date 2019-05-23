
#ifndef MT_INTERFACE_DEFS_H
#define MT_INTERFACE_DEFS_H


#define MT_INTERFACE_STARTUP_TIME 200


//-------------------------------------------------------------
enum mtInterfacePadsDef
{
//1
	interfacePad0,
	interfacePad1,
	interfacePad2,
	interfacePad3,
	interfacePad4,
	interfacePad5,
	interfacePad6,
	interfacePad7,
//2
	interfacePad8,
	interfacePad9,
	interfacePad10,
	interfacePad11,
	interfacePad12,
	interfacePad13,
	interfacePad14,
	interfacePad15,
//3
	interfacePad16,
	interfacePad17,
	interfacePad18,
	interfacePad19,
	interfacePad20,
	interfacePad21,
	interfacePad22,
	interfacePad23,
//4
	interfacePad24,
	interfacePad25,
	interfacePad26,
	interfacePad27,
	interfacePad28,
	interfacePad29,
	interfacePad30,
	interfacePad31,
//5
	interfacePadKey0,

	interfacePadsCount
};

enum mtInterfaceButtonsDef
{
	interfaceButton0,
	interfaceButton1,
	interfaceButton2,
	interfaceButton3,
	interfaceButton4,
	interfaceButton5,
	interfaceButton6,
	interfaceButton7,

	interfaceButton8,
	interfaceButton9,
	interfaceButton10,
	interfaceButton11,
	interfaceButton12,

	interfaceButton13,
	interfaceButton14,
	interfaceButton15,
	interfaceButton16,
	interfaceButton17,

	interfaceButton18,
	interfaceButton19,
	interfaceButton20,
	interfaceButton21,
	interfaceButton22,

	interfaceButton23,
	interfaceButton24,
	interfaceButton25,
	interfaceButton26,
	interfaceButton27,

	interfaceButton28,
	interfaceButton29,
	interfaceButton30,
	interfaceButton31,
	interfaceButton32,

	interfaceButtonsCount
};

enum mtInterfacePotsDef
{
	interfacePot0,
	interfacePot1,
	interfacePot2,
	interfacePot3,
	interfacePot4,

	interfacePotsCount
};





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

//-------------------------------------------------------------
enum buttonStates
{
	buttonRelease,
	buttonPress,
	buttonHold,
	buttonDouble,
};

enum padStates
{
	padRelease,
	padPress,
	padChange,
};


//-------------------------------------------------------------



enum interfaceEvents
{
	eventNone,
	eventLoadLastProjFailed,
	eventEventPadPress,
	eventLoadSampleBank,

	eventSwitchModule,


	interfaceEventsCount
};



enum enumMtOperatingMode
{
	mtOperatingModeNone,
	mtOperatingModeStartup,
	mtOperatingModeRun,
};


#endif
