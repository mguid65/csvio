CXXFLAGS = -I$(PWD)/../include -lgtest -lpthread -std=c++17 -ggdb3

all: bin_dir
	g++ -o bin/test_csv_line_reader test_csv_line_reader.cpp ${CXXFLAGS}
	g++ -o bin/test_escape_utils test_escape_utils.cpp ${CXXFLAGS}
	g++ -o bin/test_csv_input_parser test_csv_input_parser.cpp ${CXXFLAGS}
	g++ -o bin/test_csv_reader test_csv_reader.cpp ${CXXFLAGS}
	g++ -o bin/test_csv_output_formatter test_csv_output_formatter.cpp ${CXXFLAGS}
	g++ -o bin/test_csv_writer test_csv_writer.cpp ${CXXFLAGS}
	g++ -o bin/test_csv_line_writer test_csv_line_writer.cpp ${CXXFLAGS}
	./bin/test_line_reader
	./bin/test_escape_utils
	./bin/test_csv_input_parser
	./bin/test_csv_reader
	./bin/test_csv_output_formatter
	./bin/test_csv_writer
	./bin/test_csv_line_writer
bin_dir:
	mkdir -p bin
clean:
	rm bin/test_csv_line_reader bin/test_escape_utils bin/test_csv_input_parser bin/test_csv_reader bin/test_csv_output_formatter bin/test_csv_writer bin/test_csv_line_writer
