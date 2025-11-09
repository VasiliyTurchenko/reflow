/**
  ******************************************************************************
  * File Name          : framebuffer.c
  * Description        : This file provides code for all the functions
  *                      related to framebuffer operations
  ******************************************************************************
  * v.0.0.1
        * 17/11/2017
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "globaldef.h"
#include "framebuffer.h"

#ifdef USE_FRAMEBUFFER

#include <string.h>
//#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "ssd1306.h"

#include "font8x8-master/font8x8_basic.h"

static struct_Cursor      myCur;
static struct_FrameBuffer fb1;
static Struct_CharGen     cg8x8;                                  /// for the 8x8 char generator
static struct_Context     OLEDcontext = { &fb1, &cg8x8, &myCur }; // the first device context

struct_Context *context1 = &OLEDcontext;

unsigned char FrameBuffer1[(128U / CHAR_BIT) * (64U)];

/* ---------------- initializes the framebuffer ----------------------------- */
uint8_t FB_Init(void)
{
    context1->pDevFB->pFB            = &FrameBuffer1[0];
    context1->pDevFB->FBWidth        = 128;
    context1->pDevFB->FBWidthInBytes = 128;
    context1->pDevFB->FBHeight       = 64;
    context1->pDevFB->ScreenHeight   = 64;
    context1->pDevFB->FBy0           = 0;
    context1->pDevFB->BitsPerPixel   = 1;
    context1->pDevFB->fUpdateScreen  = UpdateSSD1306;
    context1->pDevFB->FBiface        = SPI;
    context1->pDevFB->FBOrientatonX  = false;
    context1->pDevFB->Locked         = false;

    context1->pDevCHG->BytesPerChar  = 8;
    context1->pDevCHG->CharHeight    = 8;
    context1->pDevCHG->CharWidth     = 8;
    context1->pDevCHG->EndCharCode   = 127;
    context1->pDevCHG->StartCharCode = 0;
    context1->pDevCHG->pROM          = &font8x8_basic[0][0];

    context1->pDevCursor->Blinking      = false;
    context1->pDevCursor->CursorPosX    = 0;
    context1->pDevCursor->CursorPosY    = 0;
    context1->pDevCursor->CursorVisible = true;
    context1->pDevCursor->CursorShape   = CBox;
    return 0;
}
/* ---------------- end of uint8_t	FB_Init(ptr_context);	--------------*/

/**
 * @brief sbfl shifts bit field "* field" of "flen" bytes left by "ns" times
 * @param field
 * @param flen
 * @param ns number of shifts
 * @return
 */
uint8_t sbfl(uint8_t *field, uint32_t flen, uint32_t ns)
{
    ns = (ns > (flen * CHAR_BIT)) ? (flen * CHAR_BIT) : ns; // limit number of shifts
    uint32_t sns;
    sns = ns;
    uint32_t i;
    uint32_t carry_flag;
    uint32_t next;
    while (ns > 0) {
        carry_flag = 0;
        next       = 0;
        for (i = 0; i < flen; ++i) {
            next = (*(field + i) & 0x80) ? 0x01 : 0x00;
            //			*(field + i) = (*(field + i) << 1U) | carry_flag;
            field[i]   = (uint8_t)(field[i] << 1U) | (uint8_t)carry_flag;
            carry_flag = next;
        }
        ns--;
    }
    return (uint8_t)sns; // return the actual nr of shifts
}

/**
 * @brief sbfr shifts bit field "* field" of "flen" bytes right by "ns" times
 * @param field
 * @param flen
 * @param ns
 * @return
 */
uint8_t sbfr(uint8_t *field, uint32_t flen, uint32_t ns)
{
    ns = (ns > flen * CHAR_BIT) ? flen * CHAR_BIT : ns; // limit number of shifts
    uint32_t sns;
    sns = ns;
    uint32_t i;
    uint32_t carry_flag;
    uint32_t next;
    while (ns > 0) {
        carry_flag = 0;
        next       = 0;
        for (i = flen; i > 0; --i) {
            next = (*(field + i - 1) & 0x01) ? 0x80 : 0x00;
            //			*(field + i - 1) = (*(field + i - 1U) >> 1U) | carry_flag;
            field[i - 1] = (uint8_t)(field[i - 1] >> 1U) | (uint8_t)carry_flag;
            carry_flag   = next;
        }
        ns--;
    }
    return (uint8_t)sns; // return the actual nr of shifts
}

/**
 * @brief fbfl fill byte string bitwise form left to right
 * @param field
 * @param flen
 * @param ns
 * @return
 */
uint8_t fbfl(uint8_t *field, uint32_t flen, uint32_t ns)
{
    ns = (ns > flen * CHAR_BIT) ? flen * CHAR_BIT : ns; // limit number of shifts
    uint32_t sns;
    sns = ns;
    uint32_t i;
    uint32_t carry_flag;
    uint32_t next;

    while (ns > 0) {
        carry_flag = 1U;
        next       = 0;
        for (i = 0; i < flen; ++i) {
            next = (*(field + i) & 0x80) ? 0x01 : 0x00;
            //			*(field + i) = (*(field + i) << 1U) | carry_flag;
            field[i]   = (uint8_t)(field[i] << 1U) | (uint8_t)carry_flag;
            carry_flag = next;
        }
        ns--;
    }
    return (uint8_t)sns;
}

/**
 * @brief fbfr fill byte string bitwise from right to left
 * @param field
 * @param flen
 * @param ns
 * @return
 */
uint8_t fbfr(uint8_t *field, uint32_t flen, uint32_t ns)
{
    ns = (ns > flen * CHAR_BIT) ? flen * CHAR_BIT : ns; // limit number of shifts
    uint32_t sns;
    sns = ns;
    uint32_t i;
    uint32_t carry_flag;
    uint32_t next;
    while (ns > 0) {
        carry_flag = 1U;
        next       = 0;
        for (i = flen - 1U; i >= flen; --i) {
            next = (*(field + i) & 0x01U) ? 0x80U : 0x00U;
            //			*(field + i) = (*(field + i) >> 1U) | carry_flag;
            field[i]   = (uint8_t)(field[i] >> 1U) | (uint8_t)carry_flag;
            carry_flag = next;
        }
        ns--;
    }
    return (uint8_t)sns; // return the actual nr of shifts
}

/**
 * @brief PutChar Puts the char and moves cursor to the next pos
 * @param sym
 * @param mode
 * @return
 */
uint8_t PutChar(char sym, uint8_t mode)
{
    // check is char out of char gen table
    if ((sym < context1->pDevCHG->StartCharCode) || (sym > context1->pDevCHG->EndCharCode))
        return 1; // no such symbol
    if ((context1->pDevFB->FBy0 + context1->pDevCursor->CursorPosY) >= context1->pDevFB->FBHeight)
        return 2; // cursor is out of the framebuffer y limit
    if (context1->pDevCursor->CursorPosX >= context1->pDevFB->FBWidth)
        return 3; //  cursor is out of the framebuffer x limit

    uint8_t tmp_field[MAX_CHAR_SIZE][TFS]; // the room to hold char to be out
    uint8_t mask_field[TFS];               // the room for bitmask
    // 1. Copy symbol to temporary location
    uint32_t xcount, ycount;
    xcount = (context1->pDevCHG->CharWidth) / CHAR_BIT; // nr of full bytes
    if ((context1->pDevCHG->CharWidth) % CHAR_BIT > 0)
        xcount++; //	there is one extra NON-FULL byte in the row
    if (xcount > (MAX_CHAR_SIZE / CHAR_BIT))
        xcount = (MAX_CHAR_SIZE / CHAR_BIT);
    ycount = context1->pDevCHG->CharHeight;
    if (ycount > (MAX_CHAR_SIZE))
        ycount = (MAX_CHAR_SIZE);

    uint32_t i, j;
    // 2. Calculate bit field left shift if target location is not byte-aligned
    uint32_t bitshifts;
    bitshifts = (context1->pDevCursor->CursorPosX) % CHAR_BIT; // amount of the right shifts
    uint8_t sinv;                                              // invert
    sinv = (mode & INVERT) * 0xff;

    for (j = 0; j < ycount; j++) {
        memset(tmp_field[j], 0, TFS); // clear the dest memory
        for (i = 0; i < xcount; i++) {
            tmp_field[j][i] = sinv ^ *((context1->pDevCHG->pROM) +
                                       sym * context1->pDevCHG->BytesPerChar + i + j);
        }
    }
    if (bitshifts > 0) {
        // 3. Shift image left by bitshifts times
        for (j = 0; j < ycount; j++) {
            sbfl(&tmp_field[j][0], TFS, bitshifts);
        }
    }
    // 4. Calculate mask for output
    memset(mask_field, 0, TFS);
    fbfl(&mask_field[0], TFS, context1->pDevCHG->CharWidth);
    sbfl(&mask_field[0], TFS, bitshifts);

    if (mode & UNDERL) {
        for (i = 0; i < xcount; i++) {
            tmp_field[ycount - 1][i] = mask_field[i];
        }
    }

    // 5. Calculate byte offset to CursorPosition Byte in the framebuffer
    uint32_t poc; // pixel_offset_to_cursor
    poc = (context1->pDevFB->FBy0 + context1->pDevCursor->CursorPosY) * context1->pDevFB->FBWidth +
          context1->pDevCursor->CursorPosX;                  // offset to cursor in pixels
    uint32_t boc;                                            // byte_offset_to_cursor_position
    boc = (poc * context1->pDevFB->BitsPerPixel) / CHAR_BIT; //  from 0
    // 6. Calculate phys. limits of the framebuffer
    uint32_t incY, incX;
    incY = context1->pDevFB->FBWidthInBytes; // Y distance between points with same X coordinate
    incX = sizeof(unsigned char);
    uint32_t bocl; // byte_offset_to_cursor_in_line
    bocl = (context1->pDevCursor->CursorPosX) / CHAR_BIT;
    // 7. Update framebuffer
    uint8_t tb;
    for (j = 0; j < ycount; j++) {
        if ((j + context1->pDevCursor->CursorPosY) < context1->pDevFB->FBHeight) {
            for (i = 0; i <= xcount; i++) {
                if ((i + bocl) < context1->pDevFB->FBWidthInBytes) {
                    tb = *((context1->pDevFB->pFB) + i * incX + j * incY + boc);
                    tb = tb & ~mask_field[i];
                    tb = (tmp_field[j][i] & mask_field[i]) | tb;
                    *((context1->pDevFB->pFB) + i * incX + j * incY + boc) = tb;
                }
            }
        } else
            break;
    } // of for j
    return 0U;
}

/**
 * @brief gotoXY
 * @param x
 * @param y
 * @return
 */
uint8_t gotoXY(uint32_t x, uint32_t y)
{
    uint8_t res;
    res = 0;
    if (x < context1->pDevFB->FBWidth) {
        context1->pDevCursor->CursorPosX = x;
    } else
        res = 1;
    if (y < context1->pDevFB->FBHeight) {
        context1->pDevCursor->CursorPosY = y;
    } else
        res = 1;
    return res;
}

/**
 * @brief whereXY
 * @param x
 * @param y
 * @return
 */
uint8_t whereXY(uint32_t *x, uint32_t *y)
{
    *x = context1->pDevCursor->CursorPosX;
    *y = context1->pDevCursor->CursorPosY;
    return 0;
}

/**
 * @brief fast_clear_screen
 * @return
 */
uint8_t fast_clear_screen(void)
{
    size_t bufsize = (128U / CHAR_BIT) * (64U);
    for (size_t i = 0U; i < bufsize; i++) {
        FrameBuffer1[i] = 0U;
    }
    return 0U;
}

/**
 * @brief zprint prints the asciiz string into current context, returns the actual number of printed symbols
 * @brief correctly deals with \n
 * @brief 25-11-2017 15:53
 * @param sz
 * @param mode
 * @return
 */
uint32_t zprint(const char *sz, uint8_t mode)
{
    uint8_t dx;
    uint8_t dy;
    dx = context1->pDevCHG->CharWidth;
    dy = context1->pDevCHG->CharHeight;
    uint32_t newx;
    uint32_t newy;
    whereXY(&newx, &newy);
    uint32_t cnt;
    cnt = 0;
    uint32_t i;
    i = 0;
    while (*(sz + i) != 0U) {
        if (*(sz + i) == 0x0A) {
            newx = 0U;
            newy = newy + dy;
            if (gotoXY(newx, newy) == 0U) {
                cnt++;
            } else
                break;
        } else {
            //			if (PutChar(*(sz+i),mode) == 0 ) {
            if (PutChar1(*(sz + i), mode) == 0U) {
                cnt++;
                if (newx + dx >= context1->pDevFB->FBWidth) {
                    newy = newy + dy;
                    newx = 0U;
                } else {
                    newx = newx + dx;
                }
                if (gotoXY(newx, newy) != 0U)
                    break;
            }
        }
        i++;
    }
    return cnt;
}

/*
 * The function draws a point with color
 * 25-11-2017 20:52
 * It does not move the cursor!
 * This function must be used when the pixels are grouped in bytes along X axis!
 */
uint32_t zpointX(uint32_t x, uint32_t y, uint32_t color)
{
    uint32_t res;
    res = 0;
    uint32_t tmpcolor;
    tmpcolor = (color > 0) ? 1 : 0;
    //	uint32_t poc; // pixel_offset_to_dest point
    res = (x >= context1->pDevFB->FBWidth) ? 1 : 0;
    res = ((context1->pDevFB->FBy0 + y) >= context1->pDevFB->FBHeight) ? 1 : 0;
    res = (context1->pDevFB->FBOrientatonX == false) ? 1 : 0;

    if (!res) {
        uint32_t poc; // pixel_offset_to_dest point
        poc = (context1->pDevFB->FBy0 + y) * context1->pDevFB->FBWidth + x; //
        uint32_t boc;                                            // byte_offset_to_dest point
        boc = (poc * context1->pDevFB->BitsPerPixel) / CHAR_BIT; // from 0
        uint32_t pib;                                            // position in the byte
        pib = x % CHAR_BIT;
        if (tmpcolor > 0) {
            *((context1->pDevFB->pFB) + boc) |= (0x01 << pib);
        } else {
            *((context1->pDevFB->pFB) + boc) &= ~(0x01 << pib);
        }
    }
    return res;
}

/*
 * The function draws a point with color
 * 25-11-2017 20:52
 * 06-12-2017 21:52
 * It does not move the cursor!
 * This function must be used when the pixels are grouped in bytes along Y axis!
 */
uint32_t zpointY(uint32_t x, uint32_t y, uint32_t color)
{
    uint32_t res;
    res = 0;
    uint32_t tmpcolor;
    tmpcolor = (color > 0) ? 1 : 0;
    res      = (x >= context1->pDevFB->FBWidth) ? 1 : 0;
    res      = ((context1->pDevFB->FBy0 + y) >= context1->pDevFB->FBHeight) ? 1 : 0;
    res      = (context1->pDevFB->FBOrientatonX == true) ? 1 : 0;

    if (!res) {
        uint32_t boc; // byte_offset_to_dest point
        boc = ((context1->pDevFB->FBy0 + y) / CHAR_BIT) * context1->pDevFB->FBWidth + x;
        uint32_t pib; // position in the byte
        pib = (context1->pDevFB->FBy0 + y) % CHAR_BIT;
        if (tmpcolor > 0) {
            *((context1->pDevFB->pFB) + boc) |= (0x01 << pib);
        } else {
            *((context1->pDevFB->pFB) + boc) &= ~(0x01 << pib);
        }
    }
    return res;
}

/**
 * @brief PutChar1 Puts the char and moves cursor to the next pos
 * @param sym
 * @param mode
 * @return
 */
uint8_t PutChar1(char sym, uint8_t mode)
{
    // check is char out of char gen table
    if ((sym < context1->pDevCHG->StartCharCode) || (sym > context1->pDevCHG->EndCharCode))
        return 1; // no such symbol
    if ((context1->pDevFB->FBy0 + context1->pDevCursor->CursorPosY) >= context1->pDevFB->FBHeight)
        return 2; // cursor is out of the framebuffer y limit
    if (context1->pDevCursor->CursorPosX >= context1->pDevFB->FBWidth)
        return 3; //  cursor is out of the framebuffer x limit

    uint32_t xc;             // x counter of the pixels
    uint32_t yc;             // y counter of the pixels
    uint8_t  tmp_field[TFS]; // the room to hold char row to be out
#define STARTX (context1->pDevCursor->CursorPosX)
#define STARTY (context1->pDevCursor->CursorPosY + context1->pDevFB->FBy0)
#define ENDX (context1->pDevCursor->CursorPosX + context1->pDevCHG->CharWidth)
#define ENDY                                                                                       \
    (context1->pDevCursor->CursorPosY + context1->pDevFB->FBy0 + context1->pDevCHG->CharHeight)
    uint32_t xcount;
    uint32_t ycount;
    xcount = (context1->pDevCHG->CharWidth) / CHAR_BIT; // nr of full bytes
    if ((context1->pDevCHG->CharWidth) % CHAR_BIT > 0)
        xcount++; //	there is one extra NON-FULL byte in the row
    if (xcount > (MAX_CHAR_SIZE / CHAR_BIT))
        xcount = (MAX_CHAR_SIZE / CHAR_BIT);
    ycount = context1->pDevCHG->CharHeight;
    if (ycount > (MAX_CHAR_SIZE))
        ycount = (MAX_CHAR_SIZE);
    uint32_t i;
    i = 0;
    uint32_t j;
    j = 0;
    uint8_t sinv; // invert
    sinv = (mode & INVERT) * 0xff;
    uint8_t underl;
    underl = (mode & UNDERL) ? 0xff : 0x00;

    for (yc = STARTY; yc < ENDY; yc++) {
        for (i = 0; i < xcount; i++) {
            tmp_field[i] = sinv ^ *((context1->pDevCHG->pROM) +
                                    sym * context1->pDevCHG->BytesPerChar + i + j);
            if (j == ycount - 1)
                tmp_field[i] |= underl;
        }
        for (xc = STARTX; xc < ENDX; xc++) {
            zpointY(xc, yc, ((tmp_field[0] & 0x01) ? 1 : 0));
            sbfr(&tmp_field[0], TFS,
                 1); // shifts bit field "* field" of "flen" bytes right by "ns" times
        }
        j++;
    }
#undef STARTX
#undef STARTY
#undef ENDX
#undef ENDY
    return 0;
}

/*
 * The function draws a straight line from x0,y0 to x1,y1
 * Uses the Bresenham algorithm from https://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm#C
 * 25-11-2017 21:44
 */
uint32_t zline(int x0, int y0, int x1, int y1, uint32_t color)
{
    uint32_t res;
    res = 0;

    uint32_t tmpcolor;
    tmpcolor = (color > 0) ? 1 : 0;

    int dx;
    int dy;
    int sx;
    int sy;

    dx = abs(x1 - x0);
    sx = (x0 < x1) ? 1 : -1;
    dy = abs(y1 - y0);
    sy = (y0 < y1) ? 1 : -1;

    int err;
    int e2;
    err = (dx > dy ? dx : -dy) / 2;
    for (;;) {
        zpointY((uint32_t)x0, (uint32_t)y0, tmpcolor);
        if (x0 == x1 && y0 == y1)
            break;
        e2 = err;
        if (e2 > -dx) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dy) {
            err += dx;
            y0 += sy;
        }
    }

    return res;
}
// ----------------- end of zline ----------------
///**
// * @brief sbfl shifts bit field "* field" of "flen" bytes left by "ns" times
// * @param field
// * @param flen
// * @param ns
// * @return the actual nr of shifts
// */
//size_t sbfl(uint8_t *field, uint32_t flen, size_t ns)
//{
//	ns = (ns > (flen * CHAR_BIT)) ? (flen * CHAR_BIT) :
//					ns; // limit number of shifts
//	size_t sns;
//	sns = ns;
//	size_t i;
//	uint8_t carry_flag;
//	uint8_t next;
//	while (ns > 0) {
//		carry_flag = 0;
//		next = 0;
//		for (i = 0; i < flen; ++i) {
//			next = ((field[i] & 0x80u) != 0u) ? 0x01u : 0x00u;
//			field[i] = (uint8_t)(field[i] << 1u) | carry_flag;
//			carry_flag = next;
//		}
//		ns--;
//	};
//	return sns;
//}

///**
// * @brief sbfr shifts bit field "* field" of "flen" bytes right by "ns" times
// * @param field
// * @param flen
// * @param ns
// * @return the actual nr of shifts
// */
//size_t sbfr(uint8_t *field, uint32_t flen, size_t ns)
//{
//	ns = (ns > flen * CHAR_BIT) ? flen * CHAR_BIT :
//				      ns; // limit number of shifts
//	size_t sns;
//	sns = ns;
//	size_t i;
//	uint8_t carry_flag;
//	uint8_t next;
//	while (ns > 0) {
//		carry_flag = 0;
//		next = 0;
//		for (i = flen; i > 0; --i) {
//			next = (field[i - 1] & 0x01) ? 0x80 : 0x00;
//			field[i - 1] =
//				(uint8_t)(field[i - 1] >> 1) | carry_flag;
//			carry_flag = next;
//		}
//		ns--;
//	};
//	return sns;
//}

#endif
/************************ (C) COPYRIGHT turchenkov@gmail.com ***** */
/******END OF FILE****/
