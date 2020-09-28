#include <stdint.h>
#include <stdbool.h>
#include "usb_main_handler.h"

#include "report.h"

/* ====================================================================================== */
/* ====================================================================================== */
/* ====================================================================================== */
/* Publically accessible routines                                                         */
/* ====================================================================================== */
/* ====================================================================================== */
/* ====================================================================================== */
void usb_main_suspend_action( void )
{

}
/* ====================================================================================== */
void usb_main_resume_action( void )
{

}
/* ====================================================================================== */
void usb_main_sof_action( void )
{

}
/* ====================================================================================== */
bool usb_main_extra_string( void )
{
    static uint8_t udi_cdc_name[] = "Serial";
    static uint8_t udi_comm_name[] = "CDC-COMM";
    static uint8_t udi_data_name[] = "CDC-DATA";
    static uint8_t udi_vendor_name[] = "Phywhisperer-USB";
    static uint8_t udi_hid_generic[] = "CMSIS-DAP";

    struct extra_strings_desc_t
    {
        usb_str_desc_t header;
        le16_t string[20];
    };
    static UDC_DESC_STORAGE struct extra_strings_desc_t extra_strings_desc =
    {
        .header.bDescriptorType = USB_DT_STRING
    };

    uint8_t i;
    uint8_t *str;
    uint8_t str_lgt = 0;

    // Link payload pointer to the string corresponding at request
    switch ( udd_g_ctrlreq.req.wValue & 0xff )
    {
        case UDI_CDC_IAD_STRING_ID:
            DBG( "Request UDI_CDC_IAD_STRING_ID" EOL );
            str_lgt = sizeof( udi_cdc_name ) - 1;
            str = udi_cdc_name;
            break;

        case UDI_CDC_COMM_STRING_ID_0:
            DBG( "Request UDI_CDC_COMM_STRING_ID_0" EOL );
            str_lgt = sizeof( udi_comm_name ) - 1;
            str = udi_comm_name;
            break;

        case UDI_CDC_DATA_STRING_ID_0:
            DBG( "Request UDI_CDC_DATA_STRING_ID_0" EOL );
            str_lgt = sizeof( udi_data_name ) - 1;
            str = udi_data_name;
            break;

        case UDI_VENDOR_STRING_ID:
            DBG( "Request UDI_VENDOR_STRING_ID" EOL );
            str_lgt = sizeof( udi_vendor_name ) - 1;
            str = udi_vendor_name;
            break;

        case UDI_HID_GENERIC_STRING_ID:
            DBG( "Request for UDI_HID_GENERIC_STRING_ID" EOL );
            str_lgt = sizeof( udi_hid_generic ) - 1;
            str = udi_hid_generic;
            break;

        default:
            DBG( "Request for unknown element %d" EOL, udd_g_ctrlreq.req.wValue & 0xff );
            return false;
    }

    if ( str_lgt != 0 )
    {
        for ( i = 0; i < str_lgt; i++ )
        {
            extra_strings_desc.string[i] = cpu_to_le16( ( le16_t )str[i] );
        }

        extra_strings_desc.header.bLength = 2 + ( str_lgt ) * 2;
        udd_g_ctrlreq.payload_size = extra_strings_desc.header.bLength;
        udd_g_ctrlreq.payload = ( uint8_t * ) &extra_strings_desc;
    }

    // if the string is larger than request length, then cut it
    if ( udd_g_ctrlreq.payload_size > udd_g_ctrlreq.req.wLength )
    {
        udd_g_ctrlreq.payload_size = udd_g_ctrlreq.req.wLength;
    }

    return true;
}
/* ====================================================================================== */
