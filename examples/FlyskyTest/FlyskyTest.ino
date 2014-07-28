// BOF preprocessor bug prevent - insert me on top of your arduino-code
#if 1
__asm volatile ("nop");
#endif

#include <spi.h>
#if defined(__SAM3X8E__)
//#include <DueTimer.h>
#elif defined(__AVR_ATmega2560__ )
//#include <TimerThree.h>
#else
#include <TimerOne.h>
#endif

#include <SPI.h>
#include "a7105.h"
#include "printf.h"
#include "FlyskyTransmitter.h"

void setup() {
	
	
  Serial.begin(115200);
  
#if !defined (__arm__) && !defined (__SAM3X8E__)
  printf_begin();
#endif
  
/*	pinMode(41, OUTPUT);
	digitalWrite(41, HIGH);
	delay(50);
	digitalWrite(41, LOW);
  */
  
  A7105_Setup(); //A7105_Reset();
  
  printf("Initializing Timer...\n");
  
#if defined (__AVR_ATmega2560__ )
  Timer3.initialize();
#else if defined (__AVR_ATmega328P__)
	Timer1.initialize();    
#endif
  
  // Set all channels to zero
  for (uint8_t i=0 ; i < NUM_OUT_CHANNELS; i++) {
	  Channels[i] = 0;
  }

  //A7105_WriteReg(0x00, 0x00);
  
  
  printf("PROTOCMD_INIT...\n");
  FLYSKY_Cmds(PROTOCMD_BIND);
	
}




void loop() {
	/*A7105_WriteReg(0x00, 0x00);
	delay(1);
*/
	uint8_t chan;
	uint16_t val;
	
	while (1) {
		
		printf("Read value (<channel(0-7)> <value(0-9999)>): ");
		
		while (!Serial.available()) {
			delay(100);
		}
		
		chan = Serial.parseInt();
		
		if (Serial.read() != ' ') {
			while (Serial.available()) Serial.read();
			continue;
		}
		
		val = Serial.parseInt();
		
		printf ("Channel %d, value %d\n", chan, val);
		
		if (chan < NUM_OUT_CHANNELS)
			Channels[chan] = val;
	}
}

