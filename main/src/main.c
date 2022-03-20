#include "config.h"
#include "light_sensor.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "http_client.h"
#include "nvs_flash.h"
#include "stdint.h"
#include "stdio.h"
#include "wifi_station.h"

int state = 0; // declaring variable defined in config for state of car light
float ambient_light = 0; // declaring ambient light sensor defined in config

void task_http() {
  esp_http_client_handle_t client = http_init();

  while (1) {
    if (ambient_light > LUX_THRESHOLD) {
        state = 1;
    }
    else {
        state = 0;
    }
    send_req(client, state);
    vTaskDelay(pdMS_TO_TICKS(POST_REQ_FREQUENCY)); // send POST request every 2 seconds
  }
}

void task_lightsense() {
    i2c_master_init();
    sensor_set_gain();
    sensor_set_rate();

    while (1) {
        ambient_light = measure_data();
        vTaskDelay(400 / portTICK_PERIOD_MS); // update every 400ms
    }
}

void app_main(void) {

  // Before initiating the tasks, the MCU must be connected to the local network
  wifi_connect();

  vTaskDelay(pdMS_TO_TICKS(1000)); // delay for a second

  xTaskCreate(task_lightsense, "Light Sensor Poller", 4096, NULL, 0, NULL);
  xTaskCreate(task_http, "HTTP POST Requester", 4096, NULL, 0, NULL);
}
