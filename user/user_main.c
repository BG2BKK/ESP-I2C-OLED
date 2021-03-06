#include "esp_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/portmacro.h"

#include "lwip/sockets.h"
#include "uart.h"
#include "dmsg.h"
#include "wifi.h"
#include "shell.h"
#include "xmit.h"
#include "i2c.h"



void ICACHE_FLASH_ATTR uart_init(void)
{
    uart_param_t uart_param;
//    uart_param.uart_baud_rate = UART_BAUD_RATE_9600;
    uart_param.uart_baud_rate = UART_BAUD_RATE_115200;
    uart_param.uart_xfer_bit = UART_XFER_8_BIT;
    uart_param.uart_parity_mode = UART_PARITY_NONE;
    uart_param.uart_stop_bit = UART_1_STOP_BIT;
    uart_param.uart_flow_ctrl = UART_NONE_FLOW_CTRL;
    uart0_init(&uart_param);
}



LOCAL void ICACHE_FLASH_ATTR shell_task(void *pvParameters)
{
    os_event_t e;
    char ch;

    for (;;)
    {
        if (xQueueReceive(xQueueUart, (void *)&e, (portTickType)portMAX_DELAY))
        {
            switch (e.event)
            {
                case UART_EVENT_RX_CHAR:
                    ch = (char)e.param;
                    shell_process_char(ch);
                    break;
                default:
                    break;
            }
        }
    }
    vTaskDelete(NULL);
}




void ICACHE_FLASH_ATTR user_init(void)
{
    int ret, i;
    dmsg_init();
    uart_init();
    shell_init(uart0_putchar);
	shell_puts("i2c_init\n");
    i2c_init();
	shell_puts("oled_init\n");
	oled_init(1);
//    wifi_init();

	shell_printf("%d %d\n", oled_get_height(1), oled_get_width(1));
	for(i=0; i < 20; i++)
		oled_draw_hline(1, 0, i, 90, 1);
	oled_refresh(1);
    xTaskCreate(shell_task, "shell", 256, NULL, tskIDLE_PRIORITY + 2, NULL);
}

