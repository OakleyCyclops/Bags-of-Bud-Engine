file(GLOB ROOT_HEADERS *.hpp)
file(GLOB ROOT_SOURCES *.cpp)

file(GLOB CONTAINER_HEADERS Containers/*.hpp)
file(GLOB CONTAINER_SOURCES Containers/*.cpp)

file(GLOB MATH_HEADERS Math/*.hpp)
file(GLOB MATH_SOURCES Math/*.cpp)

set(LIB_HEADERS
	${ROOT_HEADERS}
	${CONTAINER_HEADERS}
	${MATH_HEADERS}
	)

set(LIB_SOURCES
	${ROOT_SOURCES}
	${CONTAINER_SOURCES}
	${MATH_SOURCES}
	)

source_group("" FILES ${ROOT_HEADERS})
source_group("" FILES ${ROOT_SOURCES})

source_group("Containers" FILES ${CONTAINER_HEADERS})
source_group("Containers" FILES ${CONTAINER_SOURCES})

source_group("Math" FILES ${MATH_HEADERS})
source_group("Math" FILES ${MATH_SOURCES})

include_directories(.)
include_directories(../Core/)
include_directories(../Interfaces/)

add_library(Bud SHARED ${LIB_HEADERS} ${LIB_SOURCES})

target_precompile_headers(Bud PUBLIC libBudPCH.hpp)
