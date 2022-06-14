cmake_minimum_required(VERSION 3.0)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

project(engine CXX)

file(GLOB CORE_INCLUDES Core/*.hpp)
file(GLOB CORE_SOURCES Core/*.cpp)

file(GLOB UNIX_INCLUDES Core/unix/*.hpp)
file(GLOB UNIX_SOURCES Core/unix/*.cpp)

file(GLOB SDL_INCLUDES Core/sdl/*.hpp)
file(GLOB SDL_SOURCES Core/sdl/*.cpp)


source_group("Core" FILES ${CORE_INCLUDES})
source_group("Core" FILES ${CORE_SOURCES})

source_group("Core\\unix" FILES ${UNIX_INCLUDES})
source_group("Core\\unix" FILES ${UNIX_SOURCES})

set(ENGINE_INCLUDES
	${INTF_INCLUDES}
	${CORE_INCLUDES}
	${UNIX_INCLUDES}
   )

set(ENGINE_SOURCES
	${CORE_SOURCES}
	${UNIX_SOURCES}
   )

add_subdirectory(OakUtils)
add_subdirectory(BudRenderer)
add_subdirectory(Game)

include_directories(OakUtils)
include_directories(Core)
include_directories(BudRenderer)
include_directories(Game)

add_executable(engine ${ENGINE_SOURCES})

target_precompile_headers(engine PUBLIC Core/CorePCH.hpp)

target_link_libraries(engine
					OakUtils
					glfw 
					z
					rt 
					${ZLIB_LIBRARIES} 
					minizip
					${PNG_LIBRARY} 
					${JPEG_LIBRARY} 
					${GLEW_LIBRARY} 
					${CMAKE_DL_LIBS})
