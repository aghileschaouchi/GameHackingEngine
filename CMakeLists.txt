cmake_minimum_required(VERSION 3.27.6)
project(
    GameHackingEngine
                        VERSION 0.1.0
                        DESCRIPTION "Crossplatform C++ game hacking engine"
                        LANGUAGES CXX
    )
    
include(CTest)

add_library(${PROJECT_NAME} STATIC)

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)

target_include_directories(${PROJECT_NAME} PUBLIC include)

target_sources(${PROJECT_NAME}
	PRIVATE
		source/core/Address.cpp
		source/core/Game.cpp
		source/core/Process.cpp
)

# GameHackingEngine Tests

if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
	add_subdirectory(tests)
endif ()

enable_testing()

if(RUN_TESTS)
    run_tests(runGameHackingEngineTests GameHackingEngineTests)
endif()