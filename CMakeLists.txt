cmake_minimum_required(VERSION 3.3)
project(energyawaredc)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp datacenter.cpp datacenter.h physicalmachine.cpp physicalmachine.h virtualmachine.cpp virtualmachine.h simulator.cpp simulator.h event.cpp event.h configuration.h randomnumbergenerator.cpp randomnumbergenerator.h)
add_executable(energyawaredc ${SOURCE_FILES})

find_package(GSL)
if (GSL_FOUND)
    include_directories(${GSL_INCLUDE_DIRS})
    target_link_libraries(energyawaredc ${GSL_LIBRARIES})
endif (GSL_FOUND)