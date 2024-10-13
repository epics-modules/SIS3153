TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS BELOW HERE

# Build an IOC support library
LIBRARY_IOC += SIS3153Support

# Compile and add code to the support library
USR_CPPFLAGS += -Dunix
SIS3153Support_SRCS += drvSIS3153.cpp

SIS3153Support_LIBS += SIS3150
SIS3153Support_LIBS += SISLoader
SIS3153Support_LIBS += asyn
SIS3153Support_LIBS += $(EPICS_BASE_IOC_LIBS)

# Build the IOC application
PROD_IOC = SIS3153App

# SIS3153App.dbd will be created and installed
DBD += SIS3153App.dbd

# SIS3153App.dbd will include these files:
SIS3153App_DBD += base.dbd
SIS3153App_DBD += asyn.dbd
SIS3153App_DBD += SIS3153Support.dbd

# SIS3153App_registerRecordDeviceDriver.cpp derives from SIS3153.dbd
SIS3153App_SRCS += SIS3153App_registerRecordDeviceDriver.cpp

# Build the main IOC entry point where needed
SIS3153App_SRCS += SIS3153AppMain.cpp

# Link in the code from our support library
SIS3153App_LIBS += SIS3153Support
SIS3153App_LIBS += SIS3150
SIS3153App_LIBS += SISLoader
SIS3153App_LIBS += asyn
SIS3153App_SYS_LIBS += usb

# Link QSRV (pvAccess Server) if available
ifdef EPICS_QSRV_MAJOR_VERSION
    SIS3153App_LIBS += qsrv
    SIS3153App_LIBS += $(EPICS_BASE_PVA_CORE_LIBS)
    SIS3153App_DBD += PVAServerRegister.dbd
    SIS3153App_DBD += qsrv.dbd
endif

# Finally link IOC to the EPICS Base libraries
SIS3153App_LIBS += $(EPICS_BASE_IOC_LIBS)

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD EXTRA GNUMAKE RULES BELOW HERE
