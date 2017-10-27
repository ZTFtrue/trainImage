cmake_minimum_required(VERSION 3.8)
project(opencv_train)
find_package( OpenCV REQUIRED )
set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp Distance.cpp Distance.h dist/json/json.h dist/json/json-forwards.h dist/jsoncpp.cpp ImageTools.cpp ImageTools.h out_line.cpp out_line.h)
add_executable(opencv_train ${SOURCE_FILES} ImageTools.cpp ImageTools.h out_line.cpp out_line.h)
target_link_libraries( opencv_train ${OpenCV_LIBS} )