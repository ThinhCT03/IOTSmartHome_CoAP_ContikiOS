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

static uint16_t co_level = 0; // Biến mô phỏng nồng độ CO

// Giả lập cảm biến CO
static uint16_t get_fake_co() {
  return (random_rand() % 120) + 1; 
}

// Xử lý yêu cầu GET
static void res_get_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) {
  char msg[64]; // Tạo mảng ký tự để chứa thông báo (kích thước đủ lớn)

  // Lấy giá trị CO giả lập
  co_level = get_fake_co();

  // Kiểm tra nồng độ CO và thay đổi thông báo
  if (co_level > 100) {
    snprintf(msg, sizeof(msg), "CO: %u ppm, Danger! Take Action", co_level);
  } else if (co_level > 50) {
    snprintf(msg, sizeof(msg), "CO: %u ppm, Warning! Be Cautious", co_level);
  } else {
    snprintf(msg, sizeof(msg), "CO: %u ppm, Safe", co_level);
  }

  PRINTF("GET Request received, responding with: %s\n", msg);

  // Gửi phản hồi CoAP
  REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
  REST.set_response_payload(response, (uint8_t *)msg, strlen(msg));
}

// Tài nguyên cảm biến CO
RESOURCE(co_sensor, 
         "title=\"CO Sensor\";rt=\"State\"", 
         res_get_handler, 
         NULL, 
         NULL, 
         NULL);

PROCESS(rest_server_example, "CO Sensor Server");
AUTOSTART_PROCESSES(&rest_server_example);

PROCESS_THREAD(rest_server_example, ev, data) {
  PROCESS_BEGIN();

  PRINTF("Starting CO Sensor Server\n");

  // Initialize the REST engine
  rest_init_engine();
  rest_activate_resource(&co_sensor, "Sensor/co_sensor");

  // Khởi tạo timer định kỳ
  etimer_set(&periodic_timer, CLOCK_SECOND * 600);  // Timer mỗi 10 phút (600 giây)

  // Khởi tạo bộ sinh số ngẫu nhiên
  random_init(0);  // Sử dụng random_init không tham số

  while (1) {
    PROCESS_WAIT_EVENT();

    if (etimer_expired(&periodic_timer)) {
      co_level = get_fake_co();  // Lấy nồng độ CO giả lập mới

      PRINTF("Measured CO Level: %u ppm\n", co_level); // In mức CO hiện tại

      // Đặt lại timer
      etimer_reset(&periodic_timer);
    }
  }

  PROCESS_END();
}
