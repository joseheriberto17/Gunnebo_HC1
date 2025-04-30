/**
* \file
*
* \brief SAM Peripheral DMA Controller Example.
*
* Copyright (c) 2011-2014 Atmel Corporation. All rights reserved.
*
* \asf_license_start
*
* \page License
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
* 3. The name of Atmel may not be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
* 4. This software may only be redistributed and used in connection with an
*    Atmel microcontroller product.
*
* THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
* EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
* ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
* \asf_license_stop
*
*/
/**
* Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
*/

#include <asf.h>
#include <stdio_serial.h>
#include <conf_board.h>
//#include <conf_pdc_uart_example.h>

#define ERROR_UNDER	0x01	// Faltan datos, timeout
#define ERROR_OVER	0x02	// Se enviaron mas datos de los esperados ates del caracter 0xFC
#define ERROR_CHK	0x03	// Error de sumatoria
#define ERROR_ACS	0x04	// Se intenta escribir una entrada
#define ERROR_SCN	0x05	// Escenario invalido, < 16 ó > 128


#define ENCENDIDO	0xFF
#define APAGADO		0x00
#define	IGNORAR		0xAA

// Capturador Eltra
#define WUP_eltra	0x12
#define STX_eltra	0x02
#define DEST_eltra	0x42
#define ETX_eltra	0x03
#define EOT_eltra	0x04

#define ERROR_DLE_eltra 0x41	
#define ERROR_NACK_eltra 0x42	
#define ERROR_CMD_Refused_eltra 0x43
#define ERROR_CMD_no_eltra 0x44	

#define VOID_eltra 0x30	
#define TAIL_eltra 0x31	
#define HOME_eltra 0x32	
#define FULL_eltra 0x33	

#define CMD_insertCard_eltra 0x3C
#define CMD_sendSensorStatus_eltra 0x3F
#define CMD_initModule_eltra 0x44
#define CMD_ejectCard_eltra 0x46
#define CMD_captureCard_eltra 0x76


char	bufer_seria_tx[256];
unsigned char	bufer_serial_rx[256];
int		port_slots_write[256];
char	port_slots_read[256];
int		port_slots_default[256];
//
int		data_leng = 256;
int		dev_id = 0xFF;
int		escenario_temp = 0xFF;

int		port_address = 0;
int		rx_idx_RS485;
int		rx_idx_RS232;
char	funcion	= 0x20;
int		char_timeout_RS485 = 20;
int		char_timeout_RS232 = 40;
char	error_code = 0x00;
char	escenario_v = 0x00;
int		confirmacion_b_timeout = 101;
int		confirmacion_a_timeout = 101;
int		abierta_pmr_timeout = 101;
int		cerrada_pmr_timeout = 101;
/**
* \brief Interrupt handler for UART interrupt.
*/


char	init_eltra = 0xFE;

unsigned char code0_eltra = 0;
unsigned char code1_eltra = 0;
unsigned char code2_eltra = 0;
unsigned char comando_eltra = 0xFF;

int	inquire_eltra = 0x01;
int eltra_polling_timeout = 0x00;
char cmd_null[1]={0x00};

void	sendCmdEltra(char, char [], int);
void	sendCmdEltra(char cmd_eltra, char data_eltra[], int len)
{
	char innerData[32];
	char checksum = 0; 
	char checksum_hi = 0;
	char checksum_lo = 0;
	int index = 0;
	
	innerData[index++] = WUP_eltra;
	innerData[index++] = STX_eltra;
	innerData[index++] = DEST_eltra;
	innerData[index++] = cmd_eltra;

	if (len > 0) {
		for (int i=0; i<len; i++) {
			innerData[index++] = data_eltra[i];
		}
	}

	innerData[index++] = ETX_eltra;
	
	for (int j=0; j<index; j++) {
		checksum = checksum ^ innerData[j];
	}

	checksum_hi = (checksum >> 4 & 0x0F) + 0x30;
	checksum_lo = (checksum & 0x0F) + 0x30;

	innerData[index++] = checksum_hi;
	innerData[index++] = checksum_lo;

	innerData[index++] = EOT_eltra;

	
	Pdc	*serial_232;
	pdc_packet_t serial_232_TX_packet;
	serial_232 = uart_get_pdc_base(UART0);
	pdc_enable_transfer(serial_232, PERIPH_PTCR_TXTEN);
	serial_232_TX_packet.ul_addr = (uint32_t)innerData;
	serial_232_TX_packet.ul_size = index;
	pdc_tx_init(serial_232, &serial_232_TX_packet, NULL);
}	

void	sendInquireEltra(void);
void	sendInquireEltra()
{
	char buffer_eltra[2] = {0x12, 0x05};	
		
	inquire_eltra = 0;

	Pdc	*serial_232;
	pdc_packet_t serial_232_TX_packet;
	serial_232 = uart_get_pdc_base(UART0);
	pdc_enable_transfer(serial_232, PERIPH_PTCR_TXTEN);
	serial_232_TX_packet.ul_addr = (uint32_t)buffer_eltra;
	serial_232_TX_packet.ul_size = 2;
	pdc_tx_init(serial_232, &serial_232_TX_packet, NULL);

}

void	not_ack_RS485(void);
void	not_ack_RS485()
{
	if (bufer_serial_rx[0] != 0x80)
	{
		bufer_seria_tx[0] = 0xA1;
		bufer_seria_tx[1] = 0x15;
		bufer_seria_tx[2] = error_code;
		bufer_seria_tx[3] = -(0xA1 ^ 0x15 ^ error_code);
		bufer_seria_tx[4] = 0xFC;
		
		Pdc	*serial_485;
		pdc_packet_t serial_485_TX_packet;
		serial_485 = uart_get_pdc_base(UART1);
		pdc_enable_transfer(serial_485, PERIPH_PTCR_TXTEN);
		serial_485_TX_packet.ul_addr = (uint32_t)bufer_seria_tx;
		serial_485_TX_packet.ul_size = 5;
		pdc_tx_init(serial_485, &serial_485_TX_packet, NULL);
	}
}


void	not_ack_RS232(void);
void	not_ack_RS232()
{
	if (bufer_serial_rx[0] != 0x80)
	{
		bufer_seria_tx[0] = 0xA1;
		bufer_seria_tx[1] = 0x15;
		bufer_seria_tx[2] = error_code;
		bufer_seria_tx[3] = -(0xA1 ^ 0x15 ^ error_code);
		bufer_seria_tx[4] = 0xFC;
		
		Pdc	*serial_232;
		pdc_packet_t serial_232_TX_packet;
		serial_232 = uart_get_pdc_base(UART0);
		pdc_enable_transfer(serial_232, PERIPH_PTCR_TXTEN);
		serial_232_TX_packet.ul_addr = (uint32_t)bufer_seria_tx;
		serial_232_TX_packet.ul_size = 5;
		pdc_tx_init(serial_232, &serial_232_TX_packet, NULL);
	}
}


void	escenario_set(char);
void	escenario_set(char	escenario_idx)
{
	switch (escenario_idx)
	{
		case 0x00:
			port_slots_write[PASO_MODO_A_ANT_OFST] = ENCENDIDO;  //Kaba prueba Laboratorio
			port_slots_default[PASO_MODO_A_ANT_OFST] = ENCENDIDO; //Kaba prueba Laboratorio
			
			//port_slots_write[PASO_MODO_A_ANT_OFST] = ENCENDIDO;   Normal no Kaba
			//port_slots_default[PASO_MODO_A_ANT_OFST] = ENCENDIDO; Normal no Kaba
			
			port_slots_write[PASO_MODO_B_ANT_OFST] = APAGADO;
			port_slots_default[PASO_MODO_B_ANT_OFST] = APAGADO;
			
			port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
			port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;
			
			port_slots_write[PICTOGRAMA_2_OFST] = APAGADO;
			port_slots_default[PICTOGRAMA_2_OFST] = APAGADO;
			
			port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
			port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;
			
			port_slots_write[PICTOGRAMA_4_OFST] = ENCENDIDO;
			port_slots_default[PICTOGRAMA_4_OFST] = ENCENDIDO;
			
			port_slots_write[MODO_BIT_1_OFST] = APAGADO;
			port_slots_default[MODO_BIT_1_OFST] = APAGADO;
			
			port_slots_write[MODO_BIT_2_OFST] = APAGADO;
			port_slots_default[MODO_BIT_2_OFST] = APAGADO;
			
			port_slots_write[MODO_BIT_3_OFST] = APAGADO;
			port_slots_default[MODO_BIT_3_OFST] = APAGADO;
			
			port_slots_write[MODO_BIT_4_OFST] = APAGADO;
			port_slots_default[MODO_BIT_4_OFST] = APAGADO;
			
			port_slots_write[EMERGENCIA_PASO_OFST] = ENCENDIDO;
			port_slots_default[EMERGENCIA_PASO_OFST] = ENCENDIDO;
			
			port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
			port_slots_default[SOLENOIDE_S_OFST] = APAGADO;
			
			port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
			port_slots_default[SOLENOIDE_I_OFST] = APAGADO;
			
			port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
			port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;
			
			port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
			port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
			
			port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
			port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;
			
			port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
			port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;
			
			port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
			port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
			
			port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
			port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		break;
		
		case 0x01:
			port_slots_write[PASO_MODO_A_ANT_OFST] = APAGADO;
			port_slots_default[PASO_MODO_A_ANT_OFST] = APAGADO;
	
			port_slots_write[PASO_MODO_B_ANT_OFST] = APAGADO;
			port_slots_default[PASO_MODO_B_ANT_OFST] = APAGADO;
	
			port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
			port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;
	
			port_slots_write[PICTOGRAMA_2_OFST] = APAGADO;
			port_slots_default[PICTOGRAMA_2_OFST] = APAGADO;
	
			port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
			port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;
	
			port_slots_write[PICTOGRAMA_4_OFST] = APAGADO;
			port_slots_default[PICTOGRAMA_4_OFST] = APAGADO;
	
			port_slots_write[MODO_BIT_1_OFST] = APAGADO;
			port_slots_default[MODO_BIT_1_OFST] = APAGADO;
	
			port_slots_write[MODO_BIT_2_OFST] = APAGADO;
			port_slots_default[MODO_BIT_2_OFST] = APAGADO;
	
			port_slots_write[MODO_BIT_3_OFST] = APAGADO;
			port_slots_default[MODO_BIT_3_OFST] = APAGADO;
	
			port_slots_write[MODO_BIT_4_OFST] = APAGADO;
			port_slots_default[MODO_BIT_4_OFST] = APAGADO;
	
			//port_slots_write[PASO_MODO_A_NEW_OFST] = APAGADO;
			//port_slots_default[PASO_MODO_A_NEW_OFST] = APAGADO;
	
			//port_slots_write[PASO_MODO_B_NEW_OFST] = APAGADO;
			//port_slots_default[PASO_MODO_B_NEW_OFST] = APAGADO;
	
			port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
			port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;
	
			port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
			port_slots_default[SOLENOIDE_S_OFST] = APAGADO;
	
			port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
			port_slots_default[SOLENOIDE_I_OFST] = APAGADO;
	
			port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
			port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;
	
			port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
			port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
	
			port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
			port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;
	
			port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
			port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;
	
			port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
			port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
	
			port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
			port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		
		break;
	
	
	case 0x02:
		/* Your code here */
		port_slots_write[PASO_MODO_A_NEW_OFST] = ENCENDIDO;
		port_slots_default[PASO_MODO_A_NEW_OFST] = ENCENDIDO;

		port_slots_write[PASO_MODO_B_NEW_OFST] = ENCENDIDO;
		port_slots_default[PASO_MODO_B_NEW_OFST] = ENCENDIDO;

		port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_2_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_2_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_4_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_4_OFST] = APAGADO;

		port_slots_write[MODO_BIT_1_OFST] = APAGADO;
		port_slots_default[MODO_BIT_1_OFST] = APAGADO;

		port_slots_write[MODO_BIT_2_OFST] = APAGADO;
		port_slots_default[MODO_BIT_2_OFST] = APAGADO;

		port_slots_write[MODO_BIT_3_OFST] = APAGADO;
		port_slots_default[MODO_BIT_3_OFST] = APAGADO;

		port_slots_write[MODO_BIT_4_OFST] = ENCENDIDO;
		port_slots_default[MODO_BIT_4_OFST] = ENCENDIDO;

		port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_S_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_I_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;

	break;



	case 0x12:
		/* Your code here */
		port_slots_write[PASO_MODO_A_NEW_OFST] = ENCENDIDO;
		port_slots_default[PASO_MODO_A_NEW_OFST] = ENCENDIDO;

		port_slots_write[PASO_MODO_B_NEW_OFST] = ENCENDIDO;
		port_slots_default[PASO_MODO_B_NEW_OFST] = ENCENDIDO;

		port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_2_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_2_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_4_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_4_OFST] = APAGADO;

		port_slots_write[MODO_BIT_1_OFST] = APAGADO;
		port_slots_default[MODO_BIT_1_OFST] = APAGADO;

		port_slots_write[MODO_BIT_2_OFST] = APAGADO;
		port_slots_default[MODO_BIT_2_OFST] = APAGADO;

		port_slots_write[MODO_BIT_3_OFST] = ENCENDIDO;
		port_slots_default[MODO_BIT_3_OFST] = ENCENDIDO;

		port_slots_write[MODO_BIT_4_OFST] = APAGADO;
		port_slots_default[MODO_BIT_4_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_S_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_I_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;

	break;


	case 0x22:
		/* Your code here */
		port_slots_write[PASO_MODO_A_NEW_OFST] = ENCENDIDO;
		port_slots_default[PASO_MODO_A_NEW_OFST] = ENCENDIDO;

		port_slots_write[PASO_MODO_B_NEW_OFST] = ENCENDIDO;
		port_slots_default[PASO_MODO_B_NEW_OFST] = ENCENDIDO;

		port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_2_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_2_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_4_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_4_OFST] = APAGADO;

		port_slots_write[MODO_BIT_1_OFST] = APAGADO;
		port_slots_default[MODO_BIT_1_OFST] = APAGADO;

		port_slots_write[MODO_BIT_2_OFST] = ENCENDIDO;
		port_slots_default[MODO_BIT_2_OFST] = ENCENDIDO;

		port_slots_write[MODO_BIT_3_OFST] = APAGADO;
		port_slots_default[MODO_BIT_3_OFST] = APAGADO;

		port_slots_write[MODO_BIT_4_OFST] = ENCENDIDO;
		port_slots_default[MODO_BIT_4_OFST] = ENCENDIDO;

		port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_S_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_I_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;

	break;



	case 0x32:
		/* Your code here */
		port_slots_write[PASO_MODO_A_NEW_OFST] = ENCENDIDO;
		port_slots_default[PASO_MODO_A_NEW_OFST] = ENCENDIDO;

		port_slots_write[PASO_MODO_B_NEW_OFST] = ENCENDIDO;
		port_slots_default[PASO_MODO_B_NEW_OFST] = ENCENDIDO;

		port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_2_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_2_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_4_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_4_OFST] = APAGADO;

		port_slots_write[MODO_BIT_1_OFST] = APAGADO;
		port_slots_default[MODO_BIT_1_OFST] = APAGADO;

		port_slots_write[MODO_BIT_2_OFST] = ENCENDIDO;
		port_slots_default[MODO_BIT_2_OFST] = ENCENDIDO;

		port_slots_write[MODO_BIT_3_OFST] = ENCENDIDO;
		port_slots_default[MODO_BIT_3_OFST] = ENCENDIDO;

		port_slots_write[MODO_BIT_4_OFST] = APAGADO;
		port_slots_default[MODO_BIT_4_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_S_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_I_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;

	break;



	case 0x42:
		/* Your code here */
		port_slots_write[PASO_MODO_A_NEW_OFST] = ENCENDIDO;
		port_slots_default[PASO_MODO_A_NEW_OFST] = ENCENDIDO;

		port_slots_write[PASO_MODO_B_NEW_OFST] = ENCENDIDO;
		port_slots_default[PASO_MODO_B_NEW_OFST] = ENCENDIDO;

		port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_2_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_2_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_4_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_4_OFST] = APAGADO;

		port_slots_write[MODO_BIT_1_OFST] = ENCENDIDO;
		port_slots_default[MODO_BIT_1_OFST] = ENCENDIDO;

		port_slots_write[MODO_BIT_2_OFST] = APAGADO;
		port_slots_default[MODO_BIT_2_OFST] = APAGADO;

		port_slots_write[MODO_BIT_3_OFST] = ENCENDIDO;
		port_slots_default[MODO_BIT_3_OFST] = ENCENDIDO;

		port_slots_write[MODO_BIT_4_OFST] = APAGADO;
		port_slots_default[MODO_BIT_4_OFST] = APAGADO;
		
		port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_S_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_I_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;

	break;

			
		// MODO A CONTROLADO, MODO B BLOQUEADO
		case 0x03:
			port_slots_write[PASO_MODO_A_ANT_OFST] = ENCENDIDO;
			port_slots_default[PASO_MODO_A_ANT_OFST] = ENCENDIDO;
			
			port_slots_write[PASO_MODO_B_ANT_OFST] = ENCENDIDO;
			port_slots_default[PASO_MODO_B_ANT_OFST] = ENCENDIDO;
			
			port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
			port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;
			
			port_slots_write[PICTOGRAMA_2_OFST] = ENCENDIDO;
			port_slots_default[PICTOGRAMA_2_OFST] = ENCENDIDO;
			
			port_slots_write[PICTOGRAMA_3_OFST] = ENCENDIDO;
			port_slots_default[PICTOGRAMA_3_OFST] = ENCENDIDO;
			
			port_slots_write[PICTOGRAMA_4_OFST] = APAGADO;
			port_slots_default[PICTOGRAMA_4_OFST] = APAGADO;
			
			port_slots_write[MODO_BIT_1_OFST] = APAGADO;
			port_slots_default[MODO_BIT_1_OFST] = APAGADO;
			
			port_slots_write[MODO_BIT_2_OFST] = APAGADO;
			port_slots_default[MODO_BIT_2_OFST] = APAGADO;
			
			port_slots_write[MODO_BIT_3_OFST] = APAGADO;
			port_slots_default[MODO_BIT_3_OFST] = APAGADO;
			
			port_slots_write[MODO_BIT_4_OFST] = APAGADO;
			port_slots_default[MODO_BIT_4_OFST] = APAGADO;
			
			//port_slots_write[PASO_MODO_A_NEW_OFST] = APAGADO;
			//port_slots_default[PASO_MODO_A_NEW_OFST] = APAGADO;
			
			//port_slots_write[PASO_MODO_B_NEW_OFST] = APAGADO;
			//port_slots_default[PASO_MODO_B_NEW_OFST] = APAGADO;
			
			port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
			port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;
			
			port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
			port_slots_default[SOLENOIDE_S_OFST] = APAGADO;
			
			port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
			port_slots_default[SOLENOIDE_I_OFST] = APAGADO;
			
			port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
			port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;
			
			port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
			port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
			
			port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
			port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;
			
			port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
			port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;
			
			port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
			port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
			
			port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
			port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		break;
		
		// MODO A CONTROLADO, MODO B LIBRE
		case 0x13:
		port_slots_write[PASO_MODO_A_ANT_OFST] = ENCENDIDO;
		port_slots_default[PASO_MODO_A_ANT_OFST] = ENCENDIDO;
		
		port_slots_write[PASO_MODO_B_ANT_OFST] = APAGADO;
		port_slots_default[PASO_MODO_B_ANT_OFST] = APAGADO;
		
		port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;
		
		port_slots_write[PICTOGRAMA_2_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_2_OFST] = ENCENDIDO;
		
		port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;
		
		port_slots_write[PICTOGRAMA_4_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_4_OFST] = ENCENDIDO;
		
		port_slots_write[MODO_BIT_1_OFST] = APAGADO;
		port_slots_default[MODO_BIT_1_OFST] = APAGADO;
		
		port_slots_write[MODO_BIT_2_OFST] = APAGADO;
		port_slots_default[MODO_BIT_2_OFST] = APAGADO;
		
		port_slots_write[MODO_BIT_3_OFST] = APAGADO;
		port_slots_default[MODO_BIT_3_OFST] = APAGADO;
		
		port_slots_write[MODO_BIT_4_OFST] = APAGADO;
		port_slots_default[MODO_BIT_4_OFST] = APAGADO;
		
		//port_slots_write[PASO_MODO_A_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_A_NEW_OFST] = APAGADO;
		
		//port_slots_write[PASO_MODO_B_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_B_NEW_OFST] = APAGADO;
		
		port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;
		
		port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_S_OFST] = APAGADO;
		
		port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_I_OFST] = APAGADO;
		
		port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;
		
		port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		
		port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		
		port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;
		
		port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		
		port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		break;
		
		//MODO A BLOQUEADO, MODO B BLOQUEADO
		case 0x23:
		port_slots_write[PASO_MODO_A_ANT_OFST] = ENCENDIDO;
		port_slots_default[PASO_MODO_A_ANT_OFST] = ENCENDIDO;
		
		port_slots_write[PASO_MODO_B_ANT_OFST] = ENCENDIDO;
		port_slots_default[PASO_MODO_B_ANT_OFST] = ENCENDIDO;
		
		port_slots_write[PICTOGRAMA_1_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_1_OFST] = ENCENDIDO;
		
		port_slots_write[PICTOGRAMA_2_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_2_OFST] = APAGADO;
		
		port_slots_write[PICTOGRAMA_3_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_3_OFST] = ENCENDIDO;
		
		port_slots_write[PICTOGRAMA_4_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_4_OFST] = APAGADO;
		
		port_slots_write[MODO_BIT_1_OFST] = APAGADO;
		port_slots_default[MODO_BIT_1_OFST] = APAGADO;
		
		port_slots_write[MODO_BIT_2_OFST] = APAGADO;
		port_slots_default[MODO_BIT_2_OFST] = APAGADO;
		
		port_slots_write[MODO_BIT_3_OFST] = APAGADO;
		port_slots_default[MODO_BIT_3_OFST] = APAGADO;
		
		port_slots_write[MODO_BIT_4_OFST] = APAGADO;
		port_slots_default[MODO_BIT_4_OFST] = APAGADO;
		
		//port_slots_write[PASO_MODO_A_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_A_NEW_OFST] = APAGADO;
		
		//port_slots_write[PASO_MODO_B_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_B_NEW_OFST] = APAGADO;
		
		port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;
		
		port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_S_OFST] = APAGADO;
		
		port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_I_OFST] = APAGADO;
		
		port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;
		
		port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		
		port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		
		port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;
		
		port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		
		port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		break;
		
		
		//MODO A BLOQUEADO, MODO B LIBRE
		case 0x33:
		port_slots_write[PASO_MODO_A_ANT_OFST] = ENCENDIDO;
		port_slots_default[PASO_MODO_A_ANT_OFST] = ENCENDIDO;
		
		port_slots_write[PASO_MODO_B_ANT_OFST] = APAGADO;
		port_slots_default[PASO_MODO_B_ANT_OFST] = APAGADO;
		
		port_slots_write[PICTOGRAMA_1_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_1_OFST] = ENCENDIDO;
		
		port_slots_write[PICTOGRAMA_2_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_2_OFST] = APAGADO;
		
		port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;
		
		port_slots_write[PICTOGRAMA_4_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_4_OFST] = ENCENDIDO;
		
		port_slots_write[MODO_BIT_1_OFST] = APAGADO;
		port_slots_default[MODO_BIT_1_OFST] = APAGADO;
		
		port_slots_write[MODO_BIT_2_OFST] = APAGADO;
		port_slots_default[MODO_BIT_2_OFST] = APAGADO;
		
		port_slots_write[MODO_BIT_3_OFST] = APAGADO;
		port_slots_default[MODO_BIT_3_OFST] = APAGADO;
		
		port_slots_write[MODO_BIT_4_OFST] = APAGADO;
		port_slots_default[MODO_BIT_4_OFST] = APAGADO;
		
		//port_slots_write[PASO_MODO_A_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_A_NEW_OFST] = APAGADO;
		
		//port_slots_write[PASO_MODO_B_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_B_NEW_OFST] = APAGADO;
		
		port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;
		
		port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_S_OFST] = APAGADO;
		
		port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_I_OFST] = APAGADO;
		
		port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;
		
		port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		
		port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		
		port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;
		
		port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		
		port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		break;
		
		
		case 0x43:
		port_slots_write[PASO_MODO_A_ANT_OFST] = APAGADO;
		port_slots_default[PASO_MODO_A_ANT_OFST] = APAGADO;
		
		port_slots_write[PASO_MODO_B_ANT_OFST] = APAGADO;
		port_slots_default[PASO_MODO_B_ANT_OFST] = APAGADO;
		
		port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;
		
		port_slots_write[PICTOGRAMA_2_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_2_OFST] = ENCENDIDO;
		
		port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;
		
		port_slots_write[PICTOGRAMA_4_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_4_OFST] = ENCENDIDO;
		
		port_slots_write[MODO_BIT_1_OFST] = APAGADO;
		port_slots_default[MODO_BIT_1_OFST] = APAGADO;
		
		port_slots_write[MODO_BIT_2_OFST] = APAGADO;
		port_slots_default[MODO_BIT_2_OFST] = APAGADO;
		
		port_slots_write[MODO_BIT_3_OFST] = APAGADO;
		port_slots_default[MODO_BIT_3_OFST] = APAGADO;
		
		port_slots_write[MODO_BIT_4_OFST] = APAGADO;
		port_slots_default[MODO_BIT_4_OFST] = APAGADO;
		
		//port_slots_write[PASO_MODO_A_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_A_NEW_OFST] = APAGADO;
		
		//port_slots_write[PASO_MODO_B_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_B_NEW_OFST] = APAGADO;
		
		port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;
		
		port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_S_OFST] = APAGADO;
		
		port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_I_OFST] = APAGADO;
		
		port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;
		
		port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		
		port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		
		port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;
		
		port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		
		port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		
		break;
		
		
		case 0x04:
		/* Your code here */
		port_slots_write[PASO_MODO_A_ANT_OFST] = APAGADO;
		port_slots_default[PASO_MODO_A_ANT_OFST] = APAGADO;
		
		port_slots_write[PASO_MODO_B_ANT_OFST] = APAGADO;
		port_slots_default[PASO_MODO_B_ANT_OFST] = APAGADO;
		
		port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;
		
		port_slots_write[PICTOGRAMA_2_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_2_OFST] = APAGADO;
		
		port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;
		
		port_slots_write[PICTOGRAMA_4_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_4_OFST] = APAGADO;
		
		port_slots_write[MODO_BIT_1_OFST] = APAGADO;
		port_slots_default[MODO_BIT_1_OFST] = APAGADO;
		
		port_slots_write[MODO_BIT_2_OFST] = APAGADO;
		port_slots_default[MODO_BIT_2_OFST] = APAGADO;
		
		port_slots_write[MODO_BIT_3_OFST] = APAGADO;
		port_slots_default[MODO_BIT_3_OFST] = APAGADO;
		
		port_slots_write[MODO_BIT_4_OFST] = APAGADO;
		port_slots_default[MODO_BIT_4_OFST] = APAGADO;
		
		//port_slots_write[PASO_MODO_A_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_A_NEW_OFST] = APAGADO;
		
		//port_slots_write[PASO_MODO_B_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_B_NEW_OFST] = APAGADO;
		
		port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;
		
		port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_S_OFST] = APAGADO;
		
		port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_I_OFST] = APAGADO;
		
		port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;
		
		port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		
		port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		
		port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;
		
		port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		
		port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;

		break;
		
		
		case 0x05:
		/* Your code here */
			port_slots_write[PASO_MODO_A_ANT_OFST] = APAGADO;
			port_slots_default[PASO_MODO_A_ANT_OFST] = APAGADO;
		
			port_slots_write[PASO_MODO_B_ANT_OFST] = APAGADO;
			port_slots_default[PASO_MODO_B_ANT_OFST] = APAGADO;
		
			port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
			port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;
		
			port_slots_write[PICTOGRAMA_2_OFST] = APAGADO;
			port_slots_default[PICTOGRAMA_2_OFST] = APAGADO;
		
			port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
			port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;
		
			port_slots_write[PICTOGRAMA_4_OFST] = APAGADO;
			port_slots_default[PICTOGRAMA_4_OFST] = APAGADO;
		
			port_slots_write[MODO_BIT_1_OFST] = APAGADO;
			port_slots_default[MODO_BIT_1_OFST] = APAGADO;
		
			port_slots_write[MODO_BIT_2_OFST] = APAGADO;
			port_slots_default[MODO_BIT_2_OFST] = APAGADO;
		
			port_slots_write[MODO_BIT_3_OFST] = ENCENDIDO;
			port_slots_default[MODO_BIT_3_OFST] = ENCENDIDO;
		
			port_slots_write[MODO_BIT_4_OFST] = APAGADO;
			port_slots_default[MODO_BIT_4_OFST] = APAGADO;
		
			//port_slots_write[PASO_MODO_A_NEW_OFST] = APAGADO;
			//port_slots_default[PASO_MODO_A_NEW_OFST] = APAGADO;
		
			//port_slots_write[PASO_MODO_B_NEW_OFST] = APAGADO;
			//port_slots_default[PASO_MODO_B_NEW_OFST] = APAGADO;
		
			port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
			port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;
		
			port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
			port_slots_default[SOLENOIDE_S_OFST] = APAGADO;
		
			port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
			port_slots_default[SOLENOIDE_I_OFST] = APAGADO;
		
			port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
			port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;
		
			port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
			port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		
			port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
			port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		
			port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
			port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;
		
			port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
			port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		
			port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
			port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		break;
		
		// MODO A CONTROLADO, MODO B LIBRE
		case 0x15:
			port_slots_write[PASO_MODO_A_ANT_OFST] = APAGADO;
			port_slots_default[PASO_MODO_A_ANT_OFST] = APAGADO;
		
			port_slots_write[PASO_MODO_B_ANT_OFST] = APAGADO;
			port_slots_default[PASO_MODO_B_ANT_OFST] = APAGADO;
		
			port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
			port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;
		
			port_slots_write[PICTOGRAMA_2_OFST] = APAGADO;
			port_slots_default[PICTOGRAMA_2_OFST] = APAGADO;
		
			port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
			port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;
		
			port_slots_write[PICTOGRAMA_4_OFST] = APAGADO;
			port_slots_default[PICTOGRAMA_4_OFST] = APAGADO;
		
			port_slots_write[MODO_BIT_1_OFST] = APAGADO;
			port_slots_default[MODO_BIT_1_OFST] = APAGADO;
		
			port_slots_write[MODO_BIT_2_OFST] = APAGADO;
			port_slots_default[MODO_BIT_2_OFST] = APAGADO;
		
			port_slots_write[MODO_BIT_3_OFST] = APAGADO;
			port_slots_default[MODO_BIT_3_OFST] = APAGADO;
		
			port_slots_write[MODO_BIT_4_OFST] = ENCENDIDO;
			port_slots_default[MODO_BIT_4_OFST] = ENCENDIDO;
		
			//port_slots_write[PASO_MODO_A_NEW_OFST] = APAGADO;
			//port_slots_default[PASO_MODO_A_NEW_OFST] = APAGADO;
		
			//port_slots_write[PASO_MODO_B_NEW_OFST] = APAGADO;
			//port_slots_default[PASO_MODO_B_NEW_OFST] = APAGADO;
		
			port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
			port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;
		
			port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
			port_slots_default[SOLENOIDE_S_OFST] = APAGADO;
		
			port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
			port_slots_default[SOLENOIDE_I_OFST] = APAGADO;
		
			port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
			port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;
		
			port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
			port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		
			port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
			port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		
			port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
			port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;
		
			port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
			port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		
			port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
			port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		break;
		
		//MODO A BLOQUEADO, MODO B BLOQUEADO
		case 0x25:
		port_slots_write[PASO_MODO_A_ANT_OFST] = APAGADO;
		port_slots_default[PASO_MODO_A_ANT_OFST] = APAGADO;
		
		port_slots_write[PASO_MODO_B_ANT_OFST] = APAGADO;
		port_slots_default[PASO_MODO_B_ANT_OFST] = APAGADO;
		
		port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;
		
		port_slots_write[PICTOGRAMA_2_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_2_OFST] = APAGADO;
		
		port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;
		
		port_slots_write[PICTOGRAMA_4_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_4_OFST] = APAGADO;
		
		port_slots_write[MODO_BIT_1_OFST] = ENCENDIDO;
		port_slots_default[MODO_BIT_1_OFST] = ENCENDIDO;
		
		port_slots_write[MODO_BIT_2_OFST] = APAGADO;
		port_slots_default[MODO_BIT_2_OFST] = APAGADO;
		
		port_slots_write[MODO_BIT_3_OFST] = ENCENDIDO;
		port_slots_default[MODO_BIT_3_OFST] = ENCENDIDO;
		
		port_slots_write[MODO_BIT_4_OFST] = APAGADO;
		port_slots_default[MODO_BIT_4_OFST] = APAGADO;
		
		//port_slots_write[PASO_MODO_A_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_A_NEW_OFST] = APAGADO;
		
		//port_slots_write[PASO_MODO_B_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_B_NEW_OFST] = APAGADO;
		
		port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;
		
		port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_S_OFST] = APAGADO;
		
		port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_I_OFST] = APAGADO;
		
		port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;
		
		port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		
		port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		
		port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;
		
		port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		
		port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		break;
		
		
		//MODO A BLOQUEADO, MODO B LIBRE
		case 0x35:
		port_slots_write[PASO_MODO_A_ANT_OFST] = APAGADO;
		port_slots_default[PASO_MODO_A_ANT_OFST] = APAGADO;
		
		port_slots_write[PASO_MODO_B_ANT_OFST] = APAGADO;
		port_slots_default[PASO_MODO_B_ANT_OFST] = APAGADO;
		
		port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;
		
		port_slots_write[PICTOGRAMA_2_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_2_OFST] = APAGADO;
		
		port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;
		
		port_slots_write[PICTOGRAMA_4_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_4_OFST] = APAGADO;
		
		port_slots_write[MODO_BIT_1_OFST] = ENCENDIDO;
		port_slots_default[MODO_BIT_1_OFST] = ENCENDIDO;
		
		port_slots_write[MODO_BIT_2_OFST] = APAGADO;
		port_slots_default[MODO_BIT_2_OFST] = APAGADO;
		
		port_slots_write[MODO_BIT_3_OFST] = APAGADO;
		port_slots_default[MODO_BIT_3_OFST] = APAGADO;
		
		port_slots_write[MODO_BIT_4_OFST] = ENCENDIDO;
		port_slots_default[MODO_BIT_4_OFST] = ENCENDIDO;
		
		//port_slots_write[PASO_MODO_A_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_A_NEW_OFST] = APAGADO;
		
		//port_slots_write[PASO_MODO_B_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_B_NEW_OFST] = APAGADO;
		
		port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;
		
		port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_S_OFST] = APAGADO;
		
		port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_I_OFST] = APAGADO;
		
		port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;
		
		port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		
		port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		
		port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;
		
		port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		
		port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		break;
		
		
		case 0x45:
		port_slots_write[PASO_MODO_A_ANT_OFST] = APAGADO;
		port_slots_default[PASO_MODO_A_ANT_OFST] = APAGADO;
		
		port_slots_write[PASO_MODO_B_ANT_OFST] = APAGADO;
		port_slots_default[PASO_MODO_B_ANT_OFST] = APAGADO;
		
		port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;
		
		port_slots_write[PICTOGRAMA_2_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_2_OFST] = APAGADO;
		
		port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;
		
		port_slots_write[PICTOGRAMA_4_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_4_OFST] = APAGADO;
		
		port_slots_write[MODO_BIT_1_OFST] = APAGADO;
		port_slots_default[MODO_BIT_1_OFST] = APAGADO;
		
		port_slots_write[MODO_BIT_2_OFST] = ENCENDIDO;
		port_slots_default[MODO_BIT_2_OFST] = ENCENDIDO;
		
		port_slots_write[MODO_BIT_3_OFST] = APAGADO;
		port_slots_default[MODO_BIT_3_OFST] = APAGADO;
		
		port_slots_write[MODO_BIT_4_OFST] = ENCENDIDO;
		port_slots_default[MODO_BIT_4_OFST] = ENCENDIDO;
		
		//port_slots_write[PASO_MODO_A_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_A_NEW_OFST] = APAGADO;
		
		//port_slots_write[PASO_MODO_B_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_B_NEW_OFST] = APAGADO;
		
		port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;
		
		port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_S_OFST] = APAGADO;
		
		port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_I_OFST] = APAGADO;
		
		port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;
		
		port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		
		port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		
		port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;
		
		port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		
		port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		
		break;
		
		
		//MODO A CONTROLADO, MODO B BLOQUEADO
		case 0x06:
		/* Your code here */
		port_slots_write[PASO_MODO_A_ANT_OFST] = ENCENDIDO;
		port_slots_default[PASO_MODO_A_ANT_OFST] = ENCENDIDO;
		
		port_slots_write[PASO_MODO_B_ANT_OFST] = ENCENDIDO;
		port_slots_default[PASO_MODO_B_ANT_OFST] = ENCENDIDO;
		
		port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;
		
		port_slots_write[PICTOGRAMA_2_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_2_OFST] = ENCENDIDO;
		
		port_slots_write[PICTOGRAMA_3_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_3_OFST] = ENCENDIDO;
		
		port_slots_write[PICTOGRAMA_4_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_4_OFST] = APAGADO;
		
		port_slots_write[MODO_BIT_1_OFST] = APAGADO;
		port_slots_default[MODO_BIT_1_OFST] = APAGADO;
		
		port_slots_write[MODO_BIT_2_OFST] = APAGADO;
		port_slots_default[MODO_BIT_2_OFST] = APAGADO;
		
		port_slots_write[MODO_BIT_3_OFST] = APAGADO;
		port_slots_default[MODO_BIT_3_OFST] = APAGADO;
		
		port_slots_write[MODO_BIT_4_OFST] = APAGADO;
		port_slots_default[MODO_BIT_4_OFST] = APAGADO;
		
		//port_slots_write[PASO_MODO_A_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_A_NEW_OFST] = APAGADO;
		
		//port_slots_write[PASO_MODO_B_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_B_NEW_OFST] = APAGADO;
		
		port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;
		
		port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_S_OFST] = APAGADO;
		
		port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_I_OFST] = APAGADO;
		
		port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;
		
		port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		
		port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		
		port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;
		
		port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		
		port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;

		break;
		
		
		//MODO A CONTROLADO, MODO B LIBRE
		case 0x16:
		/* Your code here */
		port_slots_write[PASO_MODO_A_ANT_OFST] = ENCENDIDO;
		port_slots_default[PASO_MODO_A_ANT_OFST] = ENCENDIDO;
		
		port_slots_write[PASO_MODO_B_ANT_OFST] = APAGADO;
		port_slots_default[PASO_MODO_B_ANT_OFST] = APAGADO;
		
		port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;
		
		port_slots_write[PICTOGRAMA_2_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_2_OFST] = ENCENDIDO;
		
		port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;
		
		port_slots_write[PICTOGRAMA_4_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_4_OFST] = ENCENDIDO;
		
		port_slots_write[MODO_BIT_1_OFST] = APAGADO;
		port_slots_default[MODO_BIT_1_OFST] = APAGADO;
		
		port_slots_write[MODO_BIT_2_OFST] = APAGADO;
		port_slots_default[MODO_BIT_2_OFST] = APAGADO;
		
		port_slots_write[MODO_BIT_3_OFST] = APAGADO;
		port_slots_default[MODO_BIT_3_OFST] = APAGADO;
		
		port_slots_write[MODO_BIT_4_OFST] = APAGADO;
		port_slots_default[MODO_BIT_4_OFST] = APAGADO;
		
		//port_slots_write[PASO_MODO_A_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_A_NEW_OFST] = APAGADO;
		
		//port_slots_write[PASO_MODO_B_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_B_NEW_OFST] = APAGADO;
		
		port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;
		
		port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_S_OFST] = APAGADO;
		
		port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_I_OFST] = APAGADO;
		
		port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;
		
		port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		
		port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		
		port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;
		
		port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		
		port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;

		break;
		
		case 0x26:
		/* Your code here */
		port_slots_write[PASO_MODO_A_ANT_OFST] = ENCENDIDO;
		port_slots_default[PASO_MODO_A_ANT_OFST] = ENCENDIDO;
		
		port_slots_write[PASO_MODO_B_ANT_OFST] = ENCENDIDO;
		port_slots_default[PASO_MODO_B_ANT_OFST] = ENCENDIDO;
		
		port_slots_write[PICTOGRAMA_1_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_1_OFST] = ENCENDIDO;
		
		port_slots_write[PICTOGRAMA_2_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_2_OFST] = APAGADO;
		
		port_slots_write[PICTOGRAMA_3_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_3_OFST] = ENCENDIDO;
		
		port_slots_write[PICTOGRAMA_4_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_4_OFST] = APAGADO;
		
		port_slots_write[MODO_BIT_1_OFST] = APAGADO;
		port_slots_default[MODO_BIT_1_OFST] = APAGADO;
		
		port_slots_write[MODO_BIT_2_OFST] = APAGADO;
		port_slots_default[MODO_BIT_2_OFST] = APAGADO;
		
		port_slots_write[MODO_BIT_3_OFST] = APAGADO;
		port_slots_default[MODO_BIT_3_OFST] = APAGADO;
		
		port_slots_write[MODO_BIT_4_OFST] = APAGADO;
		port_slots_default[MODO_BIT_4_OFST] = APAGADO;
		
		//port_slots_write[PASO_MODO_A_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_A_NEW_OFST] = APAGADO;
		
		//port_slots_write[PASO_MODO_B_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_B_NEW_OFST] = APAGADO;
		
		port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;
		
		port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_S_OFST] = APAGADO;
		
		port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_I_OFST] = APAGADO;
		
		port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;
		
		port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		
		port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		
		port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;
		
		port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		
		port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;

		break;
		
		
		case 0x36:
		/* Your code here */
		port_slots_write[PASO_MODO_A_ANT_OFST] = ENCENDIDO;
		port_slots_default[PASO_MODO_A_ANT_OFST] = ENCENDIDO;
		
		port_slots_write[PASO_MODO_B_ANT_OFST] = APAGADO;
		port_slots_default[PASO_MODO_B_ANT_OFST] = APAGADO;
		
		port_slots_write[PICTOGRAMA_1_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_1_OFST] = ENCENDIDO;
		
		port_slots_write[PICTOGRAMA_2_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_2_OFST] = APAGADO;
		
		port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;
		
		port_slots_write[PICTOGRAMA_4_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_4_OFST] = ENCENDIDO;
		
		port_slots_write[MODO_BIT_1_OFST] = APAGADO;
		port_slots_default[MODO_BIT_1_OFST] = APAGADO;
		
		port_slots_write[MODO_BIT_2_OFST] = APAGADO;
		port_slots_default[MODO_BIT_2_OFST] = APAGADO;
		
		port_slots_write[MODO_BIT_3_OFST] = APAGADO;
		port_slots_default[MODO_BIT_3_OFST] = APAGADO;
		
		port_slots_write[MODO_BIT_4_OFST] = APAGADO;
		port_slots_default[MODO_BIT_4_OFST] = APAGADO;
		
		//port_slots_write[PASO_MODO_A_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_A_NEW_OFST] = APAGADO;
		
		//port_slots_write[PASO_MODO_B_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_B_NEW_OFST] = APAGADO;
		
		port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;
		
		port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_S_OFST] = APAGADO;
		
		port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_I_OFST] = APAGADO;
		
		port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;
		
		port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		
		port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		
		port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;
		
		port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		
		port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;

		break;
		
		case 0x46:
		/* Your code here */
		port_slots_write[PASO_MODO_A_ANT_OFST] = APAGADO;
		port_slots_default[PASO_MODO_A_ANT_OFST] = APAGADO;
		
		port_slots_write[PASO_MODO_B_ANT_OFST] = APAGADO;
		port_slots_default[PASO_MODO_B_ANT_OFST] = APAGADO;
		
		port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;
		
		port_slots_write[PICTOGRAMA_2_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_2_OFST] = ENCENDIDO;
		
		port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;
		
		port_slots_write[PICTOGRAMA_4_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_4_OFST] = ENCENDIDO;
		
		port_slots_write[MODO_BIT_1_OFST] = APAGADO;
		port_slots_default[MODO_BIT_1_OFST] = APAGADO;
		
		port_slots_write[MODO_BIT_2_OFST] = APAGADO;
		port_slots_default[MODO_BIT_2_OFST] = APAGADO;
		
		port_slots_write[MODO_BIT_3_OFST] = APAGADO;
		port_slots_default[MODO_BIT_3_OFST] = APAGADO;
		
		port_slots_write[MODO_BIT_4_OFST] = APAGADO;
		port_slots_default[MODO_BIT_4_OFST] = APAGADO;
		
		//port_slots_write[PASO_MODO_A_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_A_NEW_OFST] = APAGADO;
		
		//port_slots_write[PASO_MODO_B_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_B_NEW_OFST] = APAGADO;
		
		port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;
		
		port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_S_OFST] = APAGADO;
		
		port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_I_OFST] = APAGADO;
		
		port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;
		
		port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		
		port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		
		port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;
		
		port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		
		port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;

		break;
		
	case 0x07:
		/* Your code here */
		port_slots_write[PASO_MODO_A_ANT_OFST] = ENCENDIDO;
		port_slots_default[PASO_MODO_A_ANT_OFST] = ENCENDIDO;

		port_slots_write[PASO_MODO_B_ANT_OFST] = ENCENDIDO;
		port_slots_default[PASO_MODO_B_ANT_OFST] = ENCENDIDO;

		port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_2_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_2_OFST] = ENCENDIDO;

		port_slots_write[PICTOGRAMA_3_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_3_OFST] = ENCENDIDO;

		port_slots_write[PICTOGRAMA_4_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_4_OFST] = APAGADO;

		port_slots_write[MODO_BIT_1_OFST] = APAGADO;
		port_slots_default[MODO_BIT_1_OFST] = APAGADO;

		port_slots_write[MODO_BIT_2_OFST] = APAGADO;
		port_slots_default[MODO_BIT_2_OFST] = APAGADO;

		port_slots_write[MODO_BIT_3_OFST] = APAGADO;
		port_slots_default[MODO_BIT_3_OFST] = APAGADO;

		port_slots_write[MODO_BIT_4_OFST] = APAGADO;
		port_slots_default[MODO_BIT_4_OFST] = APAGADO;

		//port_slots_write[PASO_MODO_A_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_A_NEW_OFST] = APAGADO;

		//port_slots_write[PASO_MODO_B_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_B_NEW_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_S_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_I_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;

	break;

	case 0x17:
		/* Your code here */
		port_slots_write[PASO_MODO_A_ANT_OFST] = ENCENDIDO;
		port_slots_default[PASO_MODO_A_ANT_OFST] = ENCENDIDO;

		port_slots_write[PASO_MODO_B_ANT_OFST] = APAGADO;
		port_slots_default[PASO_MODO_B_ANT_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_2_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_2_OFST] = ENCENDIDO;

		port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_4_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_4_OFST] = ENCENDIDO;

		port_slots_write[MODO_BIT_1_OFST] = APAGADO;
		port_slots_default[MODO_BIT_1_OFST] = APAGADO;

		port_slots_write[MODO_BIT_2_OFST] = APAGADO;
		port_slots_default[MODO_BIT_2_OFST] = APAGADO;

		port_slots_write[MODO_BIT_3_OFST] = APAGADO;
		port_slots_default[MODO_BIT_3_OFST] = APAGADO;

		port_slots_write[MODO_BIT_4_OFST] = APAGADO;
		port_slots_default[MODO_BIT_4_OFST] = APAGADO;

		//port_slots_write[PASO_MODO_A_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_A_NEW_OFST] = APAGADO;

		//port_slots_write[PASO_MODO_B_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_B_NEW_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_S_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_I_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;

	break;
		
		
	case 0x27:
		/* Your code here */
		port_slots_write[PASO_MODO_A_ANT_OFST] = APAGADO;
		port_slots_default[PASO_MODO_A_ANT_OFST] = APAGADO;

		port_slots_write[PASO_MODO_B_ANT_OFST] = APAGADO;
		port_slots_default[PASO_MODO_B_ANT_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_1_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_1_OFST] = ENCENDIDO;

		port_slots_write[PICTOGRAMA_2_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_2_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_3_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_3_OFST] = ENCENDIDO;

		port_slots_write[PICTOGRAMA_4_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_4_OFST] = APAGADO;

		port_slots_write[MODO_BIT_1_OFST] = APAGADO;
		port_slots_default[MODO_BIT_1_OFST] = APAGADO;

		port_slots_write[MODO_BIT_2_OFST] = APAGADO;
		port_slots_default[MODO_BIT_2_OFST] = APAGADO;

		port_slots_write[MODO_BIT_3_OFST] = APAGADO;
		port_slots_default[MODO_BIT_3_OFST] = APAGADO;

		port_slots_write[MODO_BIT_4_OFST] = APAGADO;
		port_slots_default[MODO_BIT_4_OFST] = APAGADO;

		//port_slots_write[PASO_MODO_A_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_A_NEW_OFST] = APAGADO;

		//port_slots_write[PASO_MODO_B_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_B_NEW_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_S_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_I_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;

	break;
		
		
	case 0x37:
		/* Your code here */
		port_slots_write[PASO_MODO_A_ANT_OFST] = ENCENDIDO;
		port_slots_default[PASO_MODO_A_ANT_OFST] = ENCENDIDO;

		port_slots_write[PASO_MODO_B_ANT_OFST] = ENCENDIDO;
		port_slots_default[PASO_MODO_B_ANT_OFST] = ENCENDIDO;

		port_slots_write[PICTOGRAMA_1_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_1_OFST] = ENCENDIDO;

		port_slots_write[PICTOGRAMA_2_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_2_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_4_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_4_OFST] = ENCENDIDO;

		port_slots_write[MODO_BIT_1_OFST] = APAGADO;
		port_slots_default[MODO_BIT_1_OFST] = APAGADO;

		port_slots_write[MODO_BIT_2_OFST] = APAGADO;
		port_slots_default[MODO_BIT_2_OFST] = APAGADO;

		port_slots_write[MODO_BIT_3_OFST] = APAGADO;
		port_slots_default[MODO_BIT_3_OFST] = APAGADO;

		port_slots_write[MODO_BIT_4_OFST] = APAGADO;
		port_slots_default[MODO_BIT_4_OFST] = APAGADO;

		//port_slots_write[PASO_MODO_A_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_A_NEW_OFST] = APAGADO;

		//port_slots_write[PASO_MODO_B_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_B_NEW_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_S_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_I_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;

	break;
	
	
	case 0x47:
		/* Your code here */
		port_slots_write[PASO_MODO_A_ANT_OFST] = APAGADO;
		port_slots_default[PASO_MODO_A_ANT_OFST] = APAGADO;

		port_slots_write[PASO_MODO_B_ANT_OFST] = APAGADO;
		port_slots_default[PASO_MODO_B_ANT_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_2_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_2_OFST] = ENCENDIDO;

		port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_4_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_4_OFST] = ENCENDIDO;

		port_slots_write[MODO_BIT_1_OFST] = APAGADO;
		port_slots_default[MODO_BIT_1_OFST] = APAGADO;

		port_slots_write[MODO_BIT_2_OFST] = APAGADO;
		port_slots_default[MODO_BIT_2_OFST] = APAGADO;

		port_slots_write[MODO_BIT_3_OFST] = APAGADO;
		port_slots_default[MODO_BIT_3_OFST] = APAGADO;

		port_slots_write[MODO_BIT_4_OFST] = APAGADO;
		port_slots_default[MODO_BIT_4_OFST] = APAGADO;

		//port_slots_write[PASO_MODO_A_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_A_NEW_OFST] = APAGADO;

		//port_slots_write[PASO_MODO_B_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_B_NEW_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_S_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_I_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;

	break;
	
		
		
	case 0x08:
		/* Your code here */
		port_slots_write[PASO_MODO_A_ANT_OFST] = ENCENDIDO;
		port_slots_default[PASO_MODO_A_ANT_OFST] = ENCENDIDO;

		port_slots_write[PASO_MODO_B_ANT_OFST] = ENCENDIDO;
		port_slots_default[PASO_MODO_B_ANT_OFST] = ENCENDIDO;

		port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_2_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_2_OFST] = ENCENDIDO;

		port_slots_write[PICTOGRAMA_3_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_3_OFST] = ENCENDIDO;

		port_slots_write[PICTOGRAMA_4_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_4_OFST] = APAGADO;

		port_slots_write[MODO_BIT_1_OFST] = APAGADO;
		port_slots_default[MODO_BIT_1_OFST] = APAGADO;

		port_slots_write[MODO_BIT_2_OFST] = APAGADO;
		port_slots_default[MODO_BIT_2_OFST] = APAGADO;

		port_slots_write[MODO_BIT_3_OFST] = APAGADO;
		port_slots_default[MODO_BIT_3_OFST] = APAGADO;

		port_slots_write[MODO_BIT_4_OFST] = APAGADO;
		port_slots_default[MODO_BIT_4_OFST] = APAGADO;

		//port_slots_write[PASO_MODO_A_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_A_NEW_OFST] = APAGADO;

		//port_slots_write[PASO_MODO_B_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_B_NEW_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_S_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_I_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;

	break;
		
	
	
	case 0x18:
		/* Your code here */
		port_slots_write[PASO_MODO_A_ANT_OFST] = ENCENDIDO;
		port_slots_default[PASO_MODO_A_ANT_OFST] = ENCENDIDO;

		port_slots_write[PASO_MODO_B_ANT_OFST] = APAGADO;
		port_slots_default[PASO_MODO_B_ANT_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_2_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_2_OFST] = ENCENDIDO;

		port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_4_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_4_OFST] = ENCENDIDO;

		port_slots_write[MODO_BIT_1_OFST] = APAGADO;
		port_slots_default[MODO_BIT_1_OFST] = APAGADO;

		port_slots_write[MODO_BIT_2_OFST] = APAGADO;
		port_slots_default[MODO_BIT_2_OFST] = APAGADO;

		port_slots_write[MODO_BIT_3_OFST] = APAGADO;
		port_slots_default[MODO_BIT_3_OFST] = APAGADO;

		port_slots_write[MODO_BIT_4_OFST] = APAGADO;
		port_slots_default[MODO_BIT_4_OFST] = APAGADO;

		//port_slots_write[PASO_MODO_A_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_A_NEW_OFST] = APAGADO;

		//port_slots_write[PASO_MODO_B_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_B_NEW_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_S_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_I_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;

	break;	
		
		
	case 0x28:
		/* Your code here */
		port_slots_write[PASO_MODO_A_ANT_OFST] = APAGADO;
		port_slots_default[PASO_MODO_A_ANT_OFST] = APAGADO;

		port_slots_write[PASO_MODO_B_ANT_OFST] = APAGADO;
		port_slots_default[PASO_MODO_B_ANT_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_1_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_1_OFST] = ENCENDIDO;

		port_slots_write[PICTOGRAMA_2_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_2_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_3_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_3_OFST] = ENCENDIDO;

		port_slots_write[PICTOGRAMA_4_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_4_OFST] = APAGADO;

		port_slots_write[MODO_BIT_1_OFST] = APAGADO;
		port_slots_default[MODO_BIT_1_OFST] = APAGADO;

		port_slots_write[MODO_BIT_2_OFST] = APAGADO;
		port_slots_default[MODO_BIT_2_OFST] = APAGADO;

		port_slots_write[MODO_BIT_3_OFST] = APAGADO;
		port_slots_default[MODO_BIT_3_OFST] = APAGADO;

		port_slots_write[MODO_BIT_4_OFST] = APAGADO;
		port_slots_default[MODO_BIT_4_OFST] = APAGADO;

		//port_slots_write[PASO_MODO_A_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_A_NEW_OFST] = APAGADO;

		//port_slots_write[PASO_MODO_B_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_B_NEW_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_S_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_I_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;

	break;
		
		
		
	case 0x38:
		/* Your code here */
		port_slots_write[PASO_MODO_A_ANT_OFST] = ENCENDIDO;
		port_slots_default[PASO_MODO_A_ANT_OFST] = ENCENDIDO;

		port_slots_write[PASO_MODO_B_ANT_OFST] = APAGADO;
		port_slots_default[PASO_MODO_B_ANT_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_1_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_1_OFST] = ENCENDIDO;

		port_slots_write[PICTOGRAMA_2_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_2_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_4_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_4_OFST] = ENCENDIDO;

		port_slots_write[MODO_BIT_1_OFST] = APAGADO;
		port_slots_default[MODO_BIT_1_OFST] = APAGADO;

		port_slots_write[MODO_BIT_2_OFST] = APAGADO;
		port_slots_default[MODO_BIT_2_OFST] = APAGADO;

		port_slots_write[MODO_BIT_3_OFST] = APAGADO;
		port_slots_default[MODO_BIT_3_OFST] = APAGADO;

		port_slots_write[MODO_BIT_4_OFST] = APAGADO;
		port_slots_default[MODO_BIT_4_OFST] = APAGADO;

		//port_slots_write[PASO_MODO_A_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_A_NEW_OFST] = APAGADO;

		//port_slots_write[PASO_MODO_B_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_B_NEW_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_S_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_I_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;

	break;
		
		
		
	case 0x48:
		/* Your code here */
		port_slots_write[PASO_MODO_A_ANT_OFST] = APAGADO;
		port_slots_default[PASO_MODO_A_ANT_OFST] = APAGADO;

		port_slots_write[PASO_MODO_B_ANT_OFST] = APAGADO;
		port_slots_default[PASO_MODO_B_ANT_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_2_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_2_OFST] = ENCENDIDO;

		port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_4_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_4_OFST] = ENCENDIDO;

		port_slots_write[MODO_BIT_1_OFST] = APAGADO;
		port_slots_default[MODO_BIT_1_OFST] = APAGADO;

		port_slots_write[MODO_BIT_2_OFST] = APAGADO;
		port_slots_default[MODO_BIT_2_OFST] = APAGADO;

		port_slots_write[MODO_BIT_3_OFST] = APAGADO;
		port_slots_default[MODO_BIT_3_OFST] = APAGADO;

		port_slots_write[MODO_BIT_4_OFST] = APAGADO;
		port_slots_default[MODO_BIT_4_OFST] = APAGADO;

		//port_slots_write[PASO_MODO_A_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_A_NEW_OFST] = APAGADO;

		//port_slots_write[PASO_MODO_B_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_B_NEW_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_S_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_I_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;

	break;
		


	case 0x09:
		/* Your code here */
		port_slots_write[PASO_MODO_A_ANT_OFST] = ENCENDIDO;
		port_slots_default[PASO_MODO_A_ANT_OFST] = ENCENDIDO;

		port_slots_write[PASO_MODO_B_ANT_OFST] = ENCENDIDO;
		port_slots_default[PASO_MODO_B_ANT_OFST] = ENCENDIDO;

		port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_2_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_2_OFST] = ENCENDIDO;

		port_slots_write[PICTOGRAMA_3_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_3_OFST] = ENCENDIDO;

		port_slots_write[PICTOGRAMA_4_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_4_OFST] = APAGADO;

		port_slots_write[MODO_BIT_1_OFST] = APAGADO;
		port_slots_default[MODO_BIT_1_OFST] = APAGADO;

		port_slots_write[MODO_BIT_2_OFST] = APAGADO;
		port_slots_default[MODO_BIT_2_OFST] = APAGADO;

		port_slots_write[MODO_BIT_3_OFST] = APAGADO;
		port_slots_default[MODO_BIT_3_OFST] = APAGADO;

		port_slots_write[MODO_BIT_4_OFST] = APAGADO;
		port_slots_default[MODO_BIT_4_OFST] = APAGADO;

		//port_slots_write[PASO_MODO_A_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_A_NEW_OFST] = APAGADO;

		//port_slots_write[PASO_MODO_B_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_B_NEW_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_S_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_I_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;

	break;



	case 0x19:
		/* Your code here */
		port_slots_write[PASO_MODO_A_ANT_OFST] = ENCENDIDO;
		port_slots_default[PASO_MODO_A_ANT_OFST] = ENCENDIDO;

		port_slots_write[PASO_MODO_B_ANT_OFST] = APAGADO;
		port_slots_default[PASO_MODO_B_ANT_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_2_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_2_OFST] = ENCENDIDO;

		port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_4_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_4_OFST] = ENCENDIDO;

		port_slots_write[MODO_BIT_1_OFST] = APAGADO;
		port_slots_default[MODO_BIT_1_OFST] = APAGADO;

		port_slots_write[MODO_BIT_2_OFST] = APAGADO;
		port_slots_default[MODO_BIT_2_OFST] = APAGADO;

		port_slots_write[MODO_BIT_3_OFST] = APAGADO;
		port_slots_default[MODO_BIT_3_OFST] = APAGADO;

		port_slots_write[MODO_BIT_4_OFST] = APAGADO;
		port_slots_default[MODO_BIT_4_OFST] = APAGADO;

		//port_slots_write[PASO_MODO_A_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_A_NEW_OFST] = APAGADO;

		//port_slots_write[PASO_MODO_B_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_B_NEW_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_S_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_I_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;

	break;


	case 0x29:
		/* Your code here */
		port_slots_write[PASO_MODO_A_ANT_OFST] = APAGADO;
		port_slots_default[PASO_MODO_A_ANT_OFST] = APAGADO;

		port_slots_write[PASO_MODO_B_ANT_OFST] = APAGADO;
		port_slots_default[PASO_MODO_B_ANT_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_1_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_1_OFST] = ENCENDIDO;

		port_slots_write[PICTOGRAMA_2_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_2_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_3_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_3_OFST] = ENCENDIDO;

		port_slots_write[PICTOGRAMA_4_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_4_OFST] = APAGADO;

		port_slots_write[MODO_BIT_1_OFST] = APAGADO;
		port_slots_default[MODO_BIT_1_OFST] = APAGADO;

		port_slots_write[MODO_BIT_2_OFST] = APAGADO;
		port_slots_default[MODO_BIT_2_OFST] = APAGADO;

		port_slots_write[MODO_BIT_3_OFST] = APAGADO;
		port_slots_default[MODO_BIT_3_OFST] = APAGADO;

		port_slots_write[MODO_BIT_4_OFST] = APAGADO;
		port_slots_default[MODO_BIT_4_OFST] = APAGADO;

		//port_slots_write[PASO_MODO_A_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_A_NEW_OFST] = APAGADO;

		//port_slots_write[PASO_MODO_B_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_B_NEW_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_S_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_I_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;

	break;



	case 0x39:
		/* Your code here */
		port_slots_write[PASO_MODO_A_ANT_OFST] = ENCENDIDO;
		port_slots_default[PASO_MODO_A_ANT_OFST] = ENCENDIDO;

		port_slots_write[PASO_MODO_B_ANT_OFST] = APAGADO;
		port_slots_default[PASO_MODO_B_ANT_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_1_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_1_OFST] = ENCENDIDO;

		port_slots_write[PICTOGRAMA_2_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_2_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_4_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_4_OFST] = ENCENDIDO;

		port_slots_write[MODO_BIT_1_OFST] = APAGADO;
		port_slots_default[MODO_BIT_1_OFST] = APAGADO;

		port_slots_write[MODO_BIT_2_OFST] = APAGADO;
		port_slots_default[MODO_BIT_2_OFST] = APAGADO;

		port_slots_write[MODO_BIT_3_OFST] = APAGADO;
		port_slots_default[MODO_BIT_3_OFST] = APAGADO;

		port_slots_write[MODO_BIT_4_OFST] = APAGADO;
		port_slots_default[MODO_BIT_4_OFST] = APAGADO;

		//port_slots_write[PASO_MODO_A_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_A_NEW_OFST] = APAGADO;

		//port_slots_write[PASO_MODO_B_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_B_NEW_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_S_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_I_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;

	break;



	case 0x49:
		/* Your code here */
		port_slots_write[PASO_MODO_A_ANT_OFST] = APAGADO;
		port_slots_default[PASO_MODO_A_ANT_OFST] = APAGADO;

		port_slots_write[PASO_MODO_B_ANT_OFST] = APAGADO;
		port_slots_default[PASO_MODO_B_ANT_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_2_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_2_OFST] = ENCENDIDO;

		port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_4_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_4_OFST] = ENCENDIDO;

		port_slots_write[MODO_BIT_1_OFST] = APAGADO;
		port_slots_default[MODO_BIT_1_OFST] = APAGADO;

		port_slots_write[MODO_BIT_2_OFST] = APAGADO;
		port_slots_default[MODO_BIT_2_OFST] = APAGADO;

		port_slots_write[MODO_BIT_3_OFST] = APAGADO;
		port_slots_default[MODO_BIT_3_OFST] = APAGADO;

		port_slots_write[MODO_BIT_4_OFST] = APAGADO;
		port_slots_default[MODO_BIT_4_OFST] = APAGADO;

		//port_slots_write[PASO_MODO_A_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_A_NEW_OFST] = APAGADO;

		//port_slots_write[PASO_MODO_B_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_B_NEW_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_S_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_I_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;

	break;





	case 0x0A:
		/* Your code here */
		port_slots_write[PASO_MODO_A_ANT_OFST] = ENCENDIDO;
		port_slots_default[PASO_MODO_A_ANT_OFST] = ENCENDIDO;

		port_slots_write[PASO_MODO_B_ANT_OFST] = ENCENDIDO;
		port_slots_default[PASO_MODO_B_ANT_OFST] = ENCENDIDO;

		port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_2_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_2_OFST] = ENCENDIDO;

		port_slots_write[PICTOGRAMA_3_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_3_OFST] = ENCENDIDO;

		port_slots_write[PICTOGRAMA_4_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_4_OFST] = APAGADO;

		port_slots_write[MODO_BIT_1_OFST] = APAGADO;
		port_slots_default[MODO_BIT_1_OFST] = APAGADO;

		port_slots_write[MODO_BIT_2_OFST] = APAGADO;
		port_slots_default[MODO_BIT_2_OFST] = APAGADO;

		port_slots_write[MODO_BIT_3_OFST] = APAGADO;
		port_slots_default[MODO_BIT_3_OFST] = APAGADO;

		port_slots_write[MODO_BIT_4_OFST] = APAGADO;
		port_slots_default[MODO_BIT_4_OFST] = APAGADO;

		//port_slots_write[PASO_MODO_A_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_A_NEW_OFST] = APAGADO;

		//port_slots_write[PASO_MODO_B_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_B_NEW_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_S_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_I_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;

	break;



	case 0x1A:
		/* Your code here */
		port_slots_write[PASO_MODO_A_ANT_OFST] = ENCENDIDO;
		port_slots_default[PASO_MODO_A_ANT_OFST] = ENCENDIDO;

		port_slots_write[PASO_MODO_B_ANT_OFST] = APAGADO;
		port_slots_default[PASO_MODO_B_ANT_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_2_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_2_OFST] = ENCENDIDO;

		port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_4_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_4_OFST] = ENCENDIDO;

		port_slots_write[MODO_BIT_1_OFST] = APAGADO;
		port_slots_default[MODO_BIT_1_OFST] = APAGADO;

		port_slots_write[MODO_BIT_2_OFST] = APAGADO;
		port_slots_default[MODO_BIT_2_OFST] = APAGADO;

		port_slots_write[MODO_BIT_3_OFST] = APAGADO;
		port_slots_default[MODO_BIT_3_OFST] = APAGADO;

		port_slots_write[MODO_BIT_4_OFST] = APAGADO;
		port_slots_default[MODO_BIT_4_OFST] = APAGADO;

		//port_slots_write[PASO_MODO_A_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_A_NEW_OFST] = APAGADO;

		//port_slots_write[PASO_MODO_B_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_B_NEW_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_S_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_I_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;

	break;


	case 0x2A:
		/* Your code here */
		port_slots_write[PASO_MODO_A_ANT_OFST] = APAGADO;
		port_slots_default[PASO_MODO_A_ANT_OFST] = APAGADO;

		port_slots_write[PASO_MODO_B_ANT_OFST] = APAGADO;
		port_slots_default[PASO_MODO_B_ANT_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_1_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_1_OFST] = ENCENDIDO;

		port_slots_write[PICTOGRAMA_2_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_2_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_3_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_3_OFST] = ENCENDIDO;

		port_slots_write[PICTOGRAMA_4_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_4_OFST] = APAGADO;

		port_slots_write[MODO_BIT_1_OFST] = APAGADO;
		port_slots_default[MODO_BIT_1_OFST] = APAGADO;

		port_slots_write[MODO_BIT_2_OFST] = APAGADO;
		port_slots_default[MODO_BIT_2_OFST] = APAGADO;

		port_slots_write[MODO_BIT_3_OFST] = APAGADO;
		port_slots_default[MODO_BIT_3_OFST] = APAGADO;

		port_slots_write[MODO_BIT_4_OFST] = APAGADO;
		port_slots_default[MODO_BIT_4_OFST] = APAGADO;

		//port_slots_write[PASO_MODO_A_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_A_NEW_OFST] = APAGADO;

		//port_slots_write[PASO_MODO_B_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_B_NEW_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_S_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_I_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;

	break;



	case 0x3A:
		/* Your code here */
		port_slots_write[PASO_MODO_A_ANT_OFST] = ENCENDIDO;
		port_slots_default[PASO_MODO_A_ANT_OFST] = ENCENDIDO;

		port_slots_write[PASO_MODO_B_ANT_OFST] = APAGADO;
		port_slots_default[PASO_MODO_B_ANT_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_1_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_1_OFST] = ENCENDIDO;

		port_slots_write[PICTOGRAMA_2_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_2_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_4_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_4_OFST] = ENCENDIDO;

		port_slots_write[MODO_BIT_1_OFST] = APAGADO;
		port_slots_default[MODO_BIT_1_OFST] = APAGADO;

		port_slots_write[MODO_BIT_2_OFST] = APAGADO;
		port_slots_default[MODO_BIT_2_OFST] = APAGADO;

		port_slots_write[MODO_BIT_3_OFST] = APAGADO;
		port_slots_default[MODO_BIT_3_OFST] = APAGADO;

		port_slots_write[MODO_BIT_4_OFST] = APAGADO;
		port_slots_default[MODO_BIT_4_OFST] = APAGADO;

		//port_slots_write[PASO_MODO_A_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_A_NEW_OFST] = APAGADO;

		//port_slots_write[PASO_MODO_B_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_B_NEW_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_S_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_I_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;

	break;



	case 0x4A:
		/* Your code here */
		port_slots_write[PASO_MODO_A_ANT_OFST] = APAGADO;
		port_slots_default[PASO_MODO_A_ANT_OFST] = APAGADO;

		port_slots_write[PASO_MODO_B_ANT_OFST] = APAGADO;
		port_slots_default[PASO_MODO_B_ANT_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_2_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_2_OFST] = ENCENDIDO;

		port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
		port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;

		port_slots_write[PICTOGRAMA_4_OFST] = ENCENDIDO;
		port_slots_default[PICTOGRAMA_4_OFST] = ENCENDIDO;

		port_slots_write[MODO_BIT_1_OFST] = APAGADO;
		port_slots_default[MODO_BIT_1_OFST] = APAGADO;

		port_slots_write[MODO_BIT_2_OFST] = APAGADO;
		port_slots_default[MODO_BIT_2_OFST] = APAGADO;

		port_slots_write[MODO_BIT_3_OFST] = APAGADO;
		port_slots_default[MODO_BIT_3_OFST] = APAGADO;

		port_slots_write[MODO_BIT_4_OFST] = APAGADO;
		port_slots_default[MODO_BIT_4_OFST] = APAGADO;

		//port_slots_write[PASO_MODO_A_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_A_NEW_OFST] = APAGADO;

		//port_slots_write[PASO_MODO_B_NEW_OFST] = APAGADO;
		//port_slots_default[PASO_MODO_B_NEW_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_S_OFST] = APAGADO;

		port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
		port_slots_default[SOLENOIDE_I_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;

		port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;

		port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
		port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;

		port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
		port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;

	break;

	case 0x0B:
	/* Your code here */
	port_slots_write[PASO_MODO_A_NEW_OFST] = ENCENDIDO;
	port_slots_default[PASO_MODO_A_NEW_OFST] = ENCENDIDO;

	port_slots_write[PASO_MODO_B_NEW_OFST] = ENCENDIDO;
	port_slots_default[PASO_MODO_B_NEW_OFST] = ENCENDIDO;

	port_slots_write[MODO_BIT_1_OFST] = ENCENDIDO;
	port_slots_default[MODO_BIT_1_OFST] = ENCENDIDO;

	port_slots_write[MODO_BIT_2_OFST] = ENCENDIDO;
	port_slots_default[MODO_BIT_2_OFST] = ENCENDIDO;

	port_slots_write[MODO_BIT_3_OFST] = ENCENDIDO;
	port_slots_default[MODO_BIT_3_OFST] = ENCENDIDO;

	port_slots_write[MODO_BIT_4_OFST] = APAGADO;
	port_slots_default[MODO_BIT_4_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_2_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_2_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_4_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_4_OFST] = APAGADO;
		
	port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;
	
	port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
	port_slots_default[SOLENOIDE_S_OFST] = APAGADO;
	
	port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
	port_slots_default[SOLENOIDE_I_OFST] = APAGADO;
	
	port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
	port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;
	
	port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
	port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;
	
	port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
	port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;
	
	port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
	port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;
	
	
	break;

	case 0x1B:
	/* Your code here */
	port_slots_write[PASO_MODO_A_NEW_OFST] = ENCENDIDO;
	port_slots_default[PASO_MODO_A_NEW_OFST] = ENCENDIDO;

	port_slots_write[PASO_MODO_B_NEW_OFST] = ENCENDIDO;
	port_slots_default[PASO_MODO_B_NEW_OFST] = ENCENDIDO;

	port_slots_write[MODO_BIT_1_OFST] = ENCENDIDO;
	port_slots_default[MODO_BIT_1_OFST] = ENCENDIDO;

	port_slots_write[MODO_BIT_2_OFST] = ENCENDIDO;
	port_slots_default[MODO_BIT_2_OFST] = ENCENDIDO;

	port_slots_write[MODO_BIT_3_OFST] = APAGADO;
	port_slots_default[MODO_BIT_3_OFST] = APAGADO;

	port_slots_write[MODO_BIT_4_OFST] = ENCENDIDO;
	port_slots_default[MODO_BIT_4_OFST] = ENCENDIDO;
	
	port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_2_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_2_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_4_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_4_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;
	
	port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
	port_slots_default[SOLENOIDE_S_OFST] = APAGADO;
	
	port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
	port_slots_default[SOLENOIDE_I_OFST] = APAGADO;
	
	port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
	port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;
	
	port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
	port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;
	
	port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
	port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;
	
	port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
	port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;
	
	break;


	case 0x2B:
	/* Your code here */
	port_slots_write[PASO_MODO_A_NEW_OFST] = ENCENDIDO;
	port_slots_default[PASO_MODO_A_NEW_OFST] = ENCENDIDO;

	port_slots_write[PASO_MODO_B_NEW_OFST] = ENCENDIDO;
	port_slots_default[PASO_MODO_B_NEW_OFST] = ENCENDIDO;

	port_slots_write[MODO_BIT_1_OFST] = ENCENDIDO;
	port_slots_default[MODO_BIT_1_OFST] = ENCENDIDO;

	port_slots_write[MODO_BIT_2_OFST] = APAGADO;
	port_slots_default[MODO_BIT_2_OFST] = APAGADO;

	port_slots_write[MODO_BIT_3_OFST] = ENCENDIDO;
	port_slots_default[MODO_BIT_3_OFST] = ENCENDIDO;

	port_slots_write[MODO_BIT_4_OFST] = APAGADO;
	port_slots_default[MODO_BIT_4_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_2_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_2_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_4_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_4_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;
	
	port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
	port_slots_default[SOLENOIDE_S_OFST] = APAGADO;
	
	port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
	port_slots_default[SOLENOIDE_I_OFST] = APAGADO;
	
	port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
	port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;
	
	port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
	port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;
	
	port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
	port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;
	
	port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
	port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;
	
	break;


	case 0x3B:
	/* Your code here */
	port_slots_write[PASO_MODO_A_NEW_OFST] = ENCENDIDO;
	port_slots_default[PASO_MODO_A_NEW_OFST] = ENCENDIDO;

	port_slots_write[PASO_MODO_B_NEW_OFST] = ENCENDIDO;
	port_slots_default[PASO_MODO_B_NEW_OFST] = ENCENDIDO;

	port_slots_write[MODO_BIT_1_OFST] = ENCENDIDO;
	port_slots_default[MODO_BIT_1_OFST] = ENCENDIDO;

	port_slots_write[MODO_BIT_2_OFST] = APAGADO;
	port_slots_default[MODO_BIT_2_OFST] = APAGADO;

	port_slots_write[MODO_BIT_3_OFST] = APAGADO;
	port_slots_default[MODO_BIT_3_OFST] = APAGADO;

	port_slots_write[MODO_BIT_4_OFST] = ENCENDIDO;
	port_slots_default[MODO_BIT_4_OFST] = ENCENDIDO;
	
	port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_2_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_2_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_4_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_4_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;
	
	port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
	port_slots_default[SOLENOIDE_S_OFST] = APAGADO;
	
	port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
	port_slots_default[SOLENOIDE_I_OFST] = APAGADO;
	
	port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
	port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;
	
	port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
	port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;
	
	port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
	port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;
	
	port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
	port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;
	
	break;



	case 0x4B:
	/* Your code here */
	port_slots_write[PASO_MODO_A_NEW_OFST] = ENCENDIDO;
	port_slots_default[PASO_MODO_A_NEW_OFST] = ENCENDIDO;

	port_slots_write[PASO_MODO_B_NEW_OFST] = ENCENDIDO;
	port_slots_default[PASO_MODO_B_NEW_OFST] = ENCENDIDO;

	port_slots_write[MODO_BIT_1_OFST] = APAGADO;
	port_slots_default[MODO_BIT_1_OFST] = APAGADO;

	port_slots_write[MODO_BIT_2_OFST] = ENCENDIDO;
	port_slots_default[MODO_BIT_2_OFST] = ENCENDIDO;

	port_slots_write[MODO_BIT_3_OFST] = APAGADO;
	port_slots_default[MODO_BIT_3_OFST] = APAGADO;

	port_slots_write[MODO_BIT_4_OFST] = ENCENDIDO;
	port_slots_default[MODO_BIT_4_OFST] = ENCENDIDO;
	
	port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_2_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_2_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_4_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_4_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;
	
	port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
	port_slots_default[SOLENOIDE_S_OFST] = APAGADO;
	
	port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
	port_slots_default[SOLENOIDE_I_OFST] = APAGADO;
	
	port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
	port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;
	
	port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
	port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;
	
	port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
	port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;
	
	port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
	port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;
	
	break;

	case 0x0C:
	/* Your code here */
	port_slots_write[PASO_MODO_A_NEW_OFST] = ENCENDIDO;
	port_slots_default[PASO_MODO_A_NEW_OFST] = ENCENDIDO;

	port_slots_write[PASO_MODO_B_NEW_OFST] = ENCENDIDO;
	port_slots_default[PASO_MODO_B_NEW_OFST] = ENCENDIDO;

	port_slots_write[MODO_BIT_1_OFST] = ENCENDIDO;
	port_slots_default[MODO_BIT_1_OFST] = ENCENDIDO;

	port_slots_write[MODO_BIT_2_OFST] = ENCENDIDO;
	port_slots_default[MODO_BIT_2_OFST] = ENCENDIDO;

	port_slots_write[MODO_BIT_3_OFST] = ENCENDIDO;
	port_slots_default[MODO_BIT_3_OFST] = ENCENDIDO;

	port_slots_write[MODO_BIT_4_OFST] = APAGADO;
	port_slots_default[MODO_BIT_4_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_2_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_2_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_4_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_4_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;
	
	port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
	port_slots_default[SOLENOIDE_S_OFST] = APAGADO;
	
	port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
	port_slots_default[SOLENOIDE_I_OFST] = APAGADO;
	
	port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
	port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;
	
	port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
	port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;
	
	port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
	port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;
	
	port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
	port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;
	
	
	break;

	case 0x1C:
	/* Your code here */
	port_slots_write[PASO_MODO_A_NEW_OFST] = ENCENDIDO;
	port_slots_default[PASO_MODO_A_NEW_OFST] = ENCENDIDO;

	port_slots_write[PASO_MODO_B_NEW_OFST] = ENCENDIDO;
	port_slots_default[PASO_MODO_B_NEW_OFST] = ENCENDIDO;

	port_slots_write[MODO_BIT_1_OFST] = ENCENDIDO;
	port_slots_default[MODO_BIT_1_OFST] = ENCENDIDO;

	port_slots_write[MODO_BIT_2_OFST] = ENCENDIDO;
	port_slots_default[MODO_BIT_2_OFST] = ENCENDIDO;

	port_slots_write[MODO_BIT_3_OFST] = APAGADO;
	port_slots_default[MODO_BIT_3_OFST] = APAGADO;

	port_slots_write[MODO_BIT_4_OFST] = ENCENDIDO;
	port_slots_default[MODO_BIT_4_OFST] = ENCENDIDO;
	
	port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_2_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_2_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_4_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_4_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;
	
	port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
	port_slots_default[SOLENOIDE_S_OFST] = APAGADO;
	
	port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
	port_slots_default[SOLENOIDE_I_OFST] = APAGADO;
	
	port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
	port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;
	
	port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
	port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;
	
	port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
	port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;
	
	port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
	port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;
	
	break;


	case 0x2C:
	/* Your code here */
	port_slots_write[PASO_MODO_A_NEW_OFST] = ENCENDIDO;
	port_slots_default[PASO_MODO_A_NEW_OFST] = ENCENDIDO;

	port_slots_write[PASO_MODO_B_NEW_OFST] = ENCENDIDO;
	port_slots_default[PASO_MODO_B_NEW_OFST] = ENCENDIDO;

	port_slots_write[MODO_BIT_1_OFST] = ENCENDIDO;
	port_slots_default[MODO_BIT_1_OFST] = ENCENDIDO;

	port_slots_write[MODO_BIT_2_OFST] = APAGADO;
	port_slots_default[MODO_BIT_2_OFST] = APAGADO;

	port_slots_write[MODO_BIT_3_OFST] = ENCENDIDO;
	port_slots_default[MODO_BIT_3_OFST] = ENCENDIDO;

	port_slots_write[MODO_BIT_4_OFST] = APAGADO;
	port_slots_default[MODO_BIT_4_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_2_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_2_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_4_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_4_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;
	
	port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
	port_slots_default[SOLENOIDE_S_OFST] = APAGADO;
	
	port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
	port_slots_default[SOLENOIDE_I_OFST] = APAGADO;
	
	port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
	port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;
	
	port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
	port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;
	
	port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
	port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;
	
	port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
	port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;
	
	break;


	case 0x3C:
	/* Your code here */
	port_slots_write[PASO_MODO_A_NEW_OFST] = ENCENDIDO;
	port_slots_default[PASO_MODO_A_NEW_OFST] = ENCENDIDO;

	port_slots_write[PASO_MODO_B_NEW_OFST] = ENCENDIDO;
	port_slots_default[PASO_MODO_B_NEW_OFST] = ENCENDIDO;

	port_slots_write[MODO_BIT_1_OFST] = ENCENDIDO;
	port_slots_default[MODO_BIT_1_OFST] = ENCENDIDO;

	port_slots_write[MODO_BIT_2_OFST] = APAGADO;
	port_slots_default[MODO_BIT_2_OFST] = APAGADO;

	port_slots_write[MODO_BIT_3_OFST] = APAGADO;
	port_slots_default[MODO_BIT_3_OFST] = APAGADO;

	port_slots_write[MODO_BIT_4_OFST] = ENCENDIDO;
	port_slots_default[MODO_BIT_4_OFST] = ENCENDIDO;
	
	port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_2_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_2_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_4_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_4_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;
	
	port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
	port_slots_default[SOLENOIDE_S_OFST] = APAGADO;
	
	port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
	port_slots_default[SOLENOIDE_I_OFST] = APAGADO;
	
	port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
	port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;
	
	port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
	port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;
	
	port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
	port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;
	
	port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
	port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;
	
	break;

	case 0x4C:
	/* Your code here */
	port_slots_write[PASO_MODO_A_NEW_OFST] = ENCENDIDO;
	port_slots_default[PASO_MODO_A_NEW_OFST] = ENCENDIDO;

	port_slots_write[PASO_MODO_B_NEW_OFST] = ENCENDIDO;
	port_slots_default[PASO_MODO_B_NEW_OFST] = ENCENDIDO;

	port_slots_write[MODO_BIT_1_OFST] = APAGADO;
	port_slots_default[MODO_BIT_1_OFST] = APAGADO;

	port_slots_write[MODO_BIT_2_OFST] = ENCENDIDO;
	port_slots_default[MODO_BIT_2_OFST] = ENCENDIDO;

	port_slots_write[MODO_BIT_3_OFST] = APAGADO;
	port_slots_default[MODO_BIT_3_OFST] = APAGADO;

	port_slots_write[MODO_BIT_4_OFST] = ENCENDIDO;
	port_slots_default[MODO_BIT_4_OFST] = ENCENDIDO;
	
	port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_2_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_2_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_4_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_4_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;
	
	port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
	port_slots_default[SOLENOIDE_S_OFST] = APAGADO;
	
	port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
	port_slots_default[SOLENOIDE_I_OFST] = APAGADO;
	
	port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
	port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;
	
	port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
	port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;
	
	port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
	port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;
	
	port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
	port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;
	
	break;
	
	case 0x0D:
	/* Your code here */
	port_slots_write[PASO_MODO_A_NEW_OFST] = ENCENDIDO;
	port_slots_default[PASO_MODO_A_NEW_OFST] = ENCENDIDO;

	port_slots_write[PASO_MODO_B_NEW_OFST] = ENCENDIDO;
	port_slots_default[PASO_MODO_B_NEW_OFST] = ENCENDIDO;

	port_slots_write[MODO_BIT_1_OFST] = ENCENDIDO;
	port_slots_default[MODO_BIT_1_OFST] = ENCENDIDO;

	port_slots_write[MODO_BIT_2_OFST] = ENCENDIDO;
	port_slots_default[MODO_BIT_2_OFST] = ENCENDIDO;

	port_slots_write[MODO_BIT_3_OFST] = ENCENDIDO;
	port_slots_default[MODO_BIT_3_OFST] = ENCENDIDO;

	port_slots_write[MODO_BIT_4_OFST] = APAGADO;
	port_slots_default[MODO_BIT_4_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_2_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_2_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_4_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_4_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;
	
	port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
	port_slots_default[SOLENOIDE_S_OFST] = APAGADO;
	
	port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
	port_slots_default[SOLENOIDE_I_OFST] = APAGADO;
	
	port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
	port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;
	
	port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
	port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;
	
	port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
	port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;
	
	port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
	port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;
	
	
	break;

	case 0x1D:
	/* Your code here */
	port_slots_write[PASO_MODO_A_NEW_OFST] = ENCENDIDO;
	port_slots_default[PASO_MODO_A_NEW_OFST] = ENCENDIDO;

	port_slots_write[PASO_MODO_B_NEW_OFST] = ENCENDIDO;
	port_slots_default[PASO_MODO_B_NEW_OFST] = ENCENDIDO;

	port_slots_write[MODO_BIT_1_OFST] = ENCENDIDO;
	port_slots_default[MODO_BIT_1_OFST] = ENCENDIDO;

	port_slots_write[MODO_BIT_2_OFST] = ENCENDIDO;
	port_slots_default[MODO_BIT_2_OFST] = ENCENDIDO;

	port_slots_write[MODO_BIT_3_OFST] = APAGADO;
	port_slots_default[MODO_BIT_3_OFST] = APAGADO;

	port_slots_write[MODO_BIT_4_OFST] = ENCENDIDO;
	port_slots_default[MODO_BIT_4_OFST] = ENCENDIDO;
	
	port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_2_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_2_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_4_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_4_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;
	
	port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
	port_slots_default[SOLENOIDE_S_OFST] = APAGADO;
	
	port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
	port_slots_default[SOLENOIDE_I_OFST] = APAGADO;
	
	port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
	port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;
	
	port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
	port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;
	
	port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
	port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;
	
	port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
	port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;
	
	break;


	case 0x2D:
	/* Your code here */
	port_slots_write[PASO_MODO_A_NEW_OFST] = ENCENDIDO;
	port_slots_default[PASO_MODO_A_NEW_OFST] = ENCENDIDO;

	port_slots_write[PASO_MODO_B_NEW_OFST] = ENCENDIDO;
	port_slots_default[PASO_MODO_B_NEW_OFST] = ENCENDIDO;

	port_slots_write[MODO_BIT_1_OFST] = ENCENDIDO;
	port_slots_default[MODO_BIT_1_OFST] = ENCENDIDO;

	port_slots_write[MODO_BIT_2_OFST] = APAGADO;
	port_slots_default[MODO_BIT_2_OFST] = APAGADO;

	port_slots_write[MODO_BIT_3_OFST] = ENCENDIDO;
	port_slots_default[MODO_BIT_3_OFST] = ENCENDIDO;

	port_slots_write[MODO_BIT_4_OFST] = APAGADO;
	port_slots_default[MODO_BIT_4_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_2_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_2_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_4_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_4_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;
	
	port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
	port_slots_default[SOLENOIDE_S_OFST] = APAGADO;
	
	port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
	port_slots_default[SOLENOIDE_I_OFST] = APAGADO;
	
	port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
	port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;
	
	port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
	port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;
	
	port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
	port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;
	
	port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
	port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;
	
	break;


	case 0x3D:
	/* Your code here */
	port_slots_write[PASO_MODO_A_NEW_OFST] = ENCENDIDO;
	port_slots_default[PASO_MODO_A_NEW_OFST] = ENCENDIDO;

	port_slots_write[PASO_MODO_B_NEW_OFST] = ENCENDIDO;
	port_slots_default[PASO_MODO_B_NEW_OFST] = ENCENDIDO;

	port_slots_write[MODO_BIT_1_OFST] = ENCENDIDO;
	port_slots_default[MODO_BIT_1_OFST] = ENCENDIDO;

	port_slots_write[MODO_BIT_2_OFST] = APAGADO;
	port_slots_default[MODO_BIT_2_OFST] = APAGADO;

	port_slots_write[MODO_BIT_3_OFST] = APAGADO;
	port_slots_default[MODO_BIT_3_OFST] = APAGADO;

	port_slots_write[MODO_BIT_4_OFST] = ENCENDIDO;
	port_slots_default[MODO_BIT_4_OFST] = ENCENDIDO;
	
	port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_2_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_2_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_4_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_4_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;
	
	port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
	port_slots_default[SOLENOIDE_S_OFST] = APAGADO;
	
	port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
	port_slots_default[SOLENOIDE_I_OFST] = APAGADO;
	
	port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
	port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;
	
	port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
	port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;
	
	port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
	port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;
	
	port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
	port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;
	
	break;

	case 0x4D:
	/* Your code here */
	port_slots_write[PASO_MODO_A_NEW_OFST] = ENCENDIDO;
	port_slots_default[PASO_MODO_A_NEW_OFST] = ENCENDIDO;

	port_slots_write[PASO_MODO_B_NEW_OFST] = ENCENDIDO;
	port_slots_default[PASO_MODO_B_NEW_OFST] = ENCENDIDO;

	port_slots_write[MODO_BIT_1_OFST] = APAGADO;
	port_slots_default[MODO_BIT_1_OFST] = APAGADO;

	port_slots_write[MODO_BIT_2_OFST] = ENCENDIDO;
	port_slots_default[MODO_BIT_2_OFST] = ENCENDIDO;

	port_slots_write[MODO_BIT_3_OFST] = APAGADO;
	port_slots_default[MODO_BIT_3_OFST] = APAGADO;

	port_slots_write[MODO_BIT_4_OFST] = ENCENDIDO;
	port_slots_default[MODO_BIT_4_OFST] = ENCENDIDO;
	
	port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_2_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_2_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_4_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_4_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;
	
	port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
	port_slots_default[SOLENOIDE_S_OFST] = APAGADO;
	
	port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
	port_slots_default[SOLENOIDE_I_OFST] = APAGADO;
	
	port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
	port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;
	
	port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
	port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;
	
	port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
	port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;
	
	port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
	port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;
	
	break;
	
	
	case 0x0E:
	port_slots_write[PASO_MODO_A_ANT_OFST] = APAGADO;
	port_slots_default[PASO_MODO_A_ANT_OFST] = APAGADO;
	
	port_slots_write[PASO_MODO_B_ANT_OFST] = APAGADO;
	port_slots_default[PASO_MODO_B_ANT_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_2_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_2_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_4_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_4_OFST] = APAGADO;
	
	port_slots_write[MODO_BIT_1_OFST] = APAGADO;
	port_slots_default[MODO_BIT_1_OFST] = APAGADO;
	
	port_slots_write[MODO_BIT_2_OFST] = APAGADO;
	port_slots_default[MODO_BIT_2_OFST] = APAGADO;
	
	port_slots_write[MODO_BIT_3_OFST] = APAGADO;
	port_slots_default[MODO_BIT_3_OFST] = APAGADO;
	
	port_slots_write[MODO_BIT_4_OFST] = APAGADO;
	port_slots_default[MODO_BIT_4_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;
	
	port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
	port_slots_default[SOLENOIDE_S_OFST] = APAGADO;
	
	port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
	port_slots_default[SOLENOIDE_I_OFST] = APAGADO;
	
	port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
	port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;
	
	port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
	port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;
	
	port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
	port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;
	
	port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
	port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;
	break;
	
	
	case 0x0F:
	port_slots_write[PASO_MODO_A_ANT_OFST] = APAGADO;
	port_slots_default[PASO_MODO_A_ANT_OFST] = APAGADO;
	
	port_slots_write[PASO_MODO_B_ANT_OFST] = APAGADO;
	port_slots_default[PASO_MODO_B_ANT_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_2_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_2_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;
	
	port_slots_write[PICTOGRAMA_4_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_4_OFST] = APAGADO;
	
	port_slots_write[MODO_BIT_1_OFST] = APAGADO;
	port_slots_default[MODO_BIT_1_OFST] = APAGADO;
	
	port_slots_write[MODO_BIT_2_OFST] = APAGADO;
	port_slots_default[MODO_BIT_2_OFST] = APAGADO;
	
	port_slots_write[MODO_BIT_3_OFST] = APAGADO;
	port_slots_default[MODO_BIT_3_OFST] = APAGADO;
	
	port_slots_write[MODO_BIT_4_OFST] = APAGADO;
	port_slots_default[MODO_BIT_4_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;
	
	port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
	port_slots_default[SOLENOIDE_S_OFST] = APAGADO;
	
	port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
	port_slots_default[SOLENOIDE_I_OFST] = APAGADO;
	
	port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
	port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;
	
	port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
	port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;
	
	port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
	port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;
	
	port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
	port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
	
	port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;
	break;
	
	default:
	/* Your code here */
	
	port_slots_write[PASO_MODO_A_ANT_OFST] = APAGADO;
	port_slots_default[PASO_MODO_A_ANT_OFST] = APAGADO;

	port_slots_write[PASO_MODO_B_ANT_OFST] = APAGADO;
	port_slots_default[PASO_MODO_B_ANT_OFST] = APAGADO;

	port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;

	port_slots_write[PICTOGRAMA_2_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_2_OFST] = APAGADO;

	port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;

	port_slots_write[PICTOGRAMA_4_OFST] = APAGADO;
	port_slots_default[PICTOGRAMA_4_OFST] = APAGADO;

	port_slots_write[MODO_BIT_1_OFST] = APAGADO;
	port_slots_default[MODO_BIT_1_OFST] = APAGADO;

	port_slots_write[MODO_BIT_2_OFST] = APAGADO;
	port_slots_default[MODO_BIT_2_OFST] = APAGADO;

	port_slots_write[MODO_BIT_3_OFST] = APAGADO;
	port_slots_default[MODO_BIT_3_OFST] = APAGADO;

	port_slots_write[MODO_BIT_4_OFST] = APAGADO;
	port_slots_default[MODO_BIT_4_OFST] = APAGADO;

	port_slots_write[PASO_MODO_A_NEW_OFST] = APAGADO;
	port_slots_default[PASO_MODO_A_NEW_OFST] = APAGADO;

	port_slots_write[PASO_MODO_B_NEW_OFST] = APAGADO;
	port_slots_default[PASO_MODO_B_NEW_OFST] = APAGADO;

	port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;

	port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
	port_slots_default[SOLENOIDE_S_OFST] = APAGADO;

	port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
	port_slots_default[SOLENOIDE_I_OFST] = APAGADO;

	port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
	port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;

	port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
	port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;

	port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;

	port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
	port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;

	port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
	port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;

	port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
	port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;

	break;
	
	}
	
	port_slots_read[ESCENARIO] = escenario_idx;
}

void UART0_Handler()
{
	unsigned int static checksum_eltra = 0;
	char_timeout_RS232 = 40;
	//	static	int	rx_idx;
	uint8_t received_byte;
	//	unsigned	int	temp_char = 0;
	uint32_t dw_status = uart_get_status(UART0);
	unsigned int checksum_hi = 0;
	unsigned int checksum_lo = 0;
	
	

	if(dw_status & UART_SR_RXRDY)
	{
		uart_read(UART0, &received_byte);
		//			temp_char = (int)received_byte;
		//	uart_write(UART1, received_byte);

		bufer_serial_rx[rx_idx_RS232++] = (int)received_byte;


		if (rx_idx_RS232 == 0x01) {
			if ((unsigned int)received_byte == 0x15) {
				error_code = ERROR_NACK_eltra;
				not_ack_RS485();
				char_timeout_RS485 = 0;
				rx_idx_RS485 = 0;
			}
			else if ((unsigned int)received_byte == 0x06) {
				rx_idx_RS232 = 0;
				comando_eltra = 0xFF;
				code0_eltra = 0xFF;
				sendInquireEltra();
			}
			else if ((unsigned int)received_byte == 0x10) {
				rx_idx_RS232 = 0;
				comando_eltra = 0xFF;
                code0_eltra = 0xFF;
			}
		}
				
		
		if (rx_idx_RS232 == 0x02)
		{
			data_leng = 255;
			if (((unsigned int)received_byte != 0x42) && ((unsigned int) bufer_serial_rx[0] != 0x02))
			{
				rx_idx_RS232 = 0;
				comando_eltra = 0xFF;
				code0_eltra = 0xFF;
			}
		}
		
		if (rx_idx_RS232 == 0x03)
		{
			comando_eltra = (int)received_byte;
		}
		
		if (rx_idx_RS232 == 0x04)
		{
			code0_eltra = (int)received_byte;
		}

		if (rx_idx_RS232 == 0x05)
		{
			code1_eltra = (int)received_byte;
			
		}
		
		if (rx_idx_RS232 == 0x06)
		{
			code2_eltra = (int)received_byte;
		}
		
		//NO llega el final de trama (Cambiar esta longitud cuando se sopoerta comandos largos)
		if (rx_idx_RS232 > 0x10) {
			rx_idx_RS232 = 0;
            comando_eltra = 0xFF;
            code0_eltra = 0xFF;
		}

		//Llega FIN de trama
		if (rx_idx_RS232 > 0x06 && (unsigned int)received_byte == 0x04) {
			switch (code0_eltra)
			{
				case 0x52:
					error_code = ERROR_CMD_Refused_eltra;
					char_timeout_RS232 = 0;
					rx_idx_RS232 = 0;
					break;
				case 0x54:
					error_code = ERROR_CMD_no_eltra;
					char_timeout_RS232 = 0;
					rx_idx_RS232 = 0;
					break;
				case 0x30:
					
					if ((unsigned int) bufer_serial_rx[rx_idx_RS232-4] != 0x03) {
						error_code = ERROR_NACK_eltra;
                        char_timeout_RS232 = 0;
                        rx_idx_RS232 = 0;
						break;
                    }
					checksum_eltra = 0;
					for (int j=0; j< (rx_idx_RS232-4); j++) {
						checksum_eltra = checksum_eltra ^ (unsigned int) bufer_serial_rx[j];
					}
					checksum_hi = (checksum_eltra >> 4 & 0x0F) + 0x30;
					checksum_lo = (checksum_eltra & 0x0F) + 0x30;

					//if ((unsigned int) bufer_serial_rx[rx_idx_RS232-3] != checksum_hi && (unsigned int) bufer_serial_rx[rx_idx_RS232-2] != checksum_lo) {
					//	error_code = ERROR_NACK_eltra;
       	            //    char_timeout_RS232 = 0;
               	    //    rx_idx_RS232 = 0;
					//	break;
					//}
					break;
				default:
					error_code = ERROR_NACK_eltra;
					char_timeout_RS232 = 0;
                    rx_idx_RS232 = 0;
                    break;
			}
			//Si no se ha encontrado un error
			if (rx_idx_RS232 != 0) {
				switch (comando_eltra)
				{
					case 0x3F:
						port_slots_read[SENSOR_STATUS_ELTRA] = code1_eltra;
						if (code1_eltra == 0x30) {
							LED_SENS_S_OFF;
							LED_SENS_I_OFF;
						} else if (code1_eltra == 0x31) {
							LED_SENS_S_OFF;
							LED_SENS_I_ON;
						} else if (code1_eltra == 0x32) {
							LED_SENS_S_ON;
							LED_SENS_I_OFF;
						} else if (code1_eltra == 0x33) {
							LED_SENS_S_ON;
							LED_SENS_I_ON;
						}
						break;
					case 0x3C:
						//qué hacer con el estado 0x30?
						if (code0_eltra != 0x30) {
							port_slots_read[SENSOR_STATUS_ELTRA] = 0x30;
							break;
						}
						if (code1_eltra == 0x30) {
							port_slots_read[SENSOR_STATUS_ELTRA] = 0x31;
						} else if (code1_eltra == 0x31) {
							port_slots_read[SENSOR_STATUS_ELTRA] = 0x31;
						} else if (code1_eltra == 0x32) {
							port_slots_read[SENSOR_STATUS_ELTRA] = 0x30;
						} else if (code1_eltra == 0x38) {
							port_slots_read[SENSOR_STATUS_ELTRA] = 0x30;
						}
						break;
					case 0x76:
						if (code0_eltra == 0x30 && code1_eltra == 0x30) {
							port_slots_read[SENSOR_STATUS_ELTRA] = 0x30;
						}
						break;
							
					default:
						break;
				}
			}
		}
	}
}

void UART1_Handler()
{
	unsigned int static sumatoria_v = 0;
	char_timeout_RS485 = 20;
	//	static	int	rx_idx;
	uint8_t received_byte;
	//	unsigned	int	temp_char = 0;
	uint32_t dw_status = uart_get_status(UART1);



	if(dw_status & UART_SR_RXRDY)
	{
		uart_read(UART1, &received_byte);
		//			temp_char = (int)received_byte;
		//	uart_write(UART1, received_byte);

		bufer_serial_rx[rx_idx_RS485 ++] = (int)received_byte;
		
		if (rx_idx_RS485 == 0x01)
		{
			sumatoria_v = (int)received_byte;
			data_leng = 255;
			if (((unsigned int)received_byte != dev_id) && ((unsigned int)received_byte != 0x80))
			{
				rx_idx_RS485 = 0;
				funcion = 0x20;
			}
		}

		
		if ((rx_idx_RS485 != 0x01) && (rx_idx_RS485 < (data_leng + 4)))
		{
			sumatoria_v ^= (int)received_byte;
		}
		
		if (rx_idx_RS485 == 0x02)
		{
			if (((int)received_byte == 0x10) || ((int)received_byte == 0x1A) || ((int)received_byte == 0x1B))
			{
				funcion = (int)received_byte;
			}
			else
			{
				rx_idx_RS485 = 0;
				funcion = 0x20;
			}
		}
		
		if (rx_idx_RS485 == 3)
		{
			port_address = (int)received_byte;
		}

		if (rx_idx_RS485 == 4)
		{
			data_leng = (int)received_byte;
		}
		
		if (rx_idx_RS485 == 5)
		{
			escenario_temp = (int)received_byte;
		}
		
		
		int	i = 1;

		switch (funcion)
		{
			case 0x10:

			if (bufer_serial_rx[0] != 0x80)
			{
				if ((rx_idx_RS485 == 0x06) && ((int)received_byte != 0xFC))
				{
					error_code = ERROR_OVER;
					not_ack_RS485();
					char_timeout_RS485 = 0;
					rx_idx_RS485 = 0;
				}

				

				
				sumatoria_v = (unsigned int)bufer_serial_rx[0];
				
				for (i = 1; i < (4); i++)
				{
					sumatoria_v ^= (unsigned int)bufer_serial_rx[i];
				}
				if ((rx_idx_RS485 == 0x06) && ((-sumatoria_v & 0x000000FF) != (unsigned int)bufer_serial_rx[4] ))
				{
					error_code = ERROR_CHK;
					not_ack_RS485();
				}
				
				if ((rx_idx_RS485 == 6) && ((unsigned char)received_byte == 0xFC))
				{
					
					
					if ((-sumatoria_v & 0x000000FF) == (unsigned int)bufer_serial_rx[4] )
					{
						
						
						sumatoria_v = 0xA1 ^ 0x10 ^ (char)port_address ^ (char)data_leng;
						
						bufer_seria_tx[0] = 0xA1;
						bufer_seria_tx[1] = 0x10;
						bufer_seria_tx[2] = (char)port_address;
						bufer_seria_tx[3] = (char)data_leng;
						
						for (i = 4; i < (data_leng + 4); i++)
						{
							bufer_seria_tx[i] = (char)port_slots_read[port_address];
							sumatoria_v ^= (char)port_slots_read[port_address++];
							
							if (((port_address - 1) == CONFIRMACION_A_ANT_OFST) || ((port_address - 1) == CONFIRMACION_A_NEW_OFST))
							{
								port_slots_read[CONFIRMACION_A_ANT_OFST] = 0;
								port_slots_read[CONFIRMACION_A_NEW_OFST] = 0;
							}
							
							if (((port_address - 1) == CONFIRMACION_B_ANT_OFST) || ((port_address - 1) == CONFIRMACION_B_NEW_OFST))
							{
								port_slots_read[CONFIRMACION_B_ANT_OFST] = 0;
								port_slots_read[CONFIRMACION_B_NEW_OFST] = 0;
							}
							
							if (((port_address - 1) == ABIERTA_AS_OFST) || ((port_address - 1) == ABIERTA_GU_OFST))
							{
								port_slots_read[ABIERTA_AS_OFST] = 0;
								port_slots_read[ABIERTA_GU_OFST] = 0;
							}
							
							if (((port_address - 1) == CERRADA_AS_OFST) || ((port_address - 1) == CERRADA_GU_OFST))
							{
								port_slots_read[CERRADA_AS_OFST] = 0;
								port_slots_read[CERRADA_GU_OFST] = 0;
							}
							if (((port_address - 1) == SENSOR_STATUS_ELTRA) )
							{
								port_slots_read[SENSOR_STATUS_ELTRA] = 0x0F;
							}
						}
						
						bufer_seria_tx[i++] = -(char)sumatoria_v;
						bufer_seria_tx[i] = 0xFC;
						
						Pdc	*serial_485;
						pdc_packet_t serial_485_TX_packet;
						serial_485 = uart_get_pdc_base(UART1);
						pdc_enable_transfer(serial_485, PERIPH_PTCR_TXTEN);
						serial_485_TX_packet.ul_addr = (uint32_t)bufer_seria_tx;
						serial_485_TX_packet.ul_size = (data_leng + 6);
						pdc_tx_init(serial_485, &serial_485_TX_packet, NULL);
						
					}
					funcion = 0x20;
					rx_idx_RS485 = 0;
				}
				

			}
			
			break;
			
			
			
			//
			case 0x1A:
			sumatoria_v = (unsigned int)bufer_serial_rx[0];
			for (i = 1; i < (data_leng + 4); i++)
			{
				sumatoria_v ^= (unsigned int)bufer_serial_rx[i];
			}
			
			//			int	i = 1;
			//	if ((rx_idx_RS232 == (6 + data_leng)) && (-sumatoria_v != (int)bufer_serial_rx[data_leng + 4]) )
			if ((rx_idx_RS485 == (6 + data_leng)) && ((int)received_byte != 0xFC))
			{
				error_code = ERROR_OVER;
				not_ack_RS485();
			}
			else
			{
				if ((rx_idx_RS485 == (0x06 + data_leng)) && ((-sumatoria_v & 0x000000FF) != (unsigned int)bufer_serial_rx[data_leng + 4] ))

				{
					error_code = ERROR_CHK;
					not_ack_RS485();
				}
			}

			if ((rx_idx_RS485 == (6 + data_leng)) && ((unsigned int)received_byte == 0xFC))
			{

				// 				sumatoria_v = (unsigned int)bufer_serial_rx[0];
				//
				// 				for (i = 1; i < (data_leng + 4); i++)
				// 				{
				// 					sumatoria_v ^= (unsigned int)bufer_serial_rx[i];
				// 				}

				if ((-sumatoria_v & 0x000000FF) == (unsigned int)bufer_serial_rx[i] )
				{
					if ((port_address + data_leng) < 0x80)
					{
						int	port_address_temp = port_address;
						for (i = 0; i < data_leng; i++)
						{
							port_slots_write[port_address_temp++] = bufer_serial_rx[i + 4];
						}
						funcion = 0x20;
						
						if (bufer_serial_rx[0] != 0x80)
						{
							bufer_seria_tx[0] = 0xA1;
							bufer_seria_tx[1] = 0x06;
							bufer_seria_tx[2] = 0x00;
							bufer_seria_tx[3] = -((char)(0xA1 ^ 0x06 ^ 0x00));
							bufer_seria_tx[4] = 0xFC;
							
							Pdc	*serial_485;
							pdc_packet_t serial_485_TX_packet;
							serial_485 = uart_get_pdc_base(UART1);
							pdc_enable_transfer(serial_485, PERIPH_PTCR_TXTEN);
							serial_485_TX_packet.ul_addr = (uint32_t)bufer_seria_tx;
							serial_485_TX_packet.ul_size = 5;
							pdc_tx_init(serial_485, &serial_485_TX_packet, NULL);
						}
					}
					else
					{
						error_code = ERROR_ACS;
						not_ack_RS485();
					}
				}
			}
			//
			//
			
			break;
			
			

			case 0x1B:

			if (bufer_serial_rx[0] != 0x80)
			{
				if ((rx_idx_RS485 == 0x07) && ((int)received_byte != 0xFC))
				{
					error_code = ERROR_OVER;
					not_ack_RS485();
					char_timeout_RS485 = 0;
					rx_idx_RS485 = 0;
				}


				sumatoria_v = (unsigned int)bufer_serial_rx[0];
				
				for (i = 1; i < (5); i++)
				{
					sumatoria_v ^= (unsigned int)bufer_serial_rx[i];
				}
				
				if ((rx_idx_RS485 == 0x07) && ((-sumatoria_v & 0x000000FF) != (unsigned int)bufer_serial_rx[5] ))
				{
					error_code = ERROR_CHK;
					not_ack_RS485();
				}
				
				if ((rx_idx_RS485 == 7) && ((unsigned char)received_byte == 0xFC))
				{
					
					
					if ((-sumatoria_v & 0x000000FF) == (unsigned int)bufer_serial_rx[5] )
					{
						//if ((escenario_temp < 16) || (escenario_temp > 127))
						//{
						//	error_code = ERROR_SCN;
						//	not_ack_RS485();
						//}
						//else
						//{
							escenario_set(escenario_temp);
							bufer_seria_tx[0] = 0xA1;
							bufer_seria_tx[1] = 0x06;
							bufer_seria_tx[2] = 0x00;
							bufer_seria_tx[3] = -((char)(0xA1 ^ 0x06 ^ 0x00));
							bufer_seria_tx[4] = 0xFC;
							
							Pdc	*serial_485;
							pdc_packet_t serial_485_TX_packet;
							serial_485 = uart_get_pdc_base(UART1);
							pdc_enable_transfer(serial_485, PERIPH_PTCR_TXTEN);
							serial_485_TX_packet.ul_addr = (uint32_t)bufer_seria_tx;
							serial_485_TX_packet.ul_size = 5;
							pdc_tx_init(serial_485, &serial_485_TX_packet, NULL);
						//}
						

						
					}
					funcion = 0x20;
					rx_idx_RS485 = 0;
				}
				

			}
			
			break;
			
			
		}

		if (rx_idx_RS485 == (6 + data_leng))
		{
			rx_idx_RS485 = 0;
			funcion = 0x20;
		}
		
	}
}


/**
* \brief Application entry point for pdc_uart example.
*
* \return Unused (ANSI-C compatibility).
*/
int main(void)
{
	//! [board_setup]
	sysclk_init();
	board_init();
	/* Initialize the SAM system */
	delay_init();
	matrix_set_system_io(CCFG_SYSIO_SYSIO10 | CCFG_SYSIO_SYSIO11 | CCFG_SYSIO_SYSIO12 );
	
	
	
	//	TQ paso / confirm
	gpio_configure_pin(NA,PIO_OUTPUT_0 | PIO_DEFAULT);	//	NA
	gpio_configure_pin(EMERGENCIA_PASO,PIO_OUTPUT_0 | PIO_DEFAULT);	//
	gpio_configure_pin(PASO_MODO_A,PIO_OUTPUT_0 | PIO_DEFAULT);	//
	gpio_configure_pin(PASO_MODO_B,PIO_OUTPUT_0 | PIO_DEFAULT);	//
	gpio_configure_pin(CONFIRMACION_A,PIO_INPUT | PIO_DEFAULT);	//
	gpio_configure_pin(CONFIRMACION_B,PIO_INPUT | PIO_DEFAULT);	//
	
	//	Emergencia ppl
	gpio_configure_pin(EMERGENCIA_PPL,PIO_INPUT | PIO_DEFAULT);	//
	
	// TQ config / modo
	gpio_configure_pin(MODO_BIT_1,PIO_OUTPUT_0 | PIO_DEFAULT);	//
	gpio_configure_pin(MODO_BIT_2,PIO_OUTPUT_0 | PIO_DEFAULT);	//
	gpio_configure_pin(MODO_BIT_3,PIO_OUTPUT_0 | PIO_DEFAULT);	//
	gpio_configure_pin(MODO_BIT_4,PIO_OUTPUT_0 | PIO_DEFAULT);	//
	
	// Pictogramas
	gpio_configure_pin(PICTOGRAMA_1,PIO_OUTPUT_0 | PIO_DEFAULT);	//
	gpio_configure_pin(PICTOGRAMA_2,PIO_OUTPUT_0 | PIO_DEFAULT);	//
	gpio_configure_pin(PICTOGRAMA_3,PIO_OUTPUT_0 | PIO_DEFAULT);	//
	gpio_configure_pin(PICTOGRAMA_4,PIO_OUTPUT_0 | PIO_DEFAULT);	//
	
	// PMR
	gpio_configure_pin(ABIERTA,PIO_INPUT | PIO_DEFAULT);	//
	gpio_configure_pin(CERRADA,PIO_INPUT | PIO_DEFAULT);	//
	gpio_configure_pin(ALARMADA,PIO_INPUT | PIO_DEFAULT);	//
	gpio_configure_pin(APERTURA_DERECHA,PIO_OUTPUT_0 | PIO_DEFAULT);	//
	gpio_configure_pin(APERTURA_IZQUIERDA,PIO_OUTPUT_0 | PIO_DEFAULT);	//
	gpio_configure_pin(EMERGENCIA_PMR,PIO_OUTPUT_0 | PIO_DEFAULT);	//
	
	// Capturadora
	gpio_configure_pin(SENSOR_I,PIO_INPUT | PIO_DEFAULT);	//
	gpio_configure_pin(SENSOR_S,PIO_INPUT | PIO_DEFAULT);	//
	gpio_configure_pin(SOLENOIDE_I,PIO_OUTPUT_0 | PIO_DEFAULT);	//
	gpio_configure_pin(SOLENOIDE_S,PIO_OUTPUT_0 | PIO_DEFAULT);	//
	
	//	Dip switch
	gpio_configure_pin(BIT_0,PIO_INPUT | PIO_DEFAULT | PIO_PULLUP);	//
	gpio_configure_pin(BIT_1,PIO_INPUT | PIO_DEFAULT | PIO_PULLUP);	//
	gpio_configure_pin(BIT_2,PIO_INPUT | PIO_DEFAULT | PIO_PULLUP);	//
	gpio_configure_pin(BIT_3,PIO_INPUT | PIO_DEFAULT | PIO_PULLUP);	//
	
	//	Dip switch_escenario
	gpio_configure_pin(BIT_SC_4,PIO_INPUT | PIO_DEFAULT | PIO_PULLUP);//  SE CONFIGURA BITS
	gpio_configure_pin(BIT_SC_3,PIO_INPUT | PIO_DEFAULT | PIO_PULLUP);	//
	gpio_configure_pin(BIT_SC_2,PIO_INPUT | PIO_DEFAULT | PIO_PULLUP);	//
	gpio_configure_pin(BIT_SC_1,PIO_INPUT | PIO_DEFAULT | PIO_PULLUP);	//
	gpio_configure_pin(BIT_SC_0,PIO_INPUT | PIO_DEFAULT | PIO_PULLUP);	//

	//	LEDs
	gpio_configure_pin(LED_SENS_S,PIO_OUTPUT_0 | PIO_DEFAULT);	//
	gpio_configure_pin(LED_SENS_I,PIO_OUTPUT_0 | PIO_DEFAULT);	//
	gpio_configure_pin(LED_3,PIO_OUTPUT_0 | PIO_DEFAULT);	//
	gpio_configure_pin(LED_4,PIO_OUTPUT_0 | PIO_DEFAULT);	//



	// 	/* Configura UART. */

	//	#include "asf.h" //uart.h etc. included here
	pmc_enable_periph_clk(ID_UART1);
	pmc_enable_periph_clk(ID_UART0);
	pmc_enable_periph_clk(ID_PIOB);
	pmc_enable_periph_clk(ID_PIOA);
	#define UART_SERIAL_BAUDRATE       19200
	#define UART_SERIAL_BAUDRATE_485       115200 /// para pruebas 19200, para produccion 115200
	#define UART_SERIAL_CHANNEL_MODE   UART_MR_CHMODE_NORMAL
	#define UART_SERIAL_MODE         UART_MR_PAR_NO

	/* =============== UART1 =============== */ //(UART0 is defined but not UART1)
	#define PINS_UART1          (PIO_PB2A_URXD1 | PIO_PB3A_UTXD1)
	#define PINS_UART1_FLAGS    (PIO_PERIPH_A | PIO_DEFAULT)
	#define PINS_UART1_MASK     (PIO_PB2A_URXD1 | PIO_PB3A_UTXD1)
	#define PINS_UART1_PIO      PIOB
	#define PINS_UART1_ID       ID_PIOB
	#define PINS_UART1_TYPE     PIO_PERIPH_A
	#define PINS_UART1_ATTR     PIO_DEFAULT

	// set the pins to use the uart peripheral
	pio_configure(PINS_UART1_PIO, PINS_UART1_TYPE, PINS_UART1_MASK, PINS_UART1_ATTR);

	//enable the uart peripherial clock


	const sam_uart_opt_t uart1_settings =
	{ sysclk_get_cpu_hz(), UART_SERIAL_BAUDRATE_485, UART_SERIAL_MODE };

	uart_init(UART1,&uart1_settings);      //Init UART1 and enable Rx and Tx

	uart_enable_interrupt(UART1,UART_IER_RXRDY);   //Interrupt reading ready
	NVIC_EnableIRQ(UART1_IRQn);

	// 	uart_init(UART1, &serial_485_opt_t);
	uart_enable_tx(UART1);
	uart_enable_rx(UART1);
	uart_enable(UART1);                     //Enable UART1
	
	
	
	

	// 	// 	// Configura DMA serial TX
	Pdc	*serial_485;
	//	pdc_packet_t serial_485_TX_packet;
	serial_485 = uart_get_pdc_base(UART1);
	pdc_enable_transfer(serial_485, PERIPH_PTCR_RXTEN | PERIPH_PTCR_TXTEN);
	// 	// 	//
	// 	serial_485_TX_packet.ul_addr = (uint32_t) bufer_seria_tx;
	// 	serial_485_TX_packet.ul_size = 72;
	//
	// 	// 	// Configura DMA serial RX
	// 	serial_485_RX_packet.ul_addr = (uint32_t) bufer_serial_RX_a;
	// 	serial_485_RX_packet.ul_size = 72;
	// 	pdc_rx_init(serial_485, &serial_485_RX_packet, NULL);

	// 	// Habilita interrupciones
	uart_enable_interrupt(UART1, UART_IER_RXRDY);
	NVIC_EnableIRQ(UART1_IRQn);
	Enable_global_interrupt();
	
	
	
	
	
	
	/* =============== UART0 =============== */ //(UART0 is defined but not UART1)
	#define PINS_UART0          (PIO_PA9A_URXD0 | PIO_PA10A_UTXD0)
	#define PINS_UART0_FLAGS    (PIO_PERIPH_A | PIO_DEFAULT)
	#define PINS_UART0_MASK     (PIO_PA9A_URXD0 | PIO_PA10A_UTXD0)
	#define PINS_UART0_PIO      PIOA
	#define PINS_UART0_ID       ID_PIOA
	#define PINS_UART0_TYPE     PIO_PERIPH_A
	#define PINS_UART0_ATTR     PIO_DEFAULT

	// set the pins to use the uart peripheral
	pio_configure(PINS_UART0_PIO, PINS_UART0_TYPE, PINS_UART0_MASK, PINS_UART1_ATTR);

	//enable the uart peripherial clock


	const sam_uart_opt_t uart0_settings =
	{ sysclk_get_cpu_hz(), UART_SERIAL_BAUDRATE, UART_SERIAL_MODE };

	uart_init(UART0,&uart0_settings);      //Init UART1 and enable Rx and Tx

	uart_enable_interrupt(UART0,UART_IER_RXRDY);   //Interrupt reading ready
	NVIC_EnableIRQ(UART0_IRQn);

	// 	uart_init(UART1, &serial_485_opt_t);
	uart_enable_tx(UART0);
	uart_enable_rx(UART0);
	uart_enable(UART0);                     //Enable UART1
	


	// 	// 	// Configura DMA serial TX
	Pdc	*serial_232;	//	pdc_packet_t serial_485_TX_packet;
	serial_232 = uart_get_pdc_base(UART0);
	pdc_enable_transfer(serial_232, PERIPH_PTCR_RXTEN | PERIPH_PTCR_TXTEN);
	// 	// 	//
	// 	serial_485_TX_packet.ul_addr = (uint32_t) bufer_seria_tx;
	// 	serial_485_TX_packet.ul_size = 72;
	//
	// 	// 	// Configura DMA serial RX
	// 	serial_485_RX_packet.ul_addr = (uint32_t) bufer_serial_RX_a;
	// 	serial_485_RX_packet.ul_size = 72;
	// 	pdc_rx_init(serial_485, &serial_485_RX_packet, NULL);

	// 	// Habilita interrupciones
	uart_enable_interrupt(UART0, UART_IER_RXRDY);
	NVIC_EnableIRQ(UART0_IRQn);
	Enable_global_interrupt();
	
	
	
	
	
	
	//	PIOA->PIO_PUER = (1 << 19 | 1 << 21 | 1 << 22 | 1 << 23);
	
	// Lee dipswitch de identificacion del dispositivo
	dev_id = 0;
	if (gpio_pin_is_low(BIT_0))
	{
		dev_id += 1;
	}
	
	if (gpio_pin_is_low(BIT_1))
	{
		dev_id += 2;
	}
	
	if (gpio_pin_is_low(BIT_2))
	{
		dev_id += 4;
	}
	
	if (gpio_pin_is_low(BIT_3))
	{
		dev_id += 8;
	}
	
	dev_id |= 0x80;
	//dev_id=0x82;  //para KABA
	
	// Lee dipswitch de definicion de escenario
	escenario_v = 0;
	
	
	if (gpio_pin_is_low(BIT_SC_0))
	{
		escenario_v += 1;
	}
	
	if (gpio_pin_is_low(BIT_SC_1))
	{
		escenario_v += 2;
	}
	
	if (gpio_pin_is_low(BIT_SC_2))
	{
		escenario_v += 4;
	}
	
	if (gpio_pin_is_low(BIT_SC_3))
	{
		escenario_v += 8;
	}
	//////////NUEVO
	if (gpio_pin_is_low(BIT_SC_4))
	{
		escenario_v += 16;
	}

	//escenario_v = 0;   //para KABA
	
	int	i;
	for (i = 0; i < 128; i ++)
	{
		port_slots_write[i] = IGNORAR;
		port_slots_read[i] = IGNORAR;
		port_slots_default[i] = IGNORAR;
	}
	
	// 	serial_485_TX_packet.ul_addr = (uint32_t)bufer_seria_tx;
	// 	serial_485_TX_packet.ul_size = 32;
	// 	pdc_tx_init(serial_485, &serial_485_TX_packet, NULL);

	//	SOLENOIDE_I_ON;
	//	SOLENOIDE_I_OFF	;
	//	SOLENOIDE_S_ON	;
	//	SOLENOIDE_S_OFF	;
	//	PICTOGRAMA_1_ON;
	//	PICTOGRAMA_1_OFF;
	//	PICTOGRAMA_2_ON;
	//	PICTOGRAMA_2_OFF;
	//	PICTOGRAMA_3_ON;
	//	PICTOGRAMA_3_OFF;
	//	PICTOGRAMA_4_ON;
	//	PICTOGRAMA_4_OFF;
	//	EMERGENCIA_PASO_ON;
	//	EMERGENCIA_PASO_OFF	;
	//	PASO_MODO_A_ON	;
	//	PASO_MODO_A_OFF	;
	//	PASO_MODO_B_ON	;
	//	PASO_MODO_B_OFF	;
	escenario_set(escenario_v);
	// 	switch (escenario_v)
	// 	{
	// 		case 0x00:
	// 		port_slots_write[PASO_MODO_A_ANT_OFST] = APAGADO;
	// 		port_slots_default[PASO_MODO_A_ANT_OFST] = APAGADO;
	//
	// 		port_slots_write[PASO_MODO_B_ANT_OFST] = APAGADO;
	// 		port_slots_default[PASO_MODO_B_ANT_OFST] = APAGADO;
	//
	// 		port_slots_write[PICTOGRAMA_1_OFST] = APAGADO;
	// 		port_slots_default[PICTOGRAMA_1_OFST] = APAGADO;
	//
	// 		port_slots_write[PICTOGRAMA_2_OFST] = APAGADO;
	// 		port_slots_default[PICTOGRAMA_2_OFST] = APAGADO;
	//
	// 		port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
	// 		port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;
	//
	// 		port_slots_write[PICTOGRAMA_4_OFST] = APAGADO;
	// 		port_slots_default[PICTOGRAMA_4_OFST] = APAGADO;
	//
	// 		port_slots_write[MODO_BIT_1_OFST] = APAGADO;
	// 		port_slots_default[MODO_BIT_1_OFST] = APAGADO;
	//
	// 		port_slots_write[MODO_BIT_2_OFST] = APAGADO;
	// 		port_slots_default[MODO_BIT_2_OFST] = APAGADO;
	//
	// 		port_slots_write[MODO_BIT_3_OFST] = APAGADO;
	// 		port_slots_default[MODO_BIT_3_OFST] = APAGADO;
	//
	// 		port_slots_write[MODO_BIT_4_OFST] = APAGADO;
	// 		port_slots_default[MODO_BIT_4_OFST] = APAGADO;
	//
	// 		port_slots_write[PASO_MODO_A_NEW_OFST] = APAGADO;
	// 		port_slots_default[PASO_MODO_A_NEW_OFST] = APAGADO;
	//
	// 		port_slots_write[PASO_MODO_B_NEW_OFST] = APAGADO;
	// 		port_slots_default[PASO_MODO_B_NEW_OFST] = APAGADO;
	//
	// 		port_slots_write[EMERGENCIA_PASO_OFST] = APAGADO;
	// 		port_slots_default[EMERGENCIA_PASO_OFST] = APAGADO;
	//
	// 		port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
	// 		port_slots_default[SOLENOIDE_S_OFST] = APAGADO;
	//
	// 		port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
	// 		port_slots_default[SOLENOIDE_I_OFST] = APAGADO;
	//
	// 		port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
	// 		port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;
	//
	// 		port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
	// 		port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
	//
	// 		port_slots_write[EMERGENCIA_PMR_GU_OFST] = APAGADO;
	// 		port_slots_default[EMERGENCIA_PMR_GU_OFST] = APAGADO;
	//
	// 		port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
	// 		port_slots_default[APERTURA_DERECHA_AS_OFST] = APAGADO;
	//
	// 		port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
	// 		port_slots_default[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
	//
	// 		port_slots_write[EMERGENCIA_PMR_AS_OFST] = APAGADO;
	// 		port_slots_default[EMERGENCIA_PMR_AS_OFST] = APAGADO;
	// 		break;
	//
	// 		case 0x01:
	// 		port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
	// 		port_slots_default[SOLENOIDE_S_OFST] = APAGADO;
	//
	// 		port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
	// 		port_slots_default[SOLENOIDE_I_OFST] = APAGADO;
	//
	// 		break;
	//
	// 		case 0x02:
	// 		port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
	// 		port_slots_default[SOLENOIDE_S_OFST] = APAGADO;
	//
	// 		port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
	// 		port_slots_default[SOLENOIDE_I_OFST] = APAGADO;
	//
	// 		port_slots_write[PASO_MODO_A_ANT_OFST] = ENCENDIDO;
	// 		port_slots_default[PASO_MODO_A_ANT_OFST] = ENCENDIDO;
	//
	// 		port_slots_write[PASO_MODO_B_ANT_OFST] = ENCENDIDO;
	// 		port_slots_default[PASO_MODO_B_ANT_OFST] = ENCENDIDO;
	//
	// 		port_slots_write[PICTOGRAMA_1_OFST] = ENCENDIDO;
	// 		port_slots_default[PICTOGRAMA_1_OFST] = ENCENDIDO;
	//
	// 		port_slots_write[PICTOGRAMA_2_OFST] = APAGADO;
	// 		port_slots_default[PICTOGRAMA_2_OFST] = APAGADO;
	//
	// 		port_slots_write[PICTOGRAMA_3_OFST] = APAGADO;
	// 		port_slots_default[PICTOGRAMA_3_OFST] = APAGADO;
			
	
	//
	// 		port_slots_write[PICTOGRAMA_4_OFST] = ENCENDIDO;
	// 		port_slots_default[PICTOGRAMA_4_OFST] = ENCENDIDO;
	//
	// 		break;
	//
	// 		case 0x03:
	// 		port_slots_write[SOLENOIDE_S_OFST] = APAGADO;
	// 		port_slots_default[SOLENOIDE_S_OFST] = APAGADO;
	//
	// 		port_slots_write[SOLENOIDE_I_OFST] = APAGADO;
	// 		port_slots_default[SOLENOIDE_I_OFST] = APAGADO;
	//
	// 		port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
	// 		port_slots_default[APERTURA_DERECHA_GU_OFST] = APAGADO;
	//
	// 		port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
	// 		port_slots_default[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
	//
	// 		port_slots_write[EMERGENCIA_PMR_GU_OFST] = ENCENDIDO;
	// 		port_slots_default[EMERGENCIA_PMR_GU_OFST] = ENCENDIDO;
	//
	// 		break;
	//
	// 		case 0x04:
	// 		/* Your code here */
	// 		break;
	//
	//
	// 		default:
	// 		/* Your code here */
	// 		break;
	// 	}





   port_slots_read[ESCENARIO] = escenario_v;




	while (1) {
		delay_ms(1);
		
		if (char_timeout_RS485 == 0)
		{
			if (rx_idx_RS485 != 0)
			{
				error_code = ERROR_UNDER;
				not_ack_RS485();
			}
			rx_idx_RS485 = 0;
		}
		else
		{
			char_timeout_RS485--;
		}
		
		if (char_timeout_RS232 == 0)
		{
			//if (rx_idx_RS232 != 0)
			//{
			//	error_code = ERROR_UNDER;
			//	not_ack_RS232();
			//}
			rx_idx_RS232 = 0;
		}
		else
		{
			char_timeout_RS232--;
		}
		
		//	---------------------------------------------------------------------------------------
		//	Capturador Eltra
		/////////////////////////////////////
		//
		
		if (port_slots_write[INSERT_CARD_ELTRA] == ENCENDIDO)
		{
			eltra_polling_timeout = 350;
			inquire_eltra = 1;
			port_slots_write[INSERT_CARD_ELTRA] = IGNORAR;
			sendCmdEltra(CMD_insertCard_eltra, cmd_null, 0);
		}
		else if (port_slots_write[SEND_SENSOR_STATUS_ELTRA] == ENCENDIDO)
		{
			inquire_eltra = 1;
			eltra_polling_timeout = 120;
			port_slots_write[SEND_SENSOR_STATUS_ELTRA] = IGNORAR;
			sendCmdEltra(CMD_sendSensorStatus_eltra, cmd_null, 0);
		}
		else if (port_slots_write[INIT_MODULE_ELTRA] == ENCENDIDO)
		{
			eltra_polling_timeout = 350;
			inquire_eltra = 1;
			init_eltra = 0xFF;
			port_slots_write[INIT_MODULE_ELTRA] = IGNORAR;
			sendCmdEltra(CMD_initModule_eltra, cmd_null, 0);
		}
		else if (port_slots_write[EJECT_CARD_ELTRA] == ENCENDIDO)
		{
			eltra_polling_timeout = 350;
			inquire_eltra = 1;
			port_slots_write[EJECT_CARD_ELTRA] = IGNORAR;
			sendCmdEltra(CMD_ejectCard_eltra, cmd_null, 0);
		}
		else if (port_slots_write[CAPTURE_CARD_ELTRA] == ENCENDIDO)
		{
			eltra_polling_timeout = 350;
			inquire_eltra = 1;
			port_slots_write[CAPTURE_CARD_ELTRA] = IGNORAR;
			sendCmdEltra(CMD_captureCard_eltra, cmd_null, 0);
		}
		else if (port_slots_write[INQUIRE_ELTRA] == ENCENDIDO)
		{
			eltra_polling_timeout = 350;
			init_eltra = 0xFF;
			port_slots_write[INQUIRE_ELTRA] = IGNORAR;
			sendInquireEltra();
		}


		///// send Polling eltra
		
		if (init_eltra == 0xFF && eltra_polling_timeout == 0) {
			eltra_polling_timeout = 120;
			if (inquire_eltra == 1) {
				sendInquireEltra();
			} 
			else {
				inquire_eltra = 1;
				sendCmdEltra(CMD_sendSensorStatus_eltra, cmd_null, 0);				
			}
			
		} else {
			if (eltra_polling_timeout > 0) {
				eltra_polling_timeout--;
			} else if (eltra_polling_timeout < 0) {
				eltra_polling_timeout = 300;
			}
		}


		////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////

		if (port_slots_write[MODO_BIT_1_OFST] == ENCENDIDO)
		{
			port_slots_write[MODO_BIT_1_OFST] = IGNORAR;
			MODO_BIT_1_1;
		}
		if (port_slots_write[MODO_BIT_1_OFST] == APAGADO)
		{
			port_slots_write[MODO_BIT_1_OFST] = IGNORAR;
			MODO_BIT_1_0;
		}
		
		if (port_slots_write[MODO_BIT_2_OFST] == ENCENDIDO)
		{
			port_slots_write[MODO_BIT_2_OFST] = IGNORAR;
			MODO_BIT_2_1;
		}
		if (port_slots_write[MODO_BIT_2_OFST] == APAGADO)
		{
			port_slots_write[MODO_BIT_2_OFST] = IGNORAR;
			MODO_BIT_2_0;
		}
		if (port_slots_write[MODO_BIT_3_OFST] == ENCENDIDO)
		{
			port_slots_write[MODO_BIT_3_OFST] = IGNORAR;
			MODO_BIT_3_1;
		}
		if (port_slots_write[MODO_BIT_3_OFST] == APAGADO)
		{
			port_slots_write[MODO_BIT_3_OFST] = IGNORAR;
			MODO_BIT_3_0;
		}
		
		if (port_slots_write[MODO_BIT_4_OFST] == ENCENDIDO)
		{
			port_slots_write[MODO_BIT_4_OFST] = IGNORAR;
			MODO_BIT_4_1;
		}
		if (port_slots_write[MODO_BIT_4_OFST] == APAGADO)
		{
			port_slots_write[MODO_BIT_4_OFST] = IGNORAR;
			MODO_BIT_4_0;
		}
		//	---------------------------------------------------------------------------
		if ((port_slots_write[PASO_MODO_A_ANT_OFST] == ENCENDIDO) || (port_slots_write[PASO_MODO_A_NEW_OFST] == ENCENDIDO))
		{
			port_slots_write[PASO_MODO_A_ANT_OFST] = IGNORAR;
			port_slots_write[PASO_MODO_A_NEW_OFST] = IGNORAR;
			PASO_MODO_A_ON;
		}
		if ((port_slots_write[PASO_MODO_A_ANT_OFST] == APAGADO) || (port_slots_write[PASO_MODO_A_NEW_OFST] == APAGADO))
		{
			port_slots_write[PASO_MODO_A_ANT_OFST] = IGNORAR;
			port_slots_write[PASO_MODO_A_NEW_OFST] = IGNORAR;
			PASO_MODO_A_OFF;
		}
		
		if ((port_slots_write[PASO_MODO_B_ANT_OFST] == ENCENDIDO) || (port_slots_write[PASO_MODO_B_NEW_OFST] == ENCENDIDO))
		{
			port_slots_write[PASO_MODO_B_ANT_OFST] = IGNORAR;
			port_slots_write[PASO_MODO_B_NEW_OFST] = IGNORAR;
			PASO_MODO_B_ON;
		}
		if ((port_slots_write[PASO_MODO_B_ANT_OFST] == APAGADO) || (port_slots_write[PASO_MODO_B_NEW_OFST] == APAGADO))
		{
			port_slots_write[PASO_MODO_B_ANT_OFST] = IGNORAR;
			port_slots_write[PASO_MODO_B_NEW_OFST] = IGNORAR;
			PASO_MODO_B_OFF;
		}

		if (port_slots_write[EMERGENCIA_PASO_OFST] == ENCENDIDO)
		{
			port_slots_write[EMERGENCIA_PASO_OFST] = IGNORAR;
			EMERGENCIA_PASO_ON;
		}
		if (port_slots_write[EMERGENCIA_PASO_OFST] == APAGADO)
		{
			port_slots_write[EMERGENCIA_PASO_OFST] = IGNORAR;
			EMERGENCIA_PASO_OFF;
		}
		//	------------------------------------------------------------------------
		if (port_slots_write[PICTOGRAMA_1_OFST] == ENCENDIDO)
		{
			port_slots_write[PICTOGRAMA_1_OFST] = IGNORAR;
			PICTOGRAMA_1_ON;
		}
		if (port_slots_write[PICTOGRAMA_1_OFST] == APAGADO)
		{
			port_slots_write[PICTOGRAMA_1_OFST] = IGNORAR;
			PICTOGRAMA_1_OFF;
		}

		if (port_slots_write[PICTOGRAMA_2_OFST] == ENCENDIDO)
		{
			port_slots_write[PICTOGRAMA_2_OFST] = IGNORAR;
			PICTOGRAMA_2_ON;
		}
		if (port_slots_write[PICTOGRAMA_2_OFST] == APAGADO)
		{
			port_slots_write[PICTOGRAMA_2_OFST] = IGNORAR;
			PICTOGRAMA_2_OFF;
		}
		
		if (port_slots_write[PICTOGRAMA_3_OFST] == ENCENDIDO)
		{
			port_slots_write[PICTOGRAMA_3_OFST] = IGNORAR;
			PICTOGRAMA_3_ON;
		}
		if (port_slots_write[PICTOGRAMA_3_OFST] == APAGADO)
		{
			port_slots_write[PICTOGRAMA_3_OFST] = IGNORAR;
			PICTOGRAMA_3_OFF;
		}
		
		if (port_slots_write[PICTOGRAMA_4_OFST] == ENCENDIDO)
		{
			port_slots_write[PICTOGRAMA_4_OFST] = IGNORAR;
			PICTOGRAMA_4_ON;
		}
		if (port_slots_write[PICTOGRAMA_4_OFST] == APAGADO)
		{
			port_slots_write[PICTOGRAMA_4_OFST] = IGNORAR;
			PICTOGRAMA_4_OFF;
		}
		//	---------------------------------------------------------------------
		if ((port_slots_write[APERTURA_DERECHA_AS_OFST] == ENCENDIDO) || (port_slots_write[APERTURA_DERECHA_GU_OFST] == ENCENDIDO))
		{
			port_slots_write[APERTURA_DERECHA_AS_OFST] = IGNORAR;
			port_slots_write[APERTURA_DERECHA_GU_OFST] = IGNORAR;
			APERTURA_DERECHA_ON;
		}
		if ((port_slots_write[APERTURA_DERECHA_AS_OFST] == APAGADO) || (port_slots_write[APERTURA_DERECHA_GU_OFST] == APAGADO))
		{
			port_slots_write[APERTURA_DERECHA_AS_OFST] = IGNORAR;
			port_slots_write[APERTURA_DERECHA_GU_OFST] = IGNORAR;
			APERTURA_DERECHA_OFF;
		}

		if ((port_slots_write[APERTURA_IZQUIERDA_AS_OFST] == ENCENDIDO) || (port_slots_write[APERTURA_IZQUIERDA_GU_OFST] == ENCENDIDO))
		{
			port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = IGNORAR;
			port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = IGNORAR;
			APERTURA_IZQUIERDA_ON;
		}
		if ((port_slots_write[APERTURA_IZQUIERDA_AS_OFST] == APAGADO) || (port_slots_write[APERTURA_IZQUIERDA_GU_OFST] == APAGADO))
		{
			port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = IGNORAR;
			port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = IGNORAR;
			APERTURA_IZQUIERDA_OFF;
		}


		if ((port_slots_write[EMERGENCIA_PMR_AS_OFST] == ENCENDIDO) || (port_slots_write[EMERGENCIA_PMR_GU_OFST] == ENCENDIDO))
		{
			port_slots_write[EMERGENCIA_PMR_AS_OFST] = IGNORAR;
			port_slots_write[EMERGENCIA_PMR_GU_OFST] = IGNORAR;
			EMERGENCIA_PMR_ON;
		}

		if ((port_slots_write[EMERGENCIA_PMR_AS_OFST] == APAGADO) || (port_slots_write[EMERGENCIA_PMR_GU_OFST] == APAGADO))
		{
			port_slots_write[EMERGENCIA_PMR_AS_OFST] = IGNORAR;
			port_slots_write[EMERGENCIA_PMR_GU_OFST] = IGNORAR;
			EMERGENCIA_PMR_OFF;
		}
		
		// ---------------------------------------------------------------------
		
		if (port_slots_write[SOLENOIDE_I_OFST] == ENCENDIDO)
		{
			port_slots_write[SOLENOIDE_I_OFST] = IGNORAR;
			SOLENOIDE_I_ON;
		}
		if (port_slots_write[SOLENOIDE_I_OFST] == APAGADO)
		{
			port_slots_write[SOLENOIDE_I_OFST] = IGNORAR;
			SOLENOIDE_I_OFF;
		}
		if (port_slots_write[SOLENOIDE_S_OFST] == ENCENDIDO)
		{
			port_slots_write[SOLENOIDE_S_OFST] = IGNORAR;
			SOLENOIDE_S_ON;
		}
		if (port_slots_write[SOLENOIDE_S_OFST] == APAGADO)
		{
			port_slots_write[SOLENOIDE_S_OFST] = IGNORAR;
			SOLENOIDE_S_OFF;
		}
		
		
		
		
		//	-------------------------------------------------------------------------------------------
		//	-------------------------------------------------------------------------------------------
		//	-------------------------------------------------------------------------------------------
		//	-------------------------------------------------------------------------------------------


		//port_slots_read[ESCENARIO] = escenario_v;
		
		if(gpio_pin_is_low(PASO_MODO_A))
		{
			port_slots_read[PASO_MODO_A_ANT_OFST] = APAGADO;
			port_slots_read[PASO_MODO_A_NEW_OFST] = APAGADO;
		}
		else
		{
			port_slots_read[PASO_MODO_A_ANT_OFST] = ENCENDIDO;
			port_slots_read[PASO_MODO_A_NEW_OFST] = ENCENDIDO;
		}
		
		if(gpio_pin_is_low(PASO_MODO_B))
		{
			port_slots_read[PASO_MODO_B_ANT_OFST] = APAGADO;
			port_slots_read[PASO_MODO_B_NEW_OFST] = APAGADO;
		}
		else
		{
			port_slots_read[PASO_MODO_B_ANT_OFST] = ENCENDIDO;
			port_slots_read[PASO_MODO_B_NEW_OFST] = ENCENDIDO;
		}
		
		if(gpio_pin_is_low(PICTOGRAMA_1))
		{
			port_slots_read[PICTOGRAMA_1_OFST] = APAGADO;
		}
		else
		{
			port_slots_read[PICTOGRAMA_1_OFST] = ENCENDIDO;
		}
		
		if(gpio_pin_is_low(PICTOGRAMA_2))
		{
			port_slots_read[PICTOGRAMA_2_OFST] = APAGADO;
		}
		else
		{
			port_slots_read[PICTOGRAMA_2_OFST] = ENCENDIDO;
		}
		
		if(gpio_pin_is_low(PICTOGRAMA_3))
		{
			port_slots_read[PICTOGRAMA_3_OFST] = APAGADO;
		}
		else
		{
			port_slots_read[PICTOGRAMA_3_OFST] = ENCENDIDO;
		}
		
		if(gpio_pin_is_low(PICTOGRAMA_4))
		{
			port_slots_read[PICTOGRAMA_4_OFST] = APAGADO;
		}
		else
		{
			port_slots_read[PICTOGRAMA_4_OFST] = ENCENDIDO;
		}
		//	---------------------------------------------------------------------------------
		if(gpio_pin_is_low(MODO_BIT_1))
		{
			port_slots_read[MODO_BIT_1_OFST] = APAGADO;
		}
		else
		{
			port_slots_read[MODO_BIT_1_OFST] = ENCENDIDO;
		}
		
		if(gpio_pin_is_low(MODO_BIT_2))
		{
			port_slots_read[MODO_BIT_2_OFST] = APAGADO;
		}
		else
		{
			port_slots_read[MODO_BIT_2_OFST] = ENCENDIDO;
		}
		
		if(gpio_pin_is_low(MODO_BIT_3))
		{
			port_slots_read[MODO_BIT_3_OFST] = APAGADO;
		}
		else
		{
			port_slots_read[MODO_BIT_3_OFST] = ENCENDIDO;
		}
		
		if(gpio_pin_is_low(MODO_BIT_4))
		{
			port_slots_read[MODO_BIT_4_OFST] = APAGADO;
		}
		else
		{
			port_slots_read[MODO_BIT_4_OFST] = ENCENDIDO;
		}

		if(gpio_pin_is_low(EMERGENCIA_PASO))
		{
			port_slots_read[EMERGENCIA_PASO_OFST] = APAGADO;
		}
		else
		{
			port_slots_read[EMERGENCIA_PASO_OFST] = ENCENDIDO;
		}


		//	-----------------------------------------------------------------------------------------
		
		if(gpio_pin_is_low(SOLENOIDE_S))
		{
			port_slots_read[SOLENOIDE_S_OFST] = APAGADO;
		}
		else
		{
			port_slots_read[SOLENOIDE_S_OFST] = ENCENDIDO;
		}
		
		if(gpio_pin_is_low(SOLENOIDE_I))
		{
			port_slots_read[SOLENOIDE_I_OFST] = APAGADO;
		}
		else
		{
			port_slots_read[SOLENOIDE_I_OFST] = ENCENDIDO;
		}
		
		//	---------------------------------------------------------------------------

		if(gpio_pin_is_low(APERTURA_DERECHA))
		{
			port_slots_read[APERTURA_DERECHA_AS_OFST] = APAGADO;
			port_slots_read[APERTURA_DERECHA_GU_OFST] = APAGADO;
		}
		else
		{
			port_slots_read[APERTURA_DERECHA_AS_OFST] = ENCENDIDO;
			port_slots_read[APERTURA_DERECHA_GU_OFST] = ENCENDIDO;
		}
		
		if(gpio_pin_is_low(APERTURA_IZQUIERDA))
		{
			port_slots_read[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
			port_slots_read[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		}
		else
		{
			port_slots_read[APERTURA_IZQUIERDA_AS_OFST] = ENCENDIDO;
			port_slots_read[APERTURA_IZQUIERDA_GU_OFST] = ENCENDIDO;
		}
		
		if(gpio_pin_is_low(EMERGENCIA_PMR))
		{
			port_slots_read[EMERGENCIA_PMR_AS_OFST] = APAGADO;
			port_slots_read[EMERGENCIA_PMR_GU_OFST] = APAGADO;
		}
		else
		{
			port_slots_read[EMERGENCIA_PMR_GU_OFST] = ENCENDIDO;
			port_slots_read[EMERGENCIA_PMR_GU_OFST] = ENCENDIDO;
		}
		
		//	----------------------------------------------------------------------------------------
		if(confirmacion_a_timeout <= 100)
		{
			confirmacion_a_timeout += 1;
		}
		
		if(gpio_pin_is_high(CONFIRMACION_A))
		{
			//PASO_MODO_A_ON;			// Sin kaba Laboratorio
			confirmacion_a_timeout = 0;
		}
		
		if(confirmacion_a_timeout == 100)
		{
			port_slots_read[CONFIRMACION_A_ANT_OFST] += 1;
			port_slots_read[CONFIRMACION_A_NEW_OFST] += 1;
			port_slots_write[PASO_MODO_A_ANT_OFST] = port_slots_default[PASO_MODO_A_ANT_OFST];  //Normal sin Kaba Laboratorio
			port_slots_write[PASO_MODO_A_NEW_OFST] = port_slots_default[PASO_MODO_A_NEW_OFST];	//Normal sin Kaba labotatorio 
			port_slots_write[PICTOGRAMA_1_OFST] = port_slots_default[PICTOGRAMA_1_OFST];
			port_slots_write[PICTOGRAMA_2_OFST] = port_slots_default[PICTOGRAMA_2_OFST];
			port_slots_write[PICTOGRAMA_3_OFST] = port_slots_default[PICTOGRAMA_3_OFST];
			port_slots_write[PICTOGRAMA_4_OFST] = port_slots_default[PICTOGRAMA_4_OFST];
		}

		
		if(confirmacion_b_timeout <= 100)
		{
			confirmacion_b_timeout += 1;
		}

		if(gpio_pin_is_high(CONFIRMACION_B))
		{
			confirmacion_b_timeout = 0;
		}
		
		if(confirmacion_b_timeout == 100)
		{
			//PASO_MODO_A_OFF; // Sin kaba Laboratorio
			port_slots_read[CONFIRMACION_B_ANT_OFST] += 1;
			port_slots_read[CONFIRMACION_B_NEW_OFST] += 1;
		}



		// 		if(gpio_pin_is_low(CONFIRMACION_A))
		// 		{
		// 			port_slots_read[CONFIRMACION_A_ANT_OFST] = APAGADO;
		// 			port_slots_read[CONFIRMACION_A_NEW_OFST] = APAGADO;
		// 		}
		// 		else
		// 		{
		// 			port_slots_read[CONFIRMACION_A_ANT_OFST] = ENCENDIDO;
		// 			port_slots_read[CONFIRMACION_A_NEW_OFST] = ENCENDIDO;
		// 		}
		//
		// 		if(gpio_pin_is_low(CONFIRMACION_B))
		// 		{
		// 			port_slots_read[CONFIRMACION_B_ANT_OFST] = APAGADO;
		// 			port_slots_read[CONFIRMACION_B_NEW_OFST] = APAGADO;
		// 		}
		// 		else
		// 		{
		// 			port_slots_read[CONFIRMACION_B_ANT_OFST] = ENCENDIDO;
		// 			port_slots_read[CONFIRMACION_B_NEW_OFST] = ENCENDIDO;
		// 		}
		
		//	----------------------------------------------------------------------------------------------------

		

		
  if(gpio_pin_is_low(SENSOR_S))
  {
	  port_slots_read[SENSOR_S_OFST] = APAGADO;
	  LED_SENS_S_OFF;
  }
  else
  {
	  port_slots_read[SENSOR_S_OFST] = ENCENDIDO;
	  LED_SENS_S_ON;
  }

  if(gpio_pin_is_low(SENSOR_I))
  {
	  port_slots_read[SENSOR_I_OFST] = APAGADO;
	  LED_SENS_I_OFF;
  }
  else
  {
	  port_slots_read[SENSOR_I_OFST] = ENCENDIDO;
	  LED_SENS_I_ON;
  }
		









		//	---------------------------------------------------------------------------------------
		
		if(abierta_pmr_timeout <= 100)
		{
			abierta_pmr_timeout += 1;
		}
		
		if(gpio_pin_is_high(ABIERTA))
		{
			abierta_pmr_timeout = 0;
		}
		
		if(abierta_pmr_timeout == 100)
		{
			port_slots_read[ABIERTA_AS_OFST] += 1;
			port_slots_read[ABIERTA_GU_OFST] += 1;
			port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
			port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
			port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
			port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		}
		
		if(cerrada_pmr_timeout <= 100)
		{
			cerrada_pmr_timeout += 1;
		}
		
		if(gpio_pin_is_high(CERRADA))
		{
			cerrada_pmr_timeout = 0;
		}
	
		if(cerrada_pmr_timeout == 100)
		{
			port_slots_read[CERRADA_AS_OFST] += 1;
			port_slots_read[CERRADA_GU_OFST] += 1;
			//port_slots_write[APERTURA_DERECHA_AS_OFST] = port_slots_default[APERTURA_DERECHA_AS_OFST];
			//port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = port_slots_default[APERTURA_IZQUIERDA_AS_OFST];
			//port_slots_write[APERTURA_DERECHA_AS_OFST] = APAGADO;
			//port_slots_write[APERTURA_IZQUIERDA_AS_OFST] = APAGADO;
			//port_slots_write[APERTURA_DERECHA_GU_OFST] = APAGADO;
			//port_slots_write[APERTURA_IZQUIERDA_GU_OFST] = APAGADO;
		}

		if(gpio_pin_is_low(ALARMADA))
		{
			port_slots_read[ALARMADA_AS_OFST] = APAGADO;
			port_slots_read[ALARMADA_GU_OFST] = APAGADO;
		}
		else
		{
			port_slots_read[ALARMADA_AS_OFST] = ENCENDIDO;
			port_slots_read[ALARMADA_GU_OFST] = ENCENDIDO;
		}

		
		//	-------------------------------------------------------------------------------------------
		

		if(gpio_pin_is_low(EMERGENCIA_PPL))
		{
			port_slots_read[EMERGENCIA_PPL_OFST] = APAGADO;
			EMERGENCIA_PASO_OFF;
			EMERGENCIA_PMR_OFF;
		}
		else
		{
			port_slots_read[EMERGENCIA_PPL_OFST] = ENCENDIDO;
			EMERGENCIA_PASO_ON;
			EMERGENCIA_PMR_ON;
		}
	}
}
