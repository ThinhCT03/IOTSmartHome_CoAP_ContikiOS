#include "contiki.h"
#include "contiki-net.h"
#include "er-coap-engine.h"
#include <stdio.h>
#include <string.h>

#define DEBUG 1
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINT6ADDR(addr) PRINTF("[%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x]", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5], ((uint8_t *)addr)[6], ((uint8_t *)addr)[7], ((uint8_t *)addr)[8], ((uint8_t *)addr)[9], ((uint8_t *)addr)[10], ((uint8_t *)addr)[11], ((uint8_t *)addr)[12], ((uint8_t *)addr)[13], ((uint8_t *)addr)[14], ((uint8_t *)addr)[15])
#else
#define PRINTF(...)
#define PRINT6ADDR(addr)
#endif

/* Addresses of servers */
#define CO_SENSOR_NODE(ipaddr) uip_ip6addr(ipaddr, 0xfe80, 0, 0, 0, 0x0212, 0x7403, 0x0003, 0x0303)
#define FAN_SENSOR_NODE(ipaddr) uip_ip6addr(ipaddr, 0xfe80, 0, 0, 0, 0x0212, 0x7402, 0x0002, 0x0202)

#define LOCAL_PORT UIP_HTONS(COAP_DEFAULT_PORT + 1)
#define REMOTE_PORT UIP_HTONS(COAP_DEFAULT_PORT)
#define QUERY_INTERVAL 720

PROCESS(co_fan_client_process, "CO-Fan Client Process");
AUTOSTART_PROCESSES(&co_fan_client_process);

uip_ipaddr_t co_sensor_ipaddr;
uip_ipaddr_t fan_sensor_ipaddr;
static struct etimer periodic_timer;

static void co_sensor_response_handler(void *response) {
  const uint8_t *payload;
  int len = coap_get_payload(response, &payload);

  if (len > 0) {
    char response_msg[64];
    snprintf(response_msg, len + 1, "%s", payload);
    PRINTF("Received CO Sensor Response: %s\n", response_msg);

    int co_level;
    if (sscanf(response_msg, "CO: %d ppm", &co_level) == 1) {
      if (co_level > 100) {
        PRINTF("CO level (%d ppm) is dangerous! Sending POST to turn on fan.\n", co_level);

        static coap_packet_t request[1];
        coap_init_message(request, COAP_TYPE_CON, COAP_POST, 0);
        coap_set_header_uri_path(request, "Sensor/fan_sensor");
        coap_set_payload(request, (uint8_t *)"TURNON", strlen("TURNON"));

        COAP_BLOCKING_REQUEST(&fan_sensor_ipaddr, REMOTE_PORT, request, NULL);
      } else {
        PRINTF("CO level (%d ppm) is safe, no action needed.\n", co_level);
      }
    } else {
      PRINTF("Failed to parse CO level from response.\n");
    }
  }
}

PROCESS_THREAD(co_fan_client_process, ev, data) {
  PROCESS_BEGIN();

  PRINTF("Starting CO-Fan Client Process\n");

  CO_SENSOR_NODE(&co_sensor_ipaddr);
  FAN_SENSOR_NODE(&fan_sensor_ipaddr);

  coap_init_engine();

  etimer_set(&periodic_timer, CLOCK_SECOND * QUERY_INTERVAL);

  while (1) {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

    PRINTF("Sending GET request to CO sensor.\n");

    static coap_packet_t request[1];
    coap_init_message(request, COAP_TYPE_CON, COAP_GET, 0);
    coap_set_header_uri_path(request, "Sensor/co_sensor");

    COAP_BLOCKING_REQUEST(&co_sensor_ipaddr, REMOTE_PORT, request, co_sensor_response_handler);

    etimer_reset(&periodic_timer);
  }

  PROCESS_END();
}

