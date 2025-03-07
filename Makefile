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

CFLAGS := -O -g -Wall -Werror -std=c99 -D_DEFAULT_SOURCE -D_GNU_SOURCE

LDFLAGS := -luuid

TESTS := fileiotest pageiotest

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
