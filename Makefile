CC := gcc

CFLAGS := -O -g -Wall -Werror -std=c99 -D_DEFAULT_SOURCE -D_GNU_SOURCE $(GLIBCFLAGS)

TESTS :=

SOURCES := src/nickdb.c
OBJS := $(patsubst %.c,%.o,$(SOURCES))

LIBS :=
LIBOBJS := $(patsubst %.c,%.o,$(LIBS))


nickdb: $(OBJS) $(LIBOBJS)
	$(CC) -o $@ $^ $(GLIBLDFLAGS)

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
	$(CC) -o $@ $^

clean:
	rm -f src/*~ src/lib/*~ src/lib/*/*~ tests/*~ $(OBJS) $(LIBOBJS)
	rm -f $(TESTS)
	rm -f *~ nickdb

.PHONY: all clean
