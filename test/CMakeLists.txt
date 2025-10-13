add_executable(simple-cpp-in-dog-tests simple_cpp/in_dog/unx_socket_test.cpp)
target_link_libraries(simple-cpp-in-dog-tests PRIVATE Catch2::Catch2WithMain simple-cpp-in-dog-options simple-cpp-in-dog-lib)

include(Catch)
catch_discover_tests(simple-cpp-in-dog-tests)