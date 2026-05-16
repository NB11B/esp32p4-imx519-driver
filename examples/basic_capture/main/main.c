#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "esp_cam_ctlr.h"
#include "esp_cam_ctlr_csi.h"
#include "av_imx519.h"

static const char *TAG = "example";

#define CAM_PIN_SDA 8
#define CAM_PIN_SCL 7

void app_main(void) {
    ESP_LOGI(TAG, "Starting IMX519 Basic Capture Example");

    // 1. Initialize I2C bus
    i2c_master_bus_config_t i2c_bus_cfg = {
        .i2c_port = I2C_NUM_0,
        .sda_io_num = CAM_PIN_SDA,
        .scl_io_num = CAM_PIN_SCL,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags = { .enable_internal_pullup = true }
    };
    i2c_master_bus_handle_t i2c_bus;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_cfg, &i2c_bus));

    // 2. Initialize IMX519 and VCM
    av_imx519_handle_t cam_handle;
    esp_err_t err = av_imx519_init(i2c_bus, &cam_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize IMX519: %s", esp_err_to_name(err));
        return;
    }
    ESP_LOGI(TAG, "IMX519 initialized successfully");

    // 3. Initialize MIPI-CSI Controller
    esp_cam_ctlr_csi_config_t csi_cfg = {
        .ctlr_id = 0,
        .h_res = 160,
        .v_res = 120,
        .lane_bit_rate_mbps = 800,
        .bayer_order = CAM_CTLR_BAYER_BGGR,
        .color_format = CAM_CTLR_COLOR_RAW10,
        .out_format = CAM_CTLR_COLOR_GREY,
        .data_lane_num = 2,
        .byte_swap_en = false,
        .queue_items = 5,
    };
    esp_cam_ctlr_handle_t csi_handle;
    ESP_ERROR_CHECK(esp_cam_new_csi_ctlr(&csi_cfg, &csi_handle));
    ESP_ERROR_CHECK(esp_cam_ctlr_enable(csi_handle));
    ESP_ERROR_CHECK(esp_cam_ctlr_start(csi_handle));

    // 4. Capture loop
    size_t frame_size = 160 * 120;
    uint8_t *frame_buf = malloc(frame_size);
    assert(frame_buf);

    esp_cam_ctlr_trans_t trans = {
        .buffer = frame_buf,
        .buflen = frame_size,
    };

    int frame_count = 0;
    while (1) {
        err = esp_cam_ctlr_receive(csi_handle, &trans, pdMS_TO_TICKS(1000));
        if (err == ESP_OK) {
            ESP_LOGI(TAG, "Captured frame %d: %d bytes", ++frame_count, (int)trans.received_size);
            
            // Sweep focus back and forth
            uint16_t dac = (frame_count % 20) * 50; // 0 to 950
            av_imx519_set_vcm_dac(cam_handle, dac);
        } else {
            ESP_LOGW(TAG, "Capture timeout");
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
