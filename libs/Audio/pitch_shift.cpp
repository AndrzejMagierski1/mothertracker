#include "pitch_shift.h"

void PitchShift::setDiv(double value)
{
	Div = value; // wartość pitcha w krotnościach częstotliwosci bazowej od 0.25 do 4
}
/*
void PitchShift::update(void)
{	
	volatile int16_t * wsk=NULL;

	if(DataRest)
	{
		while(((uint16_t)indicator)<AUDIO_BLOCK_SAMPLES)
		{	
			wsk=pin+(uint16_t  ) indicator;
			*pout=*wsk;
			pout++;
			indicator+=Div;
		}

		DataRest=0;
		indicator=0;
		release(in);
	}	


	if (!(in = receiveWritable(0))) return;
	if (!in->data) return;

	if(w)
	{
		out=allocate();
		if(!out) return;
		pout=out->data;
		w--;
	} 
	pin=in->data;
	
	while(((uint16_t)indicator)<AUDIO_BLOCK_SAMPLES)
	{

		wsk=pin+(uint16_t  )indicator;
		*pout=*wsk;
		pout++;
		indicator+=Div;

		if(pout == ( ((out->data)+AUDIO_BLOCK_SAMPLES)) || (((uint16_t)indicator)>=AUDIO_BLOCK_SAMPLES) )
		{
			if( (indicator/Div) <= (AUDIO_BLOCK_SAMPLES/Div) )
			{
				DataRest=1;
			} 
			else licznik++;

			break;	
		} 

	}

	if(!DataRest)
	{
		release(in);
		indicator=0;
	}

	if(pout==((out->data)+AUDIO_BLOCK_SAMPLES))
	{

		transmit(out,0);
		release(out);
		out=allocate();
		if(!out) return;
		pout=out->data;

	}
	

}

*/

void PitchShift::update(void)
{

	
	if( w ) 																						// wskaznik informujący o potrzebie zarezerwowania pamięci dla outa
	{
		out = allocate(); 																			// rezerwacja pamięci
		if( ! out ) return;																			// jeżeli wskażnik wskazuje NULL to znaczy, że nie przydzielono pamięci dla outa
		w=0; 
	} 

	if ( DataRest ) 																				// wskaznik informujący, że zostały dane wejściowe do wysłania w poprzednim cyklu
	{

		while( (  indicator  < AUDIO_BLOCK_SAMPLES )  && ( i < AUDIO_BLOCK_SAMPLES ) ) 				// pętla wykonuje się dopoki tablica wyjścia/wejścia zostanie zapełniona/odczytana
		{	
			out -> data[i]   = in -> data[ (uint16_t) indicator ]; 									// przypisanie danych wejściowych danym wyjściowym
			i++; 																					// indeksowanie danych wyjściowych
			indicator += Div; 																		// indeksowanie danych wejściowych

			if(  i == AUDIO_BLOCK_SAMPLES  )  
			{
				if ( indicator < AUDIO_BLOCK_SAMPLES ) 												// zinkrementowany indeks danych wejsciowych jest nadal mniejszy od rozmiaru bufora - to znaczy, że nadal na wejściu są dane do odczytania
				{
					stillRest=1;  																	//wskaznik informujący ze nadal są dane do odczytu
					onlyPitchUpdate=1; 																// wskaznik informujący, że funkcje znajdujące się "przed" pitch shifterem(zazwyczaj karta SD), nie powinny być odświeżane - dzieje się to globalnie w pętli głównej
					outMustSend=1; 																	// wskaznik informujący, że dane powinny zostać wysłane bez odbierania dodatkowego bloku
				}
				else stillRest=0; 																	// zerowanie wskaźnika
				break;
			}
			if(indicator >= AUDIO_BLOCK_SAMPLES)
			{
				stillRest=0; 																		// zerowanie wskaźnika
				break;
			} 
			
		}
		
		if(! stillRest) 																			// jeżeli nie ma już danych do oczytu
		{
			DataRest=0; 																			// zerowanie wskaznika - nie ma już danych na wejściu
			indicator -= AUDIO_BLOCK_SAMPLES; 														// ustalanie indeksu na początek bufora
			release( in ); 																			// zwolnienie pamięci bufora wejściowego
			in=NULL;
			onlyPitchUpdate=0; 																		// można odświeżać dane z przed pitch shifter'a
			outMustSend=0; 																			// wyzerowanie wskaźnika o konieczności pominięcia odbioru
		}
	}	

	if( ! outMustSend ) 																			// jeżei odebrane dane nie zapchają bufora - można odebrać
	{
		if (! ( in = receiveReadOnly( 0 ) ) ) return;  												// jeżeli nie ma bloku odbiorczego, przerwanie funkcji
		if ( ! in -> data ) return ; 																// jeżeli nie ma danych w bloku, przerwanie funkcji
	}

	while( (  indicator  < AUDIO_BLOCK_SAMPLES)   && ( i < AUDIO_BLOCK_SAMPLES ) ) 					// pętla wykonuje się dopoki tablica wyjścia/wejścia zostanie zapełniona/odczytana
	{

		out -> data[i]   = in -> data[ (uint16_t) indicator ]; 										// przypisanie danych wejściowych danym wyjściowym
		i++; 																						// indeksowanie danych wyjściowych
		indicator += Div; 																			// indeksowanie danych wejściowych

		if( (i == AUDIO_BLOCK_SAMPLES) )
		{

			if(  indicator < AUDIO_BLOCK_SAMPLES) 													// zinkrementowany indeks danych wejsciowych jest nadal mniejszy od rozmiaru bufora - to znaczy, że nadal na wejściu są dane do odczytania
			{
				DataRest=1; 																		// ustawienie wskaźnika - na wejściu są jeszcze dane do odczytu
			} 
			else DataRest=0;

			break;
		}

		if(indicator >= AUDIO_BLOCK_SAMPLES)
		{
			DataRest=0;
			break;
		}

	}

	if( ( ! DataRest ) && ( ! outMustSend ) ) 														// jeżeli nie ma już danych na wejściu
	{
		release( in ); 																				// zwolnienie bufora wejściowego
		in=NULL;
		indicator -= AUDIO_BLOCK_SAMPLES; 															// ustawienie indeksu bufora wejsciowego na początek
		onlyPitchUpdate=0; 																			// można odświeżać dane z przed pitch shifter'a
	}

	if(i == AUDIO_BLOCK_SAMPLES) 																	// jeżeli zinkrementowany indeks bufora wyjsciowego jest równy rozmiarowi bufora
	{

		transmit( out , 0 ); 																		// transmisja bloku wyjściowego na kanał 0
		release( out ); 																			// zwolnienie bloku wyjściowego po transmisji
		out=NULL;
		w=1; 																						// ustawienie wskaznika informującego, że można zarezerwować nowy bufor
		i = 0; 																						// ustawienie indeksu bufora wejściowego na początek
		outfull=1; 																					// ustawienie wskażnika informującego że bufor danych wyjściowycj jest pełen - zmienna używana zewnętrznie w pętli głównej
	}

}

