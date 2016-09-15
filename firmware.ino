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

bool reply = true;

// commands

/*!
 * 
	Check <commandLine> for something of the form "<name> <number>"
	If it is found, extract the <numer> and put it in <param>, and return true
*/
bool CheckSingleParameter(String commandLine, String name, int &param, bool &ok, String errorMessage)
{

	if (commandLine.indexOf(name) == 0)
	{

		// setting
		ok = false;
		String part;
		int val = 0;
		while (1)
		{
			int pos = commandLine.indexOf(' ', pos);

			if (pos == -1) break;
			part = commandLine.substring(pos + 1);
			val = part.toInt();
     
			//if (val < 0) break;

			ok = true;
			break;
		}

		if (ok)
		{
			param = val;
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
Adafruit_ADS1015 *sig_adc = new Adafruit_ADS1015 (0x49);   // adc with raw signal input (A, B, C and D)
Adafruit_ADS1015 *diff_adc = new Adafruit_ADS1015(0x48);   // adc with the sum and difference signals
RTx* phone = new RTx();


DAC_AD5696* vc_dac = new DAC_AD5696();   // voice coil DAC
DAC_AD5696* pz_dac = new DAC_AD5696();   // Piezo DAC
//DAC_AD5696* vcdac = new fDAC_AD5696();
PiezoDACController* ctrl;
SignalSampler* sampler;
Scanner* scanner;

//This function runs once, when the arduino starts
void setup() {
  
	Serial.begin(BAUDRATE);
	ctrl = new PiezoDACController(pz_dac, STEPSIZE, LINE_LENGTH, LDAC);
	sampler = new SignalSampler(sig_adc, diff_adc, SAMPLE_SIZE);
	scanner = new Scanner(ctrl, sampler, phone, LINE_LENGTH);

	unsigned char i2csetup = ADDAC::Setup(LDAC);
	Serial.println(i2csetup == 1 ? "success!" : "failed!");

	vc_dac->Init(10, 1, 1);
	pz_dac->Init(16, 0, 0);

	// turn internal reference off
	vc_dac->InternalVoltageReference(AD569X_INT_REF_OFF);
	pz_dac->InternalVoltageReference(AD569X_INT_REF_OFF);

	// start ADCs
	diff_adc->begin();
	sig_adc->begin();

	// initialise controller
	ctrl->Init();
}

extern String const PARAM_LINE_LENGTH;

//This function keeps looping
void loop()
{

	String cmd = phone->listen();

	int idx;
	bool bl;
	uint16_t uint16;

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

		int CUSTOM_STEPSIZE = Serial.parseInt();
		int CUSTOM_LINE_LENGTH = Serial.parseInt();
		int CUSTOM_SAMPLE_SIZE = Serial.parseInt();

		ctrl->setLineSize(CUSTOM_LINE_LENGTH);
		ctrl->setStepSize(CUSTOM_STEPSIZE);
		sampler->setSampleSize(CUSTOM_SAMPLE_SIZE);
	  

		if (reply)
		{
			Serial.print(F("Setup with STEP_SIZE= "));
			Serial.print(CUSTOM_STEPSIZE);
			Serial.print(F(", CUSTOM_LINE_LENGTH= "));
			Serial.print(CUSTOM_LINE_LENGTH);
			Serial.print(" CUSTOM_SAMPLE_SIZE= ");
			Serial.println(CUSTOM_SAMPLE_SIZE);
             Serial.write(';');
		}

	}
	else if (cmd == "STREAM")
	{
		scanner->stream();
	}

	else if (cmd == F("STARTXPLUS?"))   // startXPlus (X starting point of scan)
	{
		Serial.println(ctrl->startingXPlus);
           Serial.write(';');
	}
	else if (CheckSingleParameter(cmd, F("STARTXPLUS"), idx, bl, F("STARTXPLUS error")))
	{
		if (reply)
		{
			Serial.print("Setting startingXPlus to ");
			Serial.println(idx);
             Serial.write(';');
		}
	}

#define CTRL_
#ifdef CTRL_


	else if (CheckSingleParameter(cmd, F("CTRL::MOVEX"), idx, bl, F("CTRL::MOVEX y y=int")))  // move steps in the X direction
	{
		if (reply)
		{
			Serial.print(F("MOVEX="));
			Serial.println(idx);
             Serial.write(';');
		}
		ctrl->move(X, idx, true);
	}
	else if (CheckSingleParameter(cmd, F("CTRL::MOVEY"), idx, bl, F("CTRL::MOVEY y y=int")))  // move steps in the X direction
	{
		if (reply)
		{
			Serial.print(F("MOVEY="));
			Serial.println(idx);
             Serial.write(';');
		}
		ctrl->move(Y, idx, true);
	}
	else if (CheckSingleParameter(cmd, F("CTRL::MOVEZ"), idx, bl, F("CTRL::MOVEZ y y=int")))  // move steps in the X direction
	{
		if (reply)
		{
			Serial.print(F("MOVEZ="));
			Serial.println(idx);
             Serial.write(';');
		}
		ctrl->move(Z, idx, true);
	}

	////////////////
	//// LINE LENGTH
	////////////////
  
	else if (cmd == F("LINELENGTH?"))
	{ 
	  Serial.print(F("LINELENGTH:"));
		Serial.println(ctrl->getLineSize());
           Serial.write(';');
    
	}
	else if (CheckSingleParameter(cmd, F("LINELENGTH"), idx, bl, F("LINELENGTH y - where y is integer to set.")))
	{
		if (reply)
		{
			Serial.print(F("LINELENGTH SET="));
			Serial.println(idx);
             Serial.write(';');
		}
		ctrl->setLineSize(idx);
	}

	//////////////
	// STEP SIZE
	//////////////
	else if (cmd == F("STEPSIZE?"))  // get step size
	{
		Serial.print(F("STEPSIZE:"));
		Serial.println(ctrl->getStepSize());
           Serial.write(';');
	}
	else if (CheckSingleParameter(cmd, F("STEPSIZE"), idx, bl, F("STEPSIZE - Invalid command syntax!")))
	{
		if (reply)
		{
			Serial.print("StS=");
			Serial.println(idx);
		}
		ctrl->setStepSize(idx);
	}


	// scan delay
	else if (cmd == F("SCANDELAY?"))  // get the scan delay in micros
	{
		//Serial.print("LineLength is ");
		Serial.println(scanner->getScanDelay());
	}
	else if (CheckSingleParameter(cmd, F("SCANDELAY"), idx, bl, F("SCANDELAY - Invalid command syntax!")))  // set scan delay in micros
	{
		if (idx >= 0)
		{
			scanner->setScanDelay(idx);
			if (reply)
			{
				Serial.print(F("SCANDELAY="));
				Serial.println(scanner->getScanDelay());
			}
		}
		else {
			if (reply)
			{
				Serial.println(F("SCANDELAY cannot be negative."));
			}
		}
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
		//Serial.println(sampler->sampleSize);
		//Serial.println(gSAMPLESIZE);
	}
	else if (CheckSingleParameter(cmd, F("SAMPLESIZE"), idx, bl, F("SAMPLESIZE - Invalid command syntax!")))   // set the sample size
	{
		sampler->setSampleSize(idx);
		//sampler->sampleSize = idx;
		//gSAMPLESIZE = idx;
		if (reply)
		{
			Serial.print("SaS=");
			//Serial.println(sampler->sampleSize);
			Serial.println(sampler->getSampleSize());
			//Serial.println(gSAMPLESIZE);
		}
	}
#endif

	else if (cmd == F("ERROR"))
	{

	}


	else if (cmd == F("PING"))
	{
		Serial.println(F("PONG"));
	}
	else if (CheckSingleParameter(cmd, F("REPLY"), idx, bl, F("REPLY - Invalid syntax!")))   // reply?
	{
		reply = idx == 1;
		if (reply)
		{
			Serial.print("RPL=");
			Serial.println(reply ? 1 : 0);
		}
	}
	else if (CheckSingleParameter(cmd, "ECHO", idx, bl, "ECHO!"))  // echo back?
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
#define ADCCOMMANDS
#ifdef ADCCOMMANDS

	else if (cmd == F("SIG?"))
	{
		sampler->readChannels();
		if (reply) Serial.print(F("A-B="));
		Serial.println(sampler->getAMinusB());
		if (reply) Serial.print(F("C-D="));
		Serial.println(sampler->getCMinusD());
		if (reply) Serial.print(F("SUM="));
		Serial.println(sampler->getSum());
		if (reply) Serial.print(F("A="));
		Serial.println(sampler->getA());
		if (reply) Serial.print(F("B="));
		Serial.println(sampler->getB());
		if (reply) Serial.print(F("C="));
		Serial.println(sampler->getC());
		if (reply) Serial.print(F("D="));
		Serial.println(sampler->getD());
	}

	// read single channel from diff
	else if (CheckSingleParameter(cmd, F("DIFFADC::GET"), idx, bl, "DIFFADC::GET - Invalid command syntax!"))
	{
		if (idx >=1 && idx <= 3)
		{
			uint16 = diff_adc->readADC_SingleEnded(idx - 1);
			if (reply)
			{
				Serial.print("Channel ");
				Serial.print(idx - 1);
				Serial.print(" is ");
				Serial.println(idx);
			}
			Serial.println(uint16);
		}
		else {
			if (reply) Serial.println(F("Channel number must be 0 - 3"));
		}
	}

	// read single channel from sig
	else if (CheckSingleParameter(cmd, F("SIGADC::GET"), idx, bl, F("SIGADC::GET y - y is int (channel)!")))
	{
		if (idx >=1 && idx <= 3)
		{
			uint16 = sig_adc->readADC_SingleEnded(idx - 1);
			if (reply)
			{
				Serial.print("Channel ");
				Serial.print(idx - 1);
				Serial.print(" is ");
				Serial.println(idx);
			}
			Serial.println(uint16);

		}
		else {
			if (reply) Serial.println(F("Channel number must be 0 - 3"));
		}
	}

	// set gain
	else if (CheckSingleParameter(cmd, F("DIFFADC::GAIN"), idx, bl, F("DIFFADC::GAIN - Invalid command syntax!")))
	{
		if (idx >= 0 && idx <= 5)
		{
			switch (idx)
			{
			case 0:
				diff_adc->setGain(GAIN_TWOTHIRDS);
				break;
			case 1:
				diff_adc->setGain(GAIN_ONE);
				break;
			case 2:
				diff_adc->setGain(GAIN_TWO);
				break;
			case 3:
				diff_adc->setGain(GAIN_FOUR);
				break;
			case 4:
				diff_adc->setGain(GAIN_EIGHT);
				break;
			case 5:
				diff_adc->setGain(GAIN_SIXTEEN);
				break;
			}
			if (reply)
			{
				Serial.print(F("Gain of DIFF adc = "));
				Serial.println(idx);
			}
		}
		else {
			if (reply)
			{
				Serial.print(F("Gain index "));
				Serial.print(idx);
				Serial.println(F(" must be 0 - 5"));
			}
		}
	}


	// set gain
	else if (CheckSingleParameter(cmd, F("SIGADC::GAIN"), idx, bl, F("SIGADC::GET - Invalid command syntax!")))
	{
		if (idx >= 0 && idx <= 5)
		{
			switch (idx)
			{
			case 0:
				sig_adc->setGain(GAIN_TWOTHIRDS);
				break;
			case 1:
				sig_adc->setGain(GAIN_ONE);
				break;
			case 2:
				sig_adc->setGain(GAIN_TWO);
				break;
			case 3:
				sig_adc->setGain(GAIN_FOUR);
				break;
			case 4:
				sig_adc->setGain(GAIN_EIGHT);
				break;
			case 5:
				sig_adc->setGain(GAIN_SIXTEEN);
				break;
			}
			if (reply)
			{
				Serial.print(F("Gain of SIG adc = "));
				Serial.println(idx);
			}
		}
		else {
			if (reply)
			{
				Serial.print(F("Gain index "));
				Serial.print(idx);
				Serial.println(F(" must be 0 - 5"));
			}
		}
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
	else if (CheckSingleParameter(cmd, "POS?", idx, bl, "POS?!"))  // set the internal reference state
	{
		Serial.println(idx);
		if (idx >= 1 && idx <= 3 && bl)
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
	else if (CheckSingleParameter(cmd, "VCDAC::RFST", idx, bl, "VCDAC::RFST!"))  // set the internal reference state
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
			bl = cmd[0] == 'V';  // if cmd[0] is V, it must be VCDAC...
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
					Serial.print(bl ? "VCDAC" : "PZDAC");
					Serial.print(" to ");
					Serial.println(value);
				}

				//long rand = random(0, dacMax);
				//float rnd = 5.0F;
				//rnd /= dacMax;
				//rnd *= rand;

				// which dac?
				if (bl)
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

