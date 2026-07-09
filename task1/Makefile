CC ?= cc
CFLAGS ?= -O2 -g
LDFLAGS ?=
DEPFLAGS ?= -MMD -MP
DEBUG_CFLAGS ?= -O0 -g -DDEBUG -fsanitize=address -fno-omit-frame-pointer
DEBUG_LDFLAGS ?= -fsanitize=address

MAIN_SRC := main.c
TEST_SRCS := $(wildcard test_*.c)

LIB_SRCS := $(filter-out %main.c %test_%, $(wildcard *.c ../htable/*.c ../vector/*.c ../dijkstra_heap/*.c)) 

MAIN_OBJ := $(MAIN_SRC:.c=.o)
TEST_OBJS := $(TEST_SRCS:.c=.o)
LIB_OBJS := $(LIB_SRCS:.c=.o)

ALL_OBJS := $(MAIN_OBJ) $(TEST_OBJS) $(LIB_OBJS)
ALL_DEPS := $(ALL_OBJS:.o=.d)

DEBUG_MAIN_OBJ := $(MAIN_SRC:%.c=debug/%.o)
DEBUG_LIB_OBJS := $(patsubst %.c,debug/%.o,$(patsubst ../%,%,$(LIB_SRCS)))
DEBUG_OBJS := $(DEBUG_MAIN_OBJ) $(DEBUG_LIB_OBJS)
DEBUG_DEPS := $(DEBUG_OBJS:.o=.d)

.PHONY: all clean debug

all: main tests

main: $(MAIN_OBJ) $(LIB_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

tests: $(TEST_OBJS) $(LIB_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

debug: main-debug

main-debug: $(DEBUG_MAIN_OBJ) $(DEBUG_LIB_OBJS)
	$(CC) $(DEBUG_CFLAGS) -o $@ $^ $(LDFLAGS) $(DEBUG_LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $(DEPFLAGS) -c -o $@ $<

debug/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(DEBUG_CFLAGS) $(DEPFLAGS) -c -o $@ $<

debug/htable/%.o: ../htable/%.c
	mkdir -p $(dir $@)
	$(CC) $(DEBUG_CFLAGS) $(DEPFLAGS) -c -o $@ $<

debug/vector/%.o: ../vector/%.c
	mkdir -p $(dir $@)
	$(CC) $(DEBUG_CFLAGS) $(DEPFLAGS) -c -o $@ $<

debug/dijkstra_heap/%.o: ../dijkstra_heap/%.c
	mkdir -p $(dir $@)
	$(CC) $(DEBUG_CFLAGS) $(DEPFLAGS) -c -o $@ $<

clean:
	rm -f $(ALL_OBJS) $(ALL_DEPS) $(DEBUG_OBJS) $(DEBUG_DEPS) main tests main-debug
	rm -rf debug

-include $(ALL_DEPS)
-include $(DEBUG_DEPS)
