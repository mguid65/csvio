add_executable(test_csv_input_parser test_csv_input_parser.cpp)
target_link_libraries(test_csv_input_parser csvio gtest::gtest pthread)

add_test(NAME test_csv_input_parser WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND test_csv_input_parser)

add_executable(test_csv_line_reader test_csv_line_reader.cpp)
target_link_libraries(test_csv_line_reader csvio gtest::gtest pthread)

add_test(NAME test_csv_line_reader WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND test_csv_line_reader)

add_executable(test_csv_simple_line_reader test_csv_simple_line_reader.cpp)
target_link_libraries(test_csv_simple_line_reader csvio gtest::gtest pthread)

add_test(NAME test_csv_simple_line_reader WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND test_csv_simple_line_reader)

add_executable(test_csv_line_writer test_csv_line_writer.cpp)
target_link_libraries(test_csv_line_writer csvio gtest::gtest pthread)

add_test(NAME test_csv_line_writer WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND test_csv_line_writer)

add_executable(test_csv_output_formatter test_csv_output_formatter.cpp)
target_link_libraries(test_csv_output_formatter csvio gtest::gtest pthread)

add_test(NAME test_csv_output_formatter WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND test_csv_output_formatter)

add_executable(test_csv_reader test_csv_reader.cpp)
target_link_libraries(test_csv_reader csvio gtest::gtest pthread)

add_test(NAME test_csv_reader WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND test_csv_reader)

add_executable(test_csv_writer test_csv_writer.cpp)
target_link_libraries(test_csv_writer csvio gtest::gtest pthread)

add_test(NAME test_csv_writer WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND test_csv_writer)

add_executable(test_escape_utils test_escape_utils.cpp)
target_link_libraries(test_escape_utils csvio gtest::gtest pthread)

add_test(NAME test_escape_utils WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND test_escape_utils)

add_executable(test_csv_reader_ifstream test_csv_reader_ifstream.cpp)
target_link_libraries(test_csv_reader_ifstream csvio gtest::gtest pthread)

add_test(NAME test_csv_reader_ifstream WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND test_csv_reader_ifstream)

add_executable(test_csv_map_reader test_csv_map_reader.cpp)
target_link_libraries(test_csv_map_reader csvio gtest::gtest pthread)

add_test(NAME test_csv_map_reader WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND test_csv_map_reader)













