#ifndef US_h
#define US_h
#include "Arduino.h"
#ifdef VSCODE
#include <cstdint>
#endif

class US
{
public:
	US();
	void init(uint8_t pinTrigger, uint8_t pinEcho);
	US(uint8_t pinTrigger, uint8_t pinEcho);
	float read();

private:
	uint8_t _pinTrigger;
	uint8_t _pinEcho;
	long TP_init();
};

#endif //US_h