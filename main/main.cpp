#include <string>
#include <ctype.h>
#include <iostream>
#include <cstdint>
#include <cstring>
#include <inttypes.h>

extern "C"
{
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_log.h"
}

#include "modem.h"

modem modem;


extern "C" void app_main()
{
   
    modem.init(GPIO_NUM_4,GPIO_NUM_5,UART_NUM_1,115200);

	if(modem.atCmdWaitResponse("AT\r\n","OK",NULL,sizeof("AT\r\n"), 5000, NULL, 0 ) != 0){


		ESP_LOGW(__func__, "Modem OK");
	
	}

}
