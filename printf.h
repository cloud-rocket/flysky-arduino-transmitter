/*
 Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>
 
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
 */
 
/**
 * @file printf.h
 *
 * Setup necessary to direct stdout to the Arduino Serial library, which
 * enables 'printf'
 */

#ifndef __PRINTF_H__
#define __PRINTF_H__

#ifdef ARDUINO


#if defined (__SAM3X8E__)

#include <stdarg.h>
void p(char *fmt, ... ){
	char buf[128]; // resulting string limited to 128 chars
	va_list args;
	va_start (args, fmt );
	vsnprintf(buf, 128, fmt, args);
	va_end (args);
	Serial.print(buf);
}


void p(const __FlashStringHelper *fmt, ... ){
	char buf[128]; // resulting string limited to 128 chars
	va_list args;
	va_start (args, fmt);
	#ifdef __AVR__
	vsnprintf_P(buf, sizeof(buf), (const char *)fmt, args); // progmem for AVR
	#else
	vsnprintf(buf, sizeof(buf), (const char *)fmt, args); // for the rest of the world
	#endif
	va_end(args);
	Serial.print(buf);
}

#define printf p

#else

int serial_putc( char c, FILE * ) 
{
  Serial.write( c );

  return c;
} 

void printf_begin(void)
{
  fdevopen( &serial_putc, 0 );
}

#endif

#else
#error This example is only for use on Arduino.
#endif // ARDUINO

#endif // __PRINTF_H__
