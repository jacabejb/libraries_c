#include "console.h"
#include "usbd_cdc_if.h"

static bool USB_Write(void *hw,
                      const uint8_t *buf,
                      uint16_t len)
{
    (void)hw;

    return (CDC_Transmit_FS((uint8_t *)buf, len) == USBD_OK);
}

static bool USB_Busy(void *hw)
{
    (void)hw;
    return false;
}

static void USB_Flush(void *hw)
{
    (void)hw;

    /* nic nie robimy */
}

const console_driver_t ConsoleDriverUsb =
{
    .Write = USB_Write,
    .Busy  = USB_Busy,
    .Flush = USB_Flush
};
