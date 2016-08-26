#define DEBUG

// Author A Michel
// Date 20 08 15
// Lego2Nano 2015

#include "Scanner.h"
#include "RTx.h"

// Constructor
Scanner::Scanner(PiezoDACController *controller, SignalSampler *sampler, const RTx *phone, const int lineLength)
{
	this->pixels = new int[lineLength * 2];
	this->controller = controller;
	this->startTime = 0;
	this->endTime = 0;
	this->scanning = false;
	this->lineLength = lineLength;
}

// Destructor
Scanner::~Scanner() {
	delete[] pixels;
}

// reset. Stop the scanning process and resets all parameters.
void Scanner::reset()
{
	stop();
	// TODO implement proper interruption of process.
	delay(5000);

	controller->reset();
	sampler->reset();
	startTime = 0;
	endTime = 0;
	linesScanned = 0;
}

// Scans one line and stores data in the pixel array.
// Does a trace and retrace. The retrace data will be in backward order.
int Scanner::scanLine() {
	unsigned int x = 0;

#ifdef DEBUG
	Serial.println(F("Scanner::scanLine()"));
	Serial.println(pixels == nullptr);
	Serial.println(controller == nullptr);
	Serial.println(sampler == nullptr);
#endif

	//trace
	for (int i = 0; i < controller->getLineSize(); i++) {
#ifdef DEBUG
		Serial.print(F("Detecting line forwards "));
		Serial.println(i);
#endif
		sampler->readChannels();

		// calculate and save FES
		pixels[x] = sampler->getAMinusB() - sampler->getCMinusD();
		x++;

#ifdef DEBUG
		Serial.print(F("Stepping forwards "));
		Serial.println(i);
#endif
		if (i + 1 < controller->getLineSize())
		{
			controller->increaseVoltage();
		}
	}

	//retrace
	for (int i = 0; i < controller->getLineSize(); i++) {
#ifdef DEBUG
		Serial.print(F("Detecting line backwards"));
		Serial.println(i);
#endif
		sampler->readChannels();

		// calculate and save FES
		pixels[x] = sampler->getAMinusB() - sampler->getCMinusD();
		x++;

#ifdef DEBUG
		Serial.print(F("Stepping backwards "));
		Serial.println(i);
#endif
		if(i + 1 < controller->getLineSize())
			controller->decreaseVoltage();
	}

	linesScanned++;

	return 0;
}

// Start the scanning process.
int Scanner::start() {

#ifdef DEBUG
	Serial.println(F("Scanner::start()"));
#endif

//#ifdef DEBUG
//	Serial.print(F("X_PLUS="));
//	Serial.println(controller->getCurrentXPlus());
//	Serial.print(F("X_MINUS="));
//	Serial.println(controller->getCurrentXMinus());
//	Serial.print(F("Y_PLUS="));
//	Serial.println(controller->getCurrentYPlus());
//	Serial.print(F("Y_MINUS="));
//	Serial.println(controller->getCurrentYMinus());
//#endif

	// get start time
	startTime = millis();
	scanning = true;

	// interates over y-axis calling ctrl.nextLine() 
	for (int i = 0; i < lineLength; i++) {

#ifdef DEBUG
		Serial.print(F("Line "));
		Serial.println(i);
#endif

		// scans one line (trace & re-trace)
		scanLine();

		scanning = phone->sendData(pixels, 2 * lineLength);

		// next line on y-axis
		unsigned int cl = controller->nextLine();

		if (scanning == false) {
			break;
		}
	}
	stop();
}

int Scanner::stream() {
	startTime = millis();
	int data[1];
	bool streaming = 1;
	while (true) {
		data[0] = sampler->readChannels();
		streaming = phone->sendData(data, 1);
		if (streaming == false)
			break;
	}
	stop();
	return 0;
}

// stop the scanning process and resets the parameters.
int Scanner::stop() {

#ifdef DEBUG
	Serial.println("Scan complete");
#endif

	// calculate lapsed time
	endTime = millis() - startTime;
	scanning = false;
}

// return the lapsed time
unsigned long Scanner::getLapsedTime() {
	return endTime;
}


extern String const PARAM_LINE_LENGTH = "LINELENGTH";

void Scanner::setParam(String param, String value) {
	stop();

	if (PARAM_LINE_LENGTH == param) {
		lineLength = atoi(value.c_str());
	}

}

void Scanner::invertChannels() {
	controller->invert();
}