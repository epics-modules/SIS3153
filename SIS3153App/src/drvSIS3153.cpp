/*
 * drvSIS3153.cpp
 *
 * Driver for SIS 3153 USB to VME interface
 *
 * Author: Mark Rivers
 *
 * Created October 5, 2024
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <iocsh.h>

#include <asynPortDriver.h>

#include <sis3150usb_vme.h>
#include <sis3150usb_vme_calls.h>

#include <epicsExport.h>

static const char *driverName="drvSIS3153";

/* These are the drvInfo strings that are used to identify the parameters.
 * They are used by asyn clients, including standard asyn device support */
#define P_A16D8_String                "A16D8"                  /* asynInt32,    r/w */
#define P_A16D16_String               "A16D16"                 /* asynInt32,    r/w */
#define P_A16D32_String               "A16D32"                 /* asynInt32,    r/w */
#define P_A24D8_String                "A24D8"                  /* asynInt32,    r/w */
#define P_A24D16_String               "A24D16"                 /* asynInt32,    r/w */
#define P_A24D32_String               "A24D32"                 /* asynInt32,    r/w */
#define P_A32D8_String                "A32D8"                  /* asynInt32,    r/w */
#define P_A32D16_String               "A32D16"                 /* asynInt32,    r/w */
#define P_A32D32_String               "A32D32"                 /* asynInt32,    r/w */


class drvSIS3153 : public asynPortDriver {
public:
    drvSIS3153(const char *portName);

    /* These are the methods that we override from asynPortDriver */
    virtual asynStatus getAddress(asynUser *pasynUser, int *address);
    virtual asynStatus readInt32(asynUser *pasynUser, epicsInt32 *value);
    virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
    virtual asynStatus drvUserCreate(asynUser *pasynUser, const char *drvInfo,
                                     const char **pptypeName, size_t *psize);
    virtual asynStatus drvUserDestroy(asynUser *pasynUser);


protected:
    /** Values used for pasynUser->reason, and indexes into the parameter library. */
    int P_A16D8;
    int P_A16D16;
    int P_A16D32;
    int P_A24D8;
    int P_A24D16;
    int P_A24D32;
    int P_A32D8;
    int P_A32D16;
    int P_A32D32;

private:
    struct SIS3150USB_Device_Struct devStruct_;
    struct usb_dev_handle *devHandle_;
};

drvSIS3153::drvSIS3153(const char *portName)
   : asynPortDriver(portName,
                    1, /* maxAddr */
                    asynInt32Mask | asynDrvUserMask, /* Interface mask */
                    asynInt32Mask,  /* Interrupt mask */
                    ASYN_CANBLOCK, /* asynFlags.  This driver blocks and it is not multi-device */
                    1, /* Autoconnect */
                    0, /* Default priority */
                    0) /* Default stack size*/
{
    const char *functionName = "drvSIS3153";
    int status;
    unsigned int found;

    status = FindAll_SIS3150USB_Devices(&devStruct_, &found, 1);
    printf("FindAll_SIS3150USB_Devices found %d devices, status=%d\n", found, status);
    if (status) {
        asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
        "%s::%s error calling FindAll_SIS3150USB_Devices = %d\n", 
        driverName, functionName, status);
        return;
    }

    printf("path: %s\n", devStruct_.cDName);
    printf("vendor: %04X\n", devStruct_.idVendor);
    printf("product: %04X\n", devStruct_.idProduct);
    printf("serial #: %04d\n", devStruct_.idSerNo);

    status = Sis3150usb_OpenDriver_And_Download_FX2_Setup ((PCHAR)devStruct_.cDName, &devStruct_);
    if (status) {
        asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
        "%s::%s error calling Sis3150usb_OpenDriver_And_Download_FX2_Setup = %d\n", 
        driverName, functionName, status);
        return;
    }
    devHandle_ = devStruct_.hDev;
    printf("firmware: %04d\n", devStruct_.idFirmwareVersion);
    printf("handle: %p\n", devHandle_);

    createParam(P_A16D8_String,  asynParamInt32, &P_A16D8);
    createParam(P_A16D16_String, asynParamInt32, &P_A16D16);
    createParam(P_A16D32_String, asynParamInt32, &P_A16D32);
    createParam(P_A24D8_String,  asynParamInt32, &P_A24D8);
    createParam(P_A24D16_String, asynParamInt32, &P_A24D16);
    createParam(P_A24D32_String, asynParamInt32, &P_A24D32);
    createParam(P_A32D8_String,  asynParamInt32, &P_A32D8);
    createParam(P_A32D16_String, asynParamInt32, &P_A32D16);
    createParam(P_A32D32_String, asynParamInt32, &P_A32D32);
}

asynStatus drvSIS3153::getAddress(asynUser *pasynUser, int *address)
{
    // We use the asyn address for Modbus register addressing, but not for multiple addresses in asynPortDriver
    *address = 0;
    return asynSuccess;
}

asynStatus drvSIS3153::readInt32(asynUser *pasynUser, epicsInt32 *value)
{
    int function = pasynUser->reason;
    int status = 0;
    int addr;
    const char *paramName;
    const char* functionName = "readInt32";

    /* Get the VME address which is in pasynUser->drvUser */
    addr = *(int *)pasynUser->drvUser;
    /* Fetch the parameter string name for possible use in debugging */
    getParamName(function, &paramName);

    if (function == P_A16D8) {
        u_int8_t vme_data;
        status = vme_A16D8_read(devHandle_, addr, &vme_data);
        *value = vme_data;
    }
    else if (function == P_A16D16) {
        u_int16_t vme_data;
        status = vme_A16D16_read(devHandle_, addr, &vme_data);
        *value = vme_data;
    }
    else if (function == P_A16D32) {
        u_int32_t vme_data;
        status = vme_A16D32_read(devHandle_, addr, &vme_data);
        *value = vme_data;
    }
    else if (function == P_A24D8) {
        u_int8_t vme_data;
        status = vme_A24D8_read(devHandle_, addr, &vme_data);
        *value = vme_data;
    }
    else if (function == P_A24D16) {
        u_int16_t vme_data;
        status = vme_A24D16_read(devHandle_, addr, &vme_data);
        *value = vme_data;
    }
    else if (function == P_A24D32) {
        u_int32_t vme_data;
        status = vme_A24D32_read(devHandle_, addr, &vme_data);
        *value = vme_data;
    }
    else if (function == P_A32D8) {
        u_int8_t vme_data;
        status = vme_A32D8_read(devHandle_, addr, &vme_data);
        *value = vme_data;
    }
    else if (function == P_A32D16) {
        u_int16_t vme_data;
        status = vme_A32D16_read(devHandle_, addr, &vme_data);
        *value = vme_data;
    }
    else if (function == P_A32D32) {
        u_int32_t vme_data;
        status = vme_A32D32_read(devHandle_, addr, &vme_data);
        *value = vme_data;
    }

    if (status)
        epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize,
                  "%s:%s: status=%d, function=%d, name=%s, addr=0x%x, value=%d",
                  driverName, functionName, status, function, paramName, addr, *value);
    //else
        asynPrint(pasynUserSelf, ASYN_TRACEIO_DRIVER,
              "%s:%s: function=%d, name=%s, status=%d, addr=0x%x, value=%d\n",
              driverName, functionName, function, paramName, status, addr, *value);
    return status ? asynError : asynSuccess;
}

asynStatus drvSIS3153::writeInt32(asynUser *pasynUser, epicsInt32 value)
{
    int function = pasynUser->reason;
    int status = 0;
    int addr;
    const char *paramName;
    const char* functionName = "writeInt32";

    /* Get the VME address which is in pasynUser->drvUser */
    addr = *(int *)pasynUser->drvUser;
    /* Fetch the parameter string name for possible use in debugging */
    getParamName(function, &paramName);

    if (function == P_A16D8) {
        u_int8_t vme_data = value;
        status = vme_A16D8_write(devHandle_, addr, vme_data);
    }
    else if (function == P_A16D16) {
        u_int16_t vme_data = value;
        status = vme_A16D16_write(devHandle_, addr, vme_data);
    }
    else if (function == P_A16D32) {
        u_int32_t vme_data = value;
        status = vme_A16D32_write(devHandle_, addr, vme_data);
    }
    else if (function == P_A24D8) {
        u_int8_t vme_data = value;
        status = vme_A24D8_write(devHandle_, addr, vme_data);
    }
    else if (function == P_A24D16) {
        u_int16_t vme_data = value;
        status = vme_A24D16_write(devHandle_, addr, vme_data);
    }
    else if (function == P_A24D32) {
        u_int32_t vme_data = value;
        status = vme_A24D32_write(devHandle_, addr, vme_data);
    }
    else if (function == P_A32D8) {
        u_int8_t vme_data = value;
        status = vme_A32D8_write(devHandle_, addr, vme_data);
    }
    else if (function == P_A32D16) {
        u_int16_t vme_data = value;
        status = vme_A32D16_write(devHandle_, addr, vme_data);
    }
    else if (function == P_A32D32) {
        u_int32_t vme_data = value;
        status = vme_A32D32_write(devHandle_, addr, vme_data);
    }

    if (status)
        //epicsSnprintf(pasynUser->errorMessage, pasynUser->errorMessageSize,
        asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,        
                  "%s:%s: status=%d, function=%d, name=%s, addr=0x%x, value=%d\n",
                  driverName, functionName, status, function, paramName, addr, value);
    else
        asynPrint(pasynUserSelf, ASYN_TRACEIO_DRIVER,
              "%s:%s: function=%d, name=%s, addr=0x%x, value=%d\n",
              driverName, functionName, function, paramName, addr, value);
    return status ? asynError : asynSuccess;
}

/* asynDrvUser routines */
asynStatus drvSIS3153::drvUserCreate(asynUser *pasynUser, const char *drvInfo,
                                     const char **pptypeName, size_t *psize)
{
    static const char *functionName="drvUserCreate";

    /* We are passed a string of the format "AddressMode Address"
     * We convert the address string to an int and allocate and store it in pasynUser->drvUser.
     * We remove " Address" from the string and call the base class. */

    std::string drvString = drvInfo;
    size_t pos = drvString.find(" ");
    if (pos != std::string::npos) {
        std::string addrString = drvString.substr(pos+1);
        int *pAddr = new(int);
        try {
            *pAddr = (int)std::stol(addrString, 0, 0);
        }
        catch (std::exception &e) {
            asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
                "%s::%s invalid string, %s\n",
                driverName, functionName, e.what());
            return asynError;
        }
        pasynUser->drvUser = pAddr;
    } else {
        asynPrint(pasynUserSelf, ASYN_TRACE_ERROR,
            "%s::%s Error, expected underscore in drvInfo string\n",
            driverName, functionName);
        return asynError;
    }
    std::string newDrvInfo = drvString.substr(0,pos);
    return asynPortDriver::drvUserCreate(pasynUser, newDrvInfo.c_str(), pptypeName, psize);
}

asynStatus drvSIS3153::drvUserDestroy(asynUser *pasynUser)
{
    if (pasynUser->drvUser) {
        delete (int *)(pasynUser->drvUser);
    }
    pasynUser->drvUser = NULL;
    return asynSuccess;
}

/* Configuration routine.  Called directly, or from the iocsh function below */

extern "C" {

int drvSIS3153Configure(const char *portName)
{
    new drvSIS3153(portName);
    return(asynSuccess);
}


/* EPICS iocsh shell commands */

static const iocshArg initArg0 = { "portName",iocshArgString};
static const iocshArg * const initArgs[] = {&initArg0};
static const iocshFuncDef initFuncDef = {"drvSIS3153Configure", 1, initArgs};
static void initCallFunc(const iocshArgBuf *args)
{
    drvSIS3153Configure(args[0].sval);
}

void drvSIS3153Register(void)
{
    iocshRegister(&initFuncDef, initCallFunc);
}

epicsExportRegistrar(drvSIS3153Register);

}
