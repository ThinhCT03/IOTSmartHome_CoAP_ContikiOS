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

static uint8_t door_state = 0;  // 1: Open, 0: Close

// GET handler for door sensor
static void res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) {
  const char *msg = "Door State: Closed";
  if (door_state == 1) {
    msg = "Door State: Opened";
  }
  PRINTF("GET Request received, responding with: %s\n", msg);

  REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
  REST.set_response_payload(response, (uint8_t *)msg, strlen(msg));
}

// POST handler for door sensor resource
static void res_post_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) {
  size_t len;
  const char *payload = NULL;

  // Extract the POST payload
  len = REST.get_request_payload(request, (const uint8_t **)&payload);
  if (len > 0) {
    if (strncmp(payload, "OPEN", len) == 0) {
      door_state = 1;  // Open door
      PRINTF("POST Request received: OPEN, Door state set to open\n");
    } else if (strncmp(payload, "CLOSE", len) == 0) {
      door_state = 0;  // Close door
      PRINTF("POST Request received: CLOSE, Door state set to close\n");
    } else {
      PRINTF("POST Request received: Unknown command\n");
    }
  }

  // Respond with the updated door state
  const char *response_msg = door_state ? "door State: OPENED" : "Door State: CLOSED";
  REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
  REST.set_response_payload(response, (uint8_t *)response_msg, strlen(response_msg));
}

// Define the resource for the door sensor
RESOURCE(door_sensor,
         "title=\"Door Sensor\";rt=\"State\"",
         res_get_handler,
         res_post_handler,  // Add POST handler here
         NULL, NULL);

PROCESS(rest_server_example, "Door Sensor Example Server");
AUTOSTART_PROCESSES(&rest_server_example);

PROCESS_THREAD(rest_server_example, ev, data) {
  PROCESS_BEGIN();

  PRINTF("Starting Door Sensor Example Server\n");

  // Initialize REST engine
  rest_init_engine();

  // Activate button sensor
  SENSORS_ACTIVATE(button_sensor);

  // Activate the resource for door sensor
  rest_activate_resource(&door_sensor, "Sensor/door_sensor");

  PRINTF("Starting Door Server\n");

  while (1) {
    PROCESS_WAIT_EVENT();

    if (ev == sensors_event && data == &button_sensor) {
      PRINTF("Button pressed: toggling door state\n");

      // Toggle door state
      door_state = !door_state;
      PRINTF("Door state changed to: %s\n", door_state ? "OPEN" : "CLOSE");
    }
  }

  PROCESS_END();
}

