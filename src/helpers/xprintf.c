/*------------------------------------------------------------------------/
/  Universal string handler for user console interface
/-------------------------------------------------------------------------/
/
/  Copyright (C) 2011, ChaN, all right reserved.
/
/ * This software is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/
/-------------------------------------------------------------------------*/

/* MISRA not passed - starg.h is the reason */

#include <stddef.h>
#include "xprintf.h"

#ifndef DESKTOP_TARGET
#endif

#if USE_XFUNC_OUT
#include <stdarg.h> /* MISRA violation */

/* pointer to the output function */
void (*xfunc_out)(unsigned char c);

/* Pointer to the output stream */
static char *outptr = NULL;

/*----------------------------------------------*/
/* Put a character                              */
/*----------------------------------------------*/

void xputc(char c)
{
    if ((c == '\n') && (CR_CRLF_ == 1)) {
        /* MISRA violation - recursive call */
        xputc('\r'); /* CR -> CRLF */
    }

    if (outptr != NULL) {
        *outptr = c;
        outptr++;
    } else if (xfunc_out != NULL) {
        xfunc_out((unsigned char)c);
    } else {
        /* stub */
    }
}

/**
 * @brief xputs Put a string to the default device
 * @param str
 */
void xputs(const char *str) /* Pointer to the string */
{
    while (*str != '\0') {
        xputc(*str);
        str++;
    }
}

/**
 * @brief xfputs Put a string to the specified device
 * @param str
 */
void xfputs(void (*func)(unsigned char c), /* Pointer to the output function */
            const char *str)               /* Pointer to the string */
{
    void (*pf)(unsigned char c);

    pf        = xfunc_out; /* Save current output device */
    xfunc_out = func;      /* Switch output to specified device */
    while (*str != '\0') { /* Put the string */
        xputc(*str);
        str++;
    }
    xfunc_out = pf; /* Restore output device */
}

/*----------------------------------------------*/
/* Formatted string output                      */
/*----------------------------------------------*/
/*  xprintf("%d", 1234);			"1234"
	xprintf("%6d,%3d%%", -200, 5);	"  -200,  5%"
	xprintf("%-6u", 100);			"100   "
	xprintf("%ld", 12345678L);		"12345678"
	xprintf("%04x", 0xA3);			"00a3"
	xprintf("%08LX", 0x123ABC);		"00123ABC"
	xprintf("%016b", 0x550F);		"0101010100001111"
	xprintf("%s", "String");		"String"
	xprintf("%-4s", "abc");			"abc "
	xprintf("%4s", "abc");			" abc"
	xprintf("%c", 'a');				"a"
	xprintf("%f", 10.0);            <xprintf lacks floating point support>
*/

/**
 * @brief xvprintf Formatted string output
 * @param fmt
 * @param arp
 */
static void xvprintf(const char *fmt, /* Pointer to the format string */
                     va_list     arp)     /* Pointer to arguments */
{
    char c;

    for (;;) {
        c = *fmt; /* Get a char */
        fmt++;

        if (c == '\0') {
            break; /* End of format? */
        }

        if (c != '%') { /* Pass through it if not a % sequense */
            xputc(c);
            continue;
        }

        /* % format string found */
        unsigned int f = 0U;
        c              = *fmt; /* Get first char of the sequense */
        fmt++;

        if (c == '0') { /* Flag: '0' padded */
            f = 1;
            c = *fmt;
            fmt++;
        } else if (c == '-') { /* Flag: left justified */
            f = 2;
            c = *fmt;
            fmt++;
        } else {
            /* */
        }

        unsigned int w;
        for (w = 0; ((c >= '0') && (c <= '9')); c = *fmt++) {
            /* Minimum width */
            w = (w * 10U) + (unsigned int)c - (unsigned int)'0';
        }

        if ((c == 'l') || (c == 'L')) { /* Prefix: Size is long int */
            f |= 4U;
            c = *fmt;
            fmt++;
        }

        if (c == '\0') {
            break; /* End of format? */
        }

        char d = c;

        if (d >= 'a') {
            d -= ' ';
        }

        const char   *p;
        unsigned int  j         = 0U;
        unsigned int  radix     = 16U;
        unsigned char cont_flag = 0U;
        switch (d) { /* Type is... */
        case 'S':    /* String */
            p = va_arg(arp, char *);
            for (j = 0; p[j] != '\0'; j++) {
                ;
            }
            while (((f & 2U) == 0U) && (j++ < w)) {
                xputc(' ');
            }
            xputs(p);
            while (j++ < w) {
                xputc(' ');
            }
            cont_flag = 1U;
            break;
        case 'C': /* Character */
            xputc((char)va_arg(arp, int));
            cont_flag = 1U;
            break;
        case 'B': /* Binary */
            radix = 2;
            break;
        case 'O': /* Octal */
            radix = 8;
            break;
        case 'D': /* Signed decimal */
        case 'U': /* Unsigned decimal */
            radix = 10;
            break;
        case 'X': /* Hexdecimal */
            radix = 16;
            break;
        default: /* Unknown type (passthrough) */
            xputc(c);
            cont_flag = 1U;
            break;
        }

        if (cont_flag == 1U) {
            continue;
        }

        unsigned long v = 0U;
        /* Get an argument and put it in numeral */
        v = ((f & 4U) != 0U) ?
                    va_arg(arp, long) :
                    ((d == 'D') ? (long)va_arg(arp, int) : (long)va_arg(arp, unsigned int));

        if ((d == 'D') && ((v & 0x80000000U) != 0U)) {
            v = 0 - v; /* MISRA violation */
            f |= 8U;
        }
        unsigned int i = 0;
        char         s[16];

        do {
            d = (char)(v % radix);
            v /= radix;
            if (d > (char)9) {
                d += (c == 'x') ? 0x27 : 0x07;
            }
            s[i] = d + '0';
            i++;
        } while ((v != 0U) && (i < sizeof(s)));

        if ((f & 8U) != 0U) {
            s[i] = '-';
            i++;
        }

        j = i;
        d = ((f & 1U) != 0U) ? '0' : ' ';

        while (((f & 2U) == 0U) && (j++ < w)) {
            xputc(d);
        }

        do {
            i--;
            xputc(s[i]);
        } while (i > 0U);

        while (j++ < w) {
            xputc(' ');
        }
    }
}

/**
 * @brief xprintf Put a formatted string to the default device
 * @param fmt Pointer to the format string
 * @param ... Optional arguments
 */
void xprintf(const char *fmt, ...)
{
    va_list arp;

    va_start(arp, fmt);
    xvprintf(fmt, arp);
    va_end(arp);
}

/**
 * @brief xsprintf Put a formatted string to the memory
 * @param buff Pointer to the output buffer
 * @param fmt Pointer to the format string
 * @param ... Optional arguments
 */
void xsprintf(char *buff, const char *fmt, ...)
{
    va_list arp;

    outptr = buff; /* Switch destination for memory */

    va_start(arp, fmt);
    xvprintf(fmt, arp);
    va_end(arp);

    *outptr = '\0'; /* Terminate output string with a \0 */
    outptr  = NULL; /* Switch destination for device */
}

/**
 * @brief xfprintf
 * @param func Pointer to the output function
 * @param fmt Pointer to the format string
 * @param ... Optional arguments
 */
void xfprintf(void (*func)(unsigned char c), const char *fmt, ...)
{
    va_list arp;
    void (*pf)(unsigned char c);

    pf        = xfunc_out; /* Save current output device */
    xfunc_out = func;      /* Switch output to specified device */

    va_start(arp, fmt);
    xvprintf(fmt, arp);
    va_end(arp);

    xfunc_out = pf; /* Restore output device */
}

/**
 * @brief put_dump Dumps a line of binary dump
 * @param buff Pointer to the array to be dumped
 * @param addr Heading address value
 * @param len Number of items to be dumped
 * @param width Size of the items (DF_CHAR, DF_SHORT, DF_LONG)
 */
void put_dump(const void *buff, unsigned long addr, int len, int width)
{
    int                   i;
    const unsigned char  *bp;
    const unsigned short *sp;
    const unsigned long  *lp;

    xprintf("%08lX ", addr); /* address */

    switch ((size_t)width) {
    case DW_CHAR:
        bp = (const unsigned char *)buff;
        for (i = 0; i < len; i++) { /* Hexdecimal dump */
            xprintf(" %02X", bp[i]);
        }
        xputc(' ');
        for (i = 0; i < len; i++) { /* ASCII dump */
            xputc(((bp[i] >= ' ') && (bp[i] <= '~')) ? bp[i] : '.');
        }
        break;
    case DW_SHORT:
        sp = (const unsigned short *)buff;
        do { /* Hexdecimal dump */
            xprintf(" %04X", *sp++);
        } while ((--len) > 0);
        break;
    case DW_LONG:
        lp = (const unsigned long *)buff;
        do { /* Hexdecimal dump */
            xprintf(" %08LX", *lp++);
        } while ((--len) > 0);
        break;
    default:
        /* */
        break;
    }

    xputc('\n');
}

#endif /* _USE_XFUNC_OUT */

#ifdef _USE_XFUNC_IN
unsigned char (*xfunc_in)(void); /* Pointer to the input stream */

/*----------------------------------------------*/
/* Get a line from the input                    */
/*----------------------------------------------*/

int xgets(            /* 0:End of stream, 1:A line arrived */
          char *buff, /* Pointer to the buffer */
          int   len   /* Buffer length */
)
{
    int c, i;

    if (!xfunc_in)
        return 0; /* No input function specified */

    i = 0;
    for (;;) {
        c = xfunc_in(); /* Get a char from the incoming stream */
        if (!c)
            return 0; /* End of stream? */
        if (c == '\r')
            break;            /* End of line? */
        if (c == '\b' && i) { /* Back space? */
            i--;
            if (_LINE_ECHO)
                xputc(c);
            continue;
        }
        if (c >= ' ' && i < len - 1) { /* Visible chars */
            buff[i++] = c;
            if (_LINE_ECHO)
                xputc(c);
        }
    }
    buff[i] = 0; /* Terminate with a \0 */
    if (_LINE_ECHO)
        xputc('\n');
    return 1;
}

int xfgets(                             /* 0:End of stream, 1:A line arrived */
           unsigned char (*func)(void), /* Pointer to the input stream function */
           char *buff,                  /* Pointer to the buffer */
           int   len                    /* Buffer length */
)
{
    unsigned char (*pf)(void);
    int n;

    pf       = xfunc_in;         /* Save current input device */
    xfunc_in = func;             /* Switch input to specified device */
    n        = xgets(buff, len); /* Get a line */
    xfunc_in = pf;               /* Restore input device */

    return n;
}

/*----------------------------------------------*/
/* Get a value of the string                    */
/*----------------------------------------------*/
/*	"123 -5   0x3ff 0b1111 0377  w "
		^                           1st call returns 123 and next ptr
		   ^                        2nd call returns -5 and next ptr
				   ^                3rd call returns 1023 and next ptr
						  ^         4th call returns 15 and next ptr
							   ^    5th call returns 255 and next ptr
								  ^ 6th call fails and returns 0
*/

int xatoi(            /* 0:Failed, 1:Successful */
          char **str, /* Pointer to pointer to the string */
          long  *res  /* Pointer to the valiable to store the value */
)
{
    unsigned long val;
    unsigned char c, r, s = 0;

    *res = 0;

    while ((c = **str) == ' ')
        (*str)++; /* Skip leading spaces */

    if (c == '-') { /* negative? */
        s = 1;
        c = *(++(*str));
    }

    if (c == '0') {
        c = *(++(*str));
        switch (c) {
        case 'x': /* hexdecimal */
            r = 16;
            c = *(++(*str));
            break;
        case 'b': /* binary */
            r = 2;
            c = *(++(*str));
            break;
        default:
            if (c <= ' ')
                return 1; /* single zero */
            if (c < '0' || c > '9')
                return 0; /* invalid char */
            r = 8;        /* octal */
        }
    } else {
        if (c < '0' || c > '9')
            return 0; /* EOL or invalid char */
        r = 10;       /* decimal */
    }

    val = 0;
    while (c > ' ') {
        if (c >= 'a')
            c -= 0x20;
        c -= '0';
        if (c >= 17) {
            c -= 7;
            if (c <= 9)
                return 0; /* invalid char */
        }
        if (c >= r)
            return 0; /* invalid char for current radix */
        val = val * r + c;
        c   = *(++(*str));
    }
    if (s)
        val = 0 - val; /* apply sign if needed */

    *res = val;
    return 1;
}

#endif /* _USE_XFUNC_IN */
