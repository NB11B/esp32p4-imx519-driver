#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"
#include "driver/i2c_master.h"

#ifdef __cplusplus
extern "C" {
#endif

// I2C addresses
#define AV_IMX519_I2C_ADDR  0x1A
#define AV_DW9714_I2C_ADDR  0x0C

// Sensor modes
typedef enum {
    AV_IMX519_MODE_QQVGA = 0,  // 160x120 (via 2x2 binning from 320x240 crop)
    AV_IMX519_MODE_VGA,        // 640x480 (via 2x2 binning from 1280x960 crop)
    AV_IMX519_MODE_720P,       // 1280x720
    AV_IMX519_MODE_1080P,      // 1920x1080
    AV_IMX519_MODE_MAX
} av_imx519_mode_t;

// Context handle
typedef struct av_imx519_ctx_s* av_imx519_handle_t;

/**
 * @brief Initialize the IMX519 sensor and DW9714 VCM over the given I2C bus.
 *
 * @param i2c_bus The I2C master bus handle.
 * @param out_handle Pointer to store the allocated context handle.
 * @return ESP_OK on success.
 */
esp_err_t av_imx519_init(i2c_master_bus_handle_t i2c_bus, av_imx519_handle_t *out_handle);

/**
 * @brief Deinitialize and free the sensor context.
 */
esp_err_t av_imx519_deinit(av_imx519_handle_t handle);

/**
 * @brief Switch the sensor to a specific resolution mode.
 *
 * This stops streaming, applies the register table for the new mode,
 * and restarts streaming.
 */
esp_err_t av_imx519_set_mode(av_imx519_handle_t handle, av_imx519_mode_t mode);

/**
 * @brief Set the DW9714 VCM autofocus DAC value.
 *
 * @param dac 10-bit value (0 = infinity, 1023 = macro).
 */
esp_err_t av_imx519_set_vcm_dac(av_imx519_handle_t handle, uint16_t dac);

/**
 * @brief Set the sensor exposure time.
 *
 * @param lines Exposure time in units of line length.
 */
esp_err_t av_imx519_set_exposure(av_imx519_handle_t handle, uint32_t lines);

/**
 * @brief Set the sensor analog gain.
 *
 * @param gain Gain value (0 to 960).
 */
esp_err_t av_imx519_set_analog_gain(av_imx519_handle_t handle, uint16_t gain);

#ifdef __cplusplus
}
#endif
