
#include <display.h>


//#include "mtStructs.h"
#include "interfaceDefs.h"

#include "graphicProcessing.h"
#include "mtAudioEngine.h"
cGraphicProcessing GP;



void cGraphicProcessing::spectrumResetZoom(uint16_t position, uint32_t sampleLength, strZoomParams* zoom)
{
	zoom->zoomPosition = position;

	zoom->zoomResolution = sampleLength / 600;

	uint16_t min_resolution = sampleLength/MAX_16BIT + 1;
	if(zoom->zoomResolution < min_resolution) zoom->zoomResolution  = min_resolution;

	zoom->zoomValue = 1.0;
}

void cGraphicProcessing::spectrumAutoZoomIn(uint16_t position, uint16_t zoomWidth, uint32_t sampleLength, strZoomParams* zoom)
{
	zoom->zoomPosition = position;

	zoom->zoomResolution = ((((float) zoomWidth)/MAX_16BIT) * sampleLength) / 600;

	uint16_t min_resolution = sampleLength/MAX_16BIT + 1;
	if(zoom->zoomResolution < min_resolution) zoom->zoomResolution  = min_resolution;

	zoom->zoomValue = sampleLength/((zoom->zoomResolution)*600.0);
}

void cGraphicProcessing::spectrumChangeZoom(int16_t value, uint32_t sampleLength, strZoomParams* zoom)
{
	if(sampleLength < 600)
	{
		zoom->zoomResolution = 1;
		zoom->zoomValue = 1.0;
		return;
	}

	uint16_t max_resolution = sampleLength / 600;
	uint16_t min_resolution = sampleLength / MAX_16BIT + 1;

	uint16_t step = (max_resolution-min_resolution)/ 100;
	if(step == 0) step = 1;


	if(zoom->zoomResolution - (value*step) < min_resolution) zoom->zoomResolution  = min_resolution;
	else if(zoom->zoomResolution - (value*step) > max_resolution ) zoom->zoomResolution = max_resolution;
	else zoom->zoomResolution -= value*step;

	if(zoom->zoomResolution == max_resolution) zoom->zoomValue = 1.0;
	else zoom->zoomValue = sampleLength/((zoom->zoomResolution)*600.0);

}


void cGraphicProcessing::processSpectrum(strSpectrumParams* params, strZoomParams* zoom, strTrackerSpectrum* spectrum)
{
	if(params->length == 0)
	{
		for(uint16_t i = 0; i < 600; i++)
		{
			spectrum->upperData[i] = 0;
			spectrum->lowerData[i] = 0;
		}

		zoom->zoomWidth = MAX_16BIT;
		zoom->zoomStart = 0;
		zoom->zoomEnd = MAX_16BIT;
		spectrum->spectrumType = 1;

		return;
	}

	if(params->recordInProgressFlag)
	{
		int16_t * sampleData = params->address;
		uint32_t resolution = params->length / 600;

		if(resolution < 1) resolution = 1;

		uint16_t offset_pixel = 0;

		zoom->zoomWidth = MAX_16BIT;
		zoom->zoomStart = 0;
		zoom->zoomEnd = MAX_16BIT;

		int16_t up = 0;
		int16_t low = 0;
		uint32_t step = 0;

		uint16_t max_resolution =  (resolution < 200) ? resolution : 200; 	// ogranicza liczbe przetwarzanych probek
																			// oszukujac troche na wygladzie


		for(uint16_t i = offset_pixel; i < 600; i++)
		{
			low = up = 0; //*(sampleData+step);

			for(uint16_t j = 0; j < max_resolution; j++)
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



		if(params->length/zoom->zoomValue < 3600) spectrum->spectrumType = 1;
		else spectrum->spectrumType = 0;

		return;
	}

	processSpectrum(params->address, params->length, zoom, spectrum);

}


void cGraphicProcessing::processSpectrum(strInstrument* instrument, strZoomParams* zoom, strTrackerSpectrum* spectrum, uint32_t wt_pos)
{
	// uwaga tu wazna kolejnosc + do sprawdzenia
	if(mtProject.values.lastUsedInstrument < 0 || mtProject.instrument[mtProject.values.lastUsedInstrument].isActive == 0 )
	{
		for(uint16_t i = 0; i < 600; i++)
		{
			spectrum->upperData[i] = 0;
			spectrum->lowerData[i] = 0;
		}


		spectrum->spectrumType = 1;

		return;
	}

	int16_t * sampleData;

	if(instrument->sample.type == mtSampleTypeWavetable)
	{
		zoom->zoomWidth = MAX_16BIT;
		zoom->zoomStart = 0;
		zoom->zoomValue = 1;
		zoom->zoomEnd = MAX_16BIT;
		uint16_t windowSize = instrument->sample.wavetable_window_size; //instrument->sample.wavetable_window_size;

		sampleData = instrument->sample.address
				+ (  wt_pos * windowSize);

		if(sampleData > (instrument->sample.address + instrument->sample.length) ) return;

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

	processSpectrum(instrument->sample.address, instrument->sample.length, zoom, spectrum);
}


void cGraphicProcessing::processSpectrum(int16_t* address, uint32_t length, strZoomParams* zoom, strTrackerSpectrum* spectrum)
{

	uint16_t offset_pixel;
	int16_t * sampleData;
	uint32_t resolution;


	if(zoom->zoomValue > 1.0 && length > 600)
	{
		zoom->zoomWidth = (MAX_16BIT/zoom->zoomValue);

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


		uint32_t offset = ((float)zoom->zoomPosition/MAX_16BIT) * length;

		sampleData = address + offset;

		//resolution = (((float)zoomWidth/MAX_16BIT) * editorInstrument->sample.length ) / 600;

		resolution = zoom->zoomResolution;

	}
	else
	{

		offset_pixel = 0;
		zoom->zoomWidth = MAX_16BIT;
		zoom->zoomStart = 0;
		zoom->zoomEnd = MAX_16BIT;
		sampleData = address;
		resolution = length / 600;
	}



	int16_t up = 0;
	int16_t low = 0;


	// kiedy dlugosc sampla mniejsza niz 600 probek
	if(resolution < 1) //resolution = 1;
	{
		uint16_t sample = 0;
		uint16_t px_per_sample = 600/length;

		for(uint16_t i = 0; i < 600; i++)
		{
			if(px_per_sample > 0)
			{
				px_per_sample--;
			}
			else
			{
				sample++;
				px_per_sample = ((600-i)/(length-sample));
			}

			spectrum->lowerData[i] = spectrum->upperData[i] = (*(sampleData+sample))/300;
		}

		spectrum->spectrumType = 1;
		return;
	}


	uint32_t step = 0;
	uint16_t max_resolution =  (resolution < 200) ? resolution : 200; 	// ogranicza liczbe przetwarzanych probek
																		// oszukujac troche na wygladzie
	if(offset_pixel > 0)
	{
		for(int16_t i = offset_pixel-1; i >= 0; i--)
		{
			low = up = 0; //*(sampleData+step);

			for(uint16_t j = 0; j < max_resolution; j++)
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



		for(uint16_t j = 0; j < max_resolution; j++)
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

	if(length/zoom->zoomValue < 3600) spectrum->spectrumType = 1;
	//if(length*zoom->zoomValue > 131000) spectrum->spectrumType = 1;
	else spectrum->spectrumType = 0;


}
