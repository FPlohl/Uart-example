#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "app_uart.h"
#include "app_error.h"
#include "nrf_delay.h"
#include "nrf.h"
#include "bsp.h"
#include "boards.h"
#include <string.h>
#if defined (UART_PRESENT)
#include "nrf_uart.h"
#endif
#if defined (UARTE_PRESENT)
#include "nrf_uarte.h"
#endif


#define MAX_TEST_DATA_BYTES     (15U)                /**< max number of test bytes to be used for tx and rx. */
#define UART_TX_BUF_SIZE 256                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE 256                         /**< UART RX buffer size. */

uint8_t rx_buff[100];
uint8_t i;

void uart_error_handle(app_uart_evt_t * p_event)
{
    if (p_event->evt_type == APP_UART_COMMUNICATION_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_communication);
    }
    else if (p_event->evt_type == APP_UART_FIFO_ERROR)
    {
        APP_ERROR_HANDLER(p_event->data.error_code);
    }
    else if(p_event->evt_type == APP_UART_DATA_READY){ // character in buffer and available to read
        uint8_t cr;

        app_uart_get(&cr);  // read character from rx buffer
        app_uart_put(cr);   // echo read character

        rx_buff[i++] = cr;

        if(strstr(rx_buff, "high")){
            bsp_board_leds_on();
            printf("\r\nLED ON\r\n");
            memset(rx_buff,0,100);
            i = 0;
        }

        else if(strstr(rx_buff, "low")){
            bsp_board_leds_off();
            printf("\r\nLED OFF\r\n");
            memset(rx_buff,0,100);
            i = 0;
        }
    }
}

#define UART_HWFC APP_UART_FLOW_CONTROL_DISABLED

int main(void)
{
    uint32_t err_code;

    bsp_board_init(BSP_INIT_LEDS);

    const app_uart_comm_params_t comm_params =
      {
          RX_PIN_NUMBER,
          TX_PIN_NUMBER,
          RTS_PIN_NUMBER,
          CTS_PIN_NUMBER,
          UART_HWFC,
          false,
#if defined (UART_PRESENT)
          NRF_UART_BAUDRATE_115200
#else
          NRF_UARTE_BAUDRATE_115200
#endif
      };

    APP_UART_FIFO_INIT(&comm_params,
                         UART_RX_BUF_SIZE,
                         UART_TX_BUF_SIZE,
                         uart_error_handle,
                         APP_IRQ_PRIORITY_LOWEST,
                         err_code);

    APP_ERROR_CHECK(err_code);

    while (true)
    {
        // No operation
    }
}
