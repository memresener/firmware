
#include "Scanner.h"
#include "RTx.h"

/* Pin Definitions */
#define _SDI 2
#define _SCK 3
#define LOAD 4
#define LDAC 5

#define STEPSIZE 1
#define RNG 0
#define BAUDRATE 9600
#define LINE_LENGTH 256
#define SAMPLE_SIZE 5

#define SAMPLER_PIN_A A0
#define SAMPLER_PIN_B A0

RTx phone = RTx();
DACController ctrl = DACController(STEPSIZE, LINE_LENGTH, _SDI, _SCK, LOAD, LDAC, RNG);
SignalSampler sampler = SignalSampler(SAMPLER_PIN_A, SAMPLER_PIN_B, SAMPLE_SIZE);
Scanner scanner = Scanner(ctrl, sampler, phone, LINE_LENGTH);

/* ================ */
void initialize() {Serial.println("got it;");
}


void scanLine() {
	  scanner.start();
		
}
/* ================ */

void setParameter() {
	//pixelCount = ConvertStrToInt(Serial.readStringUntil(','));
	//lineCount = ConvertStrToInt(Serial.readStringUntil(';'));
}

int ConvertStrToInt(String str) {
	return 0;
}

/* ================ */

void setup() {
	Serial.begin(BAUDRATE);
}

void loop() {
  String cmd=phone.listen();
  Serial.println(cmd);
  delay(2000);
  if (cmd=="RDY"){
  phone.sendString("scanning!");scanLine();}
  else if (cmd="ERROR"){;}

}
