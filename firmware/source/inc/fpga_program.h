/*
 * \brief Xilinx Spartan 6 FPGA Programming Routines Header File
 *
 * Copyright (c) 2014 NewAE Technology Inc.  All rights reserved.
 *   Author: Colin O'Flynn, <coflynn@newae.com>
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
 * 3. The name of NewAE Technology Inc may not be used to endorse or promote
 *    products derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY NEWAE TECHNOLOGY INC "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL NEWAE TECHNOLOGY INC
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _FPGA_PROGRAM_H_
#define _FPGA_PROGRAM_H_

#define PIN_FPGA_PROGRAM_GPIO PIO_PA20_IDX
#define PIN_FPGA_INITB_GPIO PIO_PA21_IDX
#define PIN_FPGA_DONE_GPIO PIO_PA19_IDX
#define FPGA_PROG_USART USART0
#define PIN_FPGA_CCLK_GPIO PIO_PA17_IDX
#define PIN_FPGA_DO_GPIO PIO_PA18_IDX
#define FPGA_PROG_USART_ID ID_USART0

// FPGA Registers that we use - TRACE class
#define FPGA_TRACE_REG_SELECT        (0x80)
#define FPGA_REG_NAME                (FPGA_TRACE_REG_SELECT|0x00)
#define FPGA_REG_REV                 (FPGA_TRACE_REG_SELECT|0x01)
#define FPGA_REG_CLKSETTINGS         (FPGA_TRACE_REG_SELECT|0x02)
#define FPGA_REG_PATTERN_ENABLE      (FPGA_TRACE_REG_SELECT|0x03)
#define FPGA_REG_PATTERN_TRIG_ENABLE (FPGA_TRACE_REG_SELECT|0x04)
#define FPGA_REG_TRACE_RESET_SYNC    (FPGA_TRACE_REG_SELECT|0x05)
#define FPGA_REG_TRACE_WIDTH         (FPGA_TRACE_REG_SELECT|0x06)
#define FPGA_REG_SOFT_TRIG_PASSTHRU  (FPGA_TRACE_REG_SELECT|0x07)
#define FPGA_REG_SOFT_TRIG_ENABLE    (FPGA_TRACE_REG_SELECT|0x08)
#define FPGA_REG_CAPTURE_RAW         (FPGA_TRACE_REG_SELECT|0x09)
#define FPGA_REG_SYNCHRONIZED        (FPGA_TRACE_REG_SELECT|0x0c)
#define FPGA_REG_TRACE_PATTERN(x)    (FPGA_TRACE_REG_SELECT|(x+0x0d))
#define FPGA_REG_TRACE_MASK(x)       (FPGA_TRACE_REG_SELECT|(x+0x15))
#define FPGA_REG_TRACE_COUNT         (FPGA_TRACE_REG_SELECT|0x1d)
#define FPGA_REG_RECORD_SYNCS        (FPGA_TRACE_REG_SELECT|0x1e)
#define FPGA_REG_MATCHED_DATA        (FPGA_TRACE_REG_SELECT|0x1f)

// Bitfields in the TRACE class
#define FPGA_FIFO_BYTE_SHORT_TS     (1)
#define FPGA_FIFO_BYTE_HIGH_TS      (2)
#define FPGA_FIFO_BYTE_STAT         (3)
#define FPGA_FIFO_BYTE_RULEMATCHNO  (2)
#define FPGA_FIFO_BYTE_DATA         (2)
#define FPGA_FIFO_BYTE_STREAM_STAT  (2)

#define FPGA_FE_FIFO_CMD_MASK (0x03)
#define FPGA_FE_FIFO_CMD_DATA (0x00)
#define FPGA_FE_FIFO_CMD_STAT (0x01)
#define FPGA_FE_FIFO_CMD_TIME (0x02)
#define FPGA_FE_FIFO_CMD_STRM (0x03)

#define FE_FIFO_STAT_EMPTY               (1<<2)
#define FE_FIFO_STAT_UNDERFLOW           (1<<3)
#define FE_FIFO_STAT_EMPTY_THRESHOLD     (1<<4)
#define FE_FIFO_STAT_FULL                (1<<5)
#define FE_FIFO_STAT_OVERFLOW_BLOCKED    (1<<6)
#define FE_FIFO_STAT_SYNC_FLAG           (1<<7)
#define FE_FIFO_STAT_CUSTOM_FLAG         (1<<7)

#define FE_SELECT_WIDTH 2
#define FE_TRACE        1
#define FE_USB          0


// FPGA Registers - core class
#define FPGA_MAIN_REG_SELECT          (0x00)
#define FPGA_REG_BUILDTIME            (FPGA_MAIN_REG_SELECT|0x00)
#define FPGA_REG_FE_SELECT            (FPGA_MAIN_REG_SELECT|0x01)
#define FPGA_REG_SNIFF_FIFO_STAT      (FPGA_MAIN_REG_SELECT|0x02)
#define FPGA_REG_SNIFF_FIFO_RD        (FPGA_MAIN_REG_SELECT|0x03)
#define FPGA_REG_ARM                  (FPGA_MAIN_REG_SELECT|0x04)
#define FPGA_REG_TRIGGER_ENABLE       (FPGA_MAIN_REG_SELECT|0x05)
#define FPGA_REG_NUM_TRIGGERS         (FPGA_MAIN_REG_SELECT|0x06)
#define FPGA_REG_TRIGGER_DELAY        (FPGA_MAIN_REG_SELECT|0x07)
#define FPGA_REG_TRIGGER_WIDTH        (FPGA_MAIN_REG_SELECT|0x08)
#define FPGA_REG_TRIG_CLK_PHASE_SHIFT (FPGA_MAIN_REG_SELECT|0x09)
#define FPGA_REG_CAPTURE_LEN          (FPGA_MAIN_REG_SELECT|0x0a)
#define FPGA_REG_COUNT_WRITES         (FPGA_MAIN_REG_SELECT|0x0b)
#define FPGA_REG_COUNTER_QUICK_START  (FPGA_MAIN_REG_SELECT|0x0c)
#define FPGA_REG_BOARD_REV            (FPGA_MAIN_REG_SELECT|0x0d)
#define FPGA_REG_USERIO_DATA          (FPGA_MAIN_REG_SELECT|0x0e)
#define FPGA_REG_USERIO_PWRDRIVEN     (FPGA_MAIN_REG_SELECT|0x0f)

// FPGA Registers - USB class
#define FPGA_USB_REG_SELECT           (0x40)

// Bits on the connector
#define USERIO_SWDIO_BIT (0)
#define USERIO_SWK_BIT   (1)
#define USERIO_RST_BIT   (3)

//! FPGA Programming Pins
#define PIN_FPGA_PROGRAM_FLAGS  (PIO_TYPE_PIO_OUTPUT_1 | PIO_DEFAULT)
#define FPGA_NPROG_LOW()        gpio_set_pin_low(PIN_FPGA_PROGRAM_GPIO)
#define FPGA_NPROG_HIGH()       gpio_set_pin_high(PIN_FPGA_PROGRAM_GPIO)
#define FPGA_NPROG_SETUP()      gpio_configure_pin(PIN_FPGA_PROGRAM_GPIO, PIN_FPGA_PROGRAM_FLAGS)

#define PIN_FPGA_INITB_FLAGS    (PIO_TYPE_PIO_OUTPUT_1 | PIO_DEFAULT | PIO_OPENDRAIN)
#define FPGA_INITB_LOW()        gpio_set_pin_low(PIN_FPGA_INITB_GPIO)
#define FPGA_INITB_HIGH()       gpio_set_pin_high(PIN_FPGA_INITB_GPIO)
#define FPGA_INITB_SETUP()      gpio_configure_pin(PIN_FPGA_INITB_GPIO, PIN_FPGA_INITB_FLAGS)

#define PIN_FPGA_DONE_FLAGS     (PIO_TYPE_PIO_INPUT | PIO_DEFAULT)
#define FPGA_ISDONE()           gpio_pin_is_high(PIN_FPGA_DONE_GPIO)

#define PIN_FPGA_CCLK_FLAGS     (PIO_TYPE_PIO_OUTPUT_0 | PIO_DEFAULT)
#define FPGA_CCLK_LOW()         gpio_set_pin_low(PIN_FPGA_CCLK_GPIO)
#define FPGA_CCLK_HIGH()        gpio_set_pin_high(PIN_FPGA_CCLK_GPIO)
#define FPGA_CCLK_SETUP()       gpio_configure_pin(PIN_FPGA_CCLK_GPIO, PIN_FPGA_CCLK_FLAGS)

#define PIN_FPGA_DO_FLAGS       (PIO_TYPE_PIO_OUTPUT_0 | PIO_DEFAULT)
#define FPGA_DO_LOW()           gpio_set_pin_low(PIN_FPGA_DO_GPIO)
#define FPGA_DO_HIGH()          gpio_set_pin_high(PIN_FPGA_DO_GPIO)
#define FPGA_DO_SETUP()         gpio_configure_pin(PIN_FPGA_DO_GPIO, PIN_FPGA_DO_FLAGS)

#ifndef FPGA_USE_BITBANG
    #define FPGA_USE_BITBANG 0
#endif

#if FPGA_USE_BITBANG
    #error "Bit-Bang mode might be broken currently"
#endif

#ifndef FPGA_USE_USART
    #define FPGA_USE_USART 0
#endif

/**
 * \brief Send a byte to FPGA using CCLK/DO, FPGA must be in programming mode
 *
 * \param databyte Byte to send, LSB is shifted out first
 */
void fpga_program_sendbyte( uint8_t databyte );

/**
 * \brief Setup the 'NPROG' pin
 */
void fpga_program_init( void );

/**
 * \brief Setup peripherals, erase FPGA, must be followed by call to _setup2()
 */
void fpga_program_setup1( void );

/**
 * \brief Set NPROG to idle state in preperation for programming to commence
 */
void fpga_program_setup2( void );

#endif
