/**
  ******************************************************************************
  * @file uni_vector.c
  * @author  Vasily Turchenko <turchenkov@gmail.com>
  * @brief   simplest 1D vector realization
  * @note    MISRA check passed
  ******************************************************************************
  */

#include <string.h>
#include "unused.h"
#include "uni_vector.h"

/**
 * @brief uni_vector_init_static initializes vector
 * @param pvector pointer to the vector
 * @param pdata pointer to the data storage
 * @param capacity given capacity
 * @return NULL if error, pvector if OK
 */
uni_vect_t *uni_vector_init_static(uni_vect_t *pvector, uint8_t *pdata,
                                   size_t capacity){ UNI_VECTOR_INIT_BODY(uni_vect_t, uint8_t) }

/**
 * @brief uni_vector_clear clears content of the vector
 * @param pvector pointer to the vector
 * @return NULL if error, pvector if OK
 */
uni_vect_t *uni_vector_clear(uni_vect_t *pvector){ UNI_VECTOR_CLEAR_BODY(uni_vect_t, uint8_t) }

/**
 * @brief uni_vector_push_back adds new element to the end
 * @param pvector  pointer to the vector
 * @param data value to be added
 * @return 1U if added, 0U if not (no more space left, not a vector )
 */
size_t uni_vector_push_back(uni_vect_t *pvector, uint8_t data)
{
    UNI_VECTOR_PUSH_BACK_BODY(uni_vect_t, uint8_t);
}

/**
 * @brief uni_vector_size returns current size of the vector
 * @param pvector pointer to the vector
 * @return
 */
size_t uni_vector_size(uni_vect_t *pvector)
{
    UNI_VECTOR_SIZE_BODY(uni_vect_t, uint8_t);
}

/**
 * @brief uni_vector_shallow_copy
 * @param src pointer to te source vector
 * @return a shallow copy of the sourcVasiliy Turchenkoe
 */
uni_vect_t uni_vector_shallow_copy(const uni_vect_t *src)
{
    uni_vect_t retVal = (uni_vect_t){ 0U, 0U, NULL };
    if (src != NULL) {
        retVal.capacity = src->capacity;
        retVal.data_len = src->data_len;
        retVal.pdata    = src->pdata;
    }
    return retVal;
}

/* deep copies one vector to another */
/**
 * @brief uni_vector_copy
 * @param dst
 * @param src
 * @return number of bytes copied
 */
size_t uni_vector_copy(uni_vect_t *dst, const uni_vect_t *src)
{
    size_t retVal = 0U;
    if ((dst != NULL) && (dst->pdata != NULL) && (src != NULL) && (src->pdata != NULL)) {
        size_t        to_copy = (src->data_len > dst->capacity) ? dst->capacity : src->data_len;
        RESULT_UNUSED memcpy(dst->pdata, src->pdata, to_copy);
        dst->data_len = to_copy;
        retVal        = to_copy;
    }
    return retVal;
}

/**
 * @brief uni_vector_map maps an existing uni_vector_t to the existing data array
 * @param dst pointer to the vector to be mapped
 * @param cap capacity of the result
 * @param len data length of the result
 * @param pdata pointer to existing data array
 * @return NULL if error, src otherwize
 */
uni_vect_t *uni_vector_map(uni_vect_t *dst, size_t cap, size_t len, uint8_t *pdata)
{
    uni_vect_t *retVal = NULL;
    if (dst != NULL) {
        if (pdata != NULL) {
            if (cap >= len) {
                dst->capacity = cap;
                dst->data_len = len;
                dst->pdata    = pdata;
                retVal        = dst;
            }
        }
    }

    return retVal;
}

/**
 * @brief uni_vector32_init_static vector holding 32-bit data
 * @param pvector
 * @param pdata
 * @param capacity
 * @return
 */
uni_vect32_t *uni_vector32_init_static(uni_vect32_t *pvector, uint32_t *pdata, size_t capacity){
    UNI_VECTOR_INIT_BODY(uni_vect32_t, uint32_t)
}

/**
 * @brief uni_vector32_clear
 * @param pvector
 * @return
 */
uni_vect32_t *uni_vector32_clear(uni_vect32_t *pvector){ UNI_VECTOR_CLEAR_BODY(uni_vect32_t,
                                                                               uint32_t) }

/**
 * @brief uni_vector_ptr_init_static vector holding pointers
 * @param pvector
 * @param pdata
 * @param capacity
 * @return
 */
uni_vect_ptr_t *uni_vector_ptr_init_static(uni_vect_ptr_t *pvector, void **pdata, size_t capacity){
    UNI_VECTOR_INIT_BODY(uni_vect_ptr_t, void **)
} /**
 * @brief uni_vector_ptr_clear
 * @param pvector
 * @return
 */
uni_vect_ptr_t *uni_vector_ptr_clear(uni_vect_ptr_t *pvector)
{
    UNI_VECTOR_CLEAR_BODY(uni_vect_ptr_t, void **)
}
