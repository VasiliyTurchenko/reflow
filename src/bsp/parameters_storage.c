/** @file parameters_storage.c
 *  @brief EEPROM parameters layout and store/read functions
 *
 *  @author Vasiliy Turchenko
 *  @bug
 *  @date 21-Jan-2020
 */
#include "cmsis_os.h"
#include "i2c_eeprom.h"
#include "framebuffer.h"
#include "parameters_storage.h"
#include "logging.h"
#include "sys_helpers.h"

static EEPROM_pool_t cfg_pool;
static _Bool         cfg_loaded = false;

/* offset of the pool in the EEPROM*/
static const uint32_t pool_offset  = 0U;
static const uint32_t crc32_offset = (EEPROM_SIZE - sizeof(uint32_t));

/**
 * @brief load_cfg_pool
 * @return
 */
ErrorStatus load_cfg_pool(void)
{
    ErrorStatus retVal;

    retVal = Read_Array_I2C_EEPROM((uint8_t *)&cfg_pool, sizeof(EEPROM_pool_t), pool_offset,
                                   &at24c04);
    if (retVal == SUCCESS) {
        cfg_loaded = true;
    }
    return retVal;
}

/**
 * @brief save_cfg saves requested cfg data
 * @param requested_cfg
 * @return ERROR or SUCCESS
 */
ErrorStatus save_cfg_pool(void)
{
    ErrorStatus retVal;

    retVal = Write_Array_I2C_EEPROM((uint8_t *)&cfg_pool, sizeof(EEPROM_pool_t), pool_offset,
                                    &at24c04);
    if (retVal == SUCCESS) {
        /* new crc32 */
        /** @todo add pointer to the EEPROM handle */
        uint32_t new_crc32 = crc32_over_eeprom();
        retVal = Write_Array_I2C_EEPROM((uint8_t *)&new_crc32, sizeof(uint32_t), crc32_offset,
                                        &at24c04);
    }

    if (retVal == SUCCESS) {
        RESULT_UNUSED(fast_clear_screen());
        log_xputs(MSG_LEVEL_SERIOUS, "Config saved. Rebooting.");
        sys_helpers_delay_ms_and_reboot(500U);
    }
    return retVal;
}

/**
 * @brief get_cfg returns pointer to the config data
 * @param requested_cfg
 * @return pointer to the requested type
 * @note tricky!
 */
void *get_cfg(CFG_TYPE requested_cfg)
{
    void *retVal = NULL;
    switch (requested_cfg) {
    case CFG_TYPE_TC_CAL1: {
        retVal = (void *)&cfg_pool.tc1_cal_data;
        break;
    }
    case CFG_TYPE_TC_CAL2: {
        retVal = (void *)&cfg_pool.tc2_cal_data;
        break;
    }
    case CFG_TYPE_DOOR_PWM: {
        retVal = (void *)&cfg_pool.door_pwm_cal_data;
        break;
    }
    case CFG_TYPE_HEAT_CAL: {
        retVal = (void *)&cfg_pool.heating_cal_data;
        break;
    }
    case CFG_TYPE_COOL_CAL: {
        retVal = (void *)&cfg_pool.cooling_cal_data;
        break;
    }
    case CFG_TYPE_NONE:
    default: {
        break;
    }
    }
    return retVal;
}
