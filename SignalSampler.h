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

// channel mask
typedef byte READ_CHANNELS;
const READ_CHANNELS READ_CHANNELS_A = 1;
const READ_CHANNELS READ_CHANNELS_B = 2;
const READ_CHANNELS READ_CHANNELS_C = 4;
const READ_CHANNELS READ_CHANNELS_D = 8;
const READ_CHANNELS READ_CHANNELS_ABDIFF = 16;
const READ_CHANNELS READ_CHANNELS_CDDIFF = 32;

class SignalSampler
{
	private: 
		void resetMeans();
		Adafruit_ADS1015 *sig_adc;
		Adafruit_ADS1015 *diff_adc;


		// 0 - A minus B
		// 1 - C minus D
		// 2 - Sum
		// 3 - A
		// 4 - B
		// 5 - C
		// 6 - D
		float* sampleMeans;

	public:
     unsigned int sampleSize=2;

		SignalSampler(void);
		SignalSampler(Adafruit_ADS1015 *&sig_adc, Adafruit_ADS1015 *&diff_adc, int);
		~SignalSampler();
		int readChannels();
    int readChannels(READ_CHANNELS channels);
		int detectRandom();
		int reset();
		int getSampleSize() { return sampleSize; }
		void setSampleSize(int sampleSize);
		float getAMinusB() { return sampleMeans[0]; }
		float getCMinusD() { return sampleMeans[1]; }
		float getSum() { return sampleMeans[2]; }
		float getA() { return sampleMeans[3]; }
		float getB() { return sampleMeans[4]; }
		float getC() { return sampleMeans[5]; }
		float getD() { return sampleMeans[6]; }
};

#endif

