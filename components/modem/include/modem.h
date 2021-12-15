#ifndef MODEM_H
#define MODEM_H

#include <string.h>
#include <stdio.h>
#include <cstdlib>
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"


using namespace std;
		


class modem {

    private:

    static uint8_t uart_configured;
    static const char* TAG;
    static uart_port_t uart_num;
    void infoCommand(char *, int , char *);

    public:
   
        modem();
        void init(gpio_num_t ,gpio_num_t, uart_port_t, int);
        uint8_t atCmdWaitResponse(char * , char *, char * , int , int , char **, int );
        uint8_t verifyResponse(char* , int );
        uint8_t getResponse(char *,char *, int ,  int );
        int32_t write(char* );
        int8_t write(uint8_t );
    
};

#endif
