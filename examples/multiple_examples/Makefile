CC := gcc
CFLAGS := -I../../headers -std=c99 -Wall -Wextra -pedantic
SOURCES := ../../sources/my_printf.c ../../sources/format.c ../../sources/convert.c

EXAMPLES := basic_print string_examples fd_example vlist_example

all: $(EXAMPLES)

%: %.c $(SOURCES)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(EXAMPLES)
