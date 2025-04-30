/**
 * \file
 *
 * \brief Board configuration.
 *
 * Copyright (c) 2011 - 2014 Atmel Corporation. All rights reserved.
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

#ifndef CONF_BOARD_H_INCLUDED
#define CONF_BOARD_H_INCLUDED

#define BOARD_FREQ_SLCK_XTAL		12000000


#define	ESCENARIO					0X01

//	TQ paso / confirm
#define NA 							PIO_PA15_IDX
#define	NA_OFST						0xFE
#define EMERGENCIA_PASO				PIO_PA14_IDX
#define	EMERGENCIA_PASO_OFST		0X20
#define PASO_MODO_A					PIO_PA24_IDX
#define PASO_MODO_A_ANT_OFST		0x10
#define PASO_MODO_A_NEW_OFST		0x1E
#define PASO_MODO_B					PIO_PA13_IDX
#define PASO_MODO_B_ANT_OFST		0x11
#define PASO_MODO_B_NEW_OFST		0X1F
#define CONFIRMACION_A				PIO_PA16_IDX
#define CONFIRMACION_A_ANT_OFST		0X80
#define CONFIRMACION_A_NEW_OFST		0X82
#define CONFIRMACION_B				PIO_PA20_IDX
#define CONFIRMACION_B_ANT_OFST		0X81
#define CONFIRMACION_B_NEW_OFST		0X83

#define EMERGENCIA_PASO_ON			pio_set_pin_high(EMERGENCIA_PASO)
#define EMERGENCIA_PASO_OFF			pio_set_pin_low(EMERGENCIA_PASO)
#define PASO_MODO_A_ON				pio_set_pin_high(PASO_MODO_A)
#define PASO_MODO_A_OFF				pio_set_pin_low(PASO_MODO_A)
#define PASO_MODO_B_ON				pio_set_pin_high(PASO_MODO_B)
#define PASO_MODO_B_OFF				pio_set_pin_low(PASO_MODO_B)

//	Emergencia ppl
#define EMERGENCIA_PPL				PIO_PB14_IDX
#define EMERGENCIA_PPL_OFST			0X8C

// TQ config / modo
#define MODO_BIT_1					PIO_PA11_IDX
#define MODO_BIT_1_OFST				0x1A
#define MODO_BIT_2					PIO_PA12_IDX
#define MODO_BIT_2_OFST				0x1B
#define MODO_BIT_3					PIO_PA26_IDX
#define MODO_BIT_3_OFST				0x1C
#define MODO_BIT_4					PIO_PA25_IDX
#define MODO_BIT_4_OFST				0x1D

#define MODO_BIT_1_0				pio_set_pin_low(MODO_BIT_1)
#define MODO_BIT_1_1				pio_set_pin_high(MODO_BIT_1)
#define MODO_BIT_2_0				pio_set_pin_low(MODO_BIT_2)
#define MODO_BIT_2_1				pio_set_pin_high(MODO_BIT_2)
#define MODO_BIT_3_0				pio_set_pin_low(MODO_BIT_3)
#define MODO_BIT_3_1				pio_set_pin_high(MODO_BIT_3)
#define MODO_BIT_4_0				pio_set_pin_low(MODO_BIT_4)
#define MODO_BIT_4_1				pio_set_pin_high(MODO_BIT_4)

// Pictogramas
#define PICTOGRAMA_1				PIO_PA6_IDX
#define PICTOGRAMA_1_OFST			0x12
#define PICTOGRAMA_2 				PIO_PA5_IDX
#define PICTOGRAMA_2_OFST			0x13
#define PICTOGRAMA_3 				PIO_PA7_IDX
#define PICTOGRAMA_3_OFST			0x14
#define PICTOGRAMA_4 				PIO_PA8_IDX
#define PICTOGRAMA_4_OFST			0x15

#define PICTOGRAMA_1_ON				pio_set_pin_high(PICTOGRAMA_1)
#define PICTOGRAMA_1_OFF			pio_set_pin_low(PICTOGRAMA_1)
#define PICTOGRAMA_2_ON				pio_set_pin_high(PICTOGRAMA_2)
#define PICTOGRAMA_2_OFF			pio_set_pin_low(PICTOGRAMA_2)
#define PICTOGRAMA_3_ON				pio_set_pin_high(PICTOGRAMA_3)
#define PICTOGRAMA_3_OFF			pio_set_pin_low(PICTOGRAMA_3)
#define PICTOGRAMA_4_ON				pio_set_pin_high(PICTOGRAMA_4)
#define PICTOGRAMA_4_OFF			pio_set_pin_low(PICTOGRAMA_4)

// PMR
#define ABIERTA 					PIO_PA4_IDX
#define ABIERTA_GU_OFST				0X86
#define ABIERTA_AS_OFST				0X89
#define CERRADA 					PIO_PA27_IDX
#define CERRADA_GU_OFST				0X87
#define CERRADA_AS_OFST				0X8A
#define ALARMADA					PIO_PA28_IDX
#define ALARMADA_GU_OFST			0X88
#define ALARMADA_AS_OFST			0X8b
#define APERTURA_DERECHA			PIO_PA29_IDX
#define APERTURA_DERECHA_GU_OFST	0X2A
#define APERTURA_DERECHA_AS_OFST	0X30
#define APERTURA_IZQUIERDA			PIO_PA30_IDX
#define APERTURA_IZQUIERDA_GU_OFST	0X2B
#define APERTURA_IZQUIERDA_AS_OFST	0X31
#define EMERGENCIA_PMR				PIO_PA3_IDX
#define EMERGENCIA_PMR_GU_OFST		0X2C
#define EMERGENCIA_PMR_AS_OFST		0X32

#define APERTURA_DERECHA_ON		pio_set_pin_high(APERTURA_DERECHA)			//	Version final
//	#define APERTURA_DERECHA_ON			pio_set_pin_low(APERTURA_DERECHA)		//	Prototipo
#define APERTURA_DERECHA_OFF		pio_set_pin_low(APERTURA_DERECHA)			//	Version final
//	#define APERTURA_DERECHA_OFF		pio_set_pin_high(APERTURA_DERECHA)		//	Prototipo
#define APERTURA_IZQUIERDA_ON		pio_set_pin_high(APERTURA_IZQUIERDA)			//	Version final
//	#define APERTURA_IZQUIERDA_ON		pio_set_pin_low(APERTURA_IZQUIERDA)		//	Prototipo
#define APERTURA_IZQUIERDA_OFF		pio_set_pin_low(APERTURA_IZQUIERDA)			//	Version final
//	#define APERTURA_IZQUIERDA_OFF		pio_set_pin_high(APERTURA_IZQUIERDA)	//	Prototipo

#define EMERGENCIA_PMR_ON			pio_set_pin_high(EMERGENCIA_PMR)
#define EMERGENCIA_PMR_OFF			pio_set_pin_low(EMERGENCIA_PMR)

// Capturadora
#define SENSOR_I					PIO_PB13_IDX
#define SENSOR_I_OFST				0X85
#define SENSOR_S					PIO_PB11_IDX
#define SENSOR_S_OFST				0X84
#define SOLENOIDE_I					PIO_PB12_IDX
#define SOLENOIDE_I_OFST			0X27
#define SOLENOIDE_S					PIO_PB10_IDX
#define SOLENOIDE_S_OFST			0X26

#define SOLENOIDE_I_ON				pio_set_pin_high(SOLENOIDE_I)
#define SOLENOIDE_I_OFF				pio_set_pin_low(SOLENOIDE_I)
#define SOLENOIDE_S_ON				pio_set_pin_high(SOLENOIDE_S)
#define SOLENOIDE_S_OFF				pio_set_pin_low(SOLENOIDE_S)

//	Dip switch dev_id
#define BIT_0						PIO_PA21_IDX
#define BIT_1						PIO_PA19_IDX
#define BIT_2						PIO_PA22_IDX
#define BIT_3 						PIO_PA23_IDX

//	Dip switch escenario_v
#define BIT_SC_0					PIO_PB4_IDX
#define BIT_SC_1					PIO_PB5_IDX
#define BIT_SC_2					PIO_PB7_IDX
#define BIT_SC_3 					PIO_PB6_IDX

//	LEDs
#define LED_SENS_S					PIO_PB0_IDX
#define LED_SENS_I					PIO_PB1_IDX
#define LED_3						PIO_PA17_IDX
#define LED_4						PIO_PA18_IDX

#define LED_SENS_S_ON				pio_set_pin_high(LED_SENS_S)
#define LED_SENS_S_OFF				pio_set_pin_low(LED_SENS_S)
#define LED_SENS_I_ON				pio_set_pin_high(LED_SENS_I)
#define LED_SENS_I_OFF				pio_set_pin_low(LED_SENS_I)

#define LED_3_ON					pio_set_pin_high(LED_3)
#define LED_3_OFF					pio_set_pin_low(LED_3)
#define LED_4_ON					pio_set_pin_high(LED_4)
#define LED_4_OFF					pio_set_pin_low(LED_4)

//	Answer
#define	VALIDADOR_ID	
#define	ACK							0x06
#define	NACK						0x15
#define	ERROR_CS					0x01
#define	ERROR_						0x02

//	UART X
#define BAUDRRATE					57600

/** Enable Com Port. */
#define CONF_BOARD_UART_CONSOLE

/** Usart Hw ID used by the console (UART0). */
#define CONSOLE_UART_ID          ID_UART0

#endif /* CONF_BOARD_H_INCLUDED */
