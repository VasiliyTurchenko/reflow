/** @file uni_vector_types.h
 *  @brief typedefs only header - for easy export
 *
 *  @author Vasily Turchenko <turchenkov@gmail.com>
 *  @bug
 *  @date 04-Oct-2021
 */

#ifndef UNI_VECTOR_TYPES_H
#define UNI_VECTOR_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <limits.h>

/* vector holding data */
typedef struct uni_vect {
    size_t data_len;           ///< stored data length
    size_t capacity;           ///< storage capacity
    uint8_t * pdata;           ///< pointer to the data
} uni_vect_t;

typedef struct uni_vect32 {
    size_t data_len;           ///< stored data length
    size_t capacity;           ///< storage capacity
    uint32_t * pdata;           ///< pointer to the data
} uni_vect32_t;

typedef struct uni_vect_ptr_t {
    size_t data_len;           ///< stored data length
    size_t capacity;           ///< storage capacity
    void ** pdata;              ///< pointer to the arr of pointers
} uni_vect_ptr_t;

#ifdef __cplusplus
}
#endif

#endif // UNI_VECTOR_TYPES_H
