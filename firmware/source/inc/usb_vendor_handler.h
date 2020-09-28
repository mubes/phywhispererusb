/*
 Copyright (c) 2015 NewAE Technology Inc. All rights reserved.

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


#ifndef _USB_VENDOR_HANDLER_
#define _USB_VENDOR_HANDLER_


/* Size of loopback */
#define  MAIN_LOOPBACK_SIZE    1024

extern volatile bool g_captureinprogress;

/* ====================================================================================== */
/*! \brief Notify via user interface that enumeration is ok
 * This is called by vendor interface when USB Host enable it.
 *
 * \retval true if vendor startup is successfully done
 */

bool usb_vendor_enable( void );

/*! \brief Notify via user interface that enumeration is disabled
 * This is called by vendor interface when USB Host disable it.
 */

void usb_vendor_disable( void );

/*! \brief Manage the reception of setup request OUT
 *
 * \retval true if request accepted
 */
bool usb_vendor_setup_out_received( void );

/*! \brief Manage the reception of setup request IN
 *
 * \retval true if request accepted
 */
bool usb_vendor_setup_in_received( void );

/* ====================================================================================== */

#endif /* _USB_VENDOR_HANDLER_ */
