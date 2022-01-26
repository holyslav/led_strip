#include "consts.h"

#define REST_CHECK(a, str, goto_tag, ...)                                              \
    do                                                                                 \
    {                                                                                  \
        if (!(a))                                                                      \
        {                                                                              \
            ESP_LOGE(TAG, "%s(%d): " str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
            goto goto_tag;                                                             \
        }                                                                              \
    } while (0)

#define FILE_PATH_MAX (ESP_VFS_PATH_MAX + 128)
#define SCRATCH_BUFSIZE (10240)

typedef struct rest_server_context {
    char base_path[ESP_VFS_PATH_MAX + 1];
    char scratch[SCRATCH_BUFSIZE];
} rest_server_context_t;

#define CHECK_FILE_EXTENSION(filename, ext) (strcasecmp(&filename[strlen(filename) - strlen(ext)], ext) == 0)

static int red = 0, green = 0, blue = 0;


long map(long x, long in_min, long in_max, long out_min, long out_max);

/* Set HTTP response content type according to file extension */
static esp_err_t set_content_type_from_file(httpd_req_t *req, const char *filepath)
{
    const char *type = "text/plain";
    if (CHECK_FILE_EXTENSION(filepath, ".html")) {
        type = "text/html";
    } else if (CHECK_FILE_EXTENSION(filepath, ".js")) {
        type = "application/javascript";
    } else if (CHECK_FILE_EXTENSION(filepath, ".map")) {
        type = "application/json";
    } else if (CHECK_FILE_EXTENSION(filepath, ".css")) {
        type = "text/css";
    } else if (CHECK_FILE_EXTENSION(filepath, ".png")) {
        type = "image/png";
    } else if (CHECK_FILE_EXTENSION(filepath, ".ico")) {
        type = "image/x-icon";
    } else if (CHECK_FILE_EXTENSION(filepath, ".svg")) {
        type = "text/xml";
    }
    return httpd_resp_set_type(req, type);
}

/* Send HTTP response with the contents of the requested file */
static esp_err_t rest_common_get_handler(httpd_req_t *req)
{
    char filepath[FILE_PATH_MAX];

    rest_server_context_t *rest_context = (rest_server_context_t *)req->user_ctx;
    strlcpy(filepath, rest_context->base_path, sizeof(filepath));
    if (req->uri[strlen(req->uri) - 1] == '/') {
        strlcat(filepath, "/index.html", sizeof(filepath));
    } else {
        strlcat(filepath, req->uri, sizeof(filepath));
    }


    FILE* fd = fopen(filepath, "r");
    ESP_LOGI(TAG, "Try open file : %s", filepath);
    if (fd == NULL) {
        ESP_LOGE(TAG, "Failed to open file : %s", filepath);
        /* Respond with 500 Internal Server Error */
        httpd_resp_set_status(req, HTTPD_500);
        return ESP_FAIL;
    }

    set_content_type_from_file(req, filepath);

    char *chunk = rest_context->scratch;

    size_t total_read = 0;
 
    int num_read = 0;
 
    do {
        num_read = fread(chunk, sizeof(char), SCRATCH_BUFSIZE, fd);
        if (httpd_resp_send_chunk(req, chunk, num_read) != ESP_OK)
        {
            fclose(fd);
            ESP_LOGE(TAG, "File sending failed!: %s", filepath);
            httpd_resp_send_chunk(req, NULL, 0);
            httpd_resp_set_status(req, HTTPD_500);
            return ESP_FAIL;
        }
        total_read += num_read;
    } while (num_read == SCRATCH_BUFSIZE);
 
 
    ESP_LOGI(TAG, "Read %i characters\n", total_read);

    fclose(fd);


    ESP_LOGI(TAG, "File sending complete: %s", filepath);
    httpd_resp_send_chunk(req, NULL, 0);

    return ESP_OK;
}

static esp_err_t light_brightness_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "application/json");
    
    const char* format = "{\"red\": %i, \"green\": %i, \"blue\": %i}";

    size_t size = snprintf(NULL, 0, format, red, green, blue)+1;

    char *buffer = malloc (size); //{"red": 255, "green": 255, "blue": 255}

    memset(buffer, '\0', size);

    size = snprintf(buffer, size+1, format, red, green, blue);

    httpd_resp_send(req, buffer, -1);

    ESP_LOGI(TAG, "buffer: %s", buffer);

    free((void *)buffer);

    return ESP_OK;
}

/* Simple handler for light brightness control */
static esp_err_t light_brightness_post_handler(httpd_req_t *req)
{
    int total_len = req->content_len;
    int cur_len = 0;
    char *buf = ((rest_server_context_t *)(req->user_ctx))->scratch;
    int received = 0;
    if (total_len >= SCRATCH_BUFSIZE) {
        /* Respond with 500 Internal Server Error */
        httpd_resp_set_status(req, HTTPD_500);
        return ESP_FAIL;
    }
    while (cur_len < total_len) {
        received = httpd_req_recv(req, buf + cur_len, total_len);
        if (received <= 0) {
            /* Respond with 500 Internal Server Error */
            httpd_resp_set_status(req, HTTPD_500);
            return ESP_FAIL;
        }
        cur_len += received;
    }
    buf[total_len] = '\0';

    cJSON *root = cJSON_Parse(buf);
    red = cJSON_GetObjectItem(root, "red")->valueint;
    green = cJSON_GetObjectItem(root, "green")->valueint;
    blue = cJSON_GetObjectItem(root, "blue")->valueint;
    ESP_LOGI(TAG, "Light control: red = %d, green = %d, blue = %d", red, green, blue);
    cJSON_Delete(root);
    httpd_resp_send(req, "Post control value successfully", -1);
    pwm_set_duty(0, map(red, 0, 255, 0, PWM_PERIOD));
    pwm_set_duty(1, map(green, 0, 255, 0, PWM_PERIOD));
    pwm_set_duty(2, map(blue, 0, 255, 0, PWM_PERIOD));
    return ESP_OK;
}


esp_err_t start_rest_server()
{
    rest_server_context_t *rest_context = calloc(1, sizeof(rest_server_context_t));
    REST_CHECK(rest_context, "No memory for rest context", err);
    strlcpy(rest_context->base_path, CONFIG_WEB_MOUNT_POINT, sizeof(rest_context->base_path));

    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;

    ESP_LOGI(TAG, "Starting HTTP Server");
    REST_CHECK(httpd_start(&server, &config) == ESP_OK, "Start server failed", err_start);

    /* URI handler for light brightness control */
    httpd_uri_t light_brightness_get_uri = {
        .uri = "/api/v1/color",
        .method = HTTP_GET,
        .handler = light_brightness_get_handler,
        .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &light_brightness_get_uri);

    /* URI handler for light brightness control */
    httpd_uri_t light_brightness_post_uri = {
        .uri = "/api/v1/color",
        .method = HTTP_POST,
        .handler = light_brightness_post_handler,
        .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &light_brightness_post_uri);

    /* URI handler for getting web server files */
    httpd_uri_t common_get_uri = {
        .uri = "/*",
        .method = HTTP_GET,
        .handler = rest_common_get_handler,
        .user_ctx = rest_context
    };
    httpd_register_uri_handler(server, &common_get_uri);

    return ESP_OK;
err_start:
    free(rest_context);
err:
    return ESP_FAIL;
}

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}