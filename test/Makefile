CXXFLAGS = -I$(PWD)/../include -lgtest -lpthread -std=c++17 -ggdb3

all: bin_dir
	g++ -o bin/test_line_reader test_line_reader.cpp ${CXXFLAGS}
	g++ -o bin/test_escape_utils test_escape_utils.cpp ${CXXFLAGS}
	g++ -o bin/test_split_function test_split_function.cpp ${CXXFLAGS}
	g++ -o bin/test_csv_reader test_csv_reader.cpp ${CXXFLAGS}
	./bin/test_line_reader
	./bin/test_escape_utils
	./bin/test_split_function
	./bin/test_csv_reader
bin_dir:
	mkdir -p bin
clean:
	rm bin/test_line_reader bin/test_escape_utils bin/test_split_function bin/test_csv_reader
