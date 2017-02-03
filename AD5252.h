#pragma once
#include "stdafx.h"
#include "ADDAC.h"


// register addresses
const int AD5252_REG_RDAC1 = 0b00000001;
const int AD5252_REG_RDAC1 = 0b00000011;

/*
This class describes the interface to the AD5252 dual digital potentiometer chips
*/
class AD5252
{
public:
	// initialise the AD5252
	// a1: what is the logic level of address pin a1 (AD1)? (0|1)
	// a0: what is the logic level of address pin a0 (AD1)? (0|1)
	unsigned char Init(unsigned char a1, unsigned char a0);

	unsigned char get
};