CC = gcc
CXXFLAGS = -Wall -std=gnu99
LIBS = 

PROJECT = profiler

HEADERS = gnuplot_i/gnuplot_i.h
OBJECTS = profiler.o gnuplot_i/gnuplot_i.o

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
