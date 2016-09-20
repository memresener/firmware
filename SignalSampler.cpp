#include "stdafx.h"
#include "SignalSampler.h"

//#define DEBUG

SignalSampler::SignalSampler(void) {
	//TODO default values
	//Serial.println("SignalSampler(void);");
}

SignalSampler::SignalSampler(Adafruit_ADS1015 *&sig_adc, Adafruit_ADS1015 *&diff_adc, int sampleSize)
{
#ifdef DEBUG
	Serial.println(F("SignalSampler::SignalSampler"));
#endif

	this->sampleSize = sampleSize;
    Serial.println(sampleSize);

	this->sig_adc = sig_adc;
	this->diff_adc = diff_adc;

	// start the adcs  DONE OUTSIDE!
	//sig_adc->begin();
	//diff_adc->begin();

	//this->buffer = new int[sampleSize];
	this->sampleMeans = new float[7];  // to store means of each channel
	//this scales the input by the reference pin of the arduino
	//Serial.println("SignalSampler(args);");

#ifdef DEBUG
	Serial.print(F("SignalSampler::SignalSampler sampleSize="));
	Serial.println(this->sampleSize);
#endif
}

SignalSampler::~SignalSampler() {}

int SignalSampler::readChannels()
{
  return readChannels(255);
}
int SignalSampler::readChannels(READ_CHANNELS channels)
{


	resetMeans();
 

    if (channels & READ_CHANNELS_ABDIFF) sampleMeans[0] += diff_adc->readADC_SingleEnded(ABDIFF_CHANNEL);
    if (channels & READ_CHANNELS_CDDIFF) sampleMeans[1] += diff_adc->readADC_SingleEnded(CDDIFF_CHANNEL);

    if (channels & READ_CHANNELS_A) sampleMeans[3] += sig_adc->readADC_SingleEnded(A_CHANNEL);
    if (channels & READ_CHANNELS_B) sampleMeans[4] += sig_adc->readADC_SingleEnded(B_CHANNEL);
    if (channels & READ_CHANNELS_C) sampleMeans[5] += sig_adc->readADC_SingleEnded(C_CHANNEL);
    if (channels & READ_CHANNELS_D) sampleMeans[6] += sig_adc->readADC_SingleEnded(D_CHANNEL);

	return (sampleMeans[0]+sampleMeans[1]);
}

int SignalSampler::detectRandom()
{
	return 4;
}

int SignalSampler::reset() {
	return 0;
}

void SignalSampler::setSampleSize(int sampleSize)
{
	this->sampleSize = sampleSize;
#ifdef DEBUG
	Serial.print(F("SignalSampler::setSampleSize sampleSize="));
	Serial.print(this->sampleSize);
	Serial.print(F(":"));
	Serial.println((int)&(this->sampleSize));
#endif
}

void SignalSampler::resetMeans()
{
  
	sampleMeans[0] = 0;
	sampleMeans[1] = 0;
	sampleMeans[2] = 0;
	sampleMeans[3] = 0;
	sampleMeans[4] = 0;
	sampleMeans[5] = 0;
	sampleMeans[6] = 0;
}



