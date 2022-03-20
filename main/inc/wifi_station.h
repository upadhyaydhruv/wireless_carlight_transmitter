#include "freertos/event_groups.h"

#define EXAMPLE_ESP_WIFI_SSID "Shiva-2.4G"
#define EXAMPLE_ESP_WIFI_PASS "Brahmand#2021"
#define EXAMPLE_ESP_MAXIMUM_RETRY 10

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about
 * two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

static const char *TAG = "wifi station";

static int s_retry_num = 0;
void wifi_init_sta(void);
void wifi_connect(void);