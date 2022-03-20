#include "esp_event.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "nvs_flash.h"

#define POST_REQ_FREQUENCY 2000
#define SSID "<YOUR_SSID>"
#define PSWD "<YOUR_PASSWD>"
#define HTTP_SERVER_ADDR "<SERVER_IP"
#define HTTP_SERVER_PORT 5000 // tentative

#define MAX_HTTP_RECV_BUFFER 512
#define MAX_HTTP_OUTPUT_BUFFER 2048
static const char *TAG_HTTP = "HTTP_CLIENT";

esp_err_t _http_event_handler(esp_http_client_event_t *evt);
esp_http_client_handle_t http_init();
void send_req(esp_http_client_handle_t client_in, int temp);
