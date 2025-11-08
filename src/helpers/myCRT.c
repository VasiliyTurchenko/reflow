/**
  ******************************************************************************
  * @file myCRT.c
  * @author Vasiliy Turchenko
  * @version V0.0.1
  * @date     23/03/2015
  * @note MISRA passed, deviations marked
  ******************************************************************************
**/

// myCRT.c
// implementation of simple VT-100 style
// colors and text attributes
// for usb vcp output
// 23/03/2015

#include "myCRT.h"

// set text foreground color
void CRT_textColor(uint8_t newColor)
{
    /* MISRA violation  - Prohibited escape sequence usage [MISRA 2012 Rule 4.1, required] */
    xputs("\033[0m\033["); // start of ESC-seq
    if (newColor >= (uint8_t)0x10) {
        /* MISRA violation  - Prohibited escape sequence usage [MISRA 2012 Rule 4.1, required] */
        xputs("1m\033["); // second part of ESC-seq
    }; // of if
    switch (newColor) {
    case cBLACK:
    case cBOLDBLACK: /* thrird part*/
        xputs("30m");
        break;
    case cRED:
    case cBOLDRED:
        xputs("31m");
        break;
    case cGREEN:
    case cBOLDGREEN:
        xputs("32m");
        break;
    case cYELLOW:
    case cBOLDYELLOW:
        xputs("33m");
        break;
    case cBLUE:
    case cBOLDBLUE:
        xputs("34m");
        break;
    case cMAGENTA:
    case cBOLDMAGENTA:
        xputs("35m");
        break;
    case cCYAN:
    case cBOLDCYAN:
        xputs("36m");
        break;
    case cWHITE:
    case cBOLDWHITE:
        xputs("37m");
        break;
    default:
        xputs("37m");
        break;
    }
}

// set text background color
void CRT_textBackground(uint8_t newColor)
{
    /* MISRA violation  - Prohibited escape sequence usage [MISRA 2012 Rule 4.1, required] */
    xputs("\033["); // start of ESC-seq
    switch (newColor) {
    case cBLACK:
    case cBOLDBLACK: /* thrird part*/
        xputs("40m");
        break;
    case cRED:
    case cBOLDRED:
        xputs("41m");
        break;
    case cGREEN:
    case cBOLDGREEN:
        xputs("42m");
        break;
    case cYELLOW:
    case cBOLDYELLOW:
        xputs("43m");
        break;
    case cBLUE:
    case cBOLDBLUE:
        xputs("44m");
        break;
    case cMAGENTA:
    case cBOLDMAGENTA:
        xputs("45m");
        break;
    case cCYAN:
    case cBOLDCYAN:
        xputs("46m");
        break;
    case cWHITE:
    case cBOLDWHITE:
        xputs("47m");
        break;
    default:
        xputs("40m");
        break;
    }
}

/* ################################# EOF ##########################################################*/
