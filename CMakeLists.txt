cmake_minimum_required(VERSION 3.10)

# set the project name
project(prostgres)

# add the executable

file(GLOB lib_SRC
  "lib/*.h"
  "lib/*.cpp"
)

add_executable(prostgres main.cpp ${lib_SRC})

set_target_properties(prostgres PROPERTIES CXX_STANDARD 17)

target_link_libraries(${PROJECT_NAME} stdc++fs)