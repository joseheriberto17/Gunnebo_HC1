/**
 * \file
 *
 * Copyright (c) 2014 Atmel Corporation. All rights reserved.
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

#ifndef _SAM3S_USART_COMPONENT_
#define _SAM3S_USART_COMPONENT_

/* ============================================================================= */
/**  SOFTWARE API DEFINITION FOR Universal Synchronous Asynchronous Receiver Transmitter */
/* ============================================================================= */
/** \addtogroup SAM3S_USART Universal Synchronous Asynchronous Receiver Transmitter */
/*@{*/

#if !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
/** \brief Usart hardware registers */
typedef struct {
  WoReg US_CR;         /**< \brief (Usart Offset: 0x0000) Control Register */
  RwReg US_MR;         /**< \brief (Usart Offset: 0x0004) Mode Register */
  WoReg US_IER;        /**< \brief (Usart Offset: 0x0008) Interrupt Enable Register */
  WoReg US_IDR;        /**< \brief (Usart Offset: 0x000C) Interrupt Disable Register */
  RoReg US_IMR;        /**< \brief (Usart Offset: 0x0010) Interrupt Mask Register */
  RoReg US_CSR;        /**< \brief (Usart Offset: 0x0014) Channel Status Register */
  RoReg US_RHR;        /**< \brief (Usart Offset: 0x0018) Receiver Holding Register */
  WoReg US_THR;        /**< \brief (Usart Offset: 0x001C) Transmitter Holding Register */
  RwReg US_BRGR;       /**< \brief (Usart Offset: 0x0020) Baud Rate Generator Register */
  RwReg US_RTOR;       /**< \brief (Usart Offset: 0x0024) Receiver Time-out Register */
  RwReg US_TTGR;       /**< \brief (Usart Offset: 0x0028) Transmitter Timeguard Register */
  RoReg Reserved1[5];
  RwReg US_FIDI;       /**< \brief (Usart Offset: 0x0040) FI DI Ratio Register */
  RoReg US_NER;        /**< \brief (Usart Offset: 0x0044) Number of Errors Register */
  RoReg Reserved2[1];
  RwReg US_IF;         /**< \brief (Usart Offset: 0x004C) IrDA Filter Register */
  RwReg US_MAN;        /**< \brief (Usart Offset: 0x0050) Manchester Encoder Decoder Register */
  RoReg Reserved3[36];
  RwReg US_WPMR;       /**< \brief (Usart Offset: 0xE4) Write Protect Mode Register */
  RoReg US_WPSR;       /**< \brief (Usart Offset: 0xE8) Write Protect Status Register */
  RoReg Reserved4[4];
  RoReg US_VERSION;    /**< \brief (Usart Offset: 0xFC) Version Register */
  RwReg US_RPR;        /**< \brief (Usart Offset: 0x100) Receive Pointer Register */
  RwReg US_RCR;        /**< \brief (Usart Offset: 0x104) Receive Counter Register */
  RwReg US_TPR;        /**< \brief (Usart Offset: 0x108) Transmit Pointer Register */
  RwReg US_TCR;        /**< \brief (Usart Offset: 0x10C) Transmit Counter Register */
  RwReg US_RNPR;       /**< \brief (Usart Offset: 0x110) Receive Next Pointer Register */
  RwReg US_RNCR;       /**< \brief (Usart Offset: 0x114) Receive Next Counter Register */
  RwReg US_TNPR;       /**< \brief (Usart Offset: 0x118) Transmit Next Pointer Register */
  RwReg US_TNCR;       /**< \brief (Usart Offset: 0x11C) Transmit Next Counter Register */
  WoReg US_PTCR;       /**< \brief (Usart Offset: 0x120) Transfer Control Register */
  RoReg US_PTSR;       /**< \brief (Usart Offset: 0x124) Transfer Status Register */
} Usart;
#endif /* !(defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */
/* -------- US_CR : (USART Offset: 0x0000) Control Register -------- */
#define US_CR_RSTRX (0x1u << 2) /**< \brief (US_CR) Reset Receiver */
#define US_CR_RSTTX (0x1u << 3) /**< \brief (US_CR) Reset Transmitter */
#define US_CR_RXEN (0x1u << 4) /**< \brief (US_CR) Receiver Enable */
#define US_CR_RXDIS (0x1u << 5) /**< \brief (US_CR) Receiver Disable */
#define US_CR_TXEN (0x1u << 6) /**< \brief (US_CR) Transmitter Enable */
#define US_CR_TXDIS (0x1u << 7) /**< \brief (US_CR) Transmitter Disable */
#define US_CR_RSTSTA (0x1u << 8) /**< \brief (US_CR) Reset Status Bits */
#define US_CR_STTBRK (0x1u << 9) /**< \brief (US_CR) Start Break */
#define US_CR_STPBRK (0x1u << 10) /**< \brief (US_CR) Stop Break */
#define US_CR_STTTO (0x1u << 11) /**< \brief (US_CR) Start Time-out */
#define US_CR_SENDA (0x1u << 12) /**< \brief (US_CR) Send Address */
#define US_CR_RSTIT (0x1u << 13) /**< \brief (US_CR) Reset Iterations */
#define US_CR_RSTNACK (0x1u << 14) /**< \brief (US_CR) Reset Non Acknowledge */
#define US_CR_RETTO (0x1u << 15) /**< \brief (US_CR) Rearm Time-out */
#define US_CR_DTREN (0x1u << 16) /**< \brief (US_CR) Data Terminal Ready Enable */
#define US_CR_DTRDIS (0x1u << 17) /**< \brief (US_CR) Data Terminal Ready Disable */
#define US_CR_RTSEN (0x1u << 18) /**< \brief (US_CR) Request to Send Enable */
#define US_CR_FCS (0x1u << 18) /**< \brief (US_CR) Force SPI Chip Select */
#define US_CR_RTSDIS (0x1u << 19) /**< \brief (US_CR) Request to Send Disable */
#define US_CR_RCS (0x1u << 19) /**< \brief (US_CR) Release SPI Chip Select */
/* -------- US_MR : (USART Offset: 0x0004) Mode Register -------- */
#define US_MR_USART_MODE_Pos 0
#define US_MR_USART_MODE_Msk (0xfu << US_MR_USART_MODE_Pos) /**< \brief (US_MR)  */
#define   US_MR_USART_MODE_NORMAL (0x0u << 0) /**< \brief (US_MR) Normal mode */
#define   US_MR_USART_MODE_RS485 (0x1u << 0) /**< \brief (US_MR) RS485 */
#define   US_MR_USART_MODE_HW_HANDSHAKING (0x2u << 0) /**< \brief (US_MR) Hardware Handshaking */
#define   US_MR_USART_MODE_MODEM (0x3u << 0) /**< \brief (US_MR) Modem */
#define   US_MR_USART_MODE_IS07816_T_0 (0x4u << 0) /**< \brief (US_MR) IS07816 Protocol: T = 0 */
#define   US_MR_USART_MODE_IS07816_T_1 (0x6u << 0) /**< \brief (US_MR) IS07816 Protocol: T = 1 */
#define   US_MR_USART_MODE_IRDA (0x8u << 0) /**< \brief (US_MR) IrDA */
#define   US_MR_USART_MODE_SPI_MASTER (0xEu << 0) /**< \brief (US_MR) SPI Master */
#define   US_MR_USART_MODE_SPI_SLAVE (0xFu << 0) /**< \brief (US_MR) SPI Slave */
#define US_MR_USCLKS_Pos 4
#define US_MR_USCLKS_Msk (0x3u << US_MR_USCLKS_Pos) /**< \brief (US_MR) Clock Selection */
#define   US_MR_USCLKS_MCK (0x0u << 4) /**< \brief (US_MR) Master Clock MCK is selected */
#define   US_MR_USCLKS_DIV (0x1u << 4) /**< \brief (US_MR) Internal Clock Divided MCK/DIV (DIV=8) is selected */
#define   US_MR_USCLKS_SCK (0x3u << 4) /**< \brief (US_MR) Serial Clock SLK is selected */
#define US_MR_CHRL_Pos 6
#define US_MR_CHRL_Msk (0x3u << US_MR_CHRL_Pos) /**< \brief (US_MR) Character Length. */
#define   US_MR_CHRL_5_BIT (0x0u << 6) /**< \brief (US_MR) Character length is 5 bits */
#define   US_MR_CHRL_6_BIT (0x1u << 6) /**< \brief (US_MR) Character length is 6 bits */
#define   US_MR_CHRL_7_BIT (0x2u << 6) /**< \brief (US_MR) Character length is 7 bits */
#define   US_MR_CHRL_8_BIT (0x3u << 6) /**< \brief (US_MR) Character length is 8 bits */
#define US_MR_SYNC (0x1u << 8) /**< \brief (US_MR) Synchronous Mode Select */
#define US_MR_CPHA (0x1u << 8) /**< \brief (US_MR) SPI Clock Phase */
#define US_MR_PAR_Pos 9
#define US_MR_PAR_Msk (0x7u << US_MR_PAR_Pos) /**< \brief (US_MR) Parity Type */
#define   US_MR_PAR_EVEN (0x0u << 9) /**< \brief (US_MR) Even parity */
#define   US_MR_PAR_ODD (0x1u << 9) /**< \brief (US_MR) Odd parity */
#define   US_MR_PAR_SPACE (0x2u << 9) /**< \brief (US_MR) Parity forced to 0 (Space) */
#define   US_MR_PAR_MARK (0x3u << 9) /**< \brief (US_MR) Parity forced to 1 (Mark) */
#define   US_MR_PAR_NO (0x4u << 9) /**< \brief (US_MR) No parity */
#define   US_MR_PAR_MULTIDROP (0x6u << 9) /**< \brief (US_MR) Multidrop mode */
#define US_MR_NBSTOP_Pos 12
#define US_MR_NBSTOP_Msk (0x3u << US_MR_NBSTOP_Pos) /**< \brief (US_MR) Number of Stop Bits */
#define   US_MR_NBSTOP_1_BIT (0x0u << 12) /**< \brief (US_MR) 1 stop bit */
#define   US_MR_NBSTOP_1_5_BIT (0x1u << 12) /**< \brief (US_MR) 1.5 stop bit (SYNC = 0) or reserved (SYNC = 1) */
#define   US_MR_NBSTOP_2_BIT (0x2u << 12) /**< \brief (US_MR) 2 stop bits */
#define US_MR_CHMODE_Pos 14
#define US_MR_CHMODE_Msk (0x3u << US_MR_CHMODE_Pos) /**< \brief (US_MR) Channel Mode */
#define   US_MR_CHMODE_NORMAL (0x0u << 14) /**< \brief (US_MR) Normal Mode */
#define   US_MR_CHMODE_AUTOMATIC (0x1u << 14) /**< \brief (US_MR) Automatic Echo. Receiver input is connected to the TXD pin. */
#define   US_MR_CHMODE_LOCAL_LOOPBACK (0x2u << 14) /**< \brief (US_MR) Local Loopback. Transmitter output is connected to the Receiver Input. */
#define   US_MR_CHMODE_REMOTE_LOOPBACK (0x3u << 14) /**< \brief (US_MR) Remote Loopback. RXD pin is internally connected to the TXD pin. */
#define US_MR_MSBF (0x1u << 16) /**< \brief (US_MR) Bit Order */
#define US_MR_CPOL (0x1u << 16) /**< \brief (US_MR) SPI Clock Polarity */
#define US_MR_MODE9 (0x1u << 17) /**< \brief (US_MR) 9-bit Character Length */
#define US_MR_CLKO (0x1u << 18) /**< \brief (US_MR) Clock Output Select */
#define US_MR_OVER (0x1u << 19) /**< \brief (US_MR) Oversampling Mode */
#define US_MR_INACK (0x1u << 20) /**< \brief (US_MR) Inhibit Non Acknowledge */
#define US_MR_DSNACK (0x1u << 21) /**< \brief (US_MR) Disable Successive NACK */
#define US_MR_VAR_SYNC (0x1u << 22) /**< \brief (US_MR) Variable Synchronization of Command/Data Sync Start Frame Delimiter */
#define US_MR_INVDATA (0x1u << 23) /**< \brief (US_MR) INverted Data */
#define US_MR_MAX_ITERATION_Pos 24
#define US_MR_MAX_ITERATION_Msk (0x7u << US_MR_MAX_ITERATION_Pos) /**< \brief (US_MR)  */
#define US_MR_MAX_ITERATION(value) ((US_MR_MAX_ITERATION_Msk & ((value) << US_MR_MAX_ITERATION_Pos)))
#define US_MR_FILTER (0x1u << 28) /**< \brief (US_MR) Infrared Receive Line Filter */
#define US_MR_MAN (0x1u << 29) /**< \brief (US_MR) Manchester Encoder/Decoder Enable */
#define US_MR_MODSYNC (0x1u << 30) /**< \brief (US_MR) Manchester Synchronization Mode */
#define US_MR_ONEBIT (0x1u << 31) /**< \brief (US_MR) Start Frame Delimiter Selector */
/* -------- US_IER : (USART Offset: 0x0008) Interrupt Enable Register -------- */
#define US_IER_RXRDY (0x1u << 0) /**< \brief (US_IER) RXRDY Interrupt Enable */
#define US_IER_TXRDY (0x1u << 1) /**< \brief (US_IER) TXRDY Interrupt Enable */
#define US_IER_RXBRK (0x1u << 2) /**< \brief (US_IER) Receiver Break Interrupt Enable */
#define US_IER_ENDRX (0x1u << 3) /**< \brief (US_IER) End of Receive Transfer Interrupt Enable */
#define US_IER_ENDTX (0x1u << 4) /**< \brief (US_IER) End of Transmit Interrupt Enable */
#define US_IER_OVRE (0x1u << 5) /**< \brief (US_IER) Overrun Error Interrupt Enable */
#define US_IER_FRAME (0x1u << 6) /**< \brief (US_IER) Framing Error Interrupt Enable */
#define US_IER_PARE (0x1u << 7) /**< \brief (US_IER) Parity Error Interrupt Enable */
#define US_IER_TIMEOUT (0x1u << 8) /**< \brief (US_IER) Time-out Interrupt Enable */
#define US_IER_TXEMPTY (0x1u << 9) /**< \brief (US_IER) TXEMPTY Interrupt Enable */
#define US_IER_ITER (0x1u << 10) /**< \brief (US_IER) Max number of Repetitions Reached */
#define US_IER_UNRE (0x1u << 10) /**< \brief (US_IER) SPI Underrun Error */
#define US_IER_TXBUFE (0x1u << 11) /**< \brief (US_IER) Buffer Empty Interrupt Enable */
#define US_IER_RXBUFF (0x1u << 12) /**< \brief (US_IER) Buffer Full Interrupt Enable */
#define US_IER_NACK (0x1u << 13) /**< \brief (US_IER) Non AcknowledgeInterrupt Enable */
#define US_IER_RIIC (0x1u << 16) /**< \brief (US_IER) Ring Indicator Input Change Enable */
#define US_IER_DSRIC (0x1u << 17) /**< \brief (US_IER) Data Set Ready Input Change Enable */
#define US_IER_DCDIC (0x1u << 18) /**< \brief (US_IER) Data Carrier Detect Input Change Interrupt Enable */
#define US_IER_CTSIC (0x1u << 19) /**< \brief (US_IER) Clear to Send Input Change Interrupt Enable */
#define US_IER_MANE (0x1u << 24) /**< \brief (US_IER) Manchester Error Interrupt Enable */
/* -------- US_IDR : (USART Offset: 0x000C) Interrupt Disable Register -------- */
#define US_IDR_RXRDY (0x1u << 0) /**< \brief (US_IDR) RXRDY Interrupt Disable */
#define US_IDR_TXRDY (0x1u << 1) /**< \brief (US_IDR) TXRDY Interrupt Disable */
#define US_IDR_RXBRK (0x1u << 2) /**< \brief (US_IDR) Receiver Break Interrupt Disable */
#define US_IDR_ENDRX (0x1u << 3) /**< \brief (US_IDR) End of Receive Transfer Interrupt Disable */
#define US_IDR_ENDTX (0x1u << 4) /**< \brief (US_IDR) End of Transmit Interrupt Disable */
#define US_IDR_OVRE (0x1u << 5) /**< \brief (US_IDR) Overrun Error Interrupt Disable */
#define US_IDR_FRAME (0x1u << 6) /**< \brief (US_IDR) Framing Error Interrupt Disable */
#define US_IDR_PARE (0x1u << 7) /**< \brief (US_IDR) Parity Error Interrupt Disable */
#define US_IDR_TIMEOUT (0x1u << 8) /**< \brief (US_IDR) Time-out Interrupt Disable */
#define US_IDR_TXEMPTY (0x1u << 9) /**< \brief (US_IDR) TXEMPTY Interrupt Disable */
#define US_IDR_ITER (0x1u << 10) /**< \brief (US_IDR) Max number of Repetitions Reached Disable */
#define US_IDR_UNRE (0x1u << 10) /**< \brief (US_IDR) SPI Underrun Error Disable */
#define US_IDR_TXBUFE (0x1u << 11) /**< \brief (US_IDR) Buffer Empty Interrupt Disable */
#define US_IDR_RXBUFF (0x1u << 12) /**< \brief (US_IDR) Buffer Full Interrupt Disable */
#define US_IDR_NACK (0x1u << 13) /**< \brief (US_IDR) Non AcknowledgeInterrupt Disable */
#define US_IDR_RIIC (0x1u << 16) /**< \brief (US_IDR) Ring Indicator Input Change Disable */
#define US_IDR_DSRIC (0x1u << 17) /**< \brief (US_IDR) Data Set Ready Input Change Disable */
#define US_IDR_DCDIC (0x1u << 18) /**< \brief (US_IDR) Data Carrier Detect Input Change Interrupt Disable */
#define US_IDR_CTSIC (0x1u << 19) /**< \brief (US_IDR) Clear to Send Input Change Interrupt Disable */
#define US_IDR_MANE (0x1u << 24) /**< \brief (US_IDR) Manchester Error Interrupt Disable */
/* -------- US_IMR : (USART Offset: 0x0010) Interrupt Mask Register -------- */
#define US_IMR_RXRDY (0x1u << 0) /**< \brief (US_IMR) RXRDY Interrupt Mask */
#define US_IMR_TXRDY (0x1u << 1) /**< \brief (US_IMR) TXRDY Interrupt Mask */
#define US_IMR_RXBRK (0x1u << 2) /**< \brief (US_IMR) Receiver Break Interrupt Mask */
#define US_IMR_ENDRX (0x1u << 3) /**< \brief (US_IMR) End of Receive Transfer Interrupt Mask */
#define US_IMR_ENDTX (0x1u << 4) /**< \brief (US_IMR) End of Transmit Interrupt Mask */
#define US_IMR_OVRE (0x1u << 5) /**< \brief (US_IMR) Overrun Error Interrupt Mask */
#define US_IMR_FRAME (0x1u << 6) /**< \brief (US_IMR) Framing Error Interrupt Mask */
#define US_IMR_PARE (0x1u << 7) /**< \brief (US_IMR) Parity Error Interrupt Mask */
#define US_IMR_TIMEOUT (0x1u << 8) /**< \brief (US_IMR) Time-out Interrupt Mask */
#define US_IMR_TXEMPTY (0x1u << 9) /**< \brief (US_IMR) TXEMPTY Interrupt Mask */
#define US_IMR_ITER (0x1u << 10) /**< \brief (US_IMR) Max number of Repetitions Reached Mask */
#define US_IMR_UNRE (0x1u << 10) /**< \brief (US_IMR) SPI Underrun Error Mask */
#define US_IMR_TXBUFE (0x1u << 11) /**< \brief (US_IMR) Buffer Empty Interrupt Mask */
#define US_IMR_RXBUFF (0x1u << 12) /**< \brief (US_IMR) Buffer Full Interrupt Mask */
#define US_IMR_NACK (0x1u << 13) /**< \brief (US_IMR) Non AcknowledgeInterrupt Mask */
#define US_IMR_RIIC (0x1u << 16) /**< \brief (US_IMR) Ring Indicator Input Change Mask */
#define US_IMR_DSRIC (0x1u << 17) /**< \brief (US_IMR) Data Set Ready Input Change Mask */
#define US_IMR_DCDIC (0x1u << 18) /**< \brief (US_IMR) Data Carrier Detect Input Change Interrupt Mask */
#define US_IMR_CTSIC (0x1u << 19) /**< \brief (US_IMR) Clear to Send Input Change Interrupt Mask */
#define US_IMR_MANE (0x1u << 24) /**< \brief (US_IMR) Manchester Error Interrupt Mask */
/* -------- US_CSR : (USART Offset: 0x0014) Channel Status Register -------- */
#define US_CSR_RXRDY (0x1u << 0) /**< \brief (US_CSR) Receiver Ready */
#define US_CSR_TXRDY (0x1u << 1) /**< \brief (US_CSR) Transmitter Ready */
#define US_CSR_RXBRK (0x1u << 2) /**< \brief (US_CSR) Break Received/End of Break */
#define US_CSR_ENDRX (0x1u << 3) /**< \brief (US_CSR) End of Receiver Transfer */
#define US_CSR_ENDTX (0x1u << 4) /**< \brief (US_CSR) End of Transmitter Transfer */
#define US_CSR_OVRE (0x1u << 5) /**< \brief (US_CSR) Overrun Error */
#define US_CSR_FRAME (0x1u << 6) /**< \brief (US_CSR) Framing Error */
#define US_CSR_PARE (0x1u << 7) /**< \brief (US_CSR) Parity Error */
#define US_CSR_TIMEOUT (0x1u << 8) /**< \brief (US_CSR) Receiver Time-out */
#define US_CSR_TXEMPTY (0x1u << 9) /**< \brief (US_CSR) Transmitter Empty */
#define US_CSR_ITER (0x1u << 10) /**< \brief (US_CSR) Max number of Repetitions Reached */
#define US_CSR_UNRE (0x1u << 10) /**< \brief (US_CSR) SPI Underrun Error */
#define US_CSR_TXBUFE (0x1u << 11) /**< \brief (US_CSR) Transmission Buffer Empty */
#define US_CSR_RXBUFF (0x1u << 12) /**< \brief (US_CSR) Reception Buffer Full */
#define US_CSR_NACK (0x1u << 13) /**< \brief (US_CSR) Non AcknowledgeInterrupt */
#define US_CSR_RIIC (0x1u << 16) /**< \brief (US_CSR) Ring Indicator Input Change Flag */
#define US_CSR_DSRIC (0x1u << 17) /**< \brief (US_CSR) Data Set Ready Input Change Flag */
#define US_CSR_DCDIC (0x1u << 18) /**< \brief (US_CSR) Data Carrier Detect Input Change Flag */
#define US_CSR_CTSIC (0x1u << 19) /**< \brief (US_CSR) Clear to Send Input Change Flag */
#define US_CSR_RI (0x1u << 20) /**< \brief (US_CSR) Image of RI Input */
#define US_CSR_DSR (0x1u << 21) /**< \brief (US_CSR) Image of DSR Input */
#define US_CSR_DCD (0x1u << 22) /**< \brief (US_CSR) Image of DCD Input */
#define US_CSR_CTS (0x1u << 23) /**< \brief (US_CSR) Image of CTS Input */
#define US_CSR_MANERR (0x1u << 24) /**< \brief (US_CSR) Manchester Error */
/* -------- US_RHR : (USART Offset: 0x0018) Receiver Holding Register -------- */
#define US_RHR_RXCHR_Pos 0
#define US_RHR_RXCHR_Msk (0x1ffu << US_RHR_RXCHR_Pos) /**< \brief (US_RHR) Received Character */
#define US_RHR_RXSYNH (0x1u << 15) /**< \brief (US_RHR) Received Sync */
/* -------- US_THR : (USART Offset: 0x001C) Transmitter Holding Register -------- */
#define US_THR_TXCHR_Pos 0
#define US_THR_TXCHR_Msk (0x1ffu << US_THR_TXCHR_Pos) /**< \brief (US_THR) Character to be Transmitted */
#define US_THR_TXCHR(value) ((US_THR_TXCHR_Msk & ((value) << US_THR_TXCHR_Pos)))
#define US_THR_TXSYNH (0x1u << 15) /**< \brief (US_THR) Sync Field to be transmitted */
/* -------- US_BRGR : (USART Offset: 0x0020) Baud Rate Generator Register -------- */
#define US_BRGR_CD_Pos 0
#define US_BRGR_CD_Msk (0xffffu << US_BRGR_CD_Pos) /**< \brief (US_BRGR) Clock Divider */
#define US_BRGR_CD(value) ((US_BRGR_CD_Msk & ((value) << US_BRGR_CD_Pos)))
#define US_BRGR_FP_Pos 16
#define US_BRGR_FP_Msk (0x7u << US_BRGR_FP_Pos) /**< \brief (US_BRGR) Fractional Part */
#define US_BRGR_FP(value) ((US_BRGR_FP_Msk & ((value) << US_BRGR_FP_Pos)))
/* -------- US_RTOR : (USART Offset: 0x0024) Receiver Time-out Register -------- */
#define US_RTOR_TO_Pos 0
#define US_RTOR_TO_Msk (0xffffu << US_RTOR_TO_Pos) /**< \brief (US_RTOR) Time-out Value */
#define US_RTOR_TO(value) ((US_RTOR_TO_Msk & ((value) << US_RTOR_TO_Pos)))
/* -------- US_TTGR : (USART Offset: 0x0028) Transmitter Timeguard Register -------- */
#define US_TTGR_TG_Pos 0
#define US_TTGR_TG_Msk (0xffu << US_TTGR_TG_Pos) /**< \brief (US_TTGR) Timeguard Value */
#define US_TTGR_TG(value) ((US_TTGR_TG_Msk & ((value) << US_TTGR_TG_Pos)))
/* -------- US_FIDI : (USART Offset: 0x0040) FI DI Ratio Register -------- */
#define US_FIDI_FI_DI_RATIO_Pos 0
#define US_FIDI_FI_DI_RATIO_Msk (0x7ffu << US_FIDI_FI_DI_RATIO_Pos) /**< \brief (US_FIDI) FI Over DI Ratio Value */
#define US_FIDI_FI_DI_RATIO(value) ((US_FIDI_FI_DI_RATIO_Msk & ((value) << US_FIDI_FI_DI_RATIO_Pos)))
/* -------- US_NER : (USART Offset: 0x0044) Number of Errors Register -------- */
#define US_NER_NB_ERRORS_Pos 0
#define US_NER_NB_ERRORS_Msk (0xffu << US_NER_NB_ERRORS_Pos) /**< \brief (US_NER) Number of Errors */
/* -------- US_IF : (USART Offset: 0x004C) IrDA Filter Register -------- */
#define US_IF_IRDA_FILTER_Pos 0
#define US_IF_IRDA_FILTER_Msk (0xffu << US_IF_IRDA_FILTER_Pos) /**< \brief (US_IF) IrDA Filter */
#define US_IF_IRDA_FILTER(value) ((US_IF_IRDA_FILTER_Msk & ((value) << US_IF_IRDA_FILTER_Pos)))
/* -------- US_MAN : (USART Offset: 0x0050) Manchester Encoder Decoder Register -------- */
#define US_MAN_TX_PL_Pos 0
#define US_MAN_TX_PL_Msk (0xfu << US_MAN_TX_PL_Pos) /**< \brief (US_MAN) Transmitter Preamble Length */
#define US_MAN_TX_PL(value) ((US_MAN_TX_PL_Msk & ((value) << US_MAN_TX_PL_Pos)))
#define US_MAN_TX_PP_Pos 8
#define US_MAN_TX_PP_Msk (0x3u << US_MAN_TX_PP_Pos) /**< \brief (US_MAN) Transmitter Preamble Pattern */
#define   US_MAN_TX_PP_ALL_ONE (0x0u << 8) /**< \brief (US_MAN) The preamble is composed of '1's */
#define   US_MAN_TX_PP_ALL_ZERO (0x1u << 8) /**< \brief (US_MAN) The preamble is composed of '0's */
#define   US_MAN_TX_PP_ZERO_ONE (0x2u << 8) /**< \brief (US_MAN) The preamble is composed of '01's */
#define   US_MAN_TX_PP_ONE_ZERO (0x3u << 8) /**< \brief (US_MAN) The preamble is composed of '10's */
#define US_MAN_TX_MPOL (0x1u << 12) /**< \brief (US_MAN) Transmitter Manchester Polarity */
#define US_MAN_RX_PL_Pos 16
#define US_MAN_RX_PL_Msk (0xfu << US_MAN_RX_PL_Pos) /**< \brief (US_MAN) Receiver Preamble Length */
#define US_MAN_RX_PL(value) ((US_MAN_RX_PL_Msk & ((value) << US_MAN_RX_PL_Pos)))
#define US_MAN_RX_PP_Pos 24
#define US_MAN_RX_PP_Msk (0x3u << US_MAN_RX_PP_Pos) /**< \brief (US_MAN) Receiver Preamble Pattern detected */
#define   US_MAN_RX_PP_ALL_ONE (0x0u << 24) /**< \brief (US_MAN) The preamble is composed of '1's */
#define   US_MAN_RX_PP_ALL_ZERO (0x1u << 24) /**< \brief (US_MAN) The preamble is composed of '0's */
#define   US_MAN_RX_PP_ZERO_ONE (0x2u << 24) /**< \brief (US_MAN) The preamble is composed of '01's */
#define   US_MAN_RX_PP_ONE_ZERO (0x3u << 24) /**< \brief (US_MAN) The preamble is composed of '10's */
#define US_MAN_RX_MPOL (0x1u << 28) /**< \brief (US_MAN) Receiver Manchester Polarity */
#define US_MAN_STUCKTO1 (0x1u << 29) /**< \brief (US_MAN)  */
#define US_MAN_DRIFT (0x1u << 30) /**< \brief (US_MAN) Drift compensation */
/* -------- US_WPMR : (USART Offset: 0xE4) Write Protect Mode Register -------- */
#define US_WPMR_WPEN (0x1u << 0) /**< \brief (US_WPMR) Write Protect Enable */
#define US_WPMR_WPKEY_Pos 8
#define US_WPMR_WPKEY_Msk (0xffffffu << US_WPMR_WPKEY_Pos) /**< \brief (US_WPMR) Write Protect KEY */
#define US_WPMR_WPKEY(value) ((US_WPMR_WPKEY_Msk & ((value) << US_WPMR_WPKEY_Pos)))
/* -------- US_WPSR : (USART Offset: 0xE8) Write Protect Status Register -------- */
#define US_WPSR_WPVS (0x1u << 0) /**< \brief (US_WPSR) Write Protect Violation Status */
#define US_WPSR_WPVSRC_Pos 8
#define US_WPSR_WPVSRC_Msk (0xffffu << US_WPSR_WPVSRC_Pos) /**< \brief (US_WPSR) Write Protect Violation Source */
/* -------- US_VERSION : (USART Offset: 0xFC) Version Register -------- */
#define US_VERSION_VERSION_Pos 0
#define US_VERSION_VERSION_Msk (0xfffu << US_VERSION_VERSION_Pos) /**< \brief (US_VERSION)  */
#define US_VERSION_MFN_Pos 16
#define US_VERSION_MFN_Msk (0x7u << US_VERSION_MFN_Pos) /**< \brief (US_VERSION)  */
/* -------- US_RPR : (USART Offset: 0x100) Receive Pointer Register -------- */
#define US_RPR_RXPTR_Pos 0
#define US_RPR_RXPTR_Msk (0xffffffffu << US_RPR_RXPTR_Pos) /**< \brief (US_RPR) Receive Pointer Register */
#define US_RPR_RXPTR(value) ((US_RPR_RXPTR_Msk & ((value) << US_RPR_RXPTR_Pos)))
/* -------- US_RCR : (USART Offset: 0x104) Receive Counter Register -------- */
#define US_RCR_RXCTR_Pos 0
#define US_RCR_RXCTR_Msk (0xffffu << US_RCR_RXCTR_Pos) /**< \brief (US_RCR) Receive Counter Register */
#define US_RCR_RXCTR(value) ((US_RCR_RXCTR_Msk & ((value) << US_RCR_RXCTR_Pos)))
/* -------- US_TPR : (USART Offset: 0x108) Transmit Pointer Register -------- */
#define US_TPR_TXPTR_Pos 0
#define US_TPR_TXPTR_Msk (0xffffffffu << US_TPR_TXPTR_Pos) /**< \brief (US_TPR) Transmit Counter Register */
#define US_TPR_TXPTR(value) ((US_TPR_TXPTR_Msk & ((value) << US_TPR_TXPTR_Pos)))
/* -------- US_TCR : (USART Offset: 0x10C) Transmit Counter Register -------- */
#define US_TCR_TXCTR_Pos 0
#define US_TCR_TXCTR_Msk (0xffffu << US_TCR_TXCTR_Pos) /**< \brief (US_TCR) Transmit Counter Register */
#define US_TCR_TXCTR(value) ((US_TCR_TXCTR_Msk & ((value) << US_TCR_TXCTR_Pos)))
/* -------- US_RNPR : (USART Offset: 0x110) Receive Next Pointer Register -------- */
#define US_RNPR_RXNPTR_Pos 0
#define US_RNPR_RXNPTR_Msk (0xffffffffu << US_RNPR_RXNPTR_Pos) /**< \brief (US_RNPR) Receive Next Pointer */
#define US_RNPR_RXNPTR(value) ((US_RNPR_RXNPTR_Msk & ((value) << US_RNPR_RXNPTR_Pos)))
/* -------- US_RNCR : (USART Offset: 0x114) Receive Next Counter Register -------- */
#define US_RNCR_RXNCTR_Pos 0
#define US_RNCR_RXNCTR_Msk (0xffffu << US_RNCR_RXNCTR_Pos) /**< \brief (US_RNCR) Receive Next Counter */
#define US_RNCR_RXNCTR(value) ((US_RNCR_RXNCTR_Msk & ((value) << US_RNCR_RXNCTR_Pos)))
/* -------- US_TNPR : (USART Offset: 0x118) Transmit Next Pointer Register -------- */
#define US_TNPR_TXNPTR_Pos 0
#define US_TNPR_TXNPTR_Msk (0xffffffffu << US_TNPR_TXNPTR_Pos) /**< \brief (US_TNPR) Transmit Next Pointer */
#define US_TNPR_TXNPTR(value) ((US_TNPR_TXNPTR_Msk & ((value) << US_TNPR_TXNPTR_Pos)))
/* -------- US_TNCR : (USART Offset: 0x11C) Transmit Next Counter Register -------- */
#define US_TNCR_TXNCTR_Pos 0
#define US_TNCR_TXNCTR_Msk (0xffffu << US_TNCR_TXNCTR_Pos) /**< \brief (US_TNCR) Transmit Counter Next */
#define US_TNCR_TXNCTR(value) ((US_TNCR_TXNCTR_Msk & ((value) << US_TNCR_TXNCTR_Pos)))
/* -------- US_PTCR : (USART Offset: 0x120) Transfer Control Register -------- */
#define US_PTCR_RXTEN (0x1u << 0) /**< \brief (US_PTCR) Receiver Transfer Enable */
#define US_PTCR_RXTDIS (0x1u << 1) /**< \brief (US_PTCR) Receiver Transfer Disable */
#define US_PTCR_TXTEN (0x1u << 8) /**< \brief (US_PTCR) Transmitter Transfer Enable */
#define US_PTCR_TXTDIS (0x1u << 9) /**< \brief (US_PTCR) Transmitter Transfer Disable */
/* -------- US_PTSR : (USART Offset: 0x124) Transfer Status Register -------- */
#define US_PTSR_RXTEN (0x1u << 0) /**< \brief (US_PTSR) Receiver Transfer Enable */
#define US_PTSR_TXTEN (0x1u << 8) /**< \brief (US_PTSR) Transmitter Transfer Enable */

/*@}*/


#endif /* _SAM3S_USART_COMPONENT_ */
