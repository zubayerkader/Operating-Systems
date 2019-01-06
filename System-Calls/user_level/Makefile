# Makefile for building application targeting QEMU system
# by Brian Fraser

# Edit this file to compile extra C files into their own programs.
TARGET=cs300_testapp array_stats_test process_ancestor_test 
QEMU_LAUNCH_DIR=~/sfuhome/cmpt300/linux-stable/

CC_C = gcc

CFLAGS = -Werror -Wall -g -std=c99 -D _GNU_SOURCE -static -pthread

all: clean cs300_testapp array_stats_test process_ancestor_test

cs300_testapp:
	$(CC_C) $(CFLAGS) $@.c -o $@

array_stats_test:
	$(CC_C) $(CFLAGS) $@.c -o $@

process_ancestor_test:
	$(CC_C) $(CFLAGS) $@.c -o $@

clean:
	rm -f cs300_testapp array_stats_test process_ancestor_test 

transfer:
	scp -P 2222 cs300_testapp root@localhost:~
	scp -P 2222 array_stats_test root@localhost:~
	scp -P 2222 process_ancestor_test root@localhost:~

killqemu:
	killall qemu-system-x86_64
