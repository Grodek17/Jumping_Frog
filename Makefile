CXX = g++
CXXFLAGS = -Wall -Wextra
LDLIBS = -lncurses

frogger: src/main.cpp
	$(CXX) $(CXXFLAGS) src/main.cpp -o frogger $(LDLIBS)

clean:
	rm -f frogger

.PHONY: clean