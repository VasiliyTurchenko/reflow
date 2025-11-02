/**
  ******************************************************************************
  * @file unused.h
  * @author  Vasiliy Turchenko vasily.turchenko@huld.io
  * @version V0.0.1
  * @date    26-Mar-2020
  * @brief   unused result macro
  ******************************************************************************
 **/

#ifndef UNUSED_H
#define UNUSED_H

#if defined(__GNUC__)
#define ATTR_UNUSED __attribute__((__unused__))
#else
#define ATTR_UNUSED
#endif

#define RESULT_UNUSED (void)
#define ARG_UNUSED (void)

#ifdef DESKTOP_TARGET
#define DO_ON_DESKTOP(A)                                                                           \
	{                                                                                              \
		A;                                                                                         \
	}
#else
#define DO_ON_DESKTOP(A)
#endif

#endif // UNUSED_H
