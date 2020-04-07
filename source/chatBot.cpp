#include "chatBot.h"
#include <Arduino.h>
#include "mtSequencer.h"

extern Sequencer sequencer;

void handle_chatBot(void)
{
	if (Serial.available() > 0)
	{
		String incomingString = "";
		incomingString = Serial.readString();
		incomingString.toLowerCase();

		if (incomingString.indexOf("siema") >= 0)
		{
			Serial.println("hej!");
		}

		else
		{

			Serial.println("O_o");
			Serial.println();
		}

	}
}

