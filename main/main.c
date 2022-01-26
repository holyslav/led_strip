#include "consts.h"

esp_err_t start_rest_server();

esp_err_t fs_init(void)
{
    size_t max_files = FOPEN_MAX - 3;
    esp_err_t ret = ESP_OK;    
    FILE* f = NULL;

    ESP_LOGI(TAG, "Initializing SPIFFS. max_files: %i", max_files);


    esp_vfs_spiffs_conf_t conf = {
      .base_path = "/data",
      .partition_label = "storage",
      .max_files = max_files,
      .format_if_mount_failed = false
    };
    
    ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return ret;
    }
    
    size_t total = 0, used = 0;
    ret = esp_spiffs_info("storage", &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
        return ret;
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }

    // Open renamed file for reading
    ESP_LOGI(TAG, "Reading file");
    f = fopen("/data/index.html", "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading");
        return ESP_FAIL;
    }
    char line[64];
    fgets(line, sizeof(line), f);
    fclose(f);
    // strip newline
    char* pos = strchr(line, '\n');
    if (pos) *pos = '\0';
    ESP_LOGI(TAG, "Read from file: '%s'", line);

    return ret;
}


esp_err_t esp_pwm_init(void)
{
    const uint32_t pin_num[3] = {
        PWM_0_OUT_IO_NUM,
        PWM_1_OUT_IO_NUM,
        PWM_2_OUT_IO_NUM
    };

    uint32_t duties[3] = {
        500, 500, 500
    };

    float phase[3] = {
        0, 0, 0,
    };

    pwm_init(PWM_PERIOD, duties, 3, pin_num);
    pwm_set_phases(phase);
    return pwm_start();
}

void app_main(void)
{
    ESP_LOGI(TAG, "app_main");
    ESP_ERROR_CHECK(fs_init());
    
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_ERROR_CHECK(example_connect());
    ESP_ERROR_CHECK(start_rest_server());
    ESP_ERROR_CHECK(esp_pwm_init());
}