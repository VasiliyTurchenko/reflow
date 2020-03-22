/**
  ******************************************************************************
  * File Name          : framebuffer.h
  * Description        : This file contains all the functions prototypes for
  *                      the framebuffer-based functions
  ******************************************************************************
	*	v.0.0.1
	* 16-11-2017
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>
#include <stdbool.h>
//#include <stdio.h>
#include <limits.h>

#ifdef		USE_FRAMEBUFFER

#define	MAX_CHAR_SIZE		32				//	max char size 32*32 pix
#define	TFS                     (MAX_CHAR_SIZE/CHAR_BIT)+1
#define INVERT		0x01
#define NORM		0x00
#define UNDERL		0x01 << 1
#define UPSDN		0x01 << 2



/* Structures used in the module ---------------------------------------------*/

typedef struct {			// Character generator parameters
	uint8_t		CharWidth;	// Visible symbol width in pixels
	uint8_t		CharHeight;	// Visible symbol height in pixels
	char		BytesPerChar;	// Size of the char image in bytes
	char		StartCharCode;	// Code of the first char in generator memmory
	char		EndCharCode;	// Code of the last char in generator memmory
	const uint8_t		*pROM;		// pointer to character generator array
}	Struct_CharGen;

typedef	Struct_CharGen*		ptr_chargen;

typedef struct struct_FrameBuffer {	// () Framebuffer parameters
	unsigned char	*pFB;		// pointer to the framebuffer[0]
	uint32_t	FBWidth;	// framefuffer width in pixels
	uint32_t	FBWidthInBytes;	// framefuffer width in bytes
	uint32_t	FBHeight;	// framebuffee height in pixels
	uint32_t	ScreenHeight; 	// visible area height in pixels
	uint32_t	FBy0;		// start y coordinate (in pixels) of the visible area
	uint8_t		(*fUpdateScreen)(void*, struct struct_FrameBuffer*);	// pointer to the update hardware screen function
	uint32_t	BitsPerPixel;	// how many bits per pixel
	enum		{PIO, I2C, SPI} FBiface;
	bool		Locked;
	bool		FBOrientatonX;	// X - pixels in bytes along X; !X - along Y

}	struct_FrameBuffer;

typedef	struct_FrameBuffer*		ptr_fb;

typedef struct {
	uint32_t	CursorPosX;	// current cursor position X
	uint32_t	CursorPosY;	// current cursor position X
	bool		CursorVisible;	//  visibility of the cursor
	bool		Blinking;	// blinking
	enum		{CLine, CBox} CursorShape ; // cursor shape
}	struct_Cursor;

typedef	struct_Cursor*				ptr_cursor;

typedef	struct	{																	// context of the virtual output device
	ptr_fb		pDevFB;
	ptr_chargen	pDevCHG;
	ptr_cursor	pDevCursor;
} struct_Context;

typedef struct_Context*				ptr_context;

// Exported constants --------------------------------------------------------

extern 		struct_Context*		context1;

// Private macros --------------------------------------------------------


// Exported macro ------------------------------------------------------------



// Exported functions --------------------------------------------------------

uint8_t	FB_Init(void);

// shifts bit field "* field" of "flen" bytes left by "ns" times
uint8_t sbfl(uint8_t *field, uint32_t flen, uint32_t ns);

// shifts bit field "* field" of "flen" bytes right by "ns" times
uint8_t sbfr(uint8_t *field, uint32_t flen, uint32_t ns);

// fill byte string bitwise form left to right
uint8_t fbfl(uint8_t *field, uint32_t flen, uint32_t ns);

// fill byte string bitwise from right to left -----------------
uint8_t fbfr(uint8_t *field, uint32_t flen, uint32_t ns);

// Puts the char and moves cursor to the next pos
uint8_t	PutChar(char sym, uint8_t mode);

uint8_t	PutChar1(char sym, uint8_t mode);	// Puts the char and moves cursor to the next pos

uint8_t gotoXY(uint32_t x, uint32_t y);

uint8_t whereXY(uint32_t *x, uint32_t *y);

/* prints the asciiz string into current context, returns the actual number of printed symbols
 * correctly deals with \n
 * 25-11-2017 15:53
 */
uint32_t zprint(const char *sz, uint8_t mode);

/*
 * The function draws a point with color
 * 25-11-2017 20:52
 * It does not move the cursor!
 * This function must be used when the pixels are grouped in bytes along X axis!
 */
uint32_t zpointX(uint32_t x, uint32_t y, uint32_t color);

/*
 * The function draws a point with color
 * 25-11-2017 20:52
 * 06-12-2017 21:52
 * It does not move the cursor!
 * This function must be used when the pixels are grouped in bytes along Y axis!
 */
uint32_t zpointY(uint32_t x, uint32_t y, uint32_t color);

/* The function draws a straight line from x0,y0 to x1,y1
 * Uses the Bresenham algorithm from https://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm#C
 * 25-11-2017 21:44
 */
uint32_t zline(int x0, int y0, int x1, int y1, uint32_t color);

/*
 * 20-Jan-2020
 */
uint8_t fast_clear_screen(void);

#endif	/* USE_FRAMEBUFFER */
#endif /* __framebuffer_H */
/************************ (C) COPYRIGHT turchenkov@gmail.com *****END OF FILE****/
