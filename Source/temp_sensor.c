#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"
#include "er-coap-engine.h" // For CoAP functionality
#include "rest-engine.h"
#include "random.h" // Dùng cho sinh số ngẫu nhiên

#define DEBUG 1
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)

#endif

static struct etimer periodic_timer; // Timer định kỳ

static uint16_t temperature = 0; // Biến nhiệt độ toàn cục

// Giả lập cảm biến nhiệt độ
static uint16_t get_fake_temperature() {
  return (random_rand() % 23) + 20; // Giả lập nhiệt độ từ 20 đến 42 độ C
}

// Xử lý yêu cầu GET
static void res_get_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) {
  char msg[64]; // Tạo mảng ký tự để chứa thông báo (kích thước đủ lớn)

  // Lấy giá trị nhiệt độ giả lập
  temperature = get_fake_temperature();

  // Kiểm tra nhiệt độ và thay đổi thông báo
  if (temperature > 26) {
    snprintf(msg, sizeof(msg), "Temp: %u, High Temp --> Turn Fan", temperature);
  } else {
    snprintf(msg, sizeof(msg), "Temp: %u, Temperature Normal", temperature);
  }

  PRINTF("GET Request received, responding with: %s\n", msg);

  // Gửi phản hồi CoAP
  REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
  REST.set_response_payload(response, (uint8_t *)msg, strlen(msg));
}

// Tài nguyên nhiệt độ
RESOURCE(temp_solar, 
         "title=\"Temperature\";rt=\"State\"", 
         res_get_handler, 
         NULL, 
         NULL, 
         NULL);

PROCESS(rest_server_example, "Temperature Sensor Server");
AUTOSTART_PROCESSES(&rest_server_example);

PROCESS_THREAD(rest_server_example, ev, data) {
  PROCESS_BEGIN();

  PRINTF("Starting Temperature Sensor Server\n");

  // Initialize the REST engine
  rest_init_engine();
  rest_activate_resource(&temp_solar, "Sensor/temp_solar");

  // Khởi tạo timer định kỳ (10 phút cho ví dụ)
  etimer_set(&periodic_timer, CLOCK_SECOND * 600);  // Timer mỗi 5 phút (300 giây)

  // Khởi tạo bộ sinh số ngẫu nhiên
  random_init(0);  // Sử dụng random_init không tham số

  while (1) {
    PROCESS_WAIT_EVENT();

    if (etimer_expired(&periodic_timer)) {
      temperature = get_fake_temperature();  // Lấy nhiệt độ giả lập mới

      PRINTF("Measured Temperature: %u\n", temperature); // In nhiệt độ hiện tại

      // Đặt lại timer
      etimer_reset(&periodic_timer);
    }
  }

  PROCESS_END();
}

