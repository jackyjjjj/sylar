# cmake_minimum_required(VERSION 3.15)
# project(sylar)

# set(HOME /home/jyl/space/code/sylar)
# file(GLOB SRC ${HOME}/src/*.cpp)

# include_directories(${HOME}/include)

# set(EXECUTABLE_OUTPUT_PATH ${HOME}/build)

# set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -Wall")
# # set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build)
# # set(SRC_LIST ${PROJECT_SOURCE_DIR}/src/.*cpp)




# link_directories(${HOME}/lib)

# # add_library(sylar SHARED ${SRC})


# add_executable(do_sylar ${SRC})

# # target_link_libraries(app calc)


cmake_minimum_required(VERSION 3.15)
project(sylar)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++14 -Wall")
add_compile_options(-lyaml-cpp)
file (GLOB SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

set(SRC_FILES
    ${SRC}
    ${CMAKE_CURRENT_SOURCE_DIR}/test/test04.cpp
    )

find_package(yaml-cpp REQUIRED)#找到package
# add_executable(yaml-test src/yaml-test.cpp)
# target_link_libraries(yaml-cpp)

include_directories(${PROJECT_SOURCE_DIR}/include/*.h)
# include_directories(/usr/local/include/yaml-cpp/*.h)


# set(SRC_LIST ${PROJECT_SOURCE_DIR}/src/.*cpp)



# 创建动态库

# set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
# add_library(sylar SHARED ${SRC})

#编译代码

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/test)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)
add_executable(test04 ${SRC_FILES})
target_link_libraries(test04 /usr/local/lib/libyaml-cpp.a)
# target_link_libraries(test01 sylar)