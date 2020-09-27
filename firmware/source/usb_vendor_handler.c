/*
  Copyright (c) 2014-2015 NewAE Technology Inc. All rights reserved.
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <asf.h>
#include "conf_usb.h"
#include "stdio_serial.h"
#include "ui.h"
#include "genclk.h"
#include "usb_vendor_handler.h"
#include "usb_xmem.h"
#include "fpga_program.h"
#include <string.h>

#define FW_VER_MAJOR 0
#define FW_VER_MINOR 10
#define FW_VER_DEBUG 0

#define REQ_MEMREAD_BULK 0x10
#define REQ_MEMWRITE_BULK 0x11
#define REQ_MEMREAD_CTRL 0x12
#define REQ_MEMWRITE_CTRL 0x13
#define REQ_MEMSTREAM 0x14
#define REQ_FPGA_STATUS 0x15
#define REQ_FPGA_PROGRAM 0x16
#define REQ_FW_VERSION 0x17
#define REQ_USART0_DATA 0x1A
#define REQ_USART0_CONFIG 0x1B
#define REQ_SCARD_DATA 0x1C
#define REQ_SCARD_CONFIG 0x1D
#define REQ_SCARD_AUX 0x1E
#define REQ_USART2DUMP_ENABLE 0x1F
#define REQ_XMEGA_PROGRAM 0x20
#define REQ_AVR_PROGRAM 0x21
#define REQ_SAM3U_CFG 0x22
#define REQ_CC_PROGRAM 0x23
#define REQ_CHANGE_PWR 0x24
#define REQ_FPGA_RESET 0x25

volatile bool g_captureinprogress = true;

static volatile bool main_b_vendor_enable = true;

uint8_t USB_PWR_STATE = 0;

COMPILER_WORD_ALIGNED
static uint8_t main_buf_loopback[MAIN_LOOPBACK_SIZE];

void main_vendor_bulk_in_received(udd_ep_status_t status,
                                  iram_size_t nb_transfered, udd_ep_id_t ep);
void main_vendor_bulk_out_received(udd_ep_status_t status,
                                   iram_size_t nb_transfered, udd_ep_id_t ep);

void ctrl_change_pwr(void);
void ctrl_fpga_reset(void);



/* ====================================================================================== */
/* ====================================================================================== */
/* ====================================================================================== */
/* Publically accessible routines                                                         */
/* ====================================================================================== */
/* ====================================================================================== */
/* ====================================================================================== */
bool usb_vendor_enable(void)
{
    main_b_vendor_enable = true;
    // Start data reception on OUT endpoints
#if UDI_VENDOR_EPS_SIZE_BULK_FS
    //main_vendor_bulk_in_received(UDD_EP_TRANSFER_OK, 0, 0);
    udi_vendor_bulk_out_run(
        main_buf_loopback,
        sizeof(main_buf_loopback),
        main_vendor_bulk_out_received);
#endif
    return true;
}
/* ====================================================================================== */
void usb_vendor_disable(void)
{
    main_b_vendor_enable = false;
}
/* ====================================================================================== */
#define REQ_MEMREAD_BULK 0x10
#define REQ_MEMWRITE_BULK 0x11
#define REQ_MEMREAD_CTRL 0x12
#define REQ_MEMWRITE_CTRL 0x13
#define REQ_FW_VERSION 0x17
#define REQ_SAM3U_CFG 0x22

COMPILER_WORD_ALIGNED static uint8_t ctrlbuffer[64];
#define CTRLBUFFER_WORDPTR ((uint32_t *) ((void *)ctrlbuffer))

typedef enum {
    bep_emem=0,
    bep_fpgabitstream=10
} blockep_usage_t;

static blockep_usage_t blockendpoint_usage = bep_emem;

static uint8_t * ctrlmemread_buf;
static unsigned int ctrlmemread_size;

void ctrl_readmem_bulk(void);
void ctrl_readmem_ctrl(void);
void ctrl_writemem_bulk(void);
void ctrl_writemem_ctrl(void);
void ctrl_progfpga_bulk(void);

/* ====================================================================================== */
void ctrl_readmem_bulk(void){
    uint32_t buflen = *(CTRLBUFFER_WORDPTR);
    uint32_t address = *(CTRLBUFFER_WORDPTR + 1);

    FPGA_releaselock();
    while(!FPGA_setlock(fpga_blockin));

    FPGA_setaddr(address);

    /* Do memory read */
    udi_vendor_bulk_in_run(
        (uint8_t *) PSRAM_BASE_ADDRESS,
        buflen,
        main_vendor_bulk_in_received
        );
    FPGA_releaselock();
}
/* ====================================================================================== */
void ctrl_readmem_ctrl(void){
    uint32_t buflen = *(CTRLBUFFER_WORDPTR);
    uint32_t address = *(CTRLBUFFER_WORDPTR + 1);

    FPGA_releaselock();
    while(!FPGA_setlock(fpga_ctrlmem));

    /* Set address */
    FPGA_setaddr(address);

    /* Do memory read */
    ctrlmemread_buf = (uint8_t *) PSRAM_BASE_ADDRESS;

    /* Set size to read */
    ctrlmemread_size = buflen;

    /* Start Transaction */
    FPGA_releaselock();
}
/* ====================================================================================== */
void ctrl_writemem_ctrl(void){
    uint32_t buflen = *(CTRLBUFFER_WORDPTR);
    uint32_t address = *(CTRLBUFFER_WORDPTR + 1);

    uint8_t * ctrlbuf_payload = (uint8_t *)(CTRLBUFFER_WORDPTR + 2);

    //printf("Writing to %x, %d\n", address, buflen);

    FPGA_releaselock();
    while(!FPGA_setlock(fpga_generic));

    /* Set address */
    FPGA_setaddr(address);

    /* Start Transaction */

    /* Do memory write */
    for(unsigned int i = 0; i < buflen; i++){
        xram[i] = ctrlbuf_payload[i];
    }

    FPGA_releaselock();
}
/* ====================================================================================== */
void ctrl_writemem_bulk(void){
//uint32_t buflen = *(CTRLBUFFER_WORDPTR);
    uint32_t address = *(CTRLBUFFER_WORDPTR + 1);

    // TODO: see block in
    FPGA_releaselock();
    while(!FPGA_setlock(fpga_blockout));

    /* Set address */
    FPGA_setaddr(address);

    /* Transaction done in generic callback */
    FPGA_releaselock();
}
/* ====================================================================================== */
static void ctrl_sam3ucfg_cb(void)
{
    switch(udd_g_ctrlreq.req.wValue & 0xFF)
    {
        /* Turn on slow clock */
    case 0x01:
        osc_enable(OSC_MAINCK_XTAL);
        osc_wait_ready(OSC_MAINCK_XTAL);
        pmc_switch_mck_to_mainck(CONFIG_SYSCLK_PRES);
        break;

        /* Turn off slow clock */
    case 0x02:
        pmc_switch_mck_to_pllack(CONFIG_SYSCLK_PRES);
        break;

        /* Jump to ROM-resident bootloader */
    case 0x03:
        /* Turn off connected stuff */
        //board_power(0);

        /* Clear ROM-mapping bit. */
        efc_perform_command(EFC0, EFC_FCMD_CGPB, 1);

        /* Disconnect USB (will kill connection) */
        udc_detach();

        /* With knowledge that I will rise again, I lay down my life. */
        while (RSTC->RSTC_SR & RSTC_SR_SRCMP);
        RSTC->RSTC_CR |= RSTC_CR_KEY(0xA5) | RSTC_CR_PERRST | RSTC_CR_PROCRST;
        while(1);
        break;
        /* Disconnect USB (will kill stuff) */

        /* Make the jump */
        break;

        /* Oh well, sucks to be you */
    default:
        break;
    }
}
/* ====================================================================================== */
void ctrl_progfpga_bulk(void){

    switch(udd_g_ctrlreq.req.wValue){
    case 0xA0:
        fpga_program_setup1();
        break;

    case 0xA1:
        /* Waiting on data... */
        fpga_program_setup2();
        blockendpoint_usage = bep_fpgabitstream;
        break;

    case 0xA2:
        /* Done */
        blockendpoint_usage = bep_emem;
        break;

    default:
        break;
    }
}
/* ====================================================================================== */
void ctrl_change_pwr(void) {
    switch(udd_g_ctrlreq.req.wValue) {
    case 0x00: //USB power off
        PIOA->PIO_CODR = (1 << F_VBHOST); //disable sniff power
        PIOA->PIO_CODR = (1 << F_VB5V); //disable host power
        USB_PWR_STATE = 0;
        break;
    case 0x01: //Use 5V power
        PIOA->PIO_CODR = (1 << F_VBHOST); //disable sniff power
        PIOA->PIO_SODR = (1 << F_VB5V); //enable host power
        USB_PWR_STATE = 1;
        break;
    case 0x02: //Use host power
        PIOA->PIO_CODR = (1 << F_VB5V); //disable host power
        PIOA->PIO_SODR = (1 << F_VBHOST); //enable sniff power
        USB_PWR_STATE = 2;
        break;
    }
}
/* ====================================================================================== */
void ctrl_fpga_reset(void) {
  gpio_set_pin_high(PIN_EBI_USB_SPARE0);
  gpio_set_pin_low(PIN_EBI_USB_SPARE0);
}

/* ====================================================================================== */
bool usb_vendor_setup_out_received(void)
{
    //Add buffer if used
    udd_g_ctrlreq.payload = ctrlbuffer;
    udd_g_ctrlreq.payload_size = min(udd_g_ctrlreq.req.wLength,	sizeof(ctrlbuffer));

    blockendpoint_usage = bep_emem;

    switch(udd_g_ctrlreq.req.bRequest){
        /* Memory Read */
    case REQ_MEMREAD_BULK:
        if (FPGA_setlock(fpga_usblocked)){
            udd_g_ctrlreq.callback = ctrl_readmem_bulk;
            return true;
        }
        break;
    case REQ_MEMREAD_CTRL:
        if (FPGA_setlock(fpga_usblocked)){
            udd_g_ctrlreq.callback = ctrl_readmem_ctrl;
            return true;
        }
        break;


        /* Memory Write */
    case REQ_MEMWRITE_BULK:
        if (FPGA_setlock(fpga_usblocked)){
            udd_g_ctrlreq.callback = ctrl_writemem_bulk;
            return true;
        }
        break;


    case REQ_MEMWRITE_CTRL:
        if (FPGA_setlock(fpga_usblocked)){
            udd_g_ctrlreq.callback = ctrl_writemem_ctrl;
            return true;
        }
        break;

    case REQ_FPGA_PROGRAM:
        udd_g_ctrlreq.callback = ctrl_progfpga_bulk;
        return true;

    case REQ_SAM3U_CFG:
        udd_g_ctrlreq.callback = ctrl_sam3ucfg_cb;
        return true;

    case REQ_CHANGE_PWR:
        //TODO
        udd_g_ctrlreq.callback = ctrl_change_pwr;
        return true;

    case REQ_FPGA_RESET:
        udd_g_ctrlreq.callback = ctrl_fpga_reset;
        return true;

    default:
        return false;
    }

    return false;
}


/*
  udd_g_ctrlreq.req.bRequest == 0
  && (udd_g_ctrlreq.req.bRequest == 0)
  && (0 != udd_g_ctrlreq.req.wLength)
*/

/* ====================================================================================== */
bool usb_vendor_setup_in_received(void)
{
    /*
      udd_g_ctrlreq.payload = main_buf_loopback;
      udd_g_ctrlreq.payload_size =
      min( udd_g_ctrlreq.req.wLength,
      sizeof(main_buf_loopback) );
    */

    static uint8_t  respbuf[64];

    switch(udd_g_ctrlreq.req.bRequest){
    case REQ_MEMREAD_CTRL:
        udd_g_ctrlreq.payload = ctrlmemread_buf;
        udd_g_ctrlreq.payload_size = ctrlmemread_size;
        ctrlmemread_size = 0;

        if (FPGA_lockstatus() == fpga_ctrlmem){
            FPGA_setlock(fpga_unlocked);
        }

        return true;
        break;

    case REQ_FW_VERSION:
        respbuf[0] = FW_VER_MAJOR;
        respbuf[1] = FW_VER_MINOR;
        respbuf[2] = FW_VER_DEBUG;
        udd_g_ctrlreq.payload = respbuf;
        udd_g_ctrlreq.payload_size = 3;
        return true;
        break;

    case REQ_FPGA_STATUS:
        respbuf[0] = FPGA_ISDONE();
        respbuf[1] = 0;
        respbuf[2] = 0;
        respbuf[3] = 0;
        udd_g_ctrlreq.payload = respbuf;
        udd_g_ctrlreq.payload_size = 4;
        return true;
        break;
    default:
        return false;
    }
    return false;
}
/* ====================================================================================== */
void main_vendor_bulk_in_received(udd_ep_status_t status,
                                  iram_size_t nb_transfered, udd_ep_id_t ep)
{
    UNUSED(nb_transfered);
    UNUSED(ep);
    if (UDD_EP_TRANSFER_OK != status) {
        return; // Transfer aborted/error
    }

    if (FPGA_lockstatus() == fpga_blockin){
        FPGA_setlock(fpga_unlocked);
    }
}
/* ====================================================================================== */
void main_vendor_bulk_out_received(udd_ep_status_t status,
                                   iram_size_t nb_transfered, udd_ep_id_t ep)
{
    UNUSED(ep);
    if (UDD_EP_TRANSFER_OK != status) {
        // Transfer aborted

        //restart
        udi_vendor_bulk_out_run(
            main_buf_loopback,
            sizeof(main_buf_loopback),
            main_vendor_bulk_out_received);

        return;
    }

    if (blockendpoint_usage == bep_emem){
        for(unsigned int i = 0; i < nb_transfered; i++){
            xram[i] = main_buf_loopback[i];
        }

        if (FPGA_lockstatus() == fpga_blockout){
            FPGA_releaselock();
        }
    } else if (blockendpoint_usage == bep_fpgabitstream){

        /* Send byte to FPGA - this could eventually be done via SPI */
        // TODO: is this dangerous?
        for(unsigned int i = 0; i < nb_transfered; i++){
            fpga_program_sendbyte(main_buf_loopback[i]);
        }
#if FPGA_USE_BITBANG
        FPGA_CCLK_LOW();
#endif
    }

    //printf("BULKOUT: %d bytes\n", (int)nb_transfered);

    udi_vendor_bulk_out_run(
        main_buf_loopback,
        sizeof(main_buf_loopback),
        main_vendor_bulk_out_received);
}
/* ====================================================================================== */
