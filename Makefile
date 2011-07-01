# Makefile for ToyWrench

# CHANGE THE SETTINGS BELOW TO SUIT YOUR ENVIRONMENT

# Your platform. See PLATS for possible values.
PLAT= none

# Utilities
CP= cp
FIND= find
INSTALL= install
MKDIR= mkdir
RANLIB= ranlib

# END OF USER SETTINGS. PLEASE DO NOT CHANGE ANYTHING BELOW THIS LINE

# Available platform targets
PLATS= linux macosx

all:
	cd src && $(MAKE) $@

clean:
	cd src && $(MAKE) $@

none:
	@echo "Please do"
	@echo "  make PLATFORM"
	@echo "where PLATFORM is one of these:"
	@echo "  $(PLATS)"
	@echo "See INSTALL for complete instructions."

.PHONY: all $(PLATS) clean none echo
