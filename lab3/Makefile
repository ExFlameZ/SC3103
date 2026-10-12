CC =gcc
CFLAGS_COMMON := -Wall

SRCS := $(wildcard *.c)

OBJS := $(SRCS:.c=.o)

TARGET_SRCS := printFibonacci.c printAdd1K.c
TARGETS := $(TARGET_SRCS:.c=)

LIB_OBJS := $(filter-out $(TARGET_SRCS:.c=.o),$(OBJS))

.PHONY: all debug profile clean

all: CFLAGS := $(CFLAGS_COMMON) -O2
all: $(TARGETS)

debug: CFLAGS := $(CFLAGS_COMMON) -g
debug: $(TARGETS)

profile: CFLAGS := $(CFLAGS_COMMON) -g -pg
profile: $(TARGETS)

$(TARGETS): %: %.o $(LIB_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
clean:
	rm -f *.o $(TARGETS) output.txt gmon.out
