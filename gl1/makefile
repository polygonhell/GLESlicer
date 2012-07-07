CC=g++
CFLAGS=-c -Wall -g
LDFLAGS=-lEGL -lGLESv2 -lm -lts
SOURCES=main.cpp mat4.cpp stl.cpp model.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=gl1

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@