file(GLOB ROOTGAME_HEADERS *.hpp)
file(GLOB ROOTGAME_SOURCES *.cpp)

set(ROOTGAME_HEADERS
	${ROOTGAME_HEADERS}
	)

set(ROOTGAME_SOURCES
	${ROOTGAME_SOURCES}
	)

source_group("" FILES ${ROOTGAME_HEADERS})
source_group("" FILES ${ROOTGAME_SOURCES})

include_directories(.)
include_directories(../Core/)
include_directories(../OakUtils/)

add_library(Game MODULE ${ROOTGAME_HEADERS} ${ROOTGAME_SOURCES})

target_precompile_headers(Game PUBLIC GamePCH.hpp)