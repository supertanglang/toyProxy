CC := gcc
SRC := src

CFLAGS := -c -g
LDFLAGS := -lpthread -g

BIN := query_flood
SRCS := %.c
OBJ := main.o sender.o server.o adt.o network.o


$(BIN): $(OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)

%.o: %.c %.h
	$(CC) $< -o $@ $(CFLAGS)

clean:
	-rm -rf $(OBJ) $(BIN)

