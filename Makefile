CXX = g++
CXXFLAGS = -O3 -std=c++20

all: run_binary run_eightary run_pairing

run_binary:
	$(CXX) $(CXXFLAGS) -o run_binary src/main_binary.cpp

run_eightary:
	$(CXX) $(CXXFLAGS) -o run_eightary src/main_eightary.cpp

run_pairing:
	$(CXX) $(CXXFLAGS) -o run_pairing src/main_pairing.cpp

clean:
	rm -f run_binary run_eightary run_pairing
