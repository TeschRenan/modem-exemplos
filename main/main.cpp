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

uint8_t firstConn = 1;
uint authMode = 0; //O = ABP / 1 = OTAA

extern "C" void app_main()
{
   
    modem.init(GPIO_NUM_4,GPIO_NUM_5,UART_NUM_1,9600);
	char temp[256] = {0};

	if(modem.atCmdWaitResponse("AT\n","OK",NULL,-1, 5000, NULL, 0 ) != 0){

		ESP_LOGW(__func__, "Modem OK");
	
	}
	else{

		ESP_LOGW(__func__, "Fail to connect in modem");
	
	}

	if(firstConn == 1){

		ESP_LOGE(__func__, "Initialize First Connection");

		if(authMode == 0){
		
			if(modem.atCmdWaitResponse("AT+NJM=0\n","OK",NULL,-1, 1000, NULL, 0 ) != 0){

				ESP_LOGW(__func__, "Set ABP Mode OK");

			}

		}
		else{
		
			if(modem.atCmdWaitResponse("AT+NJM=1\n","OK",NULL,-1, 1000, NULL, 0 ) != 0){

				ESP_LOGW(__func__, "Set OTAA Mode OK");

			}
			
		}

		//Waiting reboot modem
		vTaskDelay(1000 / portTICK_PERIOD_MS);

		if(modem.atCmdWaitResponse("AT+ADR=0\n","OK",NULL,-1, 1000, NULL, 0 ) != 0){

			ESP_LOGW(__func__, "Set ADR 0 OK");

		}

		if(modem.atCmdWaitResponse("AT+TXP=10\n","OK",NULL,-1, 1000, NULL, 0 ) != 0){

			ESP_LOGW(__func__, "Set Max transmit power OK");

		}

		if(modem.atCmdWaitResponse("AT+DR=0\n","OK",NULL,-1, 1000, NULL, 0 ) != 0){

			ESP_LOGW(__func__, "Set Max Data Rata SF 12 OK");

		}

		if(modem.atCmdWaitResponse("AT+SAVE\n","OK",NULL,-1, 1000, NULL, 0 ) != 0){

			ESP_LOGW(__func__, "Save the updates");

		}

		ESP_LOGW(__func__, "First Connection Success");

		esp_restart();
		
	}

	if(modem.atCmdWaitResponse("AT\r\n","OK",NULL,-1, 5000, NULL, 0 ) != 0){

		ESP_LOGW(__func__, "Modem OK");
	
	}
	else{

		ESP_LOGW(__func__, "Fail to connect in modem");
	
	}

	if(authMode == 1){

		modem.write("AT+DEUI=?\n");

		if(modem.getResponse(temp,"",sizeof(temp),10000) != 0)	{

			ESP_LOGW(__func__, "AT+DEUI RESPONSE ---> %s",temp);
		}

		memset(temp,0,256);
		
		modem.write("AT+APPKEY=?\n");

		if(modem.getResponse(temp,"",sizeof(temp),10000) != 0)	{

			ESP_LOGW(__func__, "AT+APPKEY=? RESPONSE ---> %s",temp);
		}
	
		if(modem.atCmdWaitResponse("AT+JOIN\r\n","OK",NULL,-1, 5000, NULL, 0 ) != 0){


			ESP_LOGW(__func__, "Join OK");
		
		}
		else{

			ESP_LOGW(__func__, "Join Fail");

		}

		for (size_t i = 0; i < 10; i++)
		{
			if(modem.atCmdWaitResponse("AT+NJS=?\r\n","1",NULL,-1, 1000, NULL, 0 ) != 0){


				ESP_LOGW(__func__, "Network Joined");
				break;
		
			}
		}

	}
	else{

		modem.write("AT+DEUI=?\n");

		if(modem.getResponse(temp,"",sizeof(temp),10000) != 0)	{

			ESP_LOGW(__func__, "AT+DEUI RESPONSE ---> %s",temp);
		}

		memset(temp,0,256);
		
		modem.write("AT+DADDR=?\n");

		if(modem.getResponse(temp,"",sizeof(temp),10000) != 0)	{

			ESP_LOGW(__func__, "AT+DADDR=? RESPONSE ---> %s",temp);
		}

		memset(temp,0,256);
		
		modem.write("AT+NWKSKEY=?\n");

		if(modem.getResponse(temp,"",sizeof(temp),10000) != 0)	{

			ESP_LOGW(__func__, "AT+NWKSKEY=? RESPONSE ---> %s",temp);
		}

		memset(temp,0,256);
		
		modem.write("AT+APPSKEY=?\n");

		if(modem.getResponse(temp,"",sizeof(temp),10000) != 0)	{

			ESP_LOGW(__func__, "AT+APPSKEY=? RESPONSE ---> %s",temp);
		}

	}

	char testSend[32] = {0};

	strncpy(testSend,"AT+SEND=50:",sizeof(testSend));
	strcat(testSend,"Hello World");

	if(modem.atCmdWaitResponse(testSend,"OK",NULL,-1, 5000, NULL, 0 ) != 0){


		ESP_LOGW(__func__, "Send Text to port 50 is Successfull");

	}
	else{

		ESP_LOGW(__func__, "Send Text fail");

	}

	modem.write("AT+RECV=?\n");

	if(modem.getResponse(temp,"",sizeof(temp),10000) != 0)	{

		ESP_LOGW(__func__, "AT+RECV=?\n RESPONSE ---> %s ",temp);

	}

}
