TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS BELOW HERE

# Build an IOC 
PROD_IOC += read-fw

read-fw_SRCS += read-fw.c
read-fw_LIBS += SIS3150
read-fw_LIBS += SISLoader
read-fw_SYS_LIBS += usb

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD EXTRA GNUMAKE RULES BELOW HERE

# Explicit dependency needed for generated header file
devSIS3153Version$(DEP): $(COMMON_DIR)/$(GENVERSION)
