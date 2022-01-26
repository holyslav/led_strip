#include <stdio.h>
#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <esp_system.h>
#include <esp_err.h>
#include <errno.h>
#include <esp_log.h>
#include <esp_event.h>
#include <sdkconfig.h>

#include <esp_spiffs.h>
#include <nvs_flash.h>
#include <esp_vfs.h>

#include <esp8266/gpio_register.h>
#include <esp8266/pin_mux_register.h>
#include <driver/pwm.h>

#include <esp_netif.h>
#include <protocol_examples_common.h>
#include <esp_http_server.h>
#include <cJSON.h>

#define PWM_0_OUT_IO_NUM   14
#define PWM_1_OUT_IO_NUM   12
#define PWM_2_OUT_IO_NUM   13

// PWM period 1000us(1Khz), same as depth
#define PWM_PERIOD    (1000)


static const char *TAG = "led_strip";