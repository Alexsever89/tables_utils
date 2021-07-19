cmake_minimum_required (VERSION 3.16 FATAL_ERROR)

set(LIB_NAME tables_utils)
project(${LIB_NAME})

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Core REQUIRED)

set(HEADERS
	./include/table_model.h
)

set(SOURCES
	./src/table_model.cpp
)

add_library(${LIB_NAME} STATIC ${HEADERS} ${SOURCES})

target_include_directories(${LIB_NAME} PUBLIC ./include)

target_link_libraries(${LIB_NAME} PUBLIC
	Qt5::Core
)

target_compile_features(${LIB_NAME} PUBLIC cxx_std_17)