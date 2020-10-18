#ifndef _PHYW_CONF_USB_H_
#define _PHYW_CONF_USB_H_

#include "compiler.h"

/* Endpoint allocations;
 *
 * 1 512    CMSIS-DAPv2 IN1 (Debug)
 * 2 512    CMSIS-DAPv2 OUT (Debug)
 * 3 64     CMSIS-DAPv1 IN
 * 4 64     CMSIS-DAPv2 IN2 (SWO)  TO BE CHANGED WITH (5)
 * 5 1024   VENDOR-OUT             TO BE CHANGED WITH (4)
 * 6 1024   VENDOR-IN
 */

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

extern char g_usb_serial_number[33];      /* Defined in main.c */

#define  USB_DEVICE_MANUFACTURE_NAME      "NewAE Technology Inc."
#define  USB_DEVICE_PRODUCT_NAME          "PhyWhisperer-UDT (CMSIS-DAP)"
#define  USB_DEVICE_GET_SERIAL_NAME_POINTER g_usb_serial_number
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

//! Two interfaces for this device (Vendor + CMSISDAPv2 + HID(CMSISDAPv1))
#define  USB_DEVICE_NB_INTERFACE       3

//! 3 endpoints used by Vendor(2), CMSISDAPv2(3) and HID(1) interfaces (HID OUT in EP0)
#  define  USB_DEVICE_MAX_EP           6

// In HS mode, size of bulk endpoints are 512
// If CDC and Vendor endpoints all uses 2 banks, DPRAM is not enough: 4 bulk
// endpoints requires 4K bytes. So reduce the number of banks of CDC bulk
// endpoints to use less DPRAM. Keep Vendor setting to keep performance.
#define  UDD_BULK_NB_BANK(ep) 1
//((ep == 5 || ep== 6) ? 1 : 2)
//@}

//@}


/**
 * USB Interface Configuration
 * @{
 */

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
#define xxUDI_VENDOR_EPS_SIZE_BULK_HS   512
#define UDI_VENDOR_EPS_SIZE_BULK_HS   512
#define UDI_VENDOR_EPS_SIZE_ISO_HS    0

// String to describe interface
#define UDI_VENDOR_STRING_ID          4

//! Endpoint numbers definition
#define  xxUDI_VENDOR_EP_BULK_IN       (1 | USB_EP_DIR_IN)
#define  xxUDI_VENDOR_EP_BULK_OUT      (2 | USB_EP_DIR_OUT)
#define  UDI_VENDOR_EP_BULK_IN       (6 | USB_EP_DIR_IN)
#define  UDI_VENDOR_EP_BULK_OUT      (5 | USB_EP_DIR_OUT)

//! Interface number
#define  UDI_VENDOR_IFACE_NUMBER     0

//@}

/**
 * Configuration of HID Generic interface
 * Note that udi_hid_generic.c has been modified to support this configuration.
 * Specifically, OUT in EP0 has been set up, and REPORT descriptor abstracted to the config.
 * @{
 */
//! Interface callback definition
#define  UDI_HID_GENERIC_ENABLE_EXT()        usb_hid_generic_enable()
#define  UDI_HID_GENERIC_DISABLE_EXT()       usb_hid_generic_disable()
#define  UDI_HID_GENERIC_REPORT_OUT(ptr)     usb_hid_generic_report_out(ptr)
#define  UDI_HID_GENERIC_SET_FEATURE(report) usb_hid_generic_set_feature(report)

//! Sizes of I/O reports
#define  UDI_HID_REPORT_IN_SIZE             64
#define  UDI_HID_REPORT_OUT_SIZE            64
#define  UDI_HID_REPORT_FEATURE_SIZE        1

//! Sizes of I/O endpoints
#define  UDI_HID_GENERIC_EP_SIZE            64

#define UDI_HID_GENERIC_STRING_ID           5

#define xxUDI_HID_GENERIC_EP_IN               ( 3 | USB_EP_DIR_IN )
#define UDI_HID_GENERIC_EP_IN               ( 3 | USB_EP_DIR_IN )
#define UDI_HID_GENERIC_EP_OUT              0

#define UDI_HID_GENERIC_IFACE_NUMBER        1

#define HID_REPORT_DESCRIPTOR { \
        0x06, 0x00, 0xFF,	/* Usage Page (vendor defined) */             \
	0x09, 0x01,	        /* Usage      (vendor defined) */             \
                                                                              \
	0xA1, 0x01,	        /* Collection (Application)    */             \
        0x15, 0x00,             /* Value Logical Min 0         */             \
        0x26, 0xFF, 0x00,       /* Value Logical Max 0xFF      */             \
        0x75, 0x08,             /* Report Size 8 bits          */             \
        0x95, UDI_HID_REPORT_IN_SIZE, /* Inreport Count Max Size */           \
                                                                              \
        0x09, 0x01,             /* Usage      (vendor defined) */             \
        0x81, 2,                /* HID Input (Data, Variable, Absolute) */    \
        0x95, UDI_HID_REPORT_OUT_SIZE,        /* Outreport Count Max Size */  \
                                                                              \
        0x09, 0x01,             /* Usage      (vendor defined) */             \
        0x91, 2,                /* HID Output (Data, Variable, Absolute) */   \
        0x95, UDI_HID_REPORT_FEATURE_SIZE,    /* Featreport Count Max Size */ \
                                                                              \
        0x09, 0x01,             /* Usage      (vendor defined) */             \
        0xB1, 2,                /* HID Feature (Data, Variable, Absolute) */  \
	0xC0	                /* End Collection */                          \
        }

#define xxHID_REPORT_DESCRIPTOR { \
        0x06, 0x00, 0xFF,	/* Usage Page (vendor defined) */             \
	0x09, 0x01,	        /* Usage      (vendor defined) */             \
                                                                              \
	0xA1, 0x01,	        /* Collection (Application)    */             \
        0x15, 0x00,             /* Value Logical Min 0         */             \
        0x26, 0xFF, 0x00,       /* Value Logical Max 0xFF      */             \
        0x75, 0x08,             /* Report Size 8 bits          */             \
        0x96, 2, 0,             /* Inreport Count Size */           \
                                                                              \
        0x09, 0x01,             /* Usage      (vendor defined) */             \
        0x81, 2,                /* HID Input (Data, Variable, Absolute) */    \
        0x96, 2, 0,             /* Outreport Count Size */  \
                                                                              \
        0x09, 0x01,             /* Usage      (vendor defined) */             \
        0x91, 2,                /* HID Output (Data, Variable, Absolute) */   \
        0x95, UDI_HID_REPORT_FEATURE_SIZE,    /* Featreport Count Max Size */ \
                                                                              \
        0x09, 0x01,             /* Usage      (vendor defined) */             \
        0xB1, 2,                /* HID Feature (Data, Variable, Absolute) */  \
	0xC0	                /* End Collection */                          \
        }

//@}

/**
 * Configuration of second vendor interface (for CMSIS-DAP v2)
 * @{
 */
//! Interface callback definition
#define UDI_CMSISDAP_ENABLE_EXT()           usb_cmsisdap_enable()
#define UDI_CMSISDAP_DISABLE_EXT()          usb_cmsisdap_disable()
#define UDI_CMSISDAP_SETUP_OUT_RECEIVED()   usb_cmsisdap_setup_out_received()
#define UDI_CMSISDAP_SETUP_IN_RECEIVED()   usb_cmsisdap_setup_in_received()

//! endpoints size for full speed
#define UDI_CMSISDAP_EPS_SIZE_BULK_FS  64

//! endpoints size for high speed
#define xxUDI_CMSISDAP_EPS_SIZE_BULK_HS  64
#define UDI_CMSISDAP_EPS_SIZE_BULK_OUT_HS 512
#define UDI_CMSISDAP_EPS_SIZE_BULK_IN1_HS 512
#define UDI_CMSISDAP_EPS_SIZE_BULK_IN2_HS 64

// String to describe interface
#define UDI_CMSISDAP_STRING_ID         6

//! Endpoint numbers definition
#define  UDI_CMSISDAP_EP_BULK_OUT      (2 | USB_EP_DIR_OUT)
#define  UDI_CMSISDAP_EP_BULK_IN1      (1 | USB_EP_DIR_IN)
#define  UDI_CMSISDAP_EP_BULK_IN2      (4 | USB_EP_DIR_IN)

//! Interface number
#define  UDI_CMSISDAP_IFACE_NUMBER     2

//@}


//@}



/**
 * Description of Composite Device
 * @{
 */
//! USB Interfaces descriptor structure
#define UDI_COMPOSITE_DESC_T \
        udi_vendor_desc_t      udi_vendor;      \
        udi_hid_generic_desc_t udi_hid_generic; \
        udi_cmsisdap_desc_t    udi_cmsisdap;

//! USB Interfaces descriptor value for Full Speed
#define UDI_COMPOSITE_DESC_FS \
        .udi_vendor           = UDI_VENDOR_DESC_FS,     \
        .udi_hid_generic      = UDI_HID_GENERIC_DESC,   \
        .udi_cmsisdap         = UDI_CMSISDAP_DESC_FS

        

//! USB Interfaces descriptor value for High Speed
#define UDI_COMPOSITE_DESC_HS \
        .udi_vendor           = UDI_VENDOR_DESC_HS,     \
        .udi_hid_generic      = UDI_HID_GENERIC_DESC, \
        .udi_cmsisdap         = UDI_CMSISDAP_DESC_HS

//! USB Interface APIs
#define UDI_COMPOSITE_API \
        &udi_api_vendor,  \
        &udi_api_hid_generic, \
        &udi_api_cmsisdap
        
//@}


/**
 * USB Device Driver Configuration
 * @{
 */
//@}

//! The includes of classes and other headers must be done
//! at the end of this file to avoid compile error
#include "udi_vendor.h"
#include "udi_hid_generic.h"
#include "udi_cmsisdap.h"
#include "main.h"

#endif // _CONF_USB_H_
