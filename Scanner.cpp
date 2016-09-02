//#define DEBUG

// Author A Michel
// Date 20 08 15
// Lego2Nano 2015

#include "Scanner.h"
#include "RTx.h"

// Constructor
Scanner::Scanner(PiezoDACController *controll, SignalSampler *sample, const RTx *phon, const int lineLeng)
{
	this->pixels = new int[lineLength * 2];
	this->controller = controll;
	this->startTime = 0;
	this->endTime = 0;
	this->scanning = false;
	this->lineLength = lineLeng;
	this->scanDelay = 0;
  this->sampler=sample;
  this->phone=phon;
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
int Scanner::scanLine(unsigned int lineSize) {
  
	unsigned int x = 0;
	//trace
	for (int i = 0; i < lineSize; i++) {

		sampler->readChannels();

		// calculate and save FES
		pixels[x] = sampler->getAMinusB() - sampler->getCMinusD();
		x++;

		if (i + 1 < lineSize)
		{
			controller->increaseVoltage();
		}

		if (scanDelay > 0) delay(scanDelay);
	}

	//retrace
	for (int i = 0; i < lineSize; i++) {

		sampler->readChannels();

		// calculate and save FES
		pixels[x] = sampler->getAMinusB() - sampler->getCMinusD();
		x++;

		if (i + 1 < lineSize)
		{
			controller->decreaseVoltage();
		}

		if (scanDelay > 0) delay(scanDelay);
	}

	linesScanned++;


	return 0;
}

// Start the scanning process.
int Scanner::start() {


  unsigned int lineSize=controller->getLineSize();

	// get start time
	startTime = millis();
	scanning = true;

	// interates over y-axis calling ctrl.nextLine() 
	for (int i = 0; i < lineSize; i++) {
    
		scanLine(lineSize);
		scanning = phone->sendData(pixels, 2 * lineSize);
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
	Serial.println("Scanner::stop Scan complete");
#endif

	// calculate lapsed time
	endTime = millis() - startTime;
	scanning = false;

  return 1;
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
