CXX = g++
CXXFLAGS = -O3 -std=c++20

# ------------------------------------------------------------
# Build all executables
# ------------------------------------------------------------
all: run_binary run_eightary run_pairing run_fibonacci

# ------------------------------------------------------------
# Individual builds
# ------------------------------------------------------------
run_binary:
	$(CXX) $(CXXFLAGS) -o run_binary src/main_binary.cpp

run_eightary:
	$(CXX) $(CXXFLAGS) -o run_eightary src/main_eightary.cpp

run_pairing:
	$(CXX) $(CXXFLAGS) -o run_pairing src/main_pairing.cpp

run_fibonacci:
	$(CXX) $(CXXFLAGS) -o run_fibonacci src/main_fibonacci.cpp

# ------------------------------------------------------------
# Clean targets
# ------------------------------------------------------------
clean:
	rm -f run_binary run_eightary run_pairing run_fibonacci

clean_binary:
	rm -f run_binary

clean_eightary:
	rm -f run_eightary

clean_pairing:
	rm -f run_pairing

clean_fibonacci:
	rm -f run_fibonacci

# ------------------------------------------------------------
# Execution helper (run all programs on one input file)
# Usage: make exec INPUT=graph.txt
# ------------------------------------------------------------
exec: all
	./run_binary $(INPUT)
	./run_eightary $(INPUT)
	./run_pairing $(INPUT)
	./run_fibonacci $(INPUT)
