CC=gcc
MAKE=make
LIBS=-pthread -ldl -lgc
INCLUDES=
CFLAGS=-Wall -Werror -g -rdynamic

EXECUTABLE=skelde
OBJECTS=src/bstrlib.o src/stuff.o \
		src/bytecode.o src/object.o src/number.o src/list.o \
		src/exception.o src/method.o src/call.o src/thread.o \
		src/mutex.o src/skstring.o src/message.o src/vm.o src/main.o

all: $(EXECUTABLE)

.PHONY: clean

$(EXECUTABLE): $(OBJECTS)
	$(CC) -o $(EXECUTABLE) $(LIBS) $(INCLUDES) $(CFLAGS) $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

clean:
	rm -f src/*.o $(EXECUTABLE)

