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

CFLAGS := -O2 -Wall -Werror -Wextra -std=c99 -D_DEFAULT_SOURCE -D_GNU_SOURCE

CDEBUGFLAGS := -O0 -g -Wall -Werror -Wextra -fsanitize=address -fsanitize=leak -std=c99 -D_DEFAULT_SOURCE -D_GNU_SOURCE -DDEBUG

LDFLAGS :=

LDASNFLAGS := -static-libubsan

TESTS := fileiotest pageiotest

SRCDIR := src/
LIBDIR := src/lib/

SOURCES := $(SRCDIR)nickdb.c
OBJS := $(patsubst %.c,%.o,$(SOURCES))

LIBS := $(LIBDIR)constants.c $(LIBDIR)fileio.c $(LIBDIR)buffer_manager.c 
LIBOBJS := $(patsubst %.c,%.o,$(LIBS))

GLIBH := $(shell pkg-config --cflags glib-2.0)
GLIBL := $(shell pkg-config --libs glib-2.0)

nickdb: $(SOURCES) $(LIBS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(GLIBH) -o $@ $^ $(GLIBL)

debug: $(SOURCES) $(LIBS)
	$(CC) $(CDEBUGFLAGS) $(LDASNFLAGS) $(GLIBH) -o $@ $^ $(GLIBL)

test: $(TESTS)
	@echo
	@for test in $^; do                                   \
	    printf "Running %-20s: " "$$test";                \
	    (./$$test && echo "passed\n") || echo "failed\n";       \
	done
	@echo

%: tests/%.c $(LIBS)
	$(CC) $(CDEBUGFLAGS) $(LDASNFLAGS) $(GLIBH) -o $@ $^ $(GLIBL)

clean:
	rm -f src/*~ src/lib/*~ src/lib/*/*~ tests/*~ $(OBJS) $(LIBOBJS)
	rm -f $(TESTS)
	rm -f *~nickdb
	rm -f *~debug
	rm -f *.store
	rm -f nickdb debug

.PHONY: all clean
