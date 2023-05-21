CC = gcc
CFLAGS = -Wall -Werror -pedantic
LDFLAGS = -shared

LIBRARY_NAME = st_reactor.so

SRC_FILES = reactor.c
OBJ_FILES = $(SRC_FILES:.c=.o)

all: $(LIBRARY_NAME)

$(LIBRARY_NAME): $(OBJ_FILES)
	$(CC) $(LDFLAGS) $(OBJ_FILES) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_FILES) $(LIBRARY_NAME)
