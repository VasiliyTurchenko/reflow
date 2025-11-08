/**
  ******************************************************************************
  * @file uni_vector.h
  * @author  Vasily Turchenko <turchenkov@gmail.com>
  * @version V0.0.1
  * @brief   simplest 1D vector realization
  ******************************************************************************
  */

#ifndef UNI_VECTOR_H
#define UNI_VECTOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <limits.h>
#include <stdbool.h>

#include "uni_vector_types.h"

/* TEMPLATES */

#define UNI_VECTOR_TYPEDEF_BODY(A, T, AT)                                \
    typedef struct A {  \
        size_t data_len;           /*! stored data length */  \
        size_t capacity;           /*! storage capacity   */  \
        T * pdata;                 /*! pointer to the data */ \
} AT;

#define UNI_VECTOR_INIT_BODY(A, T) \
    A * retVal = NULL; \
    if ((pvector != NULL) && (pdata != NULL)) { \
        pvector->data_len = 0U; \
        pvector->capacity = capacity; \
        pvector->pdata = pdata; \
        memset(pdata, 0, (capacity * sizeof(T))); \
        retVal = pvector; \
    } \
    return retVal;

#define UNI_VECTOR_CLEAR_BODY(A, T) \
    A * retVal = NULL; \
    if (pvector != NULL) { \
        pvector->data_len = 0U; \
        memset(pvector->pdata, 0, (pvector->capacity * sizeof(T))); \
        retVal = pvector; \
    } \
    return retVal;

/**
  * @param A - type of vector
  * @param T - type of stored data
  */
#define UNI_VECTOR_PUSH_BACK_BODY(A, T) \
    size_t retVal = 0U; \
    if (pvector != NULL) { \
        if (pvector->data_len < pvector->capacity) {\
            memcpy(&pvector->pdata[pvector->data_len], &data, sizeof(T));\
            pvector->data_len++; \
            retVal = 1U;\
        }\
    }\
    return retVal;

/**
  * @param A - type of vector
  * @param T - type of stored data
  */
#define UNI_VECTOR_SIZE_BODY(A, T) \
    size_t retVal = 0U; \
    if (pvector != NULL) { \
        retVal = pvector->data_len;\
    }\
    return retVal;

uni_vect_t * uni_vector_init_static(uni_vect_t * pvector, uint8_t * pdata, size_t capacity);

uni_vect_t * uni_vector_clear(uni_vect_t * pvector);

size_t uni_vector_push_back(uni_vect_t * pvector, uint8_t data);

size_t uni_vector_size(uni_vect_t * pvector);

size_t uni_vector_copy(uni_vect_t * dst, const uni_vect_t *src);

uni_vect_t uni_vector_shallow_copy(const uni_vect_t * src);

uni_vect_t * uni_vector_map(uni_vect_t * src, size_t cap, size_t len, uint8_t * pdata);

uni_vect32_t * uni_vector32_init_static(uni_vect32_t * pvector, uint32_t * pdata, size_t capacity);

uni_vect32_t * uni_vector32_clear(uni_vect32_t * pvector);

uni_vect_ptr_t * uni_vector_ptr_init_static(uni_vect_ptr_t * pvector, void ** pdata, size_t capacity);

uni_vect_ptr_t * uni_vector_ptr_clear(uni_vect_ptr_t * pvector);

/**
 * @brief uni_vect_is_valid validity checker
 * @param v pointer to the vector
 * @return true if pointers are not null and capacity > 0
 */
static inline _Bool uni_vect_is_valid(const uni_vect_t* const v)
{
    _Bool retVal = false;
    if (v != NULL) {
        if (v->pdata != NULL) {
            if (v->capacity > 0U) {
                retVal = true;
            }
        }
    }
    return retVal;
}

#define UNI_VECTOR

#ifdef __cplusplus
}
#endif

#endif // UNI_VECTOR_H
