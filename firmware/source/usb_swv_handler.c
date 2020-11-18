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
#include "usb_swv_handler.h"
#include "phyw_driver.h"
#include "DAP_config.h"
#include "DAP.h"
#include "DAP_queue.h"

#include "generics.h"

/* ====================================================================================== */

#ifdef SWV_SEPARATE  /* We only enable this endpoint type if we're sending swv over separate i/f */

static volatile bool _swv_enable;        /* Flag indicating this endpoint is enabled */

#define NO_SWOBUFFERS 3
static uint8_t swobuffer[NO_SWOBUFFERS][UDI_SWV_EPS_SIZE_BULK_IN_HS];
static uint8_t rdBuff;
static uint8_t wrBuff;
static uint32_t wrPos;

static volatile bool _txRunning;

/* ====================================================================================== */
/* ====================================================================================== */
/* ====================================================================================== */
/* Internal Routines                                                                      */
/* ====================================================================================== */
/* ====================================================================================== */
/* ====================================================================================== */
static void _swv_bulk_in_received( udd_ep_status_t status,
                                   iram_size_t nb_transfered, udd_ep_id_t ep )
{
    UNUSED( nb_transfered );
    UNUSED( ep );

    /* Are there any data waiting to go? Here we don't bother sending incomplete packets, only full ones */
    if ( rdBuff != wrBuff )
    {
        /* Yes, so adjust pointers then send it */
        uint8_t nrdBuff = ( rdBuff + 1 ) % NO_SWOBUFFERS;
        udi_swv_bulk_in_run( swobuffer[rdBuff], UDI_SWV_EPS_SIZE_BULK_IN_HS, _swv_bulk_in_received );
        rdBuff = nrdBuff;
    }
    else
    {
        _txRunning = false;
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

bool usb_swv_setup_out_received( void )
{
    DBG( "SWV setup out received" EOL );
    return false;
    //    udd_g_ctrlreq.payload = cmsis_buf;
    //udd_g_ctrlreq.payload_size = min( udd_g_ctrlreq.req.wLength, CMSISDAP_BUFFER_SIZE );
    //udd_g_ctrlreq.callback = _swv_msg_arrived;
    //return true;
}

/* ====================================================================================== */
/*! \brief Manage the reception of setup request IN
 *
 * \retval true if request accepted
 */

bool usb_swv_setup_in_received( void )
{
    DBG( "SWV Setup IN" EOL );

    return true;
}
/* ====================================================================================== */
/*! \brief Notify via user interface that enumeration is ok
 * This is called by swv interface when USB Host enable it.
 *
 * \retval true if swv startup is successfully done
 */

bool usb_swv_enable( void )
{
    DBG( "SWV Enable called" EOL );
    _swv_enable = true;
    _txRunning = false;
    return true;
}
/* ====================================================================================== */

/*! \brief Notify via user interface that enumeration is disabled
 * This is called by swv interface when USB Host disable it.
 */

void usb_swv_disable( void )
{
    _swv_enable = false;
    _txRunning = false;
}
/* ====================================================================================== */
void usb_swv_swo_send( uint8_t c )

{
    swobuffer[wrBuff][wrPos++] = c;

    if ( UDI_SWV_EPS_SIZE_BULK_IN_HS == wrPos )
    {
        irqflags_t flags = cpu_irq_save();

        if ( _txRunning == false )
        {
            uint8_t nrdBuff = ( rdBuff + 1 ) % NO_SWOBUFFERS;
            udi_swv_bulk_in_run( swobuffer[rdBuff], wrPos, _swv_bulk_in_received );
            rdBuff = nrdBuff;
            _txRunning = true;
        }

        wrPos = 0;
        wrBuff = ( wrBuff + 1 ) % NO_SWOBUFFERS;

        cpu_irq_restore( flags );
    }
}
/* ====================================================================================== */
#endif
