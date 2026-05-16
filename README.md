# ESP32-P4 IMX519 Camera Driver

A self-contained, bare-metal ESP-IDF component for the **Arducam 16MP IMX519** autofocus camera module, specifically designed for the **ESP32-P4** MIPI-CSI interface.

This driver provides full control over the IMX519 sensor and its companion DW9714 Voice Coil Motor (VCM) for autofocus, bypassing the need for complex Linux-style V4L2 abstractions while remaining compatible with the `esp_video` ISP pipeline.

## Features

- **Direct I2C Register Control**: Uses the official Raspberry Pi Linux kernel register tables (rpi-6.6.y) for accurate clocking and ISP tuning.
- **Multi-Resolution Support**:
  - QQVGA (160x120) via 2x2 binning — ideal for high-speed motion detection
  - VGA (640x480) via 2x2 binning
  - 720p (1280x720)
  - 1080p (1920x1080)
- **Hardware Autofocus**: Direct DAC control of the DW9714 VCM (0 = infinity, 1023 = macro).
- **Exposure & Gain**: Manual control over exposure lines and analog gain.
- **ESP-IDF 5.3+ Compatible**: Integrates cleanly with `esp_cam_sensor` and `esp_video`.

## Hardware Wiring

The ESP32-P4 uses a MIPI-CSI interface. The Arducam IMX519 uses a standard Raspberry Pi 15-pin or 22-pin FFC connector.

**Warning:** Verify the FFC cable orientation before powering on. Reversed MIPI lanes will produce no image.

| Signal | IMX519 Pin | ESP32-P4 Connector | Notes |
|---|---|---|---|
| MIPI D0± | Lane 0 | CSI D0± | Data lane 0 |
| MIPI D1± | Lane 1 | CSI D1± | Data lane 1 |
| MIPI CLK± | Clock | CSI CLK± | Clock lane |
| SDA (I2C) | SDA | GPIO 8 | I2C data (IMX519 addr: `0x1A`, VCM addr: `0x0C`) |
| SCL (I2C) | SCL | GPIO 7 | I2C clock |
| VCC 3.3V | 3.3V | 3.3V rail | Power |
| GND | GND | GND | Ground |

## Installation

Add this component to your ESP-IDF project using the component manager, or clone it directly into your `components/` directory.

```bash
cd your_project/components
git clone https://github.com/NB11B/esp32p4-imx519-driver.git av_imx519
```

In your `main/CMakeLists.txt`, add `av_imx519` to your `REQUIRES` list:

```cmake
idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    REQUIRES esp_cam_sensor esp_video driver av_imx519
)
```

## Usage Example

```c
#include "driver/i2c_master.h"
#include "av_imx519.h"

// 1. Initialize I2C bus
i2c_master_bus_config_t i2c_bus_cfg = {
    .i2c_port = I2C_NUM_0,
    .sda_io_num = 8,
    .scl_io_num = 7,
    .clk_source = I2C_CLK_SRC_DEFAULT,
    .glitch_ignore_cnt = 7,
    .flags = { .enable_internal_pullup = true }
};
i2c_master_bus_handle_t i2c_bus;
i2c_new_master_bus(&i2c_bus_cfg, &i2c_bus);

// 2. Initialize IMX519 and VCM
av_imx519_handle_t cam_handle;
esp_err_t err = av_imx519_init(i2c_bus, &cam_handle);
if (err == ESP_OK) {
    printf("IMX519 initialized successfully\n");
}

// 3. Switch resolution
av_imx519_set_mode(cam_handle, AV_IMX519_MODE_VGA);

// 4. Adjust focus (0 = infinity, 1023 = macro)
av_imx519_set_vcm_dac(cam_handle, 512);
```

## License

The register initialization tables in `av_imx519.c` are derived from the Raspberry Pi Linux kernel driver (`drivers/media/i2c/imx519.c`), authored by Arducam Technology and Raspberry Pi Ltd, and are subject to the **GPL-2.0-only** license.

The remainder of the driver wrapper code is released under the **MIT License**.

---
Nathanael J. Bocker, 2026 all rights reserved
