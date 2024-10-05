# Makefile at top of application tree
TOP = ..
include $(TOP)/configure/CONFIG

# Directories to be built, in any order.
# You can replace these wildcards with an explicit list
DIRS += $(wildcard src* *Src*)
DIRS += $(wildcard db* *Db*)

# If the build order matters, add dependency rules like this,
# which specifies that xxxSrc must be built after src:
SISSrc_DEPEND_DIRS += loaderSrc
testSrc_DEPEND_DIRS += SISSrc
src_DEPEND_DIRS += SISSrc

include $(TOP)/configure/RULES_DIRS
