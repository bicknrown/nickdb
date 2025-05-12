#
# Copyright 2025 Nick Brown <njbrown4@buffalo.edu>
#

# This file is part of nickdb.
#
# nickdb is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# nickdb is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

CC := gcc

CFLAGS := -O2 -Wall -Werror -std=c99 -D_DEFAULT_SOURCE -D_GNU_SOURCE

CDEBUGFLAGS := -O0 -g -Wall -Werror -fsanitize=address -fsanitize=leak -std=c99 -D_DEFAULT_SOURCE -D_GNU_SOURCE -DDEBUG

LDFLAGS :=

LDASNFLAGS := -static-libubsan

TESTS := fileiotest pageiotest

SRCDIR := src/
LIBDIR := src/lib/

SOURCES := $(SRCDIR)nickdb.c
OBJS := $(patsubst %.c,%.o,$(SOURCES))

LIBS := $(LIBDIR)constants.c $(LIBDIR)fileio.c $(LIBDIR)buffer_manager.c 
LIBOBJS := $(patsubst %.c,%.o,$(LIBS))


nickdb: $(SOURCES) $(LIBS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

debug-nickdb: $(SOURCES) $(LIBS)
	$(CC) $(CDEBUGFLAGS) $(LDASNFLAGS) -o $@ $^

test: $(TESTS)
	@echo
	@for test in $^; do                                   \
	    printf "Running %-20s: " "$$test";                \
	    (./$$test && echo "passed") || echo "failed";       \
	done
	@echo

%: tests/%.o $(LIBOBJS)
	$(CC) $(CDEBUGFLAGS) $(LDASNFLAGS) -o $@ $^

clean:
	rm -f src/*~ src/lib/*~ src/lib/*/*~ tests/*~ $(OBJS) $(LIBOBJS)
	rm -f $(TESTS)
	rm -f *~nickdb
	rm -f *~debug-nickdb
	rm -f *.store
	rm -f *.meta
	rm -f nickdb
	rm -f debug-nickdb

.PHONY: all clean
