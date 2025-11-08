/**
  ******************************************************************************
  * @file compiler_defs.h
  * @author Vasily Turchenko vasily.turchenko@huld.io
  * @version V0.0.1
  * @date     29-09-2021
  ******************************************************************************
  */

#ifndef COMPILER_DEFS_H
#define COMPILER_DEFS_H

#if defined (__GNUC__)
#define WARNING(A)/* #warning A*/
#define ATTR_NORETURN __attribute__((__noreturn__))
#elif defined (_MSC_VER)
#define WARNING(A)
#define ATTR_NORETURN __declspec(noreturn)
#else
#define WARNING(A)
#define ATTR_NORETURN
#endif

#if defined (_MSC_VER)
#define FALLTHROUGH
#else
#define FALLTHROUGH __attribute__((fallthrough));
#endif


#ifdef __GNUC__
#define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#endif

#ifdef _MSC_VER
#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop))
#endif

#if defined (_MSC_VER)
#pragma warning (disable: 4820)     /* about padding structures */
#pragma warning (disable: 5045)     /* about the Spectre mitigation */
#pragma warning (disable: 4201)     /* unnamed unions  */
#endif

#endif // COMPILER_DEFS_H
