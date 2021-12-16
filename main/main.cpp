#include <string>
#include <ctype.h>
#include <iostream>
#include <cstdint>
#include <cstring>
#include <inttypes.h>
#include <string.h>

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
   
    modem.init(GPIO_NUM_4,GPIO_NUM_5,UART_NUM_1,9600);

	if(modem.atCmdWaitResponse("AT\r\n","OK",NULL,sizeof("AT\r\n"), 5000, NULL, 0 ) != 0){

		ESP_LOGW(__func__, "Modem OK");
	
	}
	else{

		ESP_LOGW(__func__, "Fail to connect in modem");
	
	}

	if(modem.atCmdWaitResponse("AT+JOIN\r\n","OK",NULL,sizeof("AT+JOIN\r\n"), 5000, NULL, 0 ) != 0){


		ESP_LOGW(__func__, "Join OK");
	
	}
	else{

		ESP_LOGW(__func__, "Join Fail");

	}

	for (size_t i = 0; i < 10; i++)
	{
		if(modem.atCmdWaitResponse("AT+NJS=?\r\n","1",NULL,sizeof("AT+NJS=?\r\n"), 1000, NULL, 0 ) != 0){


			ESP_LOGW(__func__, "Network Joined");
			break;
	
		}
	}

	char testSend[32] = {0};

	strncpy(testSend,"AT+SEND=50:",sizeof(testSend));
	strcat(testSend,"Hello World");

	if(modem.atCmdWaitResponse(testSend,"OK",NULL,sizeof(testSend), 5000, NULL, 0 ) != 0){


		ESP_LOGW(__func__, "Send Text to port 50 is Successfull");

	}
	else{

		ESP_LOGW(__func__, "Send Text fail");

	}

}
