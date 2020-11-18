/**
 * \file
 *
 * \brief USB CMSIS-DAP interface definitions.
 *
 * Code from NewAE based on code that was originally
 * Copyright (c) 2011-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef _UDI_CMSISDAP_H_
#define _UDI_CMSISDAP_H_

#include "conf_usb.h"
#include "usb_protocol.h"
#include "usb_cmsisdap_handler.h"
#include "udd.h"
#include "udc_desc.h"
#include "udi.h"

#ifdef __cplusplus
extern "C" {
#endif

// Configuration Full Speed check
#ifndef UDI_CMSISDAP_EPS_SIZE_BULK_FS
# error UDI_CMSISDAP_EPS_SIZE_BULK_FS must be defined in conf_usb.h file.
#endif

// Configuration High Speed check
#ifdef USB_DEVICE_HS_SUPPORT
#ifndef UDI_CMSISDAP_EPS_SIZE_BULK_OUT_HS
  #error UDI_CMSISDAP_EPS_SIZE_BULK_OUT_HS must be defined in conf_usb.h
#endif
#ifndef UDI_CMSISDAP_EPS_SIZE_BULK_IN1_HS
  #error UDI_CMSISDAP_EPS_SIZE_BULK_IN1_HS must be defined in conf_usb.h
#endif
#endif

#ifndef UDI_CMSISDAP_STRING_ID
#error UDI_CMSISDAP_STRING_ID is mandatory, and must contain the substring "CMSIS-DAP"
#endif

/**
 * \addtogroup udi_cmsisdap_group_udc
 * @{
 */
//! Global structure which contains standard UDI interface for UDC
extern UDC_DESC_STORAGE udi_api_t udi_api_cmsisdap;
//@}

/**
 * \ingroup udi_cmsisdap_group
 * \defgroup udi_cmsisdap_group_desc USB interface descriptors
 *
 * The following structures provide predefined USB interface descriptors.
 * It must be used to define the final USB descriptors.
 */
//@{

/**
 * \name Endpoint descriptors
 * @{
 */

#ifdef UDI_CMSISDAP_EP_BULK_IN2

  // ===================== DEFINITIONS FOR CASE OF WITH SWO ===============================

# define UDI_CMSISDAP_EPS_BULK_DESC \
	.ep_bulk_out.bLength               = sizeof(usb_ep_desc_t),\
	.ep_bulk_out.bDescriptorType       = USB_DT_ENDPOINT,\
	.ep_bulk_out.bEndpointAddress      = UDI_CMSISDAP_EP_BULK_OUT,\
	.ep_bulk_out.bmAttributes          = USB_EP_TYPE_BULK,\
	.ep_bulk_out.bInterval             = 0,\
	.ep_bulk_in1.bLength               = sizeof(usb_ep_desc_t),\
	.ep_bulk_in1.bDescriptorType       = USB_DT_ENDPOINT,\
	.ep_bulk_in1.bEndpointAddress      = UDI_CMSISDAP_EP_BULK_IN1,\
	.ep_bulk_in1.bmAttributes          = USB_EP_TYPE_BULK,\
	.ep_bulk_in1.bInterval             = 0,\
	.ep_bulk_in2.bLength               = sizeof(usb_ep_desc_t),\
	.ep_bulk_in2.bDescriptorType       = USB_DT_ENDPOINT,\
	.ep_bulk_in2.bEndpointAddress      = UDI_CMSISDAP_EP_BULK_IN2,\
	.ep_bulk_in2.bmAttributes          = USB_EP_TYPE_BULK,\
	.ep_bulk_in2.bInterval             = 0,\


# define UDI_CMSISDAP_EPS_BULK_DESC_FS \
	.ep_bulk_out.wMaxPacketSize        = LE16(UDI_CMSISDAP_EPS_SIZE_BULK_FS),\
	.ep_bulk_in1.wMaxPacketSize        = LE16(UDI_CMSISDAP_EPS_SIZE_BULK_FS),\
	.ep_bulk_in2.wMaxPacketSize        = LE16(UDI_CMSISDAP_EPS_SIZE_BULK_FS)


# define UDI_CMSISDAP_EPS_BULK_DESC_HS \
	.ep_bulk_out.wMaxPacketSize        = LE16(UDI_CMSISDAP_EPS_SIZE_BULK_OUT_HS),\
	.ep_bulk_in1.wMaxPacketSize        = LE16(UDI_CMSISDAP_EPS_SIZE_BULK_IN1_HS),\
	.ep_bulk_in2.wMaxPacketSize        = LE16(UDI_CMSISDAP_EPS_SIZE_BULK_IN2_HS)

//@}

//! Interface descriptor structure for cmsisdap Class interface
typedef struct {
	usb_iface_desc_t iface0;
	usb_ep_desc_t ep_bulk_out;
	usb_ep_desc_t ep_bulk_in1;
	usb_ep_desc_t ep_bulk_in2;  
} udi_cmsisdap_desc_t;

#define UDI_CMSISDAP_EP_NB_BULK 3

//! Content of cmsisdap interface descriptor for all speeds
#define UDI_CMSISDAP_DESC      \
	.iface0.bLength            = sizeof(usb_iface_desc_t),\
	.iface0.bDescriptorType    = USB_DT_INTERFACE,\
	.iface0.bInterfaceNumber   = UDI_CMSISDAP_IFACE_NUMBER,\
	.iface0.bAlternateSetting  = 0,\
	.iface0.bNumEndpoints      = 3,\
	.iface0.bInterfaceClass    = VENDOR_CLASS,\
	.iface0.bInterfaceSubClass = 0,\
	.iface0.bInterfaceProtocol = VENDOR_PROTOCOL,\
	.iface0.iInterface         = UDI_CMSISDAP_STRING_ID,\
	UDI_CMSISDAP_EPS_BULK_DESC
#else

  // ===================== DEFINITIONS FOR CASE OF NO SWO ===============================

# define UDI_CMSISDAP_EPS_BULK_DESC \
	.ep_bulk_out.bLength               = sizeof(usb_ep_desc_t),\
	.ep_bulk_out.bDescriptorType       = USB_DT_ENDPOINT,\
	.ep_bulk_out.bEndpointAddress      = UDI_CMSISDAP_EP_BULK_OUT,\
	.ep_bulk_out.bmAttributes          = USB_EP_TYPE_BULK,\
	.ep_bulk_out.bInterval             = 0,\
	.ep_bulk_in1.bLength               = sizeof(usb_ep_desc_t),\
	.ep_bulk_in1.bDescriptorType       = USB_DT_ENDPOINT,\
	.ep_bulk_in1.bEndpointAddress      = UDI_CMSISDAP_EP_BULK_IN1,\
	.ep_bulk_in1.bmAttributes          = USB_EP_TYPE_BULK,\
	.ep_bulk_in1.bInterval             = 0,\


# define UDI_CMSISDAP_EPS_BULK_DESC_FS \
	.ep_bulk_out.wMaxPacketSize        = LE16(UDI_CMSISDAP_EPS_SIZE_BULK_FS),\
	.ep_bulk_in1.wMaxPacketSize        = LE16(UDI_CMSISDAP_EPS_SIZE_BULK_FS),\


# define UDI_CMSISDAP_EPS_BULK_DESC_HS \
	.ep_bulk_out.wMaxPacketSize        = LE16(UDI_CMSISDAP_EPS_SIZE_BULK_OUT_HS),\
	.ep_bulk_in1.wMaxPacketSize        = LE16(UDI_CMSISDAP_EPS_SIZE_BULK_IN1_HS),\

//@}

//! Interface descriptor structure for cmsisdap Class interface
typedef struct {
	usb_iface_desc_t iface0;
	usb_ep_desc_t ep_bulk_out;
	usb_ep_desc_t ep_bulk_in1;
} udi_cmsisdap_desc_t;

#define UDI_CMSISDAP_EP_NB_BULK 2

//! Content of cmsisdap interface descriptor for all speeds
#define UDI_CMSISDAP_DESC      \
	.iface0.bLength            = sizeof(usb_iface_desc_t),\
	.iface0.bDescriptorType    = USB_DT_INTERFACE,\
	.iface0.bInterfaceNumber   = UDI_CMSISDAP_IFACE_NUMBER,\
	.iface0.bAlternateSetting  = 0,\
	.iface0.bNumEndpoints      = 2,\
	.iface0.bInterfaceClass    = VENDOR_CLASS,\
	.iface0.bInterfaceSubClass = 0,\
	.iface0.bInterfaceProtocol = VENDOR_PROTOCOL,\
	.iface0.iInterface         = UDI_CMSISDAP_STRING_ID,\
	UDI_CMSISDAP_EPS_BULK_DESC

  // ======================= END OF DEFINITION OPTION ===============================
#endif

//! Content of cmsisdap interface descriptor for full speed only
#define UDI_CMSISDAP_DESC_FS {\
	UDI_CMSISDAP_DESC \
	UDI_CMSISDAP_EPS_BULK_DESC_FS \
   }

//! Content of cmsisdap interface descriptor for high speed only
#define UDI_CMSISDAP_DESC_HS   {\
	UDI_CMSISDAP_DESC \
	UDI_CMSISDAP_EPS_BULK_DESC_HS \
   }
//@}


/**
 * \ingroup udi_group
 * \defgroup udi_cmsisdap_group USB Device Interface (UDI) for CMSIS-DAP Class
 *
 * Common APIs used by high level application to use this USB class.
 *
 * These routines are used to transfer data to/from USB CMSIS-DAPv2 endpoints.
 *
 * @{
 */

#if UDI_CMSISDAP_EPS_SIZE_BULK_FS || defined(__DOXYGEN__)
/**
 * \brief Start a transfer on bulk IN
 *
 * When the transfer is finished or aborted (stall, reset, ...), the \a callback is called.
 * The \a callback returns the transfer status and eventually the number of byte transfered.
 *
 * \param buf           Buffer on Internal RAM to send or fill.
 *                      It must be align, then use COMPILER_WORD_ALIGNED.
 * \param buf_size      Buffer size to send or fill
 * \param callback      NULL or function to call at the end of transfer
 *
 * \return \c 1 if function was successfully done, otherwise \c 0.
 */
bool udi_cmsisdap_bulk_in_run(uint8_t * buf, iram_size_t buf_size,
		udd_callback_trans_t callback);

#ifdef UDI_CMSISDAP_EP_BULK_IN2
bool udi_cmsisdap_bulk_swo_in_run(uint8_t * buf, iram_size_t buf_size,
		udd_callback_trans_t callback);
#endif

/**
 * \brief Start a transfer on bulk OUT
 *
 * When the transfer is finished or aborted (stall, reset, ...), the \a callback is called.
 * The \a callback returns the transfer status and eventually the number of byte transfered.
 *
 * \param buf           Buffer on Internal RAM to send or fill.
 *                      It must be align, then use COMPILER_WORD_ALIGNED.
 * \param buf_size      Buffer size to send or fill
 * \param callback      NULL or function to call at the end of transfer
 *
 * \return \c 1 if function was successfully done, otherwise \c 0.
 */
bool udi_cmsisdap_bulk_out_run(uint8_t * buf, iram_size_t buf_size,
		udd_callback_trans_t callback);
#endif

//@}

#ifdef __cplusplus
}
#endif
#endif // _UDI_CMSISDAP_H_
