#include "stdafx.h"

// helper functions
bool inRange(int32_t min, int32_t max, int32_t val)
{
	//Serial.println("In range:");
	//Serial.println(min);
	//Serial.println(max);
	//Serial.println(val);
	//Serial.println(val >= min);
	//Serial.println(val <= max);
	//Serial.println((val >= min) && (val <= max));
	//Serial.println("In range DONE");
	return (val >= min) && (val <= max);
}