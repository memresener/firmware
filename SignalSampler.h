#include "stdafx.h"

// DataRecv.h
// Author Y_Vertex

#ifndef _SIGNALSAMPLER_h
#define _SIGNALSAMPLER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#define ABDIFF_CHANNEL 0
#define CDDIFF_CHANNEL 1
#define SUM_CHANNEL 2
#define A_CHANNEL 0
#define B_CHANNEL 1
#define C_CHANNEL 2
#define D_CHANNEL 3

class SignalSampler
{
	protected:

	private: 
		void resetMeans();
		Adafruit_ADS1015 *sig_adc;
		Adafruit_ADS1015 *diff_adc;
		int sampleSize;

		// 0 - A minus B
		// 1 - C minus D
		// 2 - Sum
		// 3 - A
		// 4 - B
		// 5 - C
		// 6 - D
		float *sampleMeans;

	public:
		SignalSampler(void);
		SignalSampler(Adafruit_ADS1015 *sig_adc, Adafruit_ADS1015 *diff_adc, int);
		~SignalSampler();
		int init(int);
		int readChannels();
		int detectRandom();
		int reset();
		int getSampleSize() { return sampleSize; }
		void setSampleSize(int sampleSize) { this->sampleSize = sampleSize; }
		float getAMinusB() { return sampleMeans[0]; }
		float getCMinusD() { return sampleMeans[1]; }
		float getSum() { return sampleMeans[2]; }
		float getA() { return sampleMeans[3]; }
		float getB() { return sampleMeans[4]; }
		float getC() { return sampleMeans[5]; }
		float getD() { return sampleMeans[6]; }
};

#endif

