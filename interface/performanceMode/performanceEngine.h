
#ifndef INTERFACE_PERFORMANCEMODE_PERFORMANCEENGINE_H_
#define INTERFACE_PERFORMANCEMODE_PERFORMANCEENGINE_H_



struct strPerformDataSource
{
	uint16_t* tracksPatterns;			//	uint16_t perfTracksPatterns[8]  = {1,1,1,1,1,1,1,1};
	uint8_t* fxInSlot;					//	uint8_t perfFxPlaces[12] 		= {1,2,3,4,5,6,7,8,9,10,11,12}; // jakie efekty w 12 slotach
	int16_t* valuesInSlot;				//	int16_t perfFxValues[12][4] 	= {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}};
	uint8_t* selectedValueInSlot;		//	uint8_t perfSelectedValues[12] 	= {0,0,0,0,0,0,0,0,0,0,0,0};
	uint8_t* trackState;				//	uint8_t perfTracksState[8] = {0,0,0,0,0,0,0,0}; // narazie nie uzywana
	void (*saveFunct)(void);
};


class cPerformFx;

class cPerformEngine
{
public:
	cPerformEngine(uint8_t fx_count, strPerformDataSource source[], const uint8_t data_sources_count)
			: fxCount(fx_count), fxList(new cPerformFx*[fx_count])/*, dataSource(new strPerformDataSource*[data_sources_count])*/
	{
		dataSources = source;
		dataSourcesCount = data_sources_count;
		readPerfData(0);
	}

	~cPerformEngine() {  }

	void turnOff();

	void setEditState(uint8_t state) { editState = state ? true : false;}
	void changeSlotFx(uint8_t slot, int8_t change);

	void swapSlotFx(uint8_t slotFrom, uint8_t slotTo);
	void copySlotFx(uint8_t slot);
	void pasteSlotFx(uint8_t slot);

	void loadSelectedValuesToTempValues();

	const uint8_t getFxCount() { return fxCount;}
	const char* getSlotFxName(uint8_t slot);
	int8_t getSelectedRow(uint8_t slot);
	uint8_t getTrackState(uint8_t track);
	uint8_t slotChanged(uint8_t slot);

	void fillSlotFxValue(uint8_t slot, uint8_t row, char* value);

	void setSelectedRow(uint8_t slot, uint8_t row);
	void setSlotState(uint8_t slot, uint8_t state);

	void setTrackState(uint8_t track, uint8_t state);
	void toggleTrackState(uint8_t track);

	void resetAllFx();
	void resetFxOnTrack(uint8_t fx, uint8_t track);
	void resetFxOnAffectedTrack(uint8_t slot);
	void loadAllSlotsFxOnTrack(uint8_t track);
	void loadFxValuesOnTrack(uint8_t slot, uint8_t track);
	void loadFxValuesOnAffectedTracks(uint8_t slot);
	void resetOtherSlotsWithSameFx(uint8_t slot);
	void resetSlotsWithBlockedFx(uint8_t slot);

	void changeSlotFxValue(uint8_t slot, int16_t value);

	void readPerfData(uint8_t source_index)
	{
		if(source_index >= dataSourcesCount) return;

		slotToFx = dataSources[source_index].fxInSlot;
		slotToRow = dataSources[source_index].selectedValueInSlot;
		slotRowToValue = dataSources[source_index].valuesInSlot;
		sourceDataSaveFunct = dataSources[source_index].saveFunct;

		// wymuszenie poprawnosci danych wejsciowych
		for(uint8_t slot = 0; slot < 12; slot++)
		{
			if(slotToFx[slot] >= fxCount) slotToFx[slot] = 0;
			if(slotToRow[slot] >= 4) slotToRow[slot] = 0;
			//if(*(slotRowToValue+) >= 4) slotRowToValue[slot] = 0; // value niech se bedzie jakie chce
		}
	}





private:
	friend cPerformFx;

	const uint8_t fxCount;
	cPerformFx** fxList;
	strPerformDataSource* dataSources;
	uint8_t dataSourcesCount;

	bool editState;
	bool isSlotCopied = false;
	uint8_t slotCopied;

	bool afectedTrack[8];

	uint8_t* slotToFx;
	uint8_t* slotToRow;
	int16_t* slotRowToValue;
	void (*sourceDataSaveFunct)(void);

	uint8_t tracksPerformanceState[8] = {0}; // afektowane tracki

	int16_t slotsTempValues[12] = {0,0,0,0,0,0,0,0,0,0,0,0}; // tymczasowe zmieniane bez reca
	uint8_t slotsState[12] = {0,0,0,0,0,0,0,0,0,0,0,0}; // czy slot jest aktywny (pad wcisniety)

	// do odpytywania przez interfejs w celu odwieznia na ekranie
	uint8_t slotChangeFlag[12] = {0,0,0,0,0,0,0,0,0,0,0,0};;
};

extern cPerformEngine performance;

extern const uint8_t performanceDataSourceCount;
extern strPerformDataSource performanceDataSource[];



#endif /* INTERFACE_PERFORMANCEMODE_PERFORMANCEENGINE_H_ */
