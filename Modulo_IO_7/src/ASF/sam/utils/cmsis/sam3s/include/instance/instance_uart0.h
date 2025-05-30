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

#ifndef _SAM3S_UART0_INSTANCE_
#define _SAM3S_UART0_INSTANCE_

/* ========== Register definition for UART0 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_UART0_CR            (0x400E0600U) /**< \brief (UART0) Control Register */
#define REG_UART0_MR            (0x400E0604U) /**< \brief (UART0) Mode Register */
#define REG_UART0_IER           (0x400E0608U) /**< \brief (UART0) Interrupt Enable Register */
#define REG_UART0_IDR           (0x400E060CU) /**< \brief (UART0) Interrupt Disable Register */
#define REG_UART0_IMR           (0x400E0610U) /**< \brief (UART0) Interrupt Mask Register */
#define REG_UART0_SR            (0x400E0614U) /**< \brief (UART0) Status Register */
#define REG_UART0_RHR           (0x400E0618U) /**< \brief (UART0) Receive Holding Register */
#define REG_UART0_THR           (0x400E061CU) /**< \brief (UART0) Transmit Holding Register */
#define REG_UART0_BRGR          (0x400E0620U) /**< \brief (UART0) Baud Rate Generator Register */
#define REG_UART0_RPR           (0x400E0700U) /**< \brief (UART0) Receive Pointer Register */
#define REG_UART0_RCR           (0x400E0704U) /**< \brief (UART0) Receive Counter Register */
#define REG_UART0_TPR           (0x400E0708U) /**< \brief (UART0) Transmit Pointer Register */
#define REG_UART0_TCR           (0x400E070CU) /**< \brief (UART0) Transmit Counter Register */
#define REG_UART0_RNPR          (0x400E0710U) /**< \brief (UART0) Receive Next Pointer Register */
#define REG_UART0_RNCR          (0x400E0714U) /**< \brief (UART0) Receive Next Counter Register */
#define REG_UART0_TNPR          (0x400E0718U) /**< \brief (UART0) Transmit Next Pointer Register */
#define REG_UART0_TNCR          (0x400E071CU) /**< \brief (UART0) Transmit Next Counter Register */
#define REG_UART0_PTCR          (0x400E0720U) /**< \brief (UART0) Transfer Control Register */
#define REG_UART0_PTSR          (0x400E0724U) /**< \brief (UART0) Transfer Status Register */
#else
#define REG_UART0_CR   (*(WoReg*)0x400E0600U) /**< \brief (UART0) Control Register */
#define REG_UART0_MR   (*(RwReg*)0x400E0604U) /**< \brief (UART0) Mode Register */
#define REG_UART0_IER  (*(WoReg*)0x400E0608U) /**< \brief (UART0) Interrupt Enable Register */
#define REG_UART0_IDR  (*(WoReg*)0x400E060CU) /**< \brief (UART0) Interrupt Disable Register */
#define REG_UART0_IMR  (*(RoReg*)0x400E0610U) /**< \brief (UART0) Interrupt Mask Register */
#define REG_UART0_SR   (*(RoReg*)0x400E0614U) /**< \brief (UART0) Status Register */
#define REG_UART0_RHR  (*(RoReg*)0x400E0618U) /**< \brief (UART0) Receive Holding Register */
#define REG_UART0_THR  (*(WoReg*)0x400E061CU) /**< \brief (UART0) Transmit Holding Register */
#define REG_UART0_BRGR (*(RwReg*)0x400E0620U) /**< \brief (UART0) Baud Rate Generator Register */
#define REG_UART0_RPR  (*(RwReg*)0x400E0700U) /**< \brief (UART0) Receive Pointer Register */
#define REG_UART0_RCR  (*(RwReg*)0x400E0704U) /**< \brief (UART0) Receive Counter Register */
#define REG_UART0_TPR  (*(RwReg*)0x400E0708U) /**< \brief (UART0) Transmit Pointer Register */
#define REG_UART0_TCR  (*(RwReg*)0x400E070CU) /**< \brief (UART0) Transmit Counter Register */
#define REG_UART0_RNPR (*(RwReg*)0x400E0710U) /**< \brief (UART0) Receive Next Pointer Register */
#define REG_UART0_RNCR (*(RwReg*)0x400E0714U) /**< \brief (UART0) Receive Next Counter Register */
#define REG_UART0_TNPR (*(RwReg*)0x400E0718U) /**< \brief (UART0) Transmit Next Pointer Register */
#define REG_UART0_TNCR (*(RwReg*)0x400E071CU) /**< \brief (UART0) Transmit Next Counter Register */
#define REG_UART0_PTCR (*(WoReg*)0x400E0720U) /**< \brief (UART0) Transfer Control Register */
#define REG_UART0_PTSR (*(RoReg*)0x400E0724U) /**< \brief (UART0) Transfer Status Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAM3S_UART0_INSTANCE_ */
