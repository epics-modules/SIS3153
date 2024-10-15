#include <stdio.h>
#include <stdlib.h>

#include "sis3150usb_vme.h"
#include "sis3150usb_vme_calls.h"

int main(int argc, char *argv[])
{
    struct SIS3150USB_Device_Struct devs[4];
    int res;
    unsigned int found;
    unsigned int addr=0;
    u_int8_t data8;
    u_int16_t data16;
    u_int32_t data32;

    if (argc > 1) {
        addr = strtol(argv[1], 0, 0);
    }
    printf("Running tests with address=0x%x\n", addr);

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

    res = Sis3150usb_OpenDriver_And_Download_FX2_Setup((PCHAR)devs[0].cDName, &devs[0]);
    if(res != 0){
        printf("Error in 'Sis3150usb_OpenDriver': %d\n", res);
       return -1;
    }
    printf("serial #: %04d\n", devs[0].idSerNo);
    printf("firmware: 0x%04X\n", devs[0].idFirmwareVersion);
    printf("handle: %p\n", devs[0].hDev);

    printf("\nRead tests\n");
    /* A16 read tests */
    res = vme_A16D8_read(devs[0].hDev, addr, &data8);
	  printf("vme_A16D8_ read:   addr = 0x%08x    data = 0x%08x   return_code = 0x%08x \n", addr, data8, res);
    res = vme_A16D16_read(devs[0].hDev, addr, &data16);
	  printf("vme_A16D16 read:   addr = 0x%08x    data = 0x%08x   return_code = 0x%08x \n", addr, data16, res);
    res = vme_A16D32_read(devs[0].hDev, addr, &data32);
	  printf("vme_A16D32 read:   addr = 0x%08x    data = 0x%08x   return_code = 0x%08x \n", addr, data32, res);

    /* A24 read tests */
    res = vme_A24D8_read(devs[0].hDev, addr, &data8);
	  printf("vme_A24D8  read:   addr = 0x%08x    data = 0x%08x   return_code = 0x%08x \n", addr, data8, res);
    res = vme_A24D16_read(devs[0].hDev, addr, &data16);
	  printf("vme_A24D16 read:   addr = 0x%08x    data = 0x%08x   return_code = 0x%08x \n", addr, data16, res);
    res = vme_A24D32_read(devs[0].hDev, addr, &data32);
	  printf("vme_A24D32 read:   addr = 0x%08x    data = 0x%08x   return_code = 0x%08x \n", addr, data32, res);

    /* A32 read tests */
    res = vme_A32D8_read(devs[0].hDev, addr, &data8);
	  printf("vme_A32D8  read:   addr = 0x%08x    data = 0x%08x   return_code = 0x%08x \n", addr, data8, res);
    res = vme_A32D16_read(devs[0].hDev, addr, &data16);
	  printf("vme_A32D16 read:   addr = 0x%08x    data = 0x%08x   return_code = 0x%08x \n", addr, data16, res);
    res = vme_A32D32_read(devs[0].hDev, addr, &data32);
	  printf("vme_A32D32 read:   addr = 0x%08x    data = 0x%08x   return_code = 0x%08x \n", addr, data32, res);


    printf("\nWrite tests\n");
    data8 = 8;
    data16 = 16;
    data32 = 32;
    /* A16 write tests */
    res = vme_A16D8_write(devs[0].hDev, addr, data8);
	  printf("vme_A16D8_ write:  addr = 0x%08x    data = 0x%08x   return_code = 0x%08x \n", addr, data8, res);
    res = vme_A16D16_write(devs[0].hDev, addr, data16);
	  printf("vme_A16D16 write:  addr = 0x%08x    data = 0x%08x   return_code = 0x%08x \n", addr, data16, res);
    res = vme_A16D32_write(devs[0].hDev, addr, data32);
	  printf("vme_A16D32 write:  addr = 0x%08x    data = 0x%08x   return_code = 0x%08x \n", addr, data32, res);

    /* A24 write tests */
    res = vme_A24D8_write(devs[0].hDev, addr, data8);
	  printf("vme_A24D8  write:  addr = 0x%08x    data = 0x%08x   return_code = 0x%08x \n", addr, data8, res);
    res = vme_A24D16_write(devs[0].hDev, addr, data16);
	  printf("vme_A24D16 write:  addr = 0x%08x    data = 0x%08x   return_code = 0x%08x \n", addr, data16, res);
    res = vme_A24D32_write(devs[0].hDev, addr, data32);
	  printf("vme_A24D32 write:  addr = 0x%08x    data = 0x%08x   return_code = 0x%08x \n", addr, data32, res);

    /* A32 write tests */
    res = vme_A32D8_write(devs[0].hDev, addr, data8);
	  printf("vme_A32D8  write:  addr = 0x%08x    data = 0x%08x   return_code = 0x%08x \n", addr, data8, res);
    res = vme_A32D16_write(devs[0].hDev, addr, data16);
	  printf("vme_A32D16 write:  addr = 0x%08x    data = 0x%08x   return_code = 0x%08x \n", addr, data16, res);
    res = vme_A32D32_write(devs[0].hDev, addr, data32);
	  printf("vme_A32D32 write:  addr = 0x%08x    data = 0x%08x   return_code = 0x%08x \n", addr, data32, res);

    res = Sis3150usb_CloseDriver(devs[0].hDev);

    return 0;
}
