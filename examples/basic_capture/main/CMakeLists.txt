idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    REQUIRES esp_cam_sensor esp_video driver av_imx519
)
