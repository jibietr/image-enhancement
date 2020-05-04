cmake_minimum_required(VERSION 2.8)

project(image_enhancer)

set(CMAKE_CXX_STANDARD 11)

find_package(OpenCV 3 REQUIRED)
find_package(dlib REQUIRED)
find_package(Armadillo REQUIRED)
include_directories(
    ${OpenCV_INCLUDE_DIRS}
    ${ARMADILLO_INCLUDE_DIRS}
    include
)

add_executable(image-enhancer 
    src/util.cpp
    src/LDR.cpp
    src/IAGCWD.cpp
    src/remove_boders.cpp
    src/draw_canvas.cpp
    src/enhancer.cpp
)
target_link_libraries(image-enhancer
    ${OpenCV_LIBS} 
    ${ARMADILLO_LIBRARIES}
    dlib::dlib 
)

