.PHONY:all bench bench2
all:bench bench2
bench:benchmark.cc
	g++ -O2 -std=c++11 $^ -o $@
bench2:benchmark2.cc
	g++ -O2 -std=c++11 $^ -o $@
.PHONY:clean
clean:
	rm -rf bench bench2 logfile/ 