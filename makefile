# ----------------------------
# Makefile Options
# ----------------------------

NAME ?= VYSION
ICON ?= icon.png
DESCRIPTION ?= "VYSION CE 0.4.1"
COMPRESSED ?= YES
ARCHIVED ?= NO

CFLAGS ?= -Wall -Wextra -Oz
CXXFLAGS ?= -Wall -Wextra -Oz

# ----------------------------

ifndef CEDEV
$(error CEDEV environment path variable is not set)
endif

include $(CEDEV)/meta/makefile.mk
