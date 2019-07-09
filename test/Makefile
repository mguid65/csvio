CXXFLAGS = -I$(PWD)/../include -lgtest -lpthread -std=c++17 

all:
	rm -r bin && mkdir bin
	g++ -o bin/test_line_reader test_line_reader.cpp ${CXXFLAGS}
	g++ -o bin/test_escape_utils test_escape_utils.cpp ${CXXFLAGS}
	./bin/test_line_reader
	./bin/test_escape_utils
clean:
	rm bin/test_line_reader bin/test_escape_utils
