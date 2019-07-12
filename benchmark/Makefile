CXXFLAGS = -I$(PWD)/../include -lbenchmark -lpthread -std=c++17

all: bin_dir
	g++ -o bin/benchmark_csv_write benchmark_csv_write.cpp ${CXXFLAGS}
	./bin/benchmark_csv_write

release: CXXFLAGS += -O3
release: all

bin_dir:
	mkdir -p bin

clean:
	rm bin/benchmark_csv_write
