#include "contiki.h"
#include "rest-engine.h"
#include "dev/button-sensor.h"
#include <stdio.h>
#include <string.h>

#define DEBUG 1
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

static uint8_t light_state = 0;  // 1: ON, 0: OFF

// GET handler for light resource
static void res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) {
  const char *msg = "Light State: OFF";
  if (light_state == 1) {
    msg = "Light State: ON";
  }
  PRINTF("GET Request received, responding with: %s\n", msg);

  REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
  REST.set_response_payload(response, (uint8_t *)msg, strlen(msg));
}

// POST handler for light resource
static void res_post_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) {
  size_t len;
  const char *payload = NULL;

  // Extract the POST payload
  len = REST.get_request_payload(request, (const uint8_t **)&payload);
  if (len > 0) {
    if (strncmp(payload, "TURNON", len) == 0) {
      light_state = 1;  // Turn fan ON
      PRINTF("POST Request received: TURNON, Light state set to ON\n");
    } else if (strncmp(payload, "TURNOFF", len) == 0) {
      light_state = 0;  // Turn fan OFF
      PRINTF("POST Request received: TURNOFF, Light state set to OFF\n");
    } else {
      PRINTF("POST Request received: Unknown command\n");
    }
  }

  // Respond with the updated light state
  const char *response_msg = light_state ? "Light State: ON" : "Light State: OFF";
  REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
  REST.set_response_payload(response, (uint8_t *)response_msg, strlen(response_msg));
}

// Define the resource for the fan sensor
RESOURCE(light_switch,
         "title=\"Light Switch\";rt=\"State\"",
         res_get_handler,
         res_post_handler,  // Add POST handler here
         NULL, NULL);

PROCESS(rest_server_example, "Light Switch Example Server");
AUTOSTART_PROCESSES(&rest_server_example);

PROCESS_THREAD(rest_server_example, ev, data) {
  PROCESS_BEGIN();

  PRINTF("Starting Light Sensor Example Server\n");

  // Initialize REST engine
  rest_init_engine();

  // Activate button sensor
  SENSORS_ACTIVATE(button_sensor);

  // Activate the resource for light switch
  rest_activate_resource(&light_switch, "Control/Light_switch");

  PRINTF("Starting Light Server\n");

  while (1) {
    PROCESS_WAIT_EVENT();

    if (ev == sensors_event && data == &button_sensor) {
      PRINTF("Button pressed: toggling light state\n");

      // Toggle light state
      light_state = !light_state;
      PRINTF("Light state changed to: %s\n", light_state ? "ON" : "OFF");
    }
  }

  PROCESS_END();
}

