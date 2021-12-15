#include "modem.h"

modem::modem(){

}

uint8_t modem::uart_configured = 0;

uart_port_t modem::uart_num;

const char* modem::TAG = "[MODEM DRIVER]";

/**
 * @brief Inicia a UART para o funcionamento .
 * @param [tx]: Pino TX da UART.
 * @param [rx]: Pino RX da UART.
 * @param [uart_num]: Numero da UART a ser usada na comunicação EX: UART_NUM_1.
 * @param [baud_rate]: Baud_rate de comunicação, default 115200.
 * 
**/
void modem::init(gpio_num_t tx,gpio_num_t rx, uart_port_t uart_num, int baud_rate = 115200)
{
	gpio_set_direction(tx, GPIO_MODE_OUTPUT);
	gpio_set_direction(rx, GPIO_MODE_INPUT);
	gpio_set_pull_mode(rx, GPIO_PULLUP_ONLY);
	
	this->uart_num = uart_num;
	
    uart_config_t uart_config = {
        .baud_rate = baud_rate,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    
	uart_param_config(uart_num, &uart_config);
    uart_set_pin(uart_num, tx, rx, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(uart_num, 2048, 1024, 0, NULL, 0);
	
	uart_configured = 1;

}

void modem::infoCommand(char *cmd, int cmdSize, char *info)
{
	char buf[cmdSize+2];
	memset(buf, 0, cmdSize+2);

	for (int i=0; i<cmdSize;i++) {
		if ((cmd[i] != 0x00) && ((cmd[i] < 0x20) || (cmd[i] > 0x7F))) 
			buf[i] = '.';
		else 
			buf[i] = cmd[i];
		if (buf[i] == '\0') 
			break;
	}
	ESP_LOGI(TAG,"%s [%s]", info, buf);
}

/**
 * 
 * @brief Envia um comando AT com verificação de mensagem de retorno e timeout.
 * 
 * @param [cmd]: Comando a ser enviado.
 * @param [resp]: Opção 0 de Resposta aguardada.
 * @param [resp1]: Opção 1 de Resposta aguardada.
 * @param [cmdSize]: Tamanho da string de comando.
 * @param [timeout]: Tempo maximo a ser esperado pela resposta
 * @param [response]: String de resposta
 * @param [size]: Tamanho do buffer de verificação.
 * 
 * @return [0]: Timeout.
 * @return [1]: Sucesso resp.
*  @return [2]: Sucesso resp1.
**/
uint8_t modem::atCmdWaitResponse(char * cmd, char *resp, char * resp1, int cmdSize, int timeout, char **response, int size)
{
	char sresp[256] = {'\0'};
	char data[256] = {'\0'};
    int len, res = 1, idx = 0, tot = 0, timeoutCnt = 0;

	// ** Send command to Modem
	vTaskDelay(100 / portTICK_PERIOD_MS);
	uart_flush(uart_num);

	if (cmd != NULL) {
		if (cmdSize == -1) 
			cmdSize = strlen(cmd);
		
		infoCommand(cmd, cmdSize, "AT COMMAND:");
		
		uart_write_bytes(uart_num, (const char*)cmd, cmdSize);
		uart_wait_tx_done(uart_num, 100 / portTICK_RATE_MS);
	}

	if (response != NULL) 
	{
		// Read Modem response into buffer
		char *pbuf = *response;
		len = uart_read_bytes(uart_num, (uint8_t*)data, 256, timeout / portTICK_RATE_MS);
		while (len > 0) {
			if ((tot+len) >= size) {
				char *ptemp = (char*)realloc(pbuf, size+512);
				if (ptemp == NULL) return 0;
				size += 512;
				pbuf = ptemp;
			}
			memcpy(pbuf+tot, data, len);
			tot += len;
			response[tot] = 0;
			len = uart_read_bytes(uart_num, (uint8_t*)data, 256, 100 / portTICK_RATE_MS);
		}
		*response = pbuf;
		return tot;
	}
    // ** Wait for and check the response
	idx = 0;
	while(1)
	{
		memset(data, 0, 256);
		len = 0;
		len = uart_read_bytes(uart_num, (uint8_t*)data, 256, 10 / portTICK_RATE_MS);
		if (len > 0) 
		{
			for (int i=0; i<len;i++) {
				if (idx < 256) {
					if ((data[i] >= 0x20) && (data[i] < 0x80)) 
						sresp[idx++] = data[i];
					else 
						sresp[idx++] = 0x2e;
				}
			}
			tot += len;
		}
		else 
		{
			if (tot > 0) {
				// Check the response
				if (strstr(sresp, resp) != NULL) 
				{
					
					ESP_LOGI(TAG,"AT RESPONSE: [%s]", sresp);
					
					break;
				}
				else 
				{
					if (resp1 != NULL) 
					{
						if (strstr(sresp, resp1) != NULL) 
						{
							
							ESP_LOGI(TAG,"AT RESPONSE (1): [%s]", sresp);
							
							res = 2;
							break;
						}
					}
					// no match
					
					ESP_LOGI(TAG,"AT BAD RESPONSE: ---> [%s]", sresp);
					ESP_LOGI(TAG,"EXPECTED RESPONSE: ---> [%s]", resp);
					
					
					res = 0;
					break;
				}
			}
		}

		timeoutCnt += 10;
		if (timeoutCnt > timeout) {
			// timeout
			
			ESP_LOGE(TAG,"AT: TIMEOUT");
			
			res = 0;
			break;
		}
	}

	return res;
}

/**
 * @brief Verifica resposta se condiz com esperado.
 * 
 * @param [resp]: String a ser validada.
 * @param [timeout]: Tempo maximo a ser esperado pela resposta
 * 
 * @return [0]: Timeout out ou retorno divergente.
 * @return [1]: Sucesso resp.
**/
uint8_t modem::verifyResponse(char* resp, int timeout){

	char sresp[256] = {'\0'};
	char data[256] = {'\0'};
    int len, res = 1, idx = 0, tot = 0, timeoutCnt = 0;
	
	while(1)
	{
		memset(data, 0, 256);
		len = 0;
		len = uart_read_bytes(uart_num, (uint8_t*)data, 256, 10 / portTICK_RATE_MS);
		if (len > 0) 
		{
			for (int i=0; i<len;i++) {
				if (idx < 256) {
					if ((data[i] >= 0x20) && (data[i] < 0x80)) 
						sresp[idx++] = data[i];
					else 
						sresp[idx++] = 0x2e;
				}
			}
			tot += len;
		}
		else 
		{
			if (tot > 0) {
				// Check the response
				if (strstr(sresp, resp) != NULL) 
				{
					
					ESP_LOGI(TAG,"AT RESPONSE: [%s]", sresp);
					

					res = 1;
					
					break;
				}
				else 
				{
					// no match
					
					ESP_LOGI(TAG,"AT BAD RESPONSE: ---> [%s]", sresp);
					
					res = 0;
					break;
				}
			}
		}

		timeoutCnt += 10;
		if (timeoutCnt > timeout) {
			// timeout
			
			ESP_LOGE(TAG,"AT: TIMEOUT");
			
			res = 0;
			break;
		}
	}

	return res;

}
/**
 * 
 * @brief Verifica resposta se condiz com esperado retornando seu valor para resp.
 * 
 * @param [*resp]: String de resposta 
 * @param [Key]: Resposta a ser validada.
 * @param [lenght]: Tamanho maximo da resposta
 * 
 * @param [timeout]: Tempo maximo a ser esperado pela resposta
 * @return [0]: Timeout out .
 * @return [1]: Sucesso resp.
 * 
**/
uint8_t modem::getResponse(char *resp,char *key, int lenght,  int timeout){

 	char sresp[256] = {'\0'};
	char data[256] = {'\0'};
    int len, res = 1, idx = 0, tot = 0, timeoutCnt = 0;
	
	
	idx = 0;
	while(1)
	{
		memset(data, 0, 256);
		len = 0;
		len = uart_read_bytes(uart_num, (uint8_t*)data, 256, 10 / portTICK_RATE_MS);
		if (len > 0) 
		{
			for (int i=0; i<len;i++) {
				if (idx < 256) {
					if ((data[i] >= 0x20) && (data[i] < 0x80)) 
						sresp[idx++] = data[i];
					else 
						sresp[idx++] = 0x2e;
				}
			}
			tot += len;
		}
		else 
		{
			if (tot > 0) {	

				if (strstr(sresp, key) != NULL){ 

					
					ESP_LOGI(TAG,"AT RESPONSE: [%s]", sresp);
						
						
					strncpy(resp, sresp, lenght);

					res = 1;
					break;	

				}

			}

		}
			
		timeoutCnt += 10;
		if (timeoutCnt > timeout) {
			// timeout
			
			ESP_LOGE(TAG,"AT: TIMEOUT");
			
			res = 0;
			break;
		}

	}

	return res;
}

/**
 * @brief Envia um byte pela UART.
 * 
 * @param [value]: Byte a ser enviado.
 * @param [timeout]: Tempo maximo a ser esperado pela resposta
 * 
 * @return [size]: tamanho do byte enviado.
**/

int8_t modem::write(uint8_t value)
{
	char data[] = { value };
    int size = uart_write_bytes(uart_num, data, 1);
	
	ESP_LOGI(TAG, "Wrote one byte %s", data);

    return size;
}

/**
 * @brief Envia uma string pela UART.
 * 
 * @param [value]: String a ser enviado.
 * @param [timeout]: Tempo maximo a ser esperado pela resposta
 * 
 * @return [size]: tamanho em bytes enviado.
**/
int32_t modem::write(char* value)
{
	int len = strlen(value);
    int size = uart_write_bytes(uart_num, value, len);
	
	ESP_LOGI(TAG, "Wrote string: %s, byte %i", value, size);

    return size;
}


