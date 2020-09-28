/**
 * \file
 *
 * \brief USB configuration file
 *
 * Copyright (c) 2011-2014 Atmel Corporation. All rights reserved.
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

#ifndef _CONF_USB_H_
#define _CONF_USB_H_

#include "compiler.h"


/**
 * USB Device Configuration
 * @{
 */

//! Device definition (mandatory)
#define  USB_DEVICE_VENDOR_ID             0x2B3E
#define  USB_DEVICE_PRODUCT_ID            0xC610
#define  USB_DEVICE_MAJOR_VERSION         1
#define  USB_DEVICE_MINOR_VERSION         0
#define  USB_DEVICE_POWER                 500 // Consumption on Vbus line (mA)
#define  USB_DEVICE_ATTR                  USB_CONFIG_ATTR_BUS_POWERED

//! USB Device string definitions

extern char usb_serial_number[33];

#define  USB_DEVICE_MANUFACTURE_NAME      "NewAE Technology Inc."
#define  USB_DEVICE_PRODUCT_NAME          "PhyWhisperer-UDT"
#define  USB_DEVICE_GET_SERIAL_NAME_POINTER usb_serial_number
#define  USB_DEVICE_GET_SERIAL_NAME_LENGTH 32

/**
 * Device speeds support
 * Low speed not supported by this vendor class
 * @{
 */
#  define  USB_DEVICE_HS_SUPPORT
//@}

/**
 * USB Device Callbacks definitions
 * @{
 */
#define  UDC_VBUS_EVENT(b_vbus_high)
#define  UDC_SOF_EVENT()                  usb_main_sof_action()
#define  UDC_SUSPEND_EVENT()              usb_main_suspend_action()
#define  UDC_RESUME_EVENT()               usb_main_resume_action()
#define  UDC_GET_EXTRA_STRING()           usb_main_extra_string()
//@}

/**
 * USB Device low level configuration
 * For composite device, these configuration must be defined here
 * @{
 */
//! Control endpoint size
#define  USB_DEVICE_EP_CTRL_SIZE       64

//! Four interfaces for this device (CDC COM + CDC DATA + Vendor + HID)
#define  USB_DEVICE_NB_INTERFACE       4

//! 6 endpoints used by CDC, Vendor and HID interfaces (HID OUT in EP0)
#  define  USB_DEVICE_MAX_EP           6

// In HS mode, size of bulk endpoints are 512
// If CDC and Vendor endpoints all uses 2 banks, DPRAM is not enough: 4 bulk
// endpoints requires 4K bytes. So reduce the number of banks of CDC bulk
// endpoints to use less DPRAM. Keep Vendor setting to keep performance.
#     define  UDD_BULK_NB_BANK(ep) 1

//((ep == 5 || ep== 6) ? 1 : 2)
//@}

//@}


/**
 * USB Interface Configuration
 * @{
 */

/**
 * Configuration of CDC interface
 * @{
 */

//! Define two USB communication ports
#define  UDI_CDC_PORT_NB 1

//! Interface callback definition
#define  UDI_CDC_ENABLE_EXT(port)         usb_cdc_enable(port)
#define  UDI_CDC_DISABLE_EXT(port)        usb_cdc_disable(port)
#define  UDI_CDC_RX_NOTIFY(port)          usb_cdc_uart_rx_notify(port)
#define  UDI_CDC_TX_EMPTY_NOTIFY(port)
#define  UDI_CDC_SET_CODING_EXT(port,cfg) usb_cdc_uart_config(port,cfg)
#define  UDI_CDC_SET_DTR_EXT(port,set)    usb_cdc_set_dtr(port,set)
#define  UDI_CDC_SET_RTS_EXT(port,set)

//! Define it when the transfer CDC Device to Host is a low rate (<512000 bauds)
//! to reduce CDC buffers size
//#define  UDI_CDC_LOW_RATE

//! Default configuration of communication port
#define  UDI_CDC_DEFAULT_RATE             115200
#define  UDI_CDC_DEFAULT_STOPBITS         CDC_STOP_BITS_1
#define  UDI_CDC_DEFAULT_PARITY           CDC_PAR_NONE
#define  UDI_CDC_DEFAULT_DATABITS         8

//! Enable id string of interface to add an extra USB string
#define UDI_CDC_IAD_STRING_ID             4
#define UDI_CDC_COMM_STRING_ID_0          5
#define UDI_CDC_DATA_STRING_ID_0          6
/**
 * USB CDC low level configuration
 * In standalone these configurations are defined by the CDC module.
 * For composite device, these configuration must be defined here
 * @{
 */
//! Endpoint numbers definition
#  define  UDI_CDC_COMM_EP_0             (3 | USB_EP_DIR_IN) // Notify endpoint
#  define  UDI_CDC_DATA_EP_IN_0          (6 | USB_EP_DIR_IN) // TX
#  define  UDI_CDC_DATA_EP_OUT_0         (5 | USB_EP_DIR_OUT)// RX

//! Interface numbers
#define  UDI_CDC_COMM_IFACE_NUMBER_0   1
#define  UDI_CDC_DATA_IFACE_NUMBER_0   2
//@}
//@}

/**
 * Configuration of vendor interface
 * @{
 */
//! Interface callback definition
#define UDI_VENDOR_ENABLE_EXT()           usb_vendor_enable()
#define UDI_VENDOR_DISABLE_EXT()          usb_vendor_disable()
#define UDI_VENDOR_SETUP_OUT_RECEIVED()   usb_vendor_setup_out_received()
#define UDI_VENDOR_SETUP_IN_RECEIVED()    usb_vendor_setup_in_received()

//! endpoints size for full speed
//! Note: Disable the endpoints of a type, if size equal 0
#define UDI_VENDOR_EPS_SIZE_INT_FS    0
#define UDI_VENDOR_EPS_SIZE_BULK_FS   64
#define UDI_VENDOR_EPS_SIZE_ISO_FS    0 

//! endpoints size for high speed
#define UDI_VENDOR_EPS_SIZE_INT_HS    0
#define UDI_VENDOR_EPS_SIZE_BULK_HS   512
#define UDI_VENDOR_EPS_SIZE_ISO_HS    0

// String to describe interface
#define UDI_VENDOR_STRING_ID          7

//! Endpoint numbers definition
#define  UDI_VENDOR_EP_BULK_IN       (1 | USB_EP_DIR_IN)
#define  UDI_VENDOR_EP_BULK_OUT      (2 | USB_EP_DIR_OUT)

//! Interface number
#define  UDI_VENDOR_IFACE_NUMBER     0

//@}

/**
 * Configuration of HID Generic interface
 * @{
 */
//! Interface callback definition
#define  UDI_HID_GENERIC_ENABLE_EXT()        usb_hid_generic_enable()
#define  UDI_HID_GENERIC_DISABLE_EXT()       usb_hid_generic_disable()
#define  UDI_HID_GENERIC_REPORT_OUT(ptr)     usb_hid_generic_report_out(ptr)
#define  UDI_HID_GENERIC_SET_FEATURE(report) usb_hid_generic_set_feature(report)

//! Sizes of I/O reports
#define  UDI_HID_REPORT_IN_SIZE             8
#define  UDI_HID_REPORT_OUT_SIZE            8
#define  UDI_HID_REPORT_FEATURE_SIZE        4

//! Sizes of I/O endpoints
#define  UDI_HID_GENERIC_EP_SIZE            8

#define UDI_HID_GENERIC_STRING_ID           8

#define UDI_HID_GENERIC_EP_IN               ( 4 | USB_EP_DIR_IN )
#define UDI_HID_GENERIC_EP_OUT              0

#define UDI_HID_GENERIC_IFACE_NUMBER        3
//@}

//@}



/**
 * Description of Composite Device
 * @{
 */
//! USB Interfaces descriptor structure
#define UDI_COMPOSITE_DESC_T \
        udi_vendor_desc_t      udi_vendor;      \
        usb_iad_desc_t         udi_cdc_iad;     \
        udi_cdc_comm_desc_t    udi_cdc_comm;    \
        udi_cdc_data_desc_t    udi_cdc_data;    \
        udi_hid_generic_desc_t udi_hid_generic;        

//! USB Interfaces descriptor value for Full Speed
#define UDI_COMPOSITE_DESC_FS \
        .udi_vendor           = UDI_VENDOR_DESC_FS,     \
        .udi_cdc_iad          = UDI_CDC_IAD_DESC_0,     \
        .udi_cdc_comm         = UDI_CDC_COMM_DESC_0,    \
        .udi_cdc_data         = UDI_CDC_DATA_DESC_0_FS, \
        .udi_hid_generic      = UDI_HID_GENERIC_DESC        

        

//! USB Interfaces descriptor value for High Speed
#define UDI_COMPOSITE_DESC_HS \
        .udi_vendor           = UDI_VENDOR_DESC_HS,     \
        .udi_cdc_iad          = UDI_CDC_IAD_DESC_0,     \
        .udi_cdc_comm         = UDI_CDC_COMM_DESC_0,    \
        .udi_cdc_data         = UDI_CDC_DATA_DESC_0_HS, \
        .udi_hid_generic      = UDI_HID_GENERIC_DESC
        

//! USB Interface APIs
#define UDI_COMPOSITE_API \
        &udi_api_vendor,  \
        &udi_api_cdc_comm, \
        &udi_api_cdc_data, \
        &udi_api_hid_generic
        
//@}


/**
 * USB Device Driver Configuration
 * @{
 */
//@}

//! The includes of classes and other headers must be done
//! at the end of this file to avoid compile error
#include "udi_cdc.h"
#include "udi_vendor.h"
#include "udi_hid_generic.h"
#include "main.h"

#endif // _CONF_USB_H_
