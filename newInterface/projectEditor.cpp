
#include "projectEditor.h"


void sBegin() {};
void sUpdate() {};

/*
static cStateMachine::strSate cStateMachine::sates[5] =
{
		{sBegin,sUpdate},
		{sBegin,sUpdate},
		{sBegin,sUpdate},
		{sBegin,sUpdate},
		{sBegin,sUpdate},
};
*/

cProjectEditor projectEditor;
cStateMachine<5> peSM;




void cProjectEditor::update()
{



}

void cProjectEditor::start(uint32_t options)
{
// zdefiniuj wszsytkie mozliwe stany w tym module
// przypsiz do nich kontorlki i dane






}

void cProjectEditor::stop()
{



}
