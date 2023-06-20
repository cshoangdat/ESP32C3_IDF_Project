#include "wifi_connect.h"

static const char *TAG = "wifi station";

static int s_retry_num = 0;
static esp_netif_t *netif = NULL;
static EventGroupHandle_t s_wifi_event_group = NULL;
wifi_config_t wifi_config = {0};
esp_event_handler_instance_t instance_any_id;
esp_event_handler_instance_t instance_got_ip;
bool is_wifi_set_up_done = false;

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < ESP_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
            ESP_LOGI(TAG,"connect to the AP fail");
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void wifi_init_sta(void) {
    s_wifi_event_group = xEventGroupCreate();
    ESP_LOGI(TAG, "xEventGroupCreate");
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_LOGI(TAG, "esp_netif_init");

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_LOGI(TAG, "esp_event_loop_create_default");

    if(netif == NULL){
        netif = esp_netif_create_default_wifi_sta();
        ESP_LOGI(TAG, "esp_netif_create_default_wifi_sta");
    }
}

void init_NVS(void){
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}

void wifi_disconnect(void){
    ESP_LOGI(TAG, "esp_wifi_disconnect");
    vTaskDelay(500);
    memset(wifi_config.sta.ssid, 0, sizeof(wifi_config.sta.ssid));
    memset(wifi_config.sta.password, 0, sizeof(wifi_config.sta.password));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_disconnect();
    esp_wifi_stop();
    ESP_LOGI(TAG, "esp_wifi_stop");
    vTaskDelay(500);
}

void wifi_connect(const char* wifi_ssid, const char* wifi_pass){
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, &instance_any_id));
    ESP_LOGI(TAG, "esp_event_handler_instance_register WIFI_EVENT");
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, &instance_got_ip));
    ESP_LOGI(TAG, "esp_event_handler_instance_register IP_EVENT");

    xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT);

    memcpy(wifi_config.sta.ssid, wifi_ssid, strlen(wifi_ssid));
    memcpy(wifi_config.sta.password, wifi_pass, strlen(wifi_pass));
   
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY);
    ESP_LOGI(TAG, "xEventGroupWaitBits");

    if (bits & WIFI_CONNECTED_BIT) 
    {
        ESP_LOGI(TAG, "connected to ap SSID: %s || password: %s", wifi_ssid, wifi_pass);
    } 
    else if (bits & WIFI_FAIL_BIT) 
    {
        ESP_LOGI(TAG, "Failed to connect to SSID: %s || password: %s", wifi_ssid, wifi_pass);
        s_retry_num = 0;
    } 
    else 
    {
        ESP_LOGI(TAG, "UNEXPECTED EVENT");
    }
    ESP_LOGI(TAG, "Done");

    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
}

bool is_wifi_connected(void){
    return esp_wifi_connect() == ESP_OK;
}

void set_up_wifi(void){
    if(is_wifi_set_up_done == false){
        init_NVS();
        wifi_init_sta();
        is_wifi_set_up_done = true;
    }
    else{
        return;
    }
}

