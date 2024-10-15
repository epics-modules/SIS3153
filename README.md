An [EPICS](http://www.aps.anl.gov/epics/) 
module for support of SIS 3150 and 3153 USB to VME interface boards.

The support is a driver derived from asynPortDriver.
The driver allows records to perform VME I/O using standard asyn device support.

The driver is based on a library from SIS for the low-level USB to VME transactions.
This library is included in this module, so nothing else needs to be downloaded.

There is a udev file in the udev directory of this module.  
This needs to be installed where udev files belong on your system, e.g. /etc/udev/rules.d or /usr/local/udev/rules.d.

The syntax of the input and output links for EPICS records is the following:
```
field(INP,  "@asyn($(PORT), 0)ADDRESS_MODE VME_ADDRESS")
field(OUT,  "@asyn($(PORT), 0)ADDRESS_MODE VME_ADDRESS")
```
ADDRESS_MODE is one of the following:
- A16D8
- A16D16
- A16D32
- A24D8
- A24D16
- A24D32
- A32D8
- A32D16
- A32D32

VME_ADDRESS is the VME address to read or write. 

This is an example from one of the databases included in the module:
```
field(INP,  "@asyn($(PORT), 0)A32D16 0xB0000028")
```

Additional information:
* [Release notes](RELEASE.md)
