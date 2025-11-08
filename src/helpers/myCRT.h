/**
  ******************************************************************************
  * @file myCRT.h
  * @author Vasiliy Turchenko
  * @version V0.0.1
  * @date     23/03/2015
  ******************************************************************************
**/

// myCRT.h
// implementation of simple VT-100 style
// cursor positioning, colors and text attributes
// for usb vcp output
// 23/03/2015

#ifndef MYCRT_H
#define MYCRT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include <stdint.h>
#include "xprintf.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

// COLORS
#define cBLACK (uint8_t)0x00	   /* Black */
#define cRED (uint8_t)0x01		   /* Red */
#define cGREEN (uint8_t)0x02	   /* Green */
#define cYELLOW (uint8_t)0x03	   /* Yellow */
#define cBLUE (uint8_t)0x04		   /* Blue */
#define cMAGENTA (uint8_t)0x05	   /* Magenta */
#define cCYAN (uint8_t)0x06		   /* Cyan */
#define cWHITE (uint8_t)0x07	   /* White */
#define cBOLDBLACK (uint8_t)0x10   /* Bold Black */
#define cBOLDRED (uint8_t)0x11	   /* Bold Red */
#define cBOLDGREEN (uint8_t)0x12   /* Bold Green */
#define cBOLDYELLOW (uint8_t)0x13  /* Bold Yellow */
#define cBOLDBLUE (uint8_t)0x14	   /* Bold Blue */
#define cBOLDMAGENTA (uint8_t)0x15 /* Bold Magenta */
#define cBOLDCYAN (uint8_t)0x16	   /* Bold Cyan */
#define cBOLDWHITE (uint8_t)0x17   /* Bold White */

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void CRT_textColor(uint8_t newColor);
void CRT_textBackground(uint8_t newColor);

/* Clear the screen */
static inline void CRT_clearScreen(void)
{
	/* MISRA violation  - Prohibited escape sequence usage [MISRA 2012 Rule 4.1, required] */
	xputs("\033[2J");
}

/**
 * @brief CRT_resetToDefaults
 */
static inline void CRT_resetToDefaults(void)
{
	extern _Bool nice_mode;
	/* MISRA violation  - Prohibited escape sequence usage [MISRA 2012 Rule 4.1, required] */
	if (nice_mode) {
		xputs("\033[39;49m");
	}
}

static inline void CRT_resetAllAttr(void)
{
	/* MISRA violation  - Prohibited escape sequence usage [MISRA 2012 Rule 4.1, required] */
	xputs("\033[0m");
}

// move cursor to UL corner
static inline void CRT_cursorHome(void)
{
	/* MISRA violation  - Prohibited escape sequence usage [MISRA 2012 Rule 4.1, required] */
	xputs("\033[0;0f");
}

// clear all from current position to end of the linr
static inline void CRT_clrEOL(void)
{
	/* MISRA violation  - Prohibited escape sequence usage [MISRA 2012 Rule 4.1, required] */
	xputs("\033[K");
}

// bells :)
static inline void CRT_bell(void)
{
	xputc((char)0x07);
}

#ifdef __cplusplus
}
#endif

#endif /* MYCRT_H */

/************************ END OF FILE ******************************************************/
