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

#ifndef _SAM3S_TC1_INSTANCE_
#define _SAM3S_TC1_INSTANCE_

/* ========== Register definition for TC1 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))
#define REG_TC1_CCR0           (0x40014000U) /**< \brief (TC1) Channel Control Register (channel = 0) */
#define REG_TC1_CMR0           (0x40014004U) /**< \brief (TC1) Channel Mode Register (channel = 0) */
#define REG_TC1_SMMR0          (0x40014008U) /**< \brief (TC1) Stepper Motor Mode Register (channel = 0) */
#define REG_TC1_CV0            (0x40014010U) /**< \brief (TC1) Counter Value (channel = 0) */
#define REG_TC1_RA0            (0x40014014U) /**< \brief (TC1) Register A (channel = 0) */
#define REG_TC1_RB0            (0x40014018U) /**< \brief (TC1) Register B (channel = 0) */
#define REG_TC1_RC0            (0x4001401CU) /**< \brief (TC1) Register C (channel = 0) */
#define REG_TC1_SR0            (0x40014020U) /**< \brief (TC1) Status Register (channel = 0) */
#define REG_TC1_IER0           (0x40014024U) /**< \brief (TC1) Interrupt Enable Register (channel = 0) */
#define REG_TC1_IDR0           (0x40014028U) /**< \brief (TC1) Interrupt Disable Register (channel = 0) */
#define REG_TC1_IMR0           (0x4001402CU) /**< \brief (TC1) Interrupt Mask Register (channel = 0) */
#define REG_TC1_CCR1           (0x40014040U) /**< \brief (TC1) Channel Control Register (channel = 1) */
#define REG_TC1_CMR1           (0x40014044U) /**< \brief (TC1) Channel Mode Register (channel = 1) */
#define REG_TC1_SMMR1          (0x40014048U) /**< \brief (TC1) Stepper Motor Mode Register (channel = 1) */
#define REG_TC1_CV1            (0x40014050U) /**< \brief (TC1) Counter Value (channel = 1) */
#define REG_TC1_RA1            (0x40014054U) /**< \brief (TC1) Register A (channel = 1) */
#define REG_TC1_RB1            (0x40014058U) /**< \brief (TC1) Register B (channel = 1) */
#define REG_TC1_RC1            (0x4001405CU) /**< \brief (TC1) Register C (channel = 1) */
#define REG_TC1_SR1            (0x40014060U) /**< \brief (TC1) Status Register (channel = 1) */
#define REG_TC1_IER1           (0x40014064U) /**< \brief (TC1) Interrupt Enable Register (channel = 1) */
#define REG_TC1_IDR1           (0x40014068U) /**< \brief (TC1) Interrupt Disable Register (channel = 1) */
#define REG_TC1_IMR1           (0x4001406CU) /**< \brief (TC1) Interrupt Mask Register (channel = 1) */
#define REG_TC1_CCR2           (0x40014080U) /**< \brief (TC1) Channel Control Register (channel = 2) */
#define REG_TC1_CMR2           (0x40014084U) /**< \brief (TC1) Channel Mode Register (channel = 2) */
#define REG_TC1_SMMR2          (0x40014088U) /**< \brief (TC1) Stepper Motor Mode Register (channel = 2) */
#define REG_TC1_CV2            (0x40014090U) /**< \brief (TC1) Counter Value (channel = 2) */
#define REG_TC1_RA2            (0x40014094U) /**< \brief (TC1) Register A (channel = 2) */
#define REG_TC1_RB2            (0x40014098U) /**< \brief (TC1) Register B (channel = 2) */
#define REG_TC1_RC2            (0x4001409CU) /**< \brief (TC1) Register C (channel = 2) */
#define REG_TC1_SR2            (0x400140A0U) /**< \brief (TC1) Status Register (channel = 2) */
#define REG_TC1_IER2           (0x400140A4U) /**< \brief (TC1) Interrupt Enable Register (channel = 2) */
#define REG_TC1_IDR2           (0x400140A8U) /**< \brief (TC1) Interrupt Disable Register (channel = 2) */
#define REG_TC1_IMR2           (0x400140ACU) /**< \brief (TC1) Interrupt Mask Register (channel = 2) */
#define REG_TC1_BCR            (0x400140C0U) /**< \brief (TC1) Block Control Register */
#define REG_TC1_BMR            (0x400140C4U) /**< \brief (TC1) Block Mode Register */
#define REG_TC1_QIER           (0x400140C8U) /**< \brief (TC1) QDEC Interrupt Enable Register */
#define REG_TC1_QIDR           (0x400140CCU) /**< \brief (TC1) QDEC Interrupt Disable Register */
#define REG_TC1_QIMR           (0x400140D0U) /**< \brief (TC1) QDEC Interrupt Mask Register */
#define REG_TC1_QISR           (0x400140D4U) /**< \brief (TC1) QDEC Interrupt Status Register */
#define REG_TC1_FMR            (0x400140D8U) /**< \brief (TC1) Fault Mode Register */
#define REG_TC1_WPMR           (0x400140E4U) /**< \brief (TC1) Write Protect Mode Register */
#else
#define REG_TC1_CCR0  (*(WoReg*)0x40014000U) /**< \brief (TC1) Channel Control Register (channel = 0) */
#define REG_TC1_CMR0  (*(RwReg*)0x40014004U) /**< \brief (TC1) Channel Mode Register (channel = 0) */
#define REG_TC1_SMMR0 (*(RwReg*)0x40014008U) /**< \brief (TC1) Stepper Motor Mode Register (channel = 0) */
#define REG_TC1_CV0   (*(RoReg*)0x40014010U) /**< \brief (TC1) Counter Value (channel = 0) */
#define REG_TC1_RA0   (*(RwReg*)0x40014014U) /**< \brief (TC1) Register A (channel = 0) */
#define REG_TC1_RB0   (*(RwReg*)0x40014018U) /**< \brief (TC1) Register B (channel = 0) */
#define REG_TC1_RC0   (*(RwReg*)0x4001401CU) /**< \brief (TC1) Register C (channel = 0) */
#define REG_TC1_SR0   (*(RoReg*)0x40014020U) /**< \brief (TC1) Status Register (channel = 0) */
#define REG_TC1_IER0  (*(WoReg*)0x40014024U) /**< \brief (TC1) Interrupt Enable Register (channel = 0) */
#define REG_TC1_IDR0  (*(WoReg*)0x40014028U) /**< \brief (TC1) Interrupt Disable Register (channel = 0) */
#define REG_TC1_IMR0  (*(RoReg*)0x4001402CU) /**< \brief (TC1) Interrupt Mask Register (channel = 0) */
#define REG_TC1_CCR1  (*(WoReg*)0x40014040U) /**< \brief (TC1) Channel Control Register (channel = 1) */
#define REG_TC1_CMR1  (*(RwReg*)0x40014044U) /**< \brief (TC1) Channel Mode Register (channel = 1) */
#define REG_TC1_SMMR1 (*(RwReg*)0x40014048U) /**< \brief (TC1) Stepper Motor Mode Register (channel = 1) */
#define REG_TC1_CV1   (*(RoReg*)0x40014050U) /**< \brief (TC1) Counter Value (channel = 1) */
#define REG_TC1_RA1   (*(RwReg*)0x40014054U) /**< \brief (TC1) Register A (channel = 1) */
#define REG_TC1_RB1   (*(RwReg*)0x40014058U) /**< \brief (TC1) Register B (channel = 1) */
#define REG_TC1_RC1   (*(RwReg*)0x4001405CU) /**< \brief (TC1) Register C (channel = 1) */
#define REG_TC1_SR1   (*(RoReg*)0x40014060U) /**< \brief (TC1) Status Register (channel = 1) */
#define REG_TC1_IER1  (*(WoReg*)0x40014064U) /**< \brief (TC1) Interrupt Enable Register (channel = 1) */
#define REG_TC1_IDR1  (*(WoReg*)0x40014068U) /**< \brief (TC1) Interrupt Disable Register (channel = 1) */
#define REG_TC1_IMR1  (*(RoReg*)0x4001406CU) /**< \brief (TC1) Interrupt Mask Register (channel = 1) */
#define REG_TC1_CCR2  (*(WoReg*)0x40014080U) /**< \brief (TC1) Channel Control Register (channel = 2) */
#define REG_TC1_CMR2  (*(RwReg*)0x40014084U) /**< \brief (TC1) Channel Mode Register (channel = 2) */
#define REG_TC1_SMMR2 (*(RwReg*)0x40014088U) /**< \brief (TC1) Stepper Motor Mode Register (channel = 2) */
#define REG_TC1_CV2   (*(RoReg*)0x40014090U) /**< \brief (TC1) Counter Value (channel = 2) */
#define REG_TC1_RA2   (*(RwReg*)0x40014094U) /**< \brief (TC1) Register A (channel = 2) */
#define REG_TC1_RB2   (*(RwReg*)0x40014098U) /**< \brief (TC1) Register B (channel = 2) */
#define REG_TC1_RC2   (*(RwReg*)0x4001409CU) /**< \brief (TC1) Register C (channel = 2) */
#define REG_TC1_SR2   (*(RoReg*)0x400140A0U) /**< \brief (TC1) Status Register (channel = 2) */
#define REG_TC1_IER2  (*(WoReg*)0x400140A4U) /**< \brief (TC1) Interrupt Enable Register (channel = 2) */
#define REG_TC1_IDR2  (*(WoReg*)0x400140A8U) /**< \brief (TC1) Interrupt Disable Register (channel = 2) */
#define REG_TC1_IMR2  (*(RoReg*)0x400140ACU) /**< \brief (TC1) Interrupt Mask Register (channel = 2) */
#define REG_TC1_BCR   (*(WoReg*)0x400140C0U) /**< \brief (TC1) Block Control Register */
#define REG_TC1_BMR   (*(RwReg*)0x400140C4U) /**< \brief (TC1) Block Mode Register */
#define REG_TC1_QIER  (*(WoReg*)0x400140C8U) /**< \brief (TC1) QDEC Interrupt Enable Register */
#define REG_TC1_QIDR  (*(WoReg*)0x400140CCU) /**< \brief (TC1) QDEC Interrupt Disable Register */
#define REG_TC1_QIMR  (*(RoReg*)0x400140D0U) /**< \brief (TC1) QDEC Interrupt Mask Register */
#define REG_TC1_QISR  (*(RoReg*)0x400140D4U) /**< \brief (TC1) QDEC Interrupt Status Register */
#define REG_TC1_FMR   (*(RwReg*)0x400140D8U) /**< \brief (TC1) Fault Mode Register */
#define REG_TC1_WPMR  (*(RwReg*)0x400140E4U) /**< \brief (TC1) Write Protect Mode Register */
#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAM3S_TC1_INSTANCE_ */
