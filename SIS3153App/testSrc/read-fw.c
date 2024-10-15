#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "sis3150usb_vme.h"
#include "sis3150usb_vme_calls.h"

int main()
{
    struct SIS3150USB_Device_Struct devs[4];
    int res;
    unsigned int found, reg;
    unsigned int addr;
    u_int8_t data8;
    u_int16_t data16;
    u_int32_t data32;

    res = FindAll_SIS3150USB_Devices(devs, &found, 4);
    if(res != 0){
        printf("Error in 'FindAll_SIS3150USB_Devices': %d\n", res);
        return -1;
    }

    if(found == 0){
        printf("found no SIS315x USB device \n");
        return -1;
    }
 
    //  use first device
    printf("path: %s\n", devs[0].cDName);
    printf("vendor: 0x%04X\n", devs[0].idVendor);
    printf("product: 0x%04X\n", devs[0].idProduct);

    res =
      Sis3150usb_OpenDriver_And_Download_FX2_Setup ( (PCHAR)devs[0].cDName,
						     &devs[0]);
    if(res != 0){
        printf("Error in 'Sis3150usb_OpenDriver': %d\n", res);
	return -1;
    }
    printf("serial #: %04d\n", devs[0].idSerNo);
    printf("firmware: 0x%04x\n", devs[0].idFirmwareVersion);
    printf("handle: %p\n", devs[0].hDev);
    //          for (uint64_t GA=0; GA<=0xf; GA++) {
    uint64_t GA=0x3;
      for (uint64_t axis=0; axis<=3; axis++) {
		addr = 0x39 << 24 | GA<<15 | axis<<12 | 0xe ;
		res = vme_A24D16_read(devs[0].hDev, addr, &data16);
		printf (
"vme_A24D16_read: \
addr = 0x%08x, \
data = 0x%04x, \
return_code = 0x%04x, \
GA=0x%02x\n", addr, data16, res, GA);
      }
      res = Sis3150usb_CloseDriver(devs[0].hDev);

    return 0;
}
