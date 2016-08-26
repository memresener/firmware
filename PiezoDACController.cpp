#include "PiezoDACController.h"
#include "DAC_AD5696.h"

#define ON HIGH
#define OFF LOW


// turn on debug messages over COM
#define DEBUG

// constructor
PiezoDACController::PiezoDACController(ADDAC *dac, int stepSize, int lineLength, int ldacPin) {
	this->stepSize = stepSize;
	this->lineSize = lineLength;

	this->dac = dac;

	this->currentStep = 0;
	this->currentX = 0;
	this->currentY = 0;
	this->currentZ = 0;

	uint16_t half = 44308;
	startingXPlus = half;
	startingYPlus = half;
	startingXMinus = half;
	startingYMinus = half;

	currentXPlus = 0;
	currentYPlus = 0;
	currentXMinus = 0;
	currentYMinus = 0;

	invertChannels = false;
}

// destructor.
PiezoDACController::~PiezoDACController() {}

void PiezoDACController::Init()
{
	// should start with DACs at mid range
	SetDACOutput(B1111, (uint16_t)44308);
}

// reset parameters
unsigned int PiezoDACController::reset(int stepSize, int lineSize, int ldacPin) {
  this->stepSize = stepSize;
  this->lineSize = lineSize;

  this->currentStep = 0;
  this->currentX = 0;
  this->currentY = 0;
  this->currentZ = 0;

  // should start with DACs at mid range
  //SetDACOutput(AD569X_ADDR_DAC_ALL, (uint16_t)0x7FFF);
  //SetDACOutput(B1111, (uint16_t)44308);  // on the currnt piezo board this gives 0 output
  //GotoCoordinates(0, 0, 0);

  invertChannels = false;

  return currentStep;
}

#undef DEBUG

/*! Set the DAC output and update internal position. */
int PiezoDACController::SetDACOutput(uint8_t channels, uint16_t value)
{
	dac->SetOutput(channels, value);

#ifdef DEBUG
	Serial.print("Setting DAC channel ");
	Serial.print(channels);
	Serial.print(" to ");
	Serial.println(value);
#endif

	Serial.print(F("X_PLUS&="));
	Serial.println(channels & X_PLUS);
	if (channels & X_PLUS)
	{
#ifdef DEBUG
		Serial.print(F("Setting X_PLUS="));
		Serial.println(value);
#endif
		currentXPlus = value;
	}
	if (channels & X_MINUS)
	{
#ifdef DEBUG
		Serial.print(F("Setting X_MINUS="));
		Serial.println(value);
#endif
		currentXMinus = value;
	}

	if (channels & Y_PLUS)
	{
#ifdef DEBUG
		Serial.print(F("Setting Y_PLUS="));
		Serial.println(value);
#endif
		currentYPlus = value;
	}
	if (channels & Y_MINUS)
	{
#ifdef DEBUG
		Serial.print(F("Setting Y_MINUS="));
		Serial.println(value);
#endif
		currentYMinus = value;
	}

#ifdef DEBUG
	Serial.print(F("X_PLUS="));
	Serial.println(currentXPlus);
	Serial.print(F("X_MINUS="));
	Serial.println(currentXMinus);
	Serial.print(F("Y_PLUS="));
	Serial.println(currentYPlus);
	Serial.print(F("Y_MINUS="));
	Serial.println(currentYMinus);
#endif

	//if (channels & Y_PLUS) currentYPlus = value;
	//if (channels & Y_MINUS) currentYMinus = value;


	return 0;
}

//int PiezoDACController::SetDACOutput(uint8_t channels, double value, double vRef)
//{
//	dac->SetVoltage(channels, value, vRef);
//#ifdef DEBUG
//	Serial.print("Setting DAC channel ");
//	Serial.print(channels);
//	Serial.print(" to ");
//	Serial.println(value);
//#endif
//
//	if (channels & X_PLUS) currentXPlus = value;
//	if (channels & X_MINUS) currentXMinus = value;
//	if (channels & Y_PLUS) currentYPlus = value;
//	if (channels & Y_MINUS) currentYMinus = value;
//
//
//	return 0;
//}

int PiezoDACController::move(PIEZO_AXIS direction, int steps, bool allAtOnce)
{
	//int diff = allAtOnce ? steps : (steps > 0 ? 1 : -1); // if all at once, change voltage by full amount in one go.  Otherwise one at a time.
	//int lim = allAtOnce ? 1 : steps;  // if all in one go, only do once, otherwise do each step
	int diff = steps;
	int lim = 1;

	uint8_t channelPlus = 0;
	uint8_t channelMinus = 0;
	uint16_t currentPlus = 0;
	uint16_t currentMinus = 0;
	bool doSingle = true;

#ifdef DEBUG
	Serial.print("Current X is ");
	Serial.println(currentX);
	Serial.print("Current Y is ");
	Serial.println(currentY);
	Serial.print("Current Z is ");
	Serial.println(currentZ);
#endif


#ifdef DEBUG
	Serial.print(F("X_PLUS="));
	Serial.println(currentXPlus);
	Serial.print(F("X_MINUS="));
	Serial.println(currentXMinus);
	Serial.print(F("Y_PLUS="));
	Serial.println(currentYPlus);
	Serial.print(F("Y_MINUS="));
	Serial.println(currentYMinus);
#endif

	// Decide what to move where
	switch (direction)
	{
		// for Z up/down, increment/decrement all dac channels
	case Z:
		//currentZ--;
		//diff = direction == Z_UP ? adiff : -adiff;  // increase or decrease?

		for (int i = 0; i < lim; i++)
		{
			SetDACOutput(X_PLUS, currentXPlus + diff);
			SetDACOutput(X_MINUS, currentXMinus + diff);
			SetDACOutput(Y_PLUS, currentYPlus + diff);
			SetDACOutput(Y_MINUS, currentYMinus + diff);
		}
		doSingle = false;
		break;

	case X:
		///diff = direction == X_UP ? adiff : -adiff;  // increase or decrease?
		currentPlus = currentXPlus;
		currentMinus = currentXMinus;
		channelPlus = X_PLUS;
		channelMinus = X_MINUS;
		doSingle = true;
		break;

	case Y:
		//diff = direction == Y_UP ? adiff : -adiff;  // increase or decrease?
		currentPlus = currentYPlus;
		currentMinus = currentYMinus;
		channelPlus = Y_PLUS;
		channelMinus = Y_MINUS;
		doSingle = true;
		break;

	}

#ifdef DEBUG
	Serial.print(F("Current Plus is "));
	Serial.println(currentPlus);
	Serial.print(F("Current Minus is "));
	Serial.println(currentMinus);

	Serial.print("Moving channel ");
	Serial.print(channelPlus);
	Serial.print(F(" and "));
	Serial.print(channelMinus);
	Serial.print(F(" by "));
	Serial.println(diff);
#endif

	// change DAC
	if (doSingle)
	{
		for (int i = 0; i < lim; i++)
		{
			SetDACOutput(channelPlus, currentPlus + diff);
			SetDACOutput(channelMinus, currentMinus - diff);
		}
	}
	return 0;
}


int PiezoDACController::GotoCoordinates(int x, int y, int z)
{
	// difference?
	int diffX = x;
	diffX -= currentX;
	int diffY = y;
	diffY -= currentY;
	int diffZ = z;
	diffZ -= currentZ;

	diffX *= stepSize;
	diffY *= stepSize;
	diffZ *= stepSize;

	if (diffX != 0)
	{
#ifdef DEBUG
		Serial.print("Moving X by ");
		Serial.println(diffX);
#endif
		move(X, diffX, true);
	}

	if (diffY != 0)
	{
#ifdef DEBUG
		Serial.print("Moving Y by ");
		Serial.println(diffY);
#endif
		move(Y, diffY, true);

	}

	if (diffZ != 0)
	{
#ifdef DEBUG
		Serial.print("Moving Z by ");
		Serial.println(diffZ);
#endif
		move(Z, diffZ, true);
	}

	currentX = x;
	currentY = y;
	currentZ = z;

#ifdef DEBUG
	Serial.print("Current position is (");
	Serial.print(x);
	Serial.print(", ");
	Serial.print(y);
	Serial.println(")");
#endif

	return 0;
}

// reset coordinates to startingX and startingY
unsigned int PiezoDACController::reset() {
	currentStep = 0;

	//setCoordinates();
	GotoCoordinates(0, 0, 0);
	//go(CHANNEL_A, currentX);
	//go(CHANNEL_B, currentY);
	return currentStep;
}

// zero the coordinates (set the corrent position as the origin)
void PiezoDACController::SetAsOrigin() {
	currentStep = 0;
	currentX = 0;
	currentY = 0;
	currentZ = 0;
}

// move to beginning of next line.
unsigned int PiezoDACController::nextLine() {
	//int delta = (((currentStep / lineSize) + 1) * lineSize) - currentStep;
	//currentStep += delta;
	//setCoordinates();
	//go(CHANNEL_A, currentX);
	//go(CHANNEL_B, currentY);
	GotoCoordinates(0, currentY + 1, currentZ);
	return currentStep;
}

// go to end of current line
unsigned int PiezoDACController::eol() {
	//nextLine();
	//currentStep--;
	//setCoordinates();
	//go(CHANNEL_A, currentX);
	//go(CHANNEL_B, currentY);
	GotoCoordinates(currentX, lineSize, currentZ);
	return currentStep;
}

//// set coordinates relative to currentStep
//int PiezoDACController::setCoordinates() {
//	currentX = currentStep % lineSize;
//	currentY = currentStep / lineSize;
//
//	if (invertChannels) {
//		int temp = currentX;
//		currentX = currentY;
//		currentY = temp;
//	}
//	return 0;
//}

// increase voltage
unsigned int PiezoDACController::increaseVoltage() {

#ifdef DEBUG
	Serial.print(F("X_PLUS="));	Serial.println(currentXPlus);
	Serial.print(F("X_MINUS=")); Serial.println(currentXMinus);
	Serial.print(F("Y_PLUS="));	Serial.println(currentYPlus);
	Serial.print(F("Y_MINUS=")); Serial.println(currentYMinus);
#endif
  // step fwd
  //currentStep += stepSize;
  
  //setCoordinates();
  
  //move(X, stepSize, false);
	GotoCoordinates(currentX + 1, currentZ, currentZ);
  //go(CHANNEL_A, currentX);
  //go(CHANNEL_B, currentY);
  
  return currentStep;
}

// decrease voltage
unsigned int PiezoDACController::decreaseVoltage() {

  // step back
  //currentStep -= stepSize;

  //setCoordinates();

	GotoCoordinates(currentX - 1, currentZ, currentZ);
  //go(CHANNEL_A, currentX);
  //go(CHANNEL_B, currentY);

  return currentStep;
}

// return current line size.
int PiezoDACController::getLineSize() {
  return lineSize;
}

// get the position
int PiezoDACController::GetPosition(PIEZO_AXIS axis)
{
	switch (axis)
	{
	case X:
#ifdef DEBUG
		Serial.print("Getting position of axis X");
#endif
		return currentX;
		break;
	case Y:
#ifdef DEBUG
		Serial.print("Getting position of axis Y");
#endif
		return currentY;
		break;
	case Z:
#ifdef DEBUG
		Serial.print("Getting position of axis Z");
#endif
		return currentZ;
		break;
	}
}

void PiezoDACController::invert() {
  invertChannels = !invertChannels;
}



