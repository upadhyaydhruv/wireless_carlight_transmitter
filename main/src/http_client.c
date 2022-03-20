#include "http_client.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "config.h"
#include "stdio.h"
#include <stdlib.h>
#include <string.h>

esp_err_t _http_event_handler(esp_http_client_event_t *evt) { return ESP_OK; }

esp_http_client_handle_t http_init() {
  char local_response_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0};

  esp_http_client_config_t config = {
      .host = HTTP_SERVER_ADDR,
      .path = "/state",
      .port = HTTP_SERVER_PORT,
      .query = "esp",
      .event_handler = _http_event_handler,
      .user_data = local_response_buffer,
  };

  esp_http_client_handle_t client = esp_http_client_init(&config);

  return client;
}

void send_req(esp_http_client_handle_t client_in, int state) {
  char data[256];
  sprintf(data, "%d", state); // casts the state of the car light (on/off) into a string to be posted
                             // used in POST request
  const char *post_data = data;
  esp_http_client_set_method(client_in, HTTP_METHOD_POST);
  esp_http_client_set_post_field(client_in, post_data, strlen(post_data));
  esp_err_t err = esp_http_client_perform(client_in);

  if (err == ESP_OK) {
    ESP_LOGI(TAG_HTTP, "HTTP POST Status = %d, content_length = %lld",
             esp_http_client_get_status_code(client_in),
             esp_http_client_get_content_length(client_in));
  } else {
    ESP_LOGE(TAG_HTTP, "HTTP POST request failed: %s", esp_err_to_name(err));
  }

  esp_http_client_cleanup(
      client_in); // deallocates all memories and closes any/all sockets
}