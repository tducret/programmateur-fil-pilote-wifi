/*
This is the core graphics library for all our displays, providing a common
set of graphics primitives (points, lines, circles, etc.).  It needs to be
paired with a hardware-specific library for each display device we carry
(to handle the lower-level functions).

Adafruit invests time and resources providing this open source code, please
support Adafruit & open-source hardware by purchasing products from Adafruit!

Copyright (c) 2013 Adafruit Industries.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice,
	this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
	this list of conditions and the following disclaimer in the documentation
	and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

History : 15/01/2015 Charles-Henri Hallard (http://hallard.me)
										 Ported to for Spark Core
										 Added Bargraph functions

					15/09/2015 Charles-Henri Hallard : Ajout compatibilité ESP8266

*/

#ifndef _ADAFRUIT_GFX_H
#define _ADAFRUIT_GFX_H

#ifdef SPARK
#include "application.h"
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#else
#include "Arduino.h"
#endif

#define _swap(a, b) { int16_t t = a; a = b; b = t; }

//class Adafruit_GFX : public Print {
class Adafruit_GFX {

 public:

	Adafruit_GFX(int16_t w, int16_t h); // Constructor

	void printf( const char * format, ...);
	void print( const char * string) ;

	// This MUST be defined by the subclass:
	virtual void drawPixel(int16_t x, int16_t y, uint16_t color) = 0;

	// These MAY be overridden by the subclass to provide device-specific
	// optimized code.  Otherwise 'generic' versions are used.
	virtual void
		drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color),
		drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color),
		drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color),
		drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color),
		fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color),
		fillScreen(uint16_t color),
		invertDisplay(boolean i);

	// These exist only with Adafruit_GFX (no subclass overrides)
	void
		drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color),
		drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername,
			uint16_t color),
		fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color),
		fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername,
			int16_t delta, uint16_t color),
		drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
			int16_t x2, int16_t y2, uint16_t color),
		fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
			int16_t x2, int16_t y2, uint16_t color),
		drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h,
			int16_t radius, uint16_t color),
		fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h,
			int16_t radius, uint16_t color),
		drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap,
			int16_t w, int16_t h, uint16_t color),
		drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color,
			uint16_t bg, uint8_t size),
		setCursor(int16_t x, int16_t y),
		setTextColor(uint16_t c),
		setTextColor(uint16_t c, uint16_t bg),
		setTextSize(uint8_t s),
		setTextWrap(boolean w),
		setRotation(uint8_t r);

	 virtual size_t write(uint8_t);

	int16_t
		height(void),
		width(void);

	uint8_t getRotation(void);

	void drawVerticalBargraph(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color, uint16_t percent) ;
	void drawHorizontalBargraph(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color, uint16_t percent) ;

protected:
	const int16_t
		WIDTH, HEIGHT;   // This is the 'raw' display w/h - never changes
	int16_t
		_width, _height, // Display w/h as modified by current rotation
		cursor_x, cursor_y;
	uint16_t
		textcolor, textbgcolor;
	uint8_t
		textsize,
		rotation;
	boolean
		wrap; // If set, 'wrap' text at right edge of display
};

#endif // _ADAFRUIT_GFX_H
