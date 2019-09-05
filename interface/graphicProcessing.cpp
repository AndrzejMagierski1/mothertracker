
#include <display.h>


//#include "mtStructs.h"
#include "interfaceDefs.h"

#include "graphicProcessing.h"


void processSpectrum(strInstrument* instrument, strZoomParams* zoom, strTrackerSpectrum* spectrum)
{

	//refreshSpectrum = 1;


	// uwaga tu wazna kolejnosc + do sprawdzenia
	if(mtProject.values.lastUsedInstrument < 0 || mtProject.instrument[mtProject.values.lastUsedInstrument].isActive == 0 )
	{
		for(uint16_t i = 0; i < 600; i++)
		{
			spectrum->upperData[i] = 0;
			spectrum->lowerData[i] = 0;
		}

		return;
	}

	uint16_t offset_pixel;
	int16_t * sampleData;

	if(instrument->sample.type == mtSampleTypeWavetable)
	{
		zoom->zoomWidth = MAX_16BIT;
		zoom->zoomStart = 0;
		zoom->zoomValue = 1;
		zoom->zoomEnd = MAX_16BIT;
		uint16_t windowSize = instrument->sample.wavetable_window_size;

		sampleData = instrument->sample.address
				+ (mtProject.instrument[mtProject.values.lastUsedInstrument].wavetableCurrentWindow * windowSize);

		float resolution = windowSize / 600.0;

		int16_t up = 0;
		int16_t low = 0;
		float step = 0;

		for(uint16_t i = 0; i < 600; i++)
		{
			low = up = 0; //*(sampleData+step);

			for(uint16_t j = 0; j < resolution; j++)
			{
				int16_t sample = *(sampleData+(uint32_t)step+j);

				if(sample > up)  up = sample;
				else if(sample < low) low = sample;

			}
			step+= resolution;

			up = up/300;
			low = low/300;

			spectrum->upperData[i] =  up;
			spectrum->lowerData[i] = low;
		}

		//if(resolution <= 1)
		spectrum->spectrumType = 1;
		//else spectrum.spectrumType = 0;


		return;
	}

	uint32_t resolution;


	switch(zoom->lastChangedPoint)
	{
		case 0: zoom->zoomPosition = instrument->startPoint; break; //MAX_16BIT/2; break;

		case 1:
			zoom->zoomPosition = instrument->startPoint;
		break;
		case 2:
			zoom->zoomPosition = instrument->endPoint;
		break;
		case 3:
			zoom->zoomPosition = instrument->loopPoint1;
		break;
		case 4:
			zoom->zoomPosition = instrument->loopPoint2;
		break;

		default: zoom->zoomPosition = instrument->startPoint; break; //MAX_16BIT/2; break;
	}




	if(zoom->zoomValue > 1.0)
	{
		zoom->zoomWidth = (MAX_16BIT/zoom->zoomValue);
/*
		if(zoomWidth < 600)
		{
			zoomWidth = 600;
			//zoomValue =
		}
*/

		zoom->zoomStart =  zoom->zoomPosition - zoom->zoomWidth/2;
		zoom->zoomEnd = zoom->zoomPosition + zoom->zoomWidth/2;

		if(zoom->zoomStart < 0)
		{
			zoom->zoomEnd = zoom->zoomWidth;
			zoom->zoomStart = 0;
			offset_pixel = ((zoom->zoomPosition-zoom->zoomStart) * 599) / zoom->zoomWidth;
		}
		else if(zoom->zoomEnd > MAX_16BIT)
		{
			zoom->zoomEnd = MAX_16BIT;
			zoom->zoomStart = MAX_16BIT-zoom->zoomWidth;
			offset_pixel = ((zoom->zoomPosition-zoom->zoomStart) * 599) / zoom->zoomWidth;
		}
		else
		{
			offset_pixel = ((zoom->zoomPosition-zoom->zoomStart) * 599) / zoom->zoomWidth;
		}


		uint32_t offset = ((float)zoom->zoomPosition/MAX_16BIT) * instrument->sample.length;

		sampleData = instrument->sample.address + offset;

		//resolution = (((float)zoomWidth/MAX_16BIT) * editorInstrument->sample.length ) / 600;

		resolution = zoom->zoomResolution;

	}
	else
	{

		offset_pixel = 0;
		zoom->zoomWidth = MAX_16BIT;
		zoom->zoomStart = 0;
		zoom->zoomEnd = MAX_16BIT;
		sampleData = instrument->sample.address;
		resolution = instrument->sample.length / 600;
	}


	if(resolution < 1) resolution = 1;


	int16_t up = 0;
	int16_t low = 0;

	uint32_t step = 0;



	if(offset_pixel > 0)
	{
		for(int16_t i = offset_pixel-1; i >= 0; i--)
		{
			low = up = 0; //*(sampleData+step);

			for(uint16_t j = 0; j < resolution; j++)
			{
				int16_t sample = *(sampleData-step+j);

				if(sample > up)  up = sample;
				else if(sample < low) low = sample;

			}
			step+= resolution;

			up = up/300;
			low = low/300;

			spectrum->upperData[i] =  up;
			spectrum->lowerData[i] = low;
		}
	}

	up = 0;
	low = 0;
	step = 0;


	for(uint16_t i = offset_pixel; i < 600; i++)
	{
		low = up = 0; //*(sampleData+step);

		for(uint16_t j = 0; j < resolution; j++)
		{
			int16_t sample = *(sampleData+step+j);


			if(sample > up)  up = sample;
			else if(sample < low) low = sample;

		}
		step+= resolution;

		up = up/300;
		low = low/300;

		spectrum->upperData[i] =  up;
		spectrum->lowerData[i] = low;
	}

	// dobrany doswiadczalnie warunek kiedy najlepiej zmienic tryb wyswietlania
	if(instrument->sample.length*zoom->zoomValue > 131000) spectrum->spectrumType = 1;
	else spectrum->spectrumType = 0;


}
