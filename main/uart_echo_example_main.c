
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include "string.h"

/* 
 * STEPS
 *     1. Set Communication Parameters - Setting baud rate, data bits, stop bits, etc.
 *     2. Set Communication Pins - Assigning pins for connection to a device
 *     3. Install Drivers - Allocating ESP32’s resources for the UART driver
 *     4. Run UART Communication - Sending/receiving data
*/

static const char *TAG = "UART TEST";

static void echo_task(void *arg)
{
    const uart_port_t uart_num = UART_NUM_2;
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = UART_SCLK_APB,
    };
    // Configure UART parameters
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));

    // Set UART pins(TX: IO4, RX: IO5, RTS: IO18, CTS: IO19)
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_2, 17, 16, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    // Setup UART buffered IO with event queue
    const int uart_buffer_size = (1024 * 2);
    QueueHandle_t uart_queue;
    // Install UART driver using an event queue here
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_2, uart_buffer_size, \
                                            uart_buffer_size, 10, &uart_queue, 0));

    // Write data to UART.
    // char* test_str = "This is a test string.\n\r";

    uint8_t rx_data[128];

    while (1) {
        // Read data from the UART
        int len = uart_read_bytes(uart_num, rx_data, 128-1, 10);

        // Write data back to the UART
        uart_write_bytes(uart_num, (const char *) rx_data, len);

        if (len) {
            rx_data[len] = '\0';
            ESP_LOGI(TAG, "Recv str: %s", (char *) rx_data);
        }

        //uart_write_bytes(uart_num, (const char*)test_str, strlen(test_str));
    }

}

void app_main(void)
{
    xTaskCreate(echo_task, "uart_echo_task", CONFIG_EXAMPLE_TASK_STACK_SIZE, NULL, 10, NULL);
}
