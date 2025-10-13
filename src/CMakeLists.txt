add_library(simple-cpp-in-dog-lib simple_cpp/in_dog/unix_socket.cpp
        simple_cpp/in_dog/fd.cpp)
target_link_libraries(simple-cpp-in-dog-lib PRIVATE simple-cpp-in-dog-options)
target_include_directories(simple-cpp-in-dog-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(simple-cpp-in-dog-main simple_cpp/in_dog/main.cpp)
target_link_libraries(simple-cpp-in-dog-main PRIVATE simple-cpp-in-dog-options simple-cpp-in-dog-lib)