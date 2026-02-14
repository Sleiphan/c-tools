cmake_minimum_required(VERSION 3.7...3.26)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(Temp_project
    VERSION 0.1
    DESCRIPTION "An example project."
    LANGUAGES CXX C)

enable_testing()


set(CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/pdb)



set(CTOOLS_LIB "ctools")
add_library(${CTOOLS_LIB} STATIC)
target_include_directories(${CTOOLS_LIB} PUBLIC include)
set_target_properties(${CTOOLS_LIB}
    PROPERTIES 
    LINKER_LANGUAGE C
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/../bin
)



# Add Google C++ Testing Framework as part of this project
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)



add_subdirectory(src)
add_subdirectory(apps)
add_subdirectory(test)

