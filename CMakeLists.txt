project(AsciiColorPrint CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

add_executable(asciicp)

set(SOURCES
	src/main.cpp
)

target_sources(asciicp PRIVATE ${SOURCES})

if(MSVC)
	target_compile_options(asciicp PRIVATE /W4 /WX /std:c++latest)
  target_compile_definitions(asciicp PRIVATE _SCL_SECURE_NO_WARNINGS)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(asciicp PRIVATE -Wall -Wextra -Werror)
endif()

target_link_libraries(asciicp PRIVATE CONAN_PKG::colmc)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
