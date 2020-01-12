/*
 * Room temperature night light
 * Author: Tony Chiarot 
 * 
 * This sketch will check the temperature of a DS18x sensor and then drive 3 LED's 
 * to indicate the temperature in the room. I am using an orange, blue and red LED.
 * Orange will indicate that the room temperature is in the normal range.
 * Blue will indicate a cold room.
 * Red will indicate a warm room.
 * The sketch will use PWM pins to transition from the normal range to the extreme ranges.
*/

#include <OneWire.h>

//CONFIGURATION
//This is the section that will be setup by the user for their specific setup

//SENSOR AND OUTPUTS -PIN DEFINITIONS
//set the three output pins that will be used for the LED's
//These should be PWM capable pins. Check your specific board for which digital pins are hardware capable
#define RLED 3
#define GLED 5
#define BLED 6

//The digital pin that will be used for the ds18B sensor
#define tempPin 2

//TEMPERATURE LIMITSokl
//Normal range
#define normLoTemp 22.0
#define normHiTemp 23.0
//Lower than this is full cold
#define coldTemp 18.0
//Higher than this is full Warm
#define warmTemp 26.0

//set the max PWM for each LED
#define RMax 255
#define GMax 255
#define BMax 255

//set the values for orange - Normal range as a percentage of max value
#define ROrange 1
#define GOrange 0.5
#define BOrange 0

//set the dim settings for the LED. It is a % of the max
#define Dim .5

//CONFIGURATION END

OneWire temp(tempPin);

//These are the LED values to write
byte red;
byte green;
byte blue;

void setup() {
  Serial.begin(9600);
}

void loop() {
  //check the temperature and set the colour output
  setColours(getTemp(temp), 1);

  //for debugging
  Serial.print(red);
  Serial.print(", ");
  Serial.print(green);
  Serial.print(", ");
  Serial.print(blue);
  Serial.print("\t");
  Serial.print(getTemp(temp));
  Serial.println(" degree C");
}

void setColours(float ambient, boolean dim){
  double ratio;
  if(ambient < normLoTemp){
    ratio = (normLoTemp-ambient)/(normLoTemp-coldTemp);
    if(ratio > 1){
      ratio = 1;
    }
    red = ROrange * RMax *(1-ratio);
    green = GOrange * GMax * (1-ratio);
    blue = BOrange * BMax * (1-ratio) + BMax * ratio;
  }
  else if(ambient > normHiTemp ){
    ratio = (ambient-normHiTemp)/(warmTemp - normHiTemp);
    if(ratio > 1){
      ratio = 1;
    }
    red = ROrange * RMax *(1-ratio) + RMax * ratio;
    green = GOrange * GMax * (1-ratio);
    blue = BOrange * BMax * (1-ratio);
  }
  else{
    red = ROrange * RMax;
    green = GOrange * GMax;
    blue = BOrange * BMax;
  }
  //now we will check to see if dimming is wanted
  if(dim){
    red = Dim*red;
    green = Dim*green;
    blue = Dim*blue;
  }
  //now set the outputs
  analogWrite(RLED, red);
  analogWrite(GLED, green);
  analogWrite(BLED, blue);
}

float getTemp(OneWire temp){
  byte data[12];
  byte i;
  byte present = 0;
  int16_t rawTemp;
  
  temp.reset();
  //there is only one one-wire device connected
  temp.skip();
  temp.write(0x44, 1);

  //wait for the conversion to take place
  delay(770); 
  present = temp.reset();
  temp.skip();
  temp.write(0xBE);

  for(i=0;i<9;i++){
    data[i] = temp.read();
  }
  rawTemp = ((data[1]<<8) | data[0]);
  return (float)rawTemp/16;
}
