/**
	@file modem.h
	@author Renan Tesch

	MIT License

	Copyright (c) Copyright 2023 Renan Tesch
	GitHub https://github.com/TeschRenan

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/


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
        void flush();
    
};

#endif
