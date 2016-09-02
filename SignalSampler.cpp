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

	// for the moment, use single sample as somewhere sampleSize is being set to 0...
	//sampleSize = 1;

	//int aSignalValue = 0;
	//int bSignalValue = 0;
	//int *sumSignalValue;
	//int j, temp;
	//int median = 0;
	//sumSignalValue = (int*)malloc(sizeof(int)*sampleSize);

	// reset sampleMeans, order is:
	// 0 - A minus B
	// 1 - C minus D
	// 2 - Sum
	// 3 - A
	// 4 - B
	// 5 - C
	// 6 - D
	//memset(sampleMeans, 0, sizeof(float) * 7);  // -- causes issues
	resetMeans();
 
	//for (int i = 0; i < this->sampleSize; i++){

		// get signals
		sampleMeans[0] += diff_adc->readADC_SingleEnded(ABDIFF_CHANNEL);
		sampleMeans[1] += diff_adc->readADC_SingleEnded(CDDIFF_CHANNEL);
		//sampleMeans[2] += diff_adc->readADC_SingleEnded(SUM_CHANNEL);
		//sampleMeans[3] += sig_adc->readADC_SingleEnded(A_CHANNEL);
		//sampleMeans[4] += sig_adc->readADC_SingleEnded(B_CHANNEL);
		//sampleMeans[5] += sig_adc->readADC_SingleEnded(C_CHANNEL);
		//sampleMeans[6] += sig_adc->readADC_SingleEnded(D_CHANNEL);


//#ifdef DEBUG
//		Serial.println(sampleMeans[0]);
//		Serial.println(sampleMeans[1]);
//		Serial.println(sampleMeans[2]);
//		Serial.println(sampleMeans[3]);
//		Serial.println(sampleMeans[4]);
//		Serial.println(sampleMeans[5]);
//		Serial.println(sampleMeans[6]);
//#endif

		//temp = aSignalValue+bSignalValue;
		//// insert sorting the sum array
		//j = i-1;
		//while(j >= 0 && temp < sumSignalValue[j]){
		//	sumSignalValue[j+1] = sumSignalValue[j]; 
		//	j--;
		//}
		//sumSignalValue[j+1] = temp;
	//}


	//// calculate means
	//for (int i = 0; i < 7; i++)
	//{
	//	sampleMeans[i] = sampleMeans[i] / sampleSize;
	//}

	//median = sumSignalValue[sampleSize/2];
	return 0;
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



