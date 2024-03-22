CC = gcc
DEBUG = -g -O0 -Wall -Wshadow -Wunreachable-code -Wredundant-decls -Wmissing-declarations -Wold-style-definition -Wmissing-prototypes -Wdeclaration-after-statement -Werror -Wno-return-local-addr -Wunsafe-loop-optimizations -Wuninitialized -fmax-errors=1
CFLAGS = $(DEBUG)
LDFLAGS = -pthread

SOURCES = rockem_client.c rockem_server.c
OBJECTS = $(SOURCES:.c=.o)
TARGETS = $(OBJECTS:.o=)
.PHONY = clean revision_control test clean_tests

all: $(TARGETS)

$(TARGETS): %: %.o
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test: all
	cd test && \
	./create_random_files.bash && \
	./tests_local.sh

TAR_FILE = $(LOGNAME)_crypt.tar.gz
tar:
	rm -f $(TAR_FILE)
	tar cvaf $(TAR_FILE) *.[ch] Makefile
	tar tvaf $(TAR_FILE)

revision_control:
	git add .
	printf "Commit message: " && \
	read MSG && \
	git commit -m "$$MSG"

clean:
	rm -f $(OBJECTS) $(TARGETS) *.out *.o rockem_client rockem_server test/.create_files_done
	find . -name '*.dat' -exec rm "{}" \;

clean_tests:
	find ./test/server -name '*.dat' -exec rm "{}" \;
	find ./test/client -name '*.dat' -exec rm "{}" \;
