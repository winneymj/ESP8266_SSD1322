/*********************************************************************
This is a library for the 256 x 64 pixel 16 color gray scale OLEDs
based on SSD1322 drivers

These displays use SPI to communicate, 4 or 5 pins are required to
interface

Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/

#ifndef ESP8266    					//Added for compatibility with ESP8266 board
#include <avr/pgmspace.h>
#endif
#ifndef __SAM3X8E__
#ifndef ESP8266    					//Added for compatibility with ESP8266 board
#include <util/delay.h>
#endif
#endif
#include <stdlib.h>

//#include <Wire.h>

#include "Adafruit_GFX.h"
#include "ESP8266_SSD1322.h"

// the memory buffer for the LCD
static uint8_t buffer[SSD1322_LCDHEIGHT * SSD1322_LCDWIDTH / 2] = { 0x00 };

// the most basic function, set a single pixel
void ESP8266_SSD1322::drawPixel(int16_t x, int16_t y, uint16_t gscale) {
	if ((x < 0) || (x >= width()) || (y < 0) || (y >= height()))
		return;

	register uint8_t mask = ((x % 2) ? gscale : gscale << 4);
	register uint8_t *pBuf = &buffer[(x >> 1) + (y * (SSD1322_LCDWIDTH / 2))];
	register uint8_t b1 = *pBuf;
	b1 &= (x % 2) ? 0xF0 : 0x0F; // cleardown nibble to be replaced

	// write our value in
	*pBuf++ = b1 | mask;

//	buffer[(x >> 1) + (y * (SSD1322_LCDWIDTH / 2))] |= ((x % 2) ? gscale : gscale << 4);
}
ESP8266_SSD1322::ESP8266_SSD1322(int8_t SID, int8_t SCLK, int8_t DC,
		int8_t RST, int8_t CS) :
		Adafruit_GFX(SSD1322_LCDWIDTH, SSD1322_LCDHEIGHT) {
	cs = CS;
	rst = RST;
	dc = DC;
	sclk = SCLK;
	sid = SID;
	hwSPI = false;
}

// constructor for hardware SPI - we indicate DataCommand, ChipSelect, Reset
ESP8266_SSD1322::ESP8266_SSD1322(int8_t DC, int8_t RST, int8_t CS) :
		Adafruit_GFX(SSD1322_LCDWIDTH, SSD1322_LCDHEIGHT) {
	dc = DC;
	rst = RST;
	cs = CS;
	hwSPI = true;
}

// initializer for I2C - we only indicate the reset pin!
ESP8266_SSD1322::ESP8266_SSD1322(int8_t reset) :
		Adafruit_GFX(SSD1322_LCDWIDTH, SSD1322_LCDHEIGHT) {
	sclk = dc = cs = sid = -1;
	rst = reset;
}

/* ------------------------------------------------------------

------------------------------------------------------------ */

void ESP8266_SSD1322::begin(uint8_t i2caddr, bool reset) {
	_i2caddr = i2caddr;

	// set pin directions
	if (sid != -1) {
		pinMode(dc, OUTPUT);
		pinMode(cs, OUTPUT);
		if (hwSPI) {
			SPI.begin();
			SPI.setClockDivider (SPI_CLOCK_DIV2); // 26/2 = 13 MHz (freq ESP8266 26 MHz)
		}
	}

	if (reset)
	{
		// Setup reset pin direction (used by both SPI and I2C)
		pinMode(rst, OUTPUT);
		// bring out of reset
		digitalWrite(rst, HIGH);
		delay(100);
		// bring reset low
		digitalWrite(rst, LOW);
		delay(400);
		// bring out of reset
		digitalWrite(rst, HIGH);
	}

#if defined SSD1322_256_64

	ssd1322_command(SSD1322_SETCOMMANDLOCK);// 0xFD
	ssd1322_data(0x12);// Unlock OLED driver IC

	ssd1322_command(SSD1322_DISPLAYOFF);// 0xAE

	ssd1322_command(SSD1322_SETCLOCKDIVIDER);// 0xB3
	ssd1322_data(0x91);// 0xB3

	ssd1322_command(SSD1322_SETMUXRATIO);// 0xCA
	ssd1322_data(0x3F);// duty = 1/64

	ssd1322_command(SSD1322_SETDISPLAYOFFSET);// 0xA2
	ssd1322_data(0x00);

	ssd1322_command(SSD1322_SETSTARTLINE);// 0xA1
	ssd1322_data(0x00);

	ssd1322_command(SSD1322_SETREMAP);// 0xA0
	ssd1322_data(0x14);//Horizontal address increment,Disable Column Address Re-map,Enable Nibble Re-map,Scan from COM[N-1] to COM0,Disable COM Split Odd Even
	ssd1322_data(0x11);//Enable Dual COM mode

	ssd1322_command(SSD1322_SETGPIO);// 0xB5
	ssd1322_data(0x00);// Disable GPIO Pins Input

	ssd1322_command(SSD1322_FUNCTIONSEL);// 0xAB
	ssd1322_data(0x01);// selection external vdd

	ssd1322_command(SSD1322_DISPLAYENHANCE);// 0xB4
	ssd1322_data(0xA0);// enables the external VSL
	ssd1322_data(0xFD);// 0xfFD,Enhanced low GS display quality;default is 0xb5(normal),

	ssd1322_command(SSD1322_SETCONTRASTCURRENT);// 0xC1
	ssd1322_data(0xFF);// 0xFF - default is 0x7f

	ssd1322_command(SSD1322_MASTERCURRENTCONTROL);// 0xC7
	ssd1322_data(0x0F);// default is 0x0F

	// Set grayscale
	ssd1322_command(SSD1322_SELECTDEFAULTGRAYSCALE); // 0xB9

 	ssd1322_command(SSD1322_SETPHASELENGTH);// 0xB1
	ssd1322_data(0xE2);// default is 0x74

	ssd1322_command(SSD1322_DISPLAYENHANCEB);// 0xD1
	ssd1322_data(0x82);// Reserved;default is 0xa2(normal)
	ssd1322_data(0x20);//

	ssd1322_command(SSD1322_SETPRECHARGEVOLTAGE);// 0xBB
	ssd1322_data(0x1F);// 0.6xVcc

	ssd1322_command(SSD1322_SETSECONDPRECHARGEPERIOD);// 0xB6
	ssd1322_data(0x08);// default

	ssd1322_command(SSD1322_SETVCOMH);// 0xBE
	ssd1322_data(0x07);// 0.86xVcc;default is 0x04

	ssd1322_command(SSD1322_NORMALDISPLAY);// 0xA6

	ssd1322_command(SSD1322_EXITPARTIALDISPLAY);// 0xA9

#endif
	//Clear down image ram before opening display
	fill(0x00);

	ssd1322_command(SSD1322_DISPLAYON);// 0xAF
}

void ESP8266_SSD1322::invertDisplay(uint8_t i) {
	if (i) {
		ssd1322_command(SSD1322_INVERSEDISPLAY);
	} else {
		ssd1322_command(SSD1322_NORMALDISPLAY);
	}
}

// startscrollright
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void ESP8266_SSD1322::startscrollright(uint8_t start, uint8_t stop) {
	ssd1322_command(SSD1322_RIGHT_HORIZONTAL_SCROLL);
	ssd1322_command(0X00);
	ssd1322_command(start);
	ssd1322_command(0X00);
	ssd1322_command(stop);
	ssd1322_command(0X00);
	ssd1322_command(0XFF);
	ssd1322_command(SSD1322_ACTIVATE_SCROLL);
}

// startscrollleft
// Activate a right handed scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void ESP8266_SSD1322::startscrollleft(uint8_t start, uint8_t stop) {
	ssd1322_command(SSD1322_LEFT_HORIZONTAL_SCROLL);
	ssd1322_command(0X00);
	ssd1322_command(start);
	ssd1322_command(0X00);
	ssd1322_command(stop);
	ssd1322_command(0X00);
	ssd1322_command(0XFF);
	ssd1322_command(SSD1322_ACTIVATE_SCROLL);
}

// startscrolldiagright
// Activate a diagonal scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void ESP8266_SSD1322::startscrolldiagright(uint8_t start, uint8_t stop) {
	ssd1322_command(SSD1322_SET_VERTICAL_SCROLL_AREA);
	ssd1322_command(0X00);
	ssd1322_command(SSD1322_LCDHEIGHT);
	ssd1322_command(SSD1322_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL);
	ssd1322_command(0X00);
	ssd1322_command(start);
	ssd1322_command(0X00);
	ssd1322_command(stop);
	ssd1322_command(0X01);
	ssd1322_command(SSD1322_ACTIVATE_SCROLL);
}

// startscrolldiagleft
// Activate a diagonal scroll for rows start through stop
// Hint, the display is 16 rows tall. To scroll the whole display, run:
// display.scrollright(0x00, 0x0F)
void ESP8266_SSD1322::startscrolldiagleft(uint8_t start, uint8_t stop) {
	ssd1322_command(SSD1322_SET_VERTICAL_SCROLL_AREA);
	ssd1322_command(0X00);
	ssd1322_command(SSD1322_LCDHEIGHT);
	ssd1322_command(SSD1322_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL);
	ssd1322_command(0X00);
	ssd1322_command(start);
	ssd1322_command(0X00);
	ssd1322_command(stop);
	ssd1322_command(0X01);
	ssd1322_command(SSD1322_ACTIVATE_SCROLL);
}

void ESP8266_SSD1322::stopscroll(void) {
	ssd1322_command(SSD1322_DEACTIVATE_SCROLL);
}

// Dim the display
// dim = true: display is dimmed
// dim = false: display is normal
void ESP8266_SSD1322::dim(boolean dim) {
	uint8_t contrast;

	if (dim) {
		contrast = 0; // Dimmed display
	}
//	else {
//		if (_vccstate == SSD1322_EXTERNALVCC) {
//			contrast = 0x9F;
//		} else {
//			contrast = 0xCF;
//		}
//	}
	// the range of contrast to too small to be really useful
	// it is useful to dim the display
	ssd1322_command(SSD1322_SETCONTRASTCURRENT);
	ssd1322_command(contrast);
}

void ESP8266_SSD1322::ssd1322_command(uint8_t c) {
	if (sid != -1) {
		// SPI
		digitalWrite(cs, HIGH);
		digitalWrite(dc, LOW);
		digitalWrite(cs, LOW);
		fastSPIwrite(c);
		digitalWrite(cs, HIGH);
	}
}

void ESP8266_SSD1322::ssd1322_data(uint8_t c) {
	if (sid != -1) {
		// SPI
		digitalWrite(cs, HIGH);
		digitalWrite(dc, HIGH);
		digitalWrite(cs, LOW);
		fastSPIwrite(c);
		digitalWrite(cs, HIGH);
	}
}

void ESP8266_SSD1322::ssd1322_dataBytes(uint8_t *buf, uint32_t size) {
	if (sid != -1) {
		// SPI
		digitalWrite(cs, HIGH);
		digitalWrite(dc, HIGH);
		digitalWrite(cs, LOW);
		SPI.writeBytes(buf, size);
		digitalWrite(cs, HIGH);
	}
}

void ESP8266_SSD1322::display() {

    ssd1322_command(SSD1322_SETCOLUMNADDR);
    ssd1322_data(MIN_SEG);
    ssd1322_data(MAX_SEG);

    ssd1322_command(SSD1322_SETROWADDR);
    ssd1322_data(0);
    ssd1322_data(63);

    ssd1322_command(SSD1322_WRITERAM);

    register uint16_t bufSize = (SSD1322_LCDWIDTH * SSD1322_LCDHEIGHT / 2); // bytes
	register uint8_t *pBuf = buffer;

	// Write as quick as possible 64 bits at a time
	ssd1322_dataBytes(pBuf, bufSize);
}

// clear everything
void ESP8266_SSD1322::clearDisplay(void) {
	memset(buffer, 0, (SSD1322_LCDWIDTH * SSD1322_LCDHEIGHT / 2));
}

inline void ESP8266_SSD1322::fastSPIwrite(uint8_t d) {

	if (hwSPI) {
		(void) SPI.transfer(d);
	} else {
		for (uint8_t bit = 0x80; bit; bit >>= 1) {
			*clkport &= ~clkpinmask;
			if (d & bit)
				*mosiport |= mosipinmask;
			else
				*mosiport &= ~mosipinmask;
			*clkport |= clkpinmask;
		}
	}
	//*csport |= cspinmask;
}

void ESP8266_SSD1322::drawFastHLine(int16_t x, int16_t y, int16_t w,
		uint16_t color) {
	boolean bSwap = false;
	switch (rotation) {
	case 0:
		// 0 degree rotation, do nothing
		break;
	case 1:
		// 90 degree rotation, swap x & y for rotation, then invert x
		bSwap = true;
		swap(x, y)
		;
		x = WIDTH - x - 1;
		break;
	case 2:
		// 180 degree rotation, invert x and y - then shift y around for height.
		x = WIDTH - x - 1;
		y = HEIGHT - y - 1;
		x -= (w - 1);
		break;
	case 3:
		// 270 degree rotation, swap x & y for rotation, then invert y  and adjust y for w (not to become h)
		bSwap = true;
		swap(x, y)
		;
		y = HEIGHT - y - 1;
		y -= (w - 1);
		break;
	}

	if (bSwap) {
		drawFastVLineInternal(x, y, w, color);
	} else {
		drawFastHLineInternal(x, y, w, color);
	}
}

void ESP8266_SSD1322::drawFastHLineInternal(int16_t x, int16_t y, int16_t w,
		uint16_t color) {
	// Do bounds/limit checks
	if (y < 0 || y >= HEIGHT) {
		return;
	}

	// make sure we don't try to draw below 0
	if (x < 0) {
		w += x;
		x = 0;
	}

	// make sure we don't go off the edge of the display
	if ((x + w) > WIDTH) {
		w = (WIDTH - x);
	}

	// if our width is now negative, punt
	if (w <= 0) {
		return;
	}

	// set up the pointer for  movement through the buffer
	register uint8_t *pBuf = buffer;
	// adjust the buffer pointer for the current row
	pBuf += (x >> 1) + (y * (SSD1322_LCDWIDTH / 2));

	register uint8_t oddmask = color;
	register uint8_t evenmask = (color << 4);
	register uint8_t fullmask = (color << 4) + color;

	uint8_t byteLen = w / 2;

	if (((x % 2) == 0) && ((w % 2) == 0))  // Start at even and length is even
	{
		while (byteLen--)
		{
			*pBuf++ = fullmask;
		}

		return;
	}

	if (((x % 2) == 1) && ((w % 2) == 1)) // Start at odd and length is odd
	{
		register uint8_t b1 = *pBuf;
		b1 &= (x % 2) ? 0xF0 : 0x0F; // cleardown nibble to be replaced

		// write our value in
		*pBuf++ = b1 | oddmask;

		while (byteLen--)
		{
			*pBuf++ = fullmask;
		}
		return;
	}

	if (((x % 2) == 0) && ((w % 2) == 1)) // Start at even and length is odd
	{
//		Serial.println("Start at even and length is odd");

		while (byteLen--)
		{
			*pBuf++ = fullmask;
		}

		register uint8_t b1 = *pBuf;
		b1 &= 0x0F; // cleardown nibble to be replaced

		// write our value in
		*pBuf++ = b1 | evenmask;
		return;
	}

	if (((x % 2) == 1) && ((w % 2) == 0)) // Start at odd and length is even
	{
		register uint8_t b1 = *pBuf;
		b1 &= (x % 2) ? 0xF0 : 0x0F; // cleardown nibble to be replaced

		// write our value in
		*pBuf++ = b1 | oddmask;

		while (byteLen--)
		{
			*pBuf++ = fullmask;
		}

		b1 = *pBuf;
		b1 &= 0x0F; // cleardown nibble to be replaced

		// write our value in
		*pBuf++ = b1 | evenmask;
		return;
	}

	return;
}

void ESP8266_SSD1322::drawFastVLine(int16_t x, int16_t y, int16_t h,
		uint16_t color) {
	bool bSwap = false;
	switch (rotation) {
	case 0:
		break;
	case 1:
		// 90 degree rotation, swap x & y for rotation, then invert x and adjust x for h (now to become w)
		bSwap = true;
		swap(x, y)
		;
		x = WIDTH - x - 1;
		x -= (h - 1);
		break;
	case 2:
		// 180 degree rotation, invert x and y - then shift y around for height.
		x = WIDTH - x - 1;
		y = HEIGHT - y - 1;
		y -= (h - 1);
		break;
	case 3:
		// 270 degree rotation, swap x & y for rotation, then invert y
		bSwap = true;
		swap(x, y)
		;
		y = HEIGHT - y - 1;
		break;
	}

	if (bSwap) {
		drawFastHLineInternal(x, y, h, color);
	} else {
		drawFastVLineInternal(x, y, h, color);
	}
}

void ESP8266_SSD1322::drawFastVLineInternal(int16_t x, int16_t __y,
		int16_t __h, uint16_t color) {

	// do nothing if we're off the left or right side of the screen
	if (x < 0 || x >= WIDTH) {
		return;
	}

	// make sure we don't try to draw below 0
	if (__y < 0) {
		// __y is negative, this will subtract enough from __h to account for __y being 0
		__h += __y;
		__y = 0;

	}

	// make sure we don't go past the height of the display
	if ((__y + __h) > HEIGHT) {
		__h = (HEIGHT - __y);
	}

	// if our height is now negative, punt
	if (__h <= 0) {
		return;
	}

	// this display doesn't need ints for coordinates, use local byte registers for faster juggling
	register uint8_t y = __y;
	register uint8_t h = __h;

	// set up the pointer for fast movement through the buffer
	register uint8_t *pBuf = buffer;
	// adjust the buffer pointer for the current row
	pBuf += (x >> 1) + (y  * (SSD1322_LCDWIDTH / 2));

	register uint8_t mask = ((x % 2) ? color : color << 4);

	while (h--)
	{
		register uint8_t b1 = *pBuf;
		b1 &= (x % 2) ? 0xF0 : 0x0F; // cleardown nibble to be replaced

		// write our value in
		*pBuf = b1 | mask;

		// adjust the buffer forward to next row worth of data
		pBuf += SSD1322_LCDWIDTH / 2;

	};
}

/**
 * Fill the display with the specified colour by setting
 * every pixel to the colour.
 * @param colour - fill the display with this colour.
 */
void ESP8266_SSD1322::fill(uint8_t colour)
{
    uint8_t x,y;

    ssd1322_command(SSD1322_SETCOLUMNADDR);
    ssd1322_data(MIN_SEG);
    ssd1322_data(MAX_SEG);

    ssd1322_command(SSD1322_SETROWADDR);
    ssd1322_data(0);
    ssd1322_data(63);

    colour = (colour & 0x0F) | (colour << 4);

    ssd1322_command(SSD1322_WRITERAM);

	for(y=0; y<64; y++)
    {
		for(x=0; x<64; x++)
		{
		    ssd1322_data(colour);
		    ssd1322_data(colour);
		}
    }
    delay(0);
}

// Right now limits are bitmap is even size on the X and must be placed evenly on the x coordinate
void ESP8266_SSD1322::fastDrawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint8_t color)
{
	// do nothing if we're off the left or right side of the screen
	if (x < 0 || x >= WIDTH) {
		return;
	}

	// calc start pos in the buffer
	register uint8_t *pBuf = &buffer[(x >> 1) + (y * (SSD1322_LCDWIDTH / 2))];
	register uint8_t wInBytes = w >> 1; // Divide by 2, as 2 pixels per byte (4 bits per pixel)
	uint16_t bitPos = 0;

	// loop the height
	for (int lh = 0; lh < h; lh++)
	{
		// loop the width
		for (int lw = 0; lw < wInBytes; lw++)
		{
			*pBuf++ = pgm_read_byte(bitmap + bitPos++);
		}

		pBuf += (SSD1322_LCDWIDTH / 2) - wInBytes; // Move buffer position to next row
	}
}


/*
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//  Gray Scale Table Setting (Full Screen)
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
void Set_Gray_Scale_Table()
{
	oled_Command_25664(0xB8);			// Set Gray Scale Table
	oled_Data_25664(0x0C);			//   Gray Scale Level 1
	oled_Data_25664(0x18);			//   Gray Scale Level 2
	oled_Data_25664(0x24);			//   Gray Scale Level 3
	oled_Data_25664(0x30);			//   Gray Scale Level 4
	oled_Data_25664(0x3C);			//   Gray Scale Level 5
	oled_Data_25664(0x48);			//   Gray Scale Level 6
	oled_Data_25664(0x54);			//   Gray Scale Level 7
	oled_Data_25664(0x60);			//   Gray Scale Level 8
	oled_Data_25664(0x6C);			//   Gray Scale Level 9
	oled_Data_25664(0x78);			//   Gray Scale Level 10
	oled_Data_25664(0x84);			//   Gray Scale Level 11
	oled_Data_25664(0x90);			//   Gray Scale Level 12
	oled_Data_25664(0x9C);			//   Gray Scale Level 13
	oled_Data_25664(0xA8);			//   Gray Scale Level 14
	oled_Data_25664(0xB4);			//   Gray Scale Level 15

	oled_Command_25664(0x00);			// Enable Gray Scale Table
}

*/
