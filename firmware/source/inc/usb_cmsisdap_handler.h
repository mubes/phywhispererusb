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


#ifndef _USB_CMSISDAP_HANDLER_
#define _USB_CMSISDAP_HANDLER_

/* ====================================================================================== */

bool usb_cmsisdap_enable( void );
void usb_cmsisdap_disable( void );
bool usb_cmsisdap_setup_out_received( void );
bool usb_cmsisdap_setup_in_received( void );

/* ====================================================================================== */

#endif /* _USB_CMSISDAP_HANDLER_ */
