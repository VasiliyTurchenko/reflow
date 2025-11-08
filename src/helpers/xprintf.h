/*------------------------------------------------------------------------*/
/* Universal string handler for user console interface  (C)ChaN, 2011     */
/*------------------------------------------------------------------------*/

#ifndef XPRINTF_H
#define XPRINTF_H

#ifdef __cplusplus
extern "C" {
#endif

#define USE_XFUNC_OUT	1	/* 1: Use output functions */
#define	CR_CRLF_		1	/* 1: Convert \n ==> \r\n in the output char */

#if(0)
#define USE_XFUNC_IN	1	/* 1: Use input function */
#else
#define USE_XFUNC_IN	0
#endif
#define	LINE_ECHO_		1	/* 1: Echo back input chars in xgets function */


#if (USE_XFUNC_OUT == 1)
#define xdev_out(func) xfunc_out = (void(*)(unsigned char c))(func)

extern void (*xfunc_out)(unsigned char c);

void xputc (char c);						/* Put a character */
void xputs (const char* str);					/* Put a null-terminated string */
void xfputs (void (*func)(unsigned char c), const char* str); /* Put a string to the specified device */
void xprintf (const char* fmt, ...); 				/* Put a formatted string to the default device */
void xsprintf (char* buff, const char* fmt, ...);		/* Put a formatted string to the memory */
void xfprintf (void (*func)(unsigned char c), const char*	fmt, ...); /* Put a formatted string to the specified device */
void put_dump (const void* buff, unsigned long addr, int len, int width); /* Dump a line of binary dump                   */
#define DW_CHAR		sizeof(char)
#define DW_SHORT	sizeof(short)
#define DW_LONG		sizeof(long)
#endif

#if (USE_XFUNC_IN == 1)
#define xdev_in(func) xfunc_in = (unsigned char(*)(void))(func)
extern unsigned char (*xfunc_in)(void);
int xgets (char* buff, int len);
int xfgets (unsigned char (*func)(void), char* buff, int len);
int xatoi (char** str, long* res);
#endif

#ifdef __cplusplus
}
#endif

#endif /* XPRINTF_H */
