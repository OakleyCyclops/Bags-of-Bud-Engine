file(GLOB ROOT_HEADERS *.hpp)
file(GLOB ROOT_SOURCES *.cpp)

file(GLOB CONTAINER_HEADERS Containers/*.hpp)
file(GLOB CONTAINER_SOURCES Containers/*.cpp)

file(GLOB MATH_HEADERS Math/*.hpp)
file(GLOB MATH_SOURCES Math/*.cpp)

set(LIB_HEADERS
	${ROOT_HEADERS}
	)

set(LIB_SOURCES
	${ROOT_SOURCES}
	)

source_group("" FILES ${ROOT_HEADERS})
source_group("" FILES ${ROOT_SOURCES})

include_directories(.)
include_directories(../Core/)
include_directories(../OakUtils/)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)

add_library(BudRenderer SHARED ${LIB_HEADERS} ${LIB_SOURCES})

target_precompile_headers(BudRenderer PUBLIC BudRendererPCH.hpp)
