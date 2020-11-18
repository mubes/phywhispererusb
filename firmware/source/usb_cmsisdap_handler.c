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
#include "usb_cmsisdap_handler.h"
#include "phyw_driver.h"
#include "DAP_config.h"
#include "DAP.h"
#include "DAP_queue.h"

#include "generics.h"

/* ====================================================================================== */

static volatile bool _cmsisdap_enable;        /* Flag indicating this endpoint is enabled */

/* Buffer for incoming data block */
#define CMSISDAP_BUFFER_SIZE UDI_CMSISDAP_EPS_SIZE_BULK_IN1_HS
static uint8_t cmsis_buf[CMSISDAP_BUFFER_SIZE];
static DAP_queue DAP_Cmd_queue;

#ifdef UDI_CMSISDAP_EP_BULK_IN2
    #define NO_SWOBUFFERS 3
    static uint8_t swobuffer[NO_SWOBUFFERS][UDI_CMSISDAP_EPS_SIZE_BULK_IN2_HS];
    static uint8_t rdBuff;
    static uint8_t wrBuff;
    static uint32_t wrPos;

    volatile bool txRunning;
#endif

/* ====================================================================================== */
/* ====================================================================================== */
/* ====================================================================================== */
/* Internal Routines                                                                      */
/* ====================================================================================== */
/* ====================================================================================== */
/* ====================================================================================== */
static void _cmsisdap_bulk_in_received( udd_ep_status_t status,
                                        iram_size_t nb_transfered, udd_ep_id_t ep )
{
    UNUSED( nb_transfered );
    UNUSED( ep );
    UNUSED( status );

#ifdef UDI_CMSISDAP_EP_BULK_IN2
    /* We don't worry about the SWD endpoint, because only single buffers are sent back */
    /* So only the SWO endpoint to be considered here. */

    if ( UDI_CMSISDAP_EP_BULK_IN2 == ep )
    {
        /* Are there any data waiting to go? */
        if ( ( rdBuff != wrBuff ) || ( wrPos > 0 ) )
        {
            /* Yes, so adjust pointers then send it */
            uint8_t nrdBuff = ( rdBuff + 1 ) % NO_SWOBUFFERS;

            udi_cmsisdap_bulk_swo_in_run( swobuffer[rdBuff], ( wrBuff == nrdBuff ) ? wrPos : UDI_CMSISDAP_EPS_SIZE_BULK_IN2_HS, _cmsisdap_bulk_in_received );
            wrPos = 0;
            wrBuff = ( wrBuff + 1 ) % NO_SWOBUFFERS;
            rdBuff = nrdBuff;
        }
        else
        {
            txRunning = false;
        }
    }

#endif
}
/* ====================================================================================== */
static void _cmsisdap_bulk_out_received( udd_ep_status_t status,
        iram_size_t nb_transfered, udd_ep_id_t ep )
{
    UNUSED( ep );
    uint8_t *rbuf;

    uint8_t *sbuf;
    int slen;

    if ( UDD_EP_TRANSFER_OK == status )
    {
        DAP_queue_execute_buf( &DAP_Cmd_queue, cmsis_buf, CMSISDAP_BUFFER_SIZE, &rbuf );

        if ( DAP_queue_get_send_buf( &DAP_Cmd_queue, &sbuf, &slen ) )
        {
            /* Better schedule a message back with the result */
            udi_cmsisdap_bulk_in_run(
                        sbuf,
                        slen,
                        _cmsisdap_bulk_in_received
            );
        }
    }

    /* Queue the next one */
    udi_cmsisdap_bulk_out_run(
                cmsis_buf,
                CMSISDAP_BUFFER_SIZE,
                _cmsisdap_bulk_out_received );
}
/* ====================================================================================== */
static void _cmsisdap_msg_arrived( void )

{
    DBG( "CTRL MESSAGE ARRIVED" EOL );
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

bool usb_cmsisdap_setup_out_received( void )
{
    DBG( "CMSISDAP setup out received" EOL );
    udd_g_ctrlreq.payload = cmsis_buf;
    udd_g_ctrlreq.payload_size = min( udd_g_ctrlreq.req.wLength, CMSISDAP_BUFFER_SIZE );
    udd_g_ctrlreq.callback = _cmsisdap_msg_arrived;

    return true;
}

/* ====================================================================================== */
/*! \brief Manage the reception of setup request IN
 *
 * \retval true if request accepted
 */

bool usb_cmsisdap_setup_in_received( void )
{
    DBG( "CMSISDAP Setup IN" EOL );

    return true;
}
/* ====================================================================================== */
/*! \brief Notify via user interface that enumeration is ok
 * This is called by cmsisdap interface when USB Host enable it.
 *
 * \retval true if cmsisdap startup is successfully done
 */

bool usb_cmsisdap_enable( void )
{
    DBG( "CMSISDAP Enable called" EOL );
    _cmsisdap_enable = true;
    DAP_queue_init( &DAP_Cmd_queue );
    udi_cmsisdap_bulk_out_run(
                cmsis_buf,
                CMSISDAP_BUFFER_SIZE,
                _cmsisdap_bulk_out_received );

    return true;
}
/* ====================================================================================== */

/*! \brief Notify via user interface that enumeration is disabled
 * This is called by cmsisdap interface when USB Host disable it.
 */

void usb_cmsisdap_disable( void )
{
    _cmsisdap_enable = false;
}
/* ====================================================================================== */

#ifdef UDI_CMSISDAP_EP_BULK_IN2

void usb_cmsisdap_swo_send( uint8_t c )

{
    irqflags_t flags;

    flags = cpu_irq_save();
    swobuffer[wrBuff][wrPos++] = c;

    if ( UDI_CMSISDAP_EPS_SIZE_BULK_IN2_HS == wrPos )
    {
        wrPos = 0;
        wrBuff = ( wrBuff + 1 ) % NO_SWOBUFFERS;
    }

    if ( txRunning == false )
    {
        uint8_t nrdBuff = ( rdBuff + 1 ) % NO_SWOBUFFERS;
        udi_cmsisdap_bulk_swo_in_run( swobuffer[rdBuff], wrPos, _cmsisdap_bulk_in_received );
        wrPos = 0;
        wrBuff = ( wrBuff + 1 ) % NO_SWOBUFFERS;
        rdBuff = nrdBuff;
        txRunning = true;
    }

    cpu_irq_restore( flags );
}

#endif

/* ====================================================================================== */
