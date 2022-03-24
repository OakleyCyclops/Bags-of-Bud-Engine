
add_definitions(-D__libBud__ -D__DOOM_DLL__)

file(GLOB ID__INCLUDES *.h)
file(GLOB ID__SOURCES *.cpp)
file(GLOB ID_BV_INCLUDES bv/*.h)
file(GLOB ID_BV_SOURCES bv/*.cpp)
file(GLOB ID_CONTAINERS_INCLUDES containers/*.h)
file(GLOB ID_CONTAINERS_SOURCES containers/*.cpp)
file(GLOB ID_GEOMETRY_INCLUDES geometry/*.h)
file(GLOB ID_GEOMETRY_SOURCES geometry/*.cpp)
file(GLOB ID_HASHING_INCLUDES hashing/*.h)
file(GLOB ID_HASHING_SOURCES hashing/*.cpp)
file(GLOB ID_MATH_INCLUDES math/*.h)
file(GLOB ID_MATH_SOURCES math/*.cpp)
file(GLOB ID_SYS_INCLUDES sys/*.h)
file(GLOB ID_SYS_SOURCES sys/*.cpp)

file(GLOB ID_SYS_POSIX_INCLUDES sys/posix/*.h)
file(GLOB ID_SYS_POSIX_SOURCES sys/posix/*.cpp)

set(ID_INCLUDES_ALL
			${ID__INCLUDES}
			${ID_BV_INCLUDES}
			${ID_CONTAINERS_INCLUDES}
			${ID_GEOMETRY_INCLUDES}
			${ID_HASHING_INCLUDES}
			${ID_MATH_INCLUDES}
			${ID_SYS_INCLUDES}
			)
			
set(ID_SOURCES_ALL
			${ID__SOURCES}
			${ID_BV_SOURCES}
			${ID_CONTAINERS_SOURCES}
			${ID_GEOMETRY_SOURCES}
			${ID_HASHING_SOURCES}
			${ID_MATH_SOURCES}
			${ID_SYS_SOURCES}
			)
			
if(MSVC)
	list(APPEND ID_INCLUDES_ALL ${ID_SYS_WIN32_INCLUDES})
	list(APPEND ID_SOURCES_ALL ${ID_SYS_WIN32_SOURCES})
else()
	list(APPEND ID_INCLUDES_ALL ${ID_SYS_POSIX_INCLUDES})
	list(APPEND ID_SOURCES_ALL ${ID_SYS_POSIX_SOURCES})
endif()

source_group("" FILES ${ID__INCLUDES})
source_group("" FILES ${ID__SOURCES})
source_group("bv" FILES ${ID_BV_INCLUDES})
source_group("bv" FILES ${ID_BV_SOURCES})
source_group("containers" FILES ${ID_CONTAINERS_INCLUDES})
source_group("containers" FILES ${ID_CONTAINERS_SOURCES})
source_group("geometry" FILES ${ID_GEOMETRY_INCLUDES})
source_group("geometry" FILES ${ID_GEOMETRY_SOURCES})
source_group("hashing" FILES ${ID_HASHING_INCLUDES})
source_group("hashing" FILES ${ID_HASHING_SOURCES})
source_group("math" FILES ${ID_MATH_INCLUDES})
source_group("math" FILES ${ID_MATH_SOURCES})
source_group("sys" FILES ${ID_SYS_INCLUDES})
source_group("sys" FILES ${ID_SYS_SOURCES})

source_group("sys\\posix" FILES ${ID_SYS_POSIX_INCLUDES})
source_group("sys\\posix" FILES ${ID_SYS_POSIX_SOURCES})

#if(STANDALONE)
#	add_definitions(-DSTANDALONE)
#endif()

set(ID_PRECOMPILED_SOURCES ${ID_SOURCES_ALL})
list(REMOVE_ITEM ID_PRECOMPILED_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/geometry/RenderMatrix.cpp)
list(REMOVE_ITEM ID_PRECOMPILED_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/SoftwareCache.cpp)

if(MSVC)
    list(REMOVE_ITEM ID_PRECOMPILED_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/PCH.cpp)

    #set_target_properties(libBud PROPERTIES COMPILE_FLAGS "/YuPCH.hpp")
    set_source_files_properties(PCH.cpp
        PROPERTIES
        COMPILE_FLAGS "/YcPCH.hpp"
        OBJECT_OUTPUTS "PCH.pch"
        )
        set_source_files_properties(
            ${ID_PRECOMPILED_SOURCES}
            PROPERTIES
            COMPILE_FLAGS "/YuPCH.hpp"
            OBJECT_DEPENDS "PCH.pch"
            )

    add_library(Bud ${ID_SOURCES_ALL} ${ID_INCLUDES_ALL})
else()
	if (USE_PRECOMPILED_HEADERS)
	foreach( src_file ${ID_PRECOMPILED_SOURCES} )
		#message(STATUS "-include PCH.hpp for ${src_file}")
		set_source_files_properties(
			${src_file}
			PROPERTIES
			COMPILE_FLAGS "-include ${CMAKE_CURRENT_SOURCE_DIR}/PCH.hpp"
			)
	endforeach()
	endif()

	include_directories(.)
	
	if (USE_PRECOMPILED_HEADERS)
	# PCH magic for GCC/clang, adapted from https://gist.github.com/573926
	STRING(TOUPPER "CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}" _flags_var_name)
	SET(_compiler_FLAGS ${${_flags_var_name}})
	GET_DIRECTORY_PROPERTY(_directory_flags INCLUDE_DIRECTORIES)
	FOREACH(item ${_directory_flags})
		LIST(APPEND _compiler_FLAGS " -I${item}")
	ENDFOREACH(item)
	endif()

	GET_DIRECTORY_PROPERTY(_directory_flags DEFINITIONS)
	LIST(APPEND _compiler_FLAGS ${_directory_flags})
	
	SEPARATE_ARGUMENTS(_compiler_FLAGS)
	
	if (USE_PRECOMPILED_HEADERS)
	add_custom_target(precomp_header_libBud ALL
	                  COMMAND ${CMAKE_CXX_COMPILER} ${_compiler_FLAGS} -x c++-header PCH.hpp -o PCH.hpp.gch
	                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	                  COMMENT "Creating libBud/PCH.hpp.gch for libBud"
	                  )
	endif()
	
	add_library(Bud ${ID_SOURCES_ALL} ${ID_INCLUDES_ALL})
	if (USE_PRECOMPILED_HEADERS)
	add_dependencies(Bud precomp_header_libBud)
	endif()
	
endif()
	

# if(MSVC)
	# # set_source_files_properties(PCH.cpp
        # # PROPERTIES
        # # COMPILE_FLAGS "/YcPCH.hpp"
        # # )
		
	# #set_target_properties(libBud PROPERTIES COMPILE_FLAGS "/YuPCH.hpp")
# endif()
