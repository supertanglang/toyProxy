CC := gcc
SRC := src

CFLAGS := -c -g
LDFLAGS := -lpthread -g

BIN := toyProxy.out
SRCS := %.c
OBJ := main.o balancer.o network.o server.o threadpool.o


$(BIN): $(OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)

%.o: %.c %.h
	$(CC) $< -o $@ $(CFLAGS)

clean:
	-rm -rf $(OBJ) $(BIN)

