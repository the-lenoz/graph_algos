CC ?= cc
CFLAGS ?= -O2 -g
LDFLAGS ?=
DEPFLAGS ?= -MMD -MP

MAIN_SRC := main.c
TEST_SRCS := $(wildcard test_*.c)

LIB_SRCS := $(filter-out %main.c %test_%, $(wildcard *.c ../htable/*.c ../vector/*.c)) 

MAIN_OBJ := $(MAIN_SRC:.c=.o)
TEST_OBJS := $(TEST_SRCS:.c=.o)
LIB_OBJS := $(LIB_SRCS:.c=.o)

ALL_OBJS := $(MAIN_OBJ) $(TEST_OBJS) $(LIB_OBJS)
ALL_DEPS := $(ALL_OBJS:.o=.d)

.PHONY: all tests clean

all: main tests

main: $(MAIN_OBJ) $(LIB_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

tests: $(TEST_OBJS) $(LIB_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $(DEPFLAGS) -c -o $@ $<

clean:
	rm -f $(ALL_OBJS) $(ALL_DEPS) main tests

-include $(ALL_DEPS)
