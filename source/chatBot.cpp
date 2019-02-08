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

		if (incomingString.indexOf("debug") >= 0)
		{
			Serial.print("debug..");

			if (incomingString.indexOf("midi") >= 0)
			{

			}
		}
		else if (incomingString.indexOf("play") >= 0)
		{
			sequencer.play();
			Serial.println("OK");
		}
		else if (incomingString.indexOf("stop") >= 0)
		{
			sequencer.stop();
			Serial.println("OK");
		}
		else if (incomingString.indexOf("pause") >= 0)
		{
			sequencer.pause();
			Serial.println("OK");
		}
		else if (incomingString.indexOf("printnotes") >= 0)
		{
			sequencer.printNotes(1);
			Serial.println("OK");
		}

		else
		{
			// Serial.println("unknown message:");
			// Serial.println(incomingString);
			Serial.println("O_o");
			Serial.println();
		}

	}
}

