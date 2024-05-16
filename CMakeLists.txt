cmake_minimum_required(VERSION 3.19)
project(image_receiver_boost)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost COMPONENTS system thread REQUIRED)
find_package(OpenCV REQUIRED)

include_directories(image_receiver_boost ${Boost_INCLUDE_DIRS} ${OpenCV_INCLUDE_DIRS})
link_directories(image_receiver_boost ${Boost_LIBRARY_DIRS})
add_executable(image_receiver_boost
        include/base64.hpp
        include/session.hpp
        src/session.cpp
        include/tcp_server.hpp
        src/tcp_server.cpp
        app/main.cpp
)
target_link_libraries(image_receiver_boost ${Boost_LIBRARIES} ${OpenCV_LIBS})