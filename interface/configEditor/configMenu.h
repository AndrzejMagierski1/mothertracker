
#ifndef INTERFACE_CONFIGEDITOR_CONFIGMENU_H_
#define INTERFACE_CONFIGEDITOR_CONFIGMENU_H_

#include "configEditor/configMenuDefs.h"


class cMenuGroup;
class cMenuItem;



class cMenuBase
{
protected:
	cMenuBase(menu_t menu_type) : type(menu_type) {}
	menu_t type;

	friend cMenuGroup;
	friend cMenuItem;
};

typedef cMenuBase* hMenuItem;


class cMenuGroup : public cMenuBase
{
public:
	cMenuGroup(cMenuGroup& parent, uint8_t slot, const char* name, uint8_t childs_count) :
		cMenuBase(menuTypeGroup),
		childsCount(childs_count),
		childs(new hMenuItem[childs_count]),
		childsNames(new char*[childs_count]),
		childsValues(new char*[childs_count]),
		groupName(name)
		{
			if(slot < parent.childsCount)
			{
				parent.childs[slot] = this;
				parent.childsNames[slot] = (char*)name;
			}
		}

	~cMenuGroup() { delete[] childs; }

	cMenuGroup* getSelChild()
	{
		return (cMenuGroup*)childs[selectedItem];
	}

	uint8_t getCount()
	{
		return childsCount;
	}

	char** getNames()
	{
		return childsNames;
	}

	char** getValues()
	{
		reloadValues();
		return childsValues;
	}

	uint8_t getSelectedItem()
	{
		return selectedItem;
	}

	void reloadValues();

	void execute();


private:
	const uint8_t childsCount;
	hMenuItem* childs;
	char** childsNames;
	char** childsValues;
	const char* groupName;


	uint8_t selectedItem = 0;

	friend class cMenuItem;
	friend class cConfigEditor;
};



class cMenuItem : public cMenuBase
{
public:
	cMenuItem(cMenuGroup& parent, uint8_t slot, const char* name, menu_item_t type, const void* setupPtr) :
		cMenuBase(menuTypeItem),
		itemName(name),
		itemType(type),
		itemSetup(setupPtr),
		ptrValueText(type==menuItemTypeValueU8 ? new char[5] : nullptr)
		{
			if(slot < parent.childsCount)
			{
				parent.childs[slot] = this;
				parent.childsNames[slot] = (char*)name;
			}
		}

	~cMenuItem() {}

	 char* getValue()
	 {
		switch(itemType)
		{
		case menuItemTypeValueU8:
		{
			sprintf(ptrValueText, "%d", *((strItemTypeValue*)itemSetup)->value);
			return ptrValueText;
		}
		case menuTypeItemListValues:
			return (char*)ptrEmptyName;	break;
		case menuTypeItemListText:
			return (char*)((strItemTypeListText*)itemSetup)->ptrText[*((strItemTypeListText*)itemSetup)->value];
		case menuTypeItemLabel:
			return ((strItemTypeLabel*)itemSetup)->text;


		case menuItemTypeEmpty:
		case menuTypeItemActionButton:
			return (char*)ptrEmptyName;
		}
		return (char*)ptrEmptyName;
	 }

	void execute1()
	{
		if(itemType == menuTypeItemActionButton && ((strItemTypeActionButton*)itemSetup)->funct1 != nullptr)
		{
			((strItemTypeActionButton*)itemSetup)->funct1();
		}

	 }
	 void execute2()
	 {

	 }

private:
	const char* itemName;
	menu_item_t itemType;
	const void* itemSetup;
	char* ptrValueText;

	friend class cMenuGroup;
};



#endif /* INTERFACE_CONFIGEDITOR_CONFIGMENU_H_ */
