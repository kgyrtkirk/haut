cmake_minimum_required(VERSION 3.0.2)
project(serialotad)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin/")

set(CMAKE_CXX_FLAGS "-Wall -O0 -Wextra -std=c++0x ${CMAKE_CXX_FLAGS}")
add_definitions(-DBOOST_LOG_DYN_LINK)

# Configure CCache if available
find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
        set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
        set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)


file(GLOB serialotad_SRC
    "src/*.h"
    "src/*.cpp"
)

FIND_PACKAGE( Boost 1.40 COMPONENTS program_options log log_setup iostreams REQUIRED )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

add_executable(serialotad ${serialotad_SRC})
target_link_libraries(serialotad ${Boost_LIBRARIES} pthread util)



