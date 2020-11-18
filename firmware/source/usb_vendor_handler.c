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
#include "genclk.h"
#include "usb_vendor_handler.h"
#include "usb_xmem.h"
#include "fpga_program.h"
#include "phyw_driver.h"
#include "led_states.h"

/* Firmware protocol version */
#define FW_VER_MAJOR 1
#define FW_VER_MINOR 1
#define FW_VER_DEBUG 0

/* Protocol message types */
#define REQ_MEMREAD_BULK      0x10
#define REQ_MEMWRITE_BULK     0x11
#define REQ_MEMREAD_CTRL      0x12
#define REQ_MEMWRITE_CTRL     0x13
#define REQ_MEMSTREAM         0x14
#define REQ_FPGA_STATUS       0x15
#define REQ_FPGA_PROGRAM      0x16
#define REQ_FW_VERSION        0x17
#define REQ_USART0_DATA       0x1A
#define REQ_USART0_CONFIG     0x1B
#define REQ_SCARD_DATA        0x1C
#define REQ_SCARD_CONFIG      0x1D
#define REQ_SCARD_AUX         0x1E
#define REQ_USART2DUMP_ENABLE 0x1F
#define REQ_XMEGA_PROGRAM     0x20
#define REQ_AVR_PROGRAM       0x21
#define REQ_SAM3U_CFG         0x22
#define REQ_CC_PROGRAM        0x23
#define REQ_CHANGE_PWR        0x24
#define REQ_FPGA_RESET        0x25

/* Size of loopback */
#define  MAIN_LOOPBACK_SIZE    1024

typedef enum
{
    bep_emem = 0,
    bep_fpgabitstream = 10
} blockep_usage_t;

#define CTRLBUFFER_WORDPTR ((uint32_t *) ((void *)ctrlbuffer))

/* ====================================================================================== */

static volatile bool _vendor_enable;          /* Flag indicating this endpoint is enabled */

COMPILER_WORD_ALIGNED
static uint8_t main_buf_loopback[MAIN_LOOPBACK_SIZE];

COMPILER_WORD_ALIGNED static uint8_t ctrlbuffer[64];

static blockep_usage_t blockendpoint_usage = bep_emem;

static uint8_t *ctrlmemread_buf;
static unsigned int ctrlmemread_size;

/* ====================================================================================== */
/* ====================================================================================== */
/* ====================================================================================== */
/* Internal Routines                                                                      */
/* ====================================================================================== */
/* ====================================================================================== */
/* ====================================================================================== */
static void _vendor_bulk_in_received( udd_ep_status_t status,
                                      iram_size_t nb_transfered, udd_ep_id_t ep )
{
    UNUSED( nb_transfered );
    UNUSED( ep );

    if ( UDD_EP_TRANSFER_OK != status )
    {
        return; // Transfer aborted/error
    }

    if ( FPGA_lockstatus() == fpga_blockin )
    {
        FPGA_setlock( fpga_unlocked );
    }
}
/* ====================================================================================== */
static void _vendor_bulk_out_received( udd_ep_status_t status,
                                       iram_size_t nb_transfered, udd_ep_id_t ep )
{
    UNUSED( ep );

    if ( UDD_EP_TRANSFER_OK == status )
    {
        if ( blockendpoint_usage == bep_emem )
        {
            for ( unsigned int i = 0; i < nb_transfered; i++ )
            {
                xram[i] = main_buf_loopback[i];
            }

            if ( FPGA_lockstatus() == fpga_blockout )
            {
                FPGA_releaselock();
            }
        }
        else if ( blockendpoint_usage == bep_fpgabitstream )
        {

            /* Send byte to FPGA - this could eventually be done via SPI */
            // TODO: is this dangerous?
            for ( unsigned int i = 0; i < nb_transfered; i++ )
            {
                fpga_program_sendbyte( main_buf_loopback[i] );
            }

#if FPGA_USE_BITBANG
            FPGA_CCLK_LOW();
#endif
        }
    }

    udi_vendor_bulk_out_run(
                main_buf_loopback,
                sizeof( main_buf_loopback ),
                _vendor_bulk_out_received );
}
/* ====================================================================================== */
static void _ctrl_readmem_bulk( void )
{
    uint32_t buflen = *( CTRLBUFFER_WORDPTR );
    uint32_t address = *( CTRLBUFFER_WORDPTR + 1 );

    FPGA_releaselock();

    while ( !FPGA_setlock( fpga_blockin ) );

    FPGA_setaddr( address );

    /* Do memory read */
    udi_vendor_bulk_in_run(
                ( uint8_t * ) PSRAM_BASE_ADDRESS,
                buflen,
                _vendor_bulk_in_received
    );
    FPGA_releaselock();
}
/* ====================================================================================== */
static void _ctrl_readmem_ctrl( void )
{
    uint32_t buflen = *( CTRLBUFFER_WORDPTR );
    uint32_t address = *( CTRLBUFFER_WORDPTR + 1 );

    FPGA_releaselock();

    while ( !FPGA_setlock( fpga_ctrlmem ) );

    /* Set address */
    FPGA_setaddr( address );

    /* Do memory read */
    ctrlmemread_buf = ( uint8_t * ) PSRAM_BASE_ADDRESS;

    /* Set size to read */
    ctrlmemread_size = buflen;

    /* Start Transaction */
    FPGA_releaselock();
}
/* ====================================================================================== */
static void _ctrl_writemem_ctrl( void )
{
    uint32_t buflen = *( CTRLBUFFER_WORDPTR );
    uint32_t address = *( CTRLBUFFER_WORDPTR + 1 );

    uint8_t *ctrlbuf_payload = ( uint8_t * )( CTRLBUFFER_WORDPTR + 2 );

    FPGA_releaselock();

    while ( !FPGA_setlock( fpga_generic ) );

    /* Set address */
    FPGA_setaddr( address );

    /* Start Transaction */

    /* Do memory write */
    for ( unsigned int i = 0; i < buflen; i++ )
    {
        xram[i] = ctrlbuf_payload[i];
    }

    FPGA_releaselock();
}
/* ====================================================================================== */
static void _ctrl_writemem_bulk( void )
{
    //uint32_t buflen = *(CTRLBUFFER_WORDPTR);
    uint32_t address = *( CTRLBUFFER_WORDPTR + 1 );

    // TODO: see block in
    FPGA_releaselock();

    while ( !FPGA_setlock( fpga_blockout ) );

    /* Set address */
    FPGA_setaddr( address );

    /* Transaction done in generic callback */
    FPGA_releaselock();
}
/* ====================================================================================== */
static void ctrl_sam3ucfg_cb( void )
{
    switch ( udd_g_ctrlreq.req.wValue & 0xFF )
    {
        case 0x01: /* Turn on slow clock ------------------------------------- */

            osc_enable( OSC_MAINCK_XTAL );
            osc_wait_ready( OSC_MAINCK_XTAL );
            pmc_switch_mck_to_mainck( CONFIG_SYSCLK_PRES );
            break;

        case 0x02: /* Turn off slow clock ------------------------------------ */

            pmc_switch_mck_to_pllack( CONFIG_SYSCLK_PRES );
            break;


        case 0x03: /* Jump to ROM-resident bootloader ------------------------ */
            /* Turn off connected stuff */
            //board_power(0);

            /* Clear ROM-mapping bit. */
            efc_perform_command( EFC0, EFC_FCMD_CGPB, 1 );

            /* Disconnect USB (will kill connection) */
            udc_detach();

            /* With knowledge that I will rise again, I lay down my life. */
            while ( RSTC->RSTC_SR & RSTC_SR_SRCMP );

            RSTC->RSTC_CR |= RSTC_CR_KEY( 0xA5 ) | RSTC_CR_PERRST | RSTC_CR_PROCRST;

            while ( 1 );

            break;
            /* Disconnect USB (will kill stuff) */

            /* Make the jump */
            break;


        default: /* Oh well, sucks to be you --------------------------------- */
            break;
    }
}
/* ====================================================================================== */
static void _ctrl_progfpga_bulk( void )
{

    switch ( udd_g_ctrlreq.req.wValue )
    {
        case 0xA0: /* First stage setup -------------------------------------- */
            fpga_program_setup1();
            break;

        case 0xA1: /* Waiting on data ---------------------------------------- */
            fpga_program_setup2();
            blockendpoint_usage = bep_fpgabitstream;
            break;

        case 0xA2: /* Done --------------------------------------------------- */
            blockendpoint_usage = bep_emem;

            /* See if it programmed OK */
            check_fpga();
            break;

        default: /* ---------------------------------------------------------- */
            break;
    }
}
/* ====================================================================================== */
static void _ctrl_change_pwr( void )
{
    switch ( udd_g_ctrlreq.req.wValue )
    {
        case 0x00: /* USB power off ----------------------------------------- */
            phyw_driver_set_pwr_source( PWR_OFF );
            phyw_driver_set_pwr_on( false );
            break;

        case 0x01: /* Use 5V power ------------------------------------------ */
            phyw_driver_set_pwr_source( PWR_5V );
            phyw_driver_set_pwr_on( true );
            break;

        case 0x02: /* Use host power ---------------------------------------- */
            phyw_driver_set_pwr_source( PWR_HOST );
            phyw_driver_set_pwr_on( true );
            break;

        default:
            break;
    }
}
/* ====================================================================================== */
/* ====================================================================================== */
/* ====================================================================================== */
/* Publically accessible routines                                                         */
/* ====================================================================================== */
/* ====================================================================================== */
/* ====================================================================================== */
/*! \brief Manage the reception of setup request OUT
 *
 * \retval true if request accepted
 */

bool usb_vendor_setup_out_received( void )
{
    //Add buffer if used
    udd_g_ctrlreq.payload = ctrlbuffer;
    udd_g_ctrlreq.payload_size = min( udd_g_ctrlreq.req.wLength, sizeof( ctrlbuffer ) );

    blockendpoint_usage = bep_emem;

    switch ( udd_g_ctrlreq.req.bRequest )
    {
        case REQ_MEMREAD_BULK: /* Memory Read (Bulk) --------------------------- */

            if ( FPGA_setlock( fpga_usblocked ) )
            {
                udd_g_ctrlreq.callback = _ctrl_readmem_bulk;
                return true;
            }

            break;

        case REQ_MEMREAD_CTRL: /* Memory Read (Ctrl) --------------------------- */
            if ( FPGA_setlock( fpga_usblocked ) )
            {
                udd_g_ctrlreq.callback = _ctrl_readmem_ctrl;
                return true;
            }

            break;

        case REQ_MEMWRITE_BULK: /* Memory Write (Bulk) ------------------------- */

            if ( FPGA_setlock( fpga_usblocked ) )
            {
                udd_g_ctrlreq.callback = _ctrl_writemem_bulk;
                return true;
            }

            break;

        case REQ_MEMWRITE_CTRL: /* Memory Write (Ctrl) ------------------------- */
            if ( FPGA_setlock( fpga_usblocked ) )
            {
                udd_g_ctrlreq.callback = _ctrl_writemem_ctrl;
                return true;
            }

            break;

        case REQ_FPGA_PROGRAM: /* Request FPGA Program ------------------------- */
            udd_g_ctrlreq.callback = _ctrl_progfpga_bulk;
            return true;

        case REQ_SAM3U_CFG: /* Request SAM3U reconfigure ----------------------- */
            udd_g_ctrlreq.callback = ctrl_sam3ucfg_cb;
            return true;

        case REQ_CHANGE_PWR: /* Request power change --------------------------- */
            //TODO
            udd_g_ctrlreq.callback = _ctrl_change_pwr;
            return true;

        case REQ_FPGA_RESET: /* Request FPGA reset ----------------------------- */
            udd_g_ctrlreq.callback = FPGA_reset;
            return true;

        default: /* ------------------------------------------------------------ */
            return false;
    }

    return false;
}

/* ====================================================================================== */
/*! \brief Manage the reception of setup request IN
 *
 * \retval true if request accepted
 */

bool usb_vendor_setup_in_received( void )
{
    static uint8_t  respbuf[64];

    switch ( udd_g_ctrlreq.req.bRequest )
    {
        case REQ_MEMREAD_CTRL: /* Memory Read (Ctrl) ------------------------- */
            udd_g_ctrlreq.payload = ctrlmemread_buf;
            udd_g_ctrlreq.payload_size = ctrlmemread_size;
            ctrlmemread_size = 0;

            if ( FPGA_lockstatus() == fpga_ctrlmem )
            {
                FPGA_setlock( fpga_unlocked );
            }

            return true;
            break;

        case REQ_FW_VERSION: /* Get Firmware Version ------------------------- */
            respbuf[0] = FW_VER_MAJOR;
            respbuf[1] = FW_VER_MINOR;
            respbuf[2] = FW_VER_DEBUG;
            udd_g_ctrlreq.payload = respbuf;
            udd_g_ctrlreq.payload_size = 3;
            return true;
            break;

        case REQ_FPGA_STATUS: /* Get FPGA Status ----------------------------- */
            respbuf[0] = FPGA_ISDONE();
            respbuf[1] = 0;
            respbuf[2] = 0;
            respbuf[3] = 0;
            udd_g_ctrlreq.payload = respbuf;
            udd_g_ctrlreq.payload_size = 4;
            return true;
            break;

        default: /* ---------------------------------------------------------- */
            return false;
    }

    return false;
}
/* ====================================================================================== */
/*! \brief Notify via user interface that enumeration is ok
 * This is called by vendor interface when USB Host enable it.
 *
 * \retval true if vendor startup is successfully done
 */

bool usb_vendor_enable( void )
{
    _vendor_enable = true;

    // Start data reception on OUT endpoint
    udi_vendor_bulk_out_run(
                main_buf_loopback,
                sizeof( main_buf_loopback ),
                _vendor_bulk_out_received );
    return true;
}
/* ====================================================================================== */

/*! \brief Notify via user interface that enumeration is disabled
 * This is called by vendor interface when USB Host disable it.
 */

void usb_vendor_disable( void )
{
    _vendor_enable = false;
}
/* ====================================================================================== */
