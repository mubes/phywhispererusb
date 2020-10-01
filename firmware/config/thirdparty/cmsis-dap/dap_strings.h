#include <string.h>
#include "compiler.h"

/** Get Vendor ID string.
\param str Pointer to buffer to store the string.
\return String length.
*/
__STATIC_INLINE uint8_t DAP_GetVendorString (char *str) {
  (void)str;
  return (0U);
}

/** Get Product ID string.
\param str Pointer to buffer to store the string.
\return String length.
*/
__STATIC_INLINE uint8_t DAP_GetProductString (char *str) {
  strcpy(str,USB_DEVICE_MANUFACTURE_NAME);
  return (strlen(str));
}

/** Get Serial Number string.
\param str Pointer to buffer to store the string.
\return String length.
*/
__STATIC_INLINE uint8_t DAP_GetSerNumString (char *str)
{
  strcpy(str,USB_DEVICE_GET_SERIAL_NAME_POINTER);
  return strlen(str);
}

/** Get firmware version string.
\param str Pointer to buffer to store the string.
\return String length.
*/
__STATIC_INLINE uint8_t DAP_GetFirmwareVersionString (char *str) {
  strcpy(str,"UNSET");
  return strlen(str);
}
