#ifndef NEWINTERFACE_PROJECTEDITOR_H_
#define NEWINTERFACE_PROJECTEDITOR_H_


#include "modulesBase.h"

enum mtProjectStartMode
{
	mtProjectStartModeDoNothing,
	mtProjectStartModeOpenLast,
	mtProjectStartModeOpenProject,
	mtProjectStartModeNewProject,

};


enum mtProjectEditorEvents
{
	mtPriojectEditorEventNone,
	mtPriojectEditorEventLoadLastProjFailed,
	mtPriojectEditorEventPadPress,
	mtProjectEditorEventLoadSampleBank,

	ProjEditEventCount
};




class cProjectEditor: public cModuleBase
{

public:

	virtual void update();
	virtual void start(uint32_t options);
	virtual void stop();











};





extern cProjectEditor projectEditor;




#endif /* NEWINTERFACE_PROJECTEDITOR_H_ */
