# Based on "Multi-Architecture Builds Using GNU make" by Paul D. Smith, 2000
# http://make.mad-scientist.net/multi-arch.html

.SUFFIXES:

#ifndef _ARCH
#	_ARCH := $(shell print_arch)
#	export _ARCH
#endif

#OBJDIR := _$(_ARCH)
OBJDIR := build

MAKETARGET = $(MAKE) --no-print-directory -C $@ -f $(CURDIR)/Makefile \
	SRCDIR=$(CURDIR) $(MAKECMDGOALS)

.PHONY: $(OBJDIR)
$(OBJDIR):
	+@[ -d $@ ] || mkdir -p $@
	+@$(MAKETARGET)

Makefile : ;
%.mk :: ;

% :: $(OBJDIR) ; :

.PHONY: clean
clean:
	rm -rf $(OBJDIR)

