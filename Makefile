CXXFLAGS=-std=c++17 -O2 -fvisibility=hidden -Wall
LDFLAGS=-lncurses -lopenal
OBJECTS=core.o screen.o sound.o synth.o

all: rps

rps: $(OBJECTS) 
	$(CXX) -o rps $(OBJECTS) $(LDFLAGS)

.PHONY: clean
clean:
	rm -rf $(OBJECTS) rps
