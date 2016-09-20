#pragma once

#ifndef _STDAFX_
#define _STDAFX_

#ifndef _ADS1015_
#define _ADS1015_
#include "Adafruit_ADS1015.h"
#endif

#include "ADDAC.h"

// helper functions
bool inRange(int32_t min, int32_t max, int32_t val);


//extern int gSAMPLESIZE = 0;

#endif