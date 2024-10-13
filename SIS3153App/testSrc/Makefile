TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS BELOW HERE

# Build an IOC 
PROD_IOC += read-fw
PROD_IOC += vme_cycle_test_all

USR_CFLAGS += -Dunix

PROD_LIBS += SIS3150
PROD_LIBS += SISLoader
PROD_SYS_LIBS += usb

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD EXTRA GNUMAKE RULES BELOW HERE
