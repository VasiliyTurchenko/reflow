/** @file my_comm.h
 *  @brief
 *
 *  @author turchenkov@gmail.com
 *  @bug
 *  @date 30-Dec-2017, 20-Jan-2019
 */
/*! file my_comm.h
 * Copyright (c) 30-Dec-2017 turchenkov@gmail.com
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 * associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY,  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 * IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef MY_COMM_H
#define MY_COMM_H

#include <stdbool.h>
#include <stdint.h>
#include "uni_vector.h"
#include "hal_ll.h"

/*
USART   bytes  |-------------------------------------------------|
Speed   /sec   |	amount, bytes                                |
bod            |  64		128		256		512		1024	2048 |
               |-------------------------------------------------|
               |	transmit time, ms                            |

115200	11520	5,6		11,1	22,2	44,4	88,9	177,8
57600	5760	11,1	22,2	44,4	88,9	177,8	355,6
38400	3840	16,7	33,3	66,7	133,3	266,7	533,3
19200	1920	33,3	66,7	133,3	266,7	533,3	1066,7
14400	1440	44,4	88,9	177,8	355,6	711,1	1422,2
9600	960		66,7	133,3	266,7	533,3	1066,7	2133,3
4800	480		133,3	266,7	533,3	1066,7	2133,3	4266,7
2400	240		266,7	533,3	1066,7	2133,3	4266,7	8533,3
1200	120		533,3	1066,7	2133,3	4266,7	8533,3	17066,7
*/

/* transmission complete callback type */
typedef void (*tc_fun)(void *arg);

typedef struct {
    int32_t (*init_sink)(void *init_params, tc_fun callback,
                         void *callback_param);       ///< open real sink (ser. port, etc.)
    int32_t (*deinit_sink)(int32_t h);                ///< close the sink
    size_t (*send_bytes)(uint8_t *pbytes, size_t nb); ///< actual transmit function
} ostream_sink_functions_t;

ErrorStatus InitComm(uni_vect_t buffer, ostream_sink_functions_t *fun_list, void *sink_init_params);
ErrorStatus Transmit(void);

#ifdef WITH_RTOS
ErrorStatus Transmit_RTOS(void);
void        myxfunc_out_RTOS(unsigned char c);
void        my_comm_switch_to_RTOS_func(void);
void        my_comm_update_mutexes(void);
#endif

/* prints to nowhere */
void myxfunc_out_dummy(unsigned char c);

/* when no RTOS has started */
void myxfunc_out_no_RTOS(unsigned char c);

#endif

/* ############################### end of the file ########################## */
