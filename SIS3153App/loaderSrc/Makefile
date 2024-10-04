TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS BELOW HERE

# Build an IOC support library
LIBRARY_IOC += SISLoader

INC += usrp_prims.h
INC += usrp_slots.h

# Compile and add code to the support library
SISLoader_SRCS += fusb.c
SISLoader_SRCS += fusb_linux.c
SISLoader_SRCS += fusb_sysconfig_linux.c
SISLoader_SRCS += md5.c
SISLoader_SRCS += usrp_basic.cc
SISLoader_SRCS += usrp_config.cc
SISLoader_SRCS += usrp_dbid.cc
SISLoader_SRCS += usrp_local_sighandler.cc
SISLoader_SRCS += usrp_prims.cc
SISLoader_SRCS += usrp_standard.cc


include $(TOP)/configure/RULES
#----------------------------------------
#  ADD EXTRA GNUMAKE RULES BELOW HERE

# Explicit dependency needed for generated header file
devSIS3153Version$(DEP): $(COMMON_DIR)/$(GENVERSION)
