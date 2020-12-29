# ----------------------------
# Makefile Options
# ----------------------------

NAME ?= VYSION
ICON ?= icon.png
DESCRIPTION ?= "VYSION CE 0.5.1"
COMPRESSED ?= YES
ARCHIVED ?= YES
HAS_PRINTF ?= NO

CFLAGS ?= -Wall -Wextra -Oz
CXXFLAGS ?= -Wall -Wextra -Oz

# ----------------------------

ifndef CEDEV
$(error CEDEV environment path variable is not set)
endif

include $(CEDEV)/meta/makefile.mk
