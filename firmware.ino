//#include <Adafruit_ADS1015.h>
#include "stdafx.h"
#include "Scanner.h"
#include "RTx.h"
#include "DAC_AD5696.h"
#include "ADDAC.h"


// commands
//#define ADCCOMMANDS
//#define DEBUG

/* Pin Definitions */

//Pins for DAC (tlc5620)
#define LDAC 7 //ldac HIGH to stop DAC output while clocking.

//Imaging parameters
#define STEPSIZE 1      //Stepsize of DAC(linelength/stepsize=number of pixel per line)
#define LINE_LENGTH 10 //Voltage range over (MAX256). Defines size of image in real space
#define SAMPLE_SIZE 5   //Number of samples taken at each pixel. Median is taken as true value

//Communication parameters
#define BAUDRATE 115200   //Serial interfaces communication speed (bps)

bool reply = false;

// commands
const String InvalidCommandString = " - Invalid command syntax!";
const String SXP = "SXP";   // set startingXPosition
const String SYP = "SYP";   // set startingXPosition
const String SZP = "SZP";   // set startingXPosition
const String SXPq = "SXP?";   // startingXPosition
const String SYPq = "SYP?";   // startingXPosition
const String SZPq = "SZP?";   // startingXPosition
const String LL = "LL";		// line length
const String LLq = "LLq";

/*!
	Check <commandLine> for something of the form "<name> <number>"
	If it is found, extract the <numer> and put it in <param>, and return true
*/
bool CheckSingleParameter(String commandLine, String name, int &param, bool &ok, String errorMessage)
{
#ifdef DEBUG
	Serial.println(commandLine);
	Serial.println(name);
	Serial.println(commandLine.indexOf(name));
#endif
	if (commandLine.indexOf(name) == 0)
	{
#ifdef DEBUG
		Serial.println("Found....");
#endif
		// setting
		ok = false;
		String part;
		int val = 0;
		while (1)
		{
			int pos = commandLine.indexOf(' ', pos);
#ifdef DEBUG
			Serial.println(pos);
#endif
			if (pos == -1) break;
			part = commandLine.substring(pos + 1);

#ifdef DEBUG
			Serial.println(part);
#endif
			val = part.toInt();
			//if (val < 0) break;

			ok = true;
			break;
		}

		if (ok)
		{
			param = val;
#ifdef DEBUG
			Serial.println("OK");
			Serial.println(param);
#endif
		}
		else
		{
			if (reply) Serial.println(errorMessage);
		}

		return true;
	}
	return false;
}


/* Setup */
Adafruit_ADS1015 sig_adc(0x49);   // adc with raw signal input (A, B, C and D)
Adafruit_ADS1015 diff_adc(0x48);   // adc with the sum and difference signals
RTx* phone = new RTx();


DAC_AD5696* vc_dac = new DAC_AD5696();   // voice coil DAC
DAC_AD5696* pz_dac = new DAC_AD5696();   // Piezo DAC
//DAC_AD5696* vcdac = new DAC_AD5696();
PiezoDACController* ctrl = new PiezoDACController(pz_dac, STEPSIZE, LINE_LENGTH, LDAC);
SignalSampler* sampler = new SignalSampler(diff_adc, SAMPLE_SIZE);
Scanner* scanner = new Scanner(*ctrl, *sampler, *phone, LINE_LENGTH);

//This function runs once, when the arduino starts
void setup() {
	Serial.begin(BAUDRATE);
	//Serial.print("Initialising I2C...");
	unsigned char i2csetup = ADDAC::Setup(LDAC);
	//Serial.println(i2csetup == 1 ? "success!" : "failed!");

	vc_dac->Init(10, 1, 1);
	pz_dac->Init(16, 0, 0);

	// turn internal reference off
	vc_dac->InternalVoltageReference(AD569X_INT_REF_OFF);
	pz_dac->InternalVoltageReference(AD569X_INT_REF_OFF);

	// start ADCs
	diff_adc.begin();

	// initialise controller
	ctrl->Init();
}

extern String const PARAM_LINE_LENGTH;

//This function keeps looping
void loop()
{
	//delay(20);
	//////pz_dac->SetOutput(15, 0xFFFF);
	//ctrl->SetDACOutput(15, 0x0000);
	//delay(20);
	//ctrl->SetDACOutput(15, 0xFFFF);

	//return;

	//// listen for command
	//String cmd = Serial.readStringUntil(';');
	//if (phone->echo)
	//{
	//	Serial.print(cmd);
	//	Serial.print(';');
	//}

	String cmd = phone->listen();

	//delay(1);
	int idx;
	bool boolean;
	uint16_t uint16;

	//idx = cmd.indexOf("VCDAC::REFSET");
	//Serial.println(idx);

	 /*
	  * Get command over serial
	  * VCDAC::SET <x> <y>
	  *   Set the channel <x> voltage of the piezo DAC to <y>
	  */
	if (cmd == F("GO"))
	{
		scanner->start();
	}
	else if (cmd == F("SETUP"))
	{
		delete ctrl;
		delete sampler;
		delete scanner;

		int CUSTOM_STEPSIZE = Serial.parseInt();
		int CUSTOM_LINE_LENGTH = Serial.parseInt();
		int CUSTOM_SAMPLE_SIZE = Serial.parseInt();

		ctrl = new PiezoDACController(pz_dac, CUSTOM_STEPSIZE, CUSTOM_LINE_LENGTH, LDAC);
		sampler = new SignalSampler(sig_adc, CUSTOM_SAMPLE_SIZE);
		scanner = new Scanner(*ctrl, *sampler, *phone, CUSTOM_LINE_LENGTH);

		// initialise the controller
		ctrl->Init();

		if (reply)
		{
			Serial.print(F("Setup with STEP_SIZE= "));
			Serial.print(CUSTOM_STEPSIZE);
			Serial.print(F(", CUSTOM_LINE_LENGTH= "));
			Serial.print(CUSTOM_LINE_LENGTH);
			Serial.print(" CUSTOM_SAMPLE_SIZE= ");
			Serial.println(CUSTOM_SAMPLE_SIZE);
		}

	}
	else if (cmd == "STREAM")
	{
		scanner->stream();
	}

	else if (cmd == SXPq)   // startXPlus (X starting point of scan)
	{
		Serial.println(ctrl->startingXPlus);
	}
	else if (CheckSingleParameter(cmd, SXP, idx, boolean, SXP + InvalidCommandString))
	{
		if (reply)
		{
			Serial.print("Setting startingXPlus to ");
			Serial.println(idx);
		}
	}

#define CTRL_
#ifdef CTRL_


	else if (CheckSingleParameter(cmd, F("CTRL::MOVEX"), idx, boolean, F("CTRL::MOVEX y y=int")))  // move steps in the X direction
	{
		if (reply)
		{
			Serial.print(F("MOVEX="));
			Serial.println(idx);
		}
		ctrl->move(X, idx, true);
	}
	else if (CheckSingleParameter(cmd, F("CTRL::MOVEY"), idx, boolean, F("CTRL::MOVEY y y=int")))  // move steps in the X direction
	{
		if (reply)
		{
			Serial.print(F("MOVEY="));
			Serial.println(idx);
		}
		ctrl->move(Y, idx, true);
	}
	else if (CheckSingleParameter(cmd, F("CTRL::MOVEZ"), idx, boolean, F("CTRL::MOVEZ y y=int")))  // move steps in the X direction
	{
		if (reply)
		{
			Serial.print(F("MOVEZ="));
			Serial.println(idx);
		}
		ctrl->move(Z, idx, true);
	}
	//else if (CheckSingleParameter(cmd, F("CTRL::MOVE"), idx, boolean, F("CTRL::MOVEx y - x=X,Y,Z y=int")))  // move steps in the _ direction
	//{
	//	PIEZO_AXIS axis = X;
	//	if (reply)
	//	{
	//		// what channel
	//		if (cmd[10] == 'X') axis = X;
	//		if (cmd[10] == 'Y') axis = Y;
	//		if (cmd[10] == 'Z') axis = Z;
	//		Serial.print(F("MOVE"));
	//		Serial.print(axis);
	//		Serial.print(F("="));
	//		Serial.println(idx);
	//	}
	//	ctrl->move(axis, idx, true);
	//}

	////////////////
	//// LINE LENGTH
	////////////////
	else if (cmd == LLq)
	{
		//Serial.print("LineLength is ");
		Serial.println(ctrl->getLineSize());
	}
	else if (CheckSingleParameter(cmd, LL, idx, boolean, LL + InvalidCommandString))
	{
		if (reply)
		{
			Serial.print("LL=");
			Serial.println(idx);
		}
		ctrl->setLineSize(idx);
	}

	//////////////
	// STEP SIZE
	//////////////
	else if (cmd == F("STEPSIZE?"))  // get step size
	{
		//Serial.print("LineLength is ");
		Serial.println(ctrl->getStepSize());
	}
	else if (CheckSingleParameter(cmd, F("STEPSIZE"), idx, boolean, F("STEPSIZE - Invalid command syntax!")))
	{
		if (reply)
		{
			Serial.print("StS=");
			Serial.println(idx);
		}
		ctrl->setStepSize(idx);
	}
#endif

#define SaS_
#ifdef SaS_
	//////////////
	// SAMPLE SIZE
	//////////////
	else if (cmd == F("SAMPLESIZE?"))   // get sample size
	{
		//Serial.print("LineLength is ");
		Serial.println(sampler->getSampleSize());
	}
	else if (CheckSingleParameter(cmd, F("SAMPLESIZE"), idx, boolean, F("SAMPLESIZE - Invalid command syntax!")))   // set the sample size
	{
		if (reply)
		{
			Serial.print("SaS=");
			Serial.println(idx);
		}
		sampler->setSampleSize(idx);
	}
#endif

	else if (cmd == F("ERROR"))
	{

	}


	else if (cmd == F("PING"))
	{
		Serial.println(F("PONG"));
	}
	else if (CheckSingleParameter(cmd, F("REPLY"), idx, boolean, F("REPLY - Invalid syntax!")))   // reply?
	{
		reply = idx == 1;
		if (reply)
		{
			Serial.print("RPL=");
			Serial.println(reply ? 1 : 0);
		}
	}
	else if (CheckSingleParameter(cmd, "ECHO", idx, boolean, "ECHO!"))  // echo back?
	{
		phone->echo = idx == 1;
		if (reply)
		{
			Serial.print("ECHO=");
			Serial.println(phone->echo ? 1 : 0);
		}
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// ADC COMMANDS
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#define ADCCOMMANDS
#ifdef ADCCOMMANDS

	else if (CheckSingleParameter(cmd, "ADCDIFF::GET", idx, boolean, "ADCDIFF::GET - Invalid command syntax!"))
	{
		uint16 = diff_adc.readADC_SingleEnded(idx - 1);
		if (reply)
		{
			Serial.print("Channel ");
			Serial.print(idx - 1);
			Serial.print(" is ");
			Serial.println(idx);
		}
		Serial.println(uint16);
	}
	else if (CheckSingleParameter(cmd, "ADCSIG::GET", idx, boolean, "ADCSIG::GET - Invalid command syntax!"))
	{
		uint16 = diff_adc.readADC_SingleEnded(idx - 1);
		if (reply)
		{
			Serial.print("Channel ");
			Serial.print(idx - 1);
			Serial.print(" is ");
			Serial.println(idx);
		}
		Serial.println(uint16);
	}

#endif

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/// DAC COMMANDS
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define DACCOMMANDS
#ifdef DACCOMMANDS

	else if (cmd == F("CTRL::INIT"))
	{
		ctrl->Init();
	}
	else if (cmd.indexOf(F("GOTO")) == 0)
	{
		int x = Serial.parseInt();
		int y = Serial.parseInt();
		int z = Serial.parseInt();

		ctrl->GotoCoordinates(x, y, z);

		if (reply)
		{
			Serial.print(F("Goint to ("));
			Serial.print(x);
			Serial.print(",");
			Serial.print(y);
			Serial.print(",");
			Serial.print(z);
			Serial.println(")");
		}
	}
	else if (CheckSingleParameter(cmd, "POS?", idx, boolean, "POS?!"))  // set the internal reference state
	{
		Serial.println(idx);
		if (idx >= 1 && idx <= 3 && boolean)
		{
			if (reply)
			{
				Serial.print(idx);
				Serial.print("=");
			}

			int pos = ctrl->GetPosition((PIEZO_AXIS)(idx - 1));
			Serial.println(pos);

			if (reply)
			{
				Serial.print(idx);
				Serial.print("=");
				Serial.println(pos);
			}
		}
	}



	else if (cmd == "DAC::PRINT")  /// print dac details
	{
		Serial.print("Max value (u) = ");
		Serial.println(vc_dac->getMaxValueU());
		Serial.print("Max value (f) = ");
		Serial.println(vc_dac->getMaxValueF());
		Serial.print("Bits = ");
		Serial.println(vc_dac->getBits());
	}
	else if (cmd == "VCDAC:RST")
	{
		vc_dac->Reset(AD569X_RST_MIDSCALE);
		if (reply) Serial.println("Resetting Piezo DAC");
	}
	else if (CheckSingleParameter(cmd, "VCDAC::RFST", idx, boolean, "VCDAC::RFST!"))  // set the internal reference state
	{
		if (reply)
		{
			Serial.print("IntRef=");
			Serial.println(idx);
		}
		vc_dac->InternalVoltageReference(idx == 0 ? AD569X_INT_REF_OFF : AD569X_INT_REF_ON);
	}

	else
	{
		//////////////////////////////////////////////////////
		// SET DAC VOLTAGE (FOR DEBUG)
		//////////////////////////////////////////////////////

		if (idx = cmd.indexOf("DAC::SET") == 2)   // will be either VCDAC::SET...  or PZDAC::SET...
		{
			//Serial.println("DACC!!!");
			bool ok = false;
			String channelPart;
			String valuePart;
			int channel;
			float value;
			boolean = cmd[0] == 'V';  // if cmd[0] is V, it must be VCDAC...
			while (1)
			{
				String *parts;
				//int num = splitString(cmd, ' ', parts);
				//Serial.println("There were " + String(num) + " parts");
				// extract channel
				int pos = cmd.indexOf(' ', pos);
				int pos2 = cmd.indexOf(' ', pos + 1);
				if (pos == -1 || pos2 == -1) break;
				channelPart = cmd.substring(pos, pos2);
				channel = channelPart.toInt();

				// check range
				if (channel < 0 || channel > 15)
				{
					if (reply) Serial.println("Channel number must be a bit mask of 4 bits (0 to 15)");
					break;
				}

				// extract value
				pos = pos2 + 1;
				valuePart = cmd.substring(pos);
				value = valuePart.toFloat();

				// check range
				if (value < 0.0f || value > 5.0f)
				{
					if (reply) Serial.println("Channel value must be between 0.0 and 5.0");
					break;
				}

				ok = true;
				break;
			}

			if (ok)
			{
				if (reply)
				{
					Serial.print("Setting channel mask ");
					Serial.print(channel);
					Serial.print(" of ");
					Serial.print(boolean ? "VCDAC" : "PZDAC");
					Serial.print(" to ");
					Serial.println(value);
				}

				//long rand = random(0, dacMax);
				//float rnd = 5.0F;
				//rnd /= dacMax;
				//rnd *= rand;

				// which dac?
				if (boolean)
				{
					vc_dac->SetVoltage(channel, value, 5.0f);
				} else 
				{
					pz_dac->SetVoltage(channel, value, 5.0f);
				}
				//dac->SetOutput(1U << (channel - 1), 



			}
			else {
				if (reply) Serial.println("Invalid command syntax!");
			}

		}

		////////////////////////////////////////////
		// READ THE DAC VOLTAGE
		////////////////////////////////////////////
		//else if (idx = cmd.indexOf("VCDAC?") == 0)  // get the dac voltage
		//{
		//	//Serial.println("DACC!!!");
		//	bool ok = false;
		//	String channelPart;
		//	int channel;
		//	float value;
		//	while (1)
		//	{
		//		String *parts;
		//		//int num = splitString(cmd, ' ', parts);
		//		//Serial.println("There were " + String(num) + " parts");
		//		// extract channel
		//		int pos = cmd.indexOf(' ', pos);
		//		if (pos == -1) break;
		//		channelPart = cmd.substring(pos + 1);
		//		channel = channelPart.toInt();

		//		// check range
		//		if (channel < 0 || channel > 15)
		//		{
		//			if (reply) Serial.println("Channel number must be a bit mask of 4 bits (0 to 15)");
		//			break;
		//		}

		//		ok = true;
		//		break;
		//	}

		//	if (ok)
		//	{

		//		//long rand = random(0, dacMax);
		//		//float rnd = 5.0F;
		//		//rnd /= dacMax;
		//		//rnd *= rand;
		//		unsigned short val = vc_dac->ReadBack((unsigned char)channel);
		//		if (reply)
		//		{
		//			Serial.print("Channel mask ");
		//			Serial.print(channel);
		//			Serial.print(" is set to ");
		//			Serial.println(val);
		//		}



		//	}
		//	else {
		//		if (reply) Serial.println("VCDAC::GET - Invalid command syntax!");
		//	}

		//}

	}
#endif

}

