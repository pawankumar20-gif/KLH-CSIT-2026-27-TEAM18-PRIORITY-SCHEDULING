CC=gcc
CFLAGS=-Wall -Wextra -O2
LDFLAGS=-pthread

TARGET=priority_system
SRC=main.c co1_system.c co2_scheduling.c co3_ipc.c co4_memory.c co5_files.c co6_threads.c

all: $(TARGET)

$(TARGET): $(SRC) project.h
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET) scheduling_results.txt
	rm -f /tmp/priority_project_fifo
