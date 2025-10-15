add_library(simple-cpp-in-dog-lib simple_cpp/in_dog/unix_socket.cpp
        simple_cpp/in_dog/fd.cpp
        simple_cpp/in_dog/event_fd.cpp
        simple_cpp/in_dog/input.cpp
        simple_cpp/in_dog/app.cpp)
target_link_libraries(simple-cpp-in-dog-lib PRIVATE simple-cpp-in-dog-options)
target_include_directories(simple-cpp-in-dog-lib PUBLIC ${LIBUDEV_INCLUDE_DIRECTORIES})
target_link_libraries(simple-cpp-in-dog-lib PRIVATE ${LIBUDEV_LIBRARIES} Libinput::Libinput)
target_include_directories(simple-cpp-in-dog-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(simple-cpp-in-dog-main simple_cpp/in_dog/main.cpp)
target_link_libraries(simple-cpp-in-dog-main PRIVATE simple-cpp-in-dog-options simple-cpp-in-dog-lib)

add_executable(simple-cpp-in-dog-client simple_cpp/in_dog/client.cpp )
target_link_libraries(simple-cpp-in-dog-client PRIVATE simple-cpp-in-dog-options)