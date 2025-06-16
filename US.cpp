#include "US.h"

//****** US ******//
US::US(){
}

US::US(uint8_t pinTrigger, uint8_t pinEcho){
  US::init(pinTrigger,pinEcho);
}

void US::init(uint8_t pinTrigger, uint8_t pinEcho)
{
  _pinTrigger = pinTrigger;
  _pinEcho = pinEcho;
  pinMode( _pinTrigger , OUTPUT );
  pinMode( _pinEcho , INPUT );
}

long US::TP_init()
{
    digitalWrite(_pinTrigger, LOW);
    delayMicroseconds(2);
    digitalWrite(_pinTrigger, HIGH);
    delayMicroseconds(10);
    digitalWrite(_pinTrigger, LOW);
    long microseconds = pulseIn(_pinEcho,HIGH,40000); //40000
    return microseconds;
}

float US::read(){
  long microseconds = US::TP_init();
  long distance;
  distance = microseconds/29/2;
  if (distance == 0){
    distance = 999;
  }
  return distance;
}