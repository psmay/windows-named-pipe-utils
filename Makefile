#-- Header - See target.mk for details
ifeq (,$(filter build,$(notdir $(CURDIR))))
include target.mk
else

VPATH = $(SRCDIR)
#-- End Header

ALL_SOURCES_SRCDIR=$(wildcard $(SRCDIR)/*.c)
ALL_SOURCES=$(patsubst $(SRCDIR)/%, %, $(ALL_SOURCES_SRCDIR))

MAIN_SOURCES=$(filter %-main.c,$(ALL_SOURCES))
COMMON_SOURCES=$(filter-out $(MAIN_SOURCES),$(ALL_SOURCES))

ALL_OBJECTS=$(patsubst %.c,%.o,$(ALL_SOURCES))
COMMON_OBJECTS=$(patsubst %.c,%.o,$(COMMON_SOURCES))

CFLAGS=-Os
LDFLAGS=

all: createAndReadPipe

-include $(ALL_OBJECTS:.o=.d)



%.d : %.c
	gcc -MM $(CFLAGS) $< > $@

%.o : %.c
	gcc -c $(CFLAGS) $< -o $@

createAndReadPipe: $(COMMON_OBJECTS) createAndReadPipe-main.o
	gcc $(CFLAGS) $(LDFLAGS) -s -o $@ $^

#-- Footer - See target.mk for details
endif
#--
