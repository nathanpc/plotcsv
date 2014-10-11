CC = gcc
CXXFLAGS = -Wall -std=gnu99
LIBS = -lm

PROJECT = profiler

HEADERS = src/gnuplot_i.h
OBJECTS = src/profiler.o src/gnuplot_i.o

all: $(PROJECT)

$(PROJECT): $(OBJECTS)
	$(CC) $(CXXFLAGS) $(OBJECTS) -o $@ $(LIBS)

debug: CXXFLAGS += -g3 -DDEBUG
debug: $(PROJECT)

%.o: %.c $(HEADERS)
	$(CC) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf src/*.o
	rm -rf $(PROJECT)
