#
# Copyright 2025 Nick Brown <njbrown4@buffalo.edu>
#

CC := gcc

CFLAGS := -O -g -Wall -Werror -std=c99 -D_DEFAULT_SOURCE -D_GNU_SOURCE

LDFLAGS := -luuid

TESTS := fileiotest

SOURCES := src/nickdb.c
OBJS := $(patsubst %.c,%.o,$(SOURCES))

LIBS := src/lib/fileio.c
LIBOBJS := $(patsubst %.c,%.o,$(LIBS))


nickdb: $(OBJS) $(LIBOBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

test: $(TESTS)
	@echo
	@for test in $^; do                                   \
	    printf "Running %-20s: " "$$test";                \
	    (./$$test && echo "passed") || echo "failed";       \
	done
	@echo

%.o: %.c
	$(CC) -c $< $(CFLAGS) -o $@

%: tests/%.o $(LIBOBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -f src/*~ src/lib/*~ src/lib/*/*~ tests/*~ $(OBJS) $(LIBOBJS)
	rm -f $(TESTS)
	rm -f *~ nickdb
	rm -f *.store
	rm -f *.meta

.PHONY: all clean
