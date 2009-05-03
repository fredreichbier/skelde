CC=gcc
MAKE=make
LIBS=-lgc
INCLUDES=
CFLAGS=-g -Wall -Werror

EXECUTABLE=skelde
OBJECTS=src/cvector.o src/hashmap.o src/bstrlib.o src/objlist.o \
		src/bytecode.o src/object.o src/number.o src/list.o \
		src/exception.o \
		src/skstring.o src/message.o src/vm.o src/main.o

all: $(EXECUTABLE)

.PHONY: clean

$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $(EXECUTABLE) $(LIBS) $(INCLUDES) $(CFLAGS) $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

clean:
	rm -f src/*.o $(EXECUTABLE)

