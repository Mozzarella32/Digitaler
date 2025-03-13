cmake_minimum_required(VERSION 3.13)

project(Digitaler)

include(FetchContent)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND NOT WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()


# WxWidgets
find_package(wxWidgets QUIET)

if (NOT wxWidgets_FOUND)
    message(STATUS "wxWidgets not found")
    set(wxBUILD_SHARED OFF)
    set(wxBUILD_TESTS OFF)
    set(wxBUILD_DEMOS OFF)
    set(wxBUILD_EXAMPLES OFF)
    set(wxBUILD_CXX_STANDARD 20)

    message(STATUS "Fetching wxWidgets...")
    FetchContent_Declare(
       wxWidgets
       GIT_REPOSITORY https://github.com/wxWidgets/wxWidgets.git
       GIT_SHALLOW ON
       CMAKE_ARGS
            -DwxUSE_STD=0
    )
    FetchContent_MakeAvailable(wxWidgets)
else()
    message(STATUS "wxWidgets Found")
endif()

# GLEW
find_package(GLEW QUIET)

if (NOT GLEW_FOUND)
    message(STATUS "GLEW not found")
    message(STATUS "Fetching GLEW...")

    set(glew-cmake_BUILD_SHARED OFF)
    set(USE_GLU ON)
    # https://github.com/Perlmint/glew-cmake/blob/master/glew-cmake/fetch-content.cmake
    FetchContent_Declare(
       GLEW
       GIT_REPOSITORY https://github.com/Perlmint/glew-cmake
       GIT_SHALLOW ON
    )
    FetchContent_MakeAvailable(glew)

    #add_subdirectory(${glew_SOURCE_DIR} ${glew_BINARY_DIR} EXCLUDE_FROM_ALL)
else()
    message(STATUS "GLEW Found")
endif()

# STB - Header-only-Bibliothek
message(STATUS "Fetching stb...")

FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_SHALLOW ON
)

FetchContent_MakeAvailable(stb)

set(STB_INCLUDE_DIR ${stb_SOURCE_DIR})

# wxWidgets
find_package(wxWidgets QUIET)

if (NOT wxWidgets_FOUND)
    message(STATUS "wxWidgets not found")
    set(wxBUILD_SHARED OFF)
    set(wxBUILD_TESTS OFF)
    set(wxBUILD_DEMOS OFF)
    set(wxBUILD_EXAMPLES OFF)

    message(STATUS "Fetching wxWidgets...")
    FetchContent_Declare(
       wxWidgets
       GIT_REPOSITORY https://github.com/wxWidgets/wxWidgets.git
       GIT_SHALLOW ON
       CMAKE_ARGS
           -DwxUSE_STD=0
    )
    FetchContent_MakeAvailable(wxWidgets)
else()
    message(STATUS "wxWidgets Found")
endif()

# Eigen
find_package(Eigen3 QUIET)

if (NOT Eigen3_FOUND)
    message(STATUS "Eigen3 not found")
    message(STATUS "Fetching Eigen3...")
    FetchContent_Declare(
       Eigen3
       GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
       GIT_SHALLOW ON
    )
    FetchContent_MakeAvailable(Eigen3)
else()
    message(STATUS "Eigen3 Found")
endif()


# MyWxWidgets
message(STATUS "Fetching MyWxWidgets...")

FetchContent_Declare(
       MyWxWidgets
       GIT_REPOSITORY https://github.com/Mozzarella32/MyWxWidgets.git
       GIT_SHALLOW ON
       GIT_TAG main
)
FetchContent_MakeAvailable(MyWxWidgets)

# MyOpenGl
message(STATUS "Fetching MyOpenGl...")

FetchContent_Declare(
       MyOpenGl
       GIT_REPOSITORY https://github.com/Mozzarella32/MyOpenGl.git
       GIT_SHALLOW ON
       GIT_TAG main
)
FetchContent_MakeAvailable(MyOpenGl)

# PreparePngs
FetchContent_Declare(
    PreparePngs
    GIT_REPOSITORY git@github.com:Mozzarella32/PreparePngs.git
    GIT_TAG main
)
FetchContent_MakeAvailable(PreparePngs)

# ShadersToCharArray
FetchContent_Declare(
    ShadersToCharArray
    GIT_REPOSITORY git@github.com:Mozzarella32/ShadersToCharArray.git
    GIT_TAG main
)
FetchContent_MakeAvailable(ShadersToCharArray)

# Directories
set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(LIB_DIR "${PROJECT_SOURCE_DIR}/lib")

set(SHADER_DIR "${PROJECT_SOURCE_DIR}/resources/shaders/Source")
set(PNG_DIR "${PROJECT_SOURCE_DIR}/resources/art/Source")

set(PCH_HEADERS "${INCLUDE_DIR}/pch.hpp")

file(GLOB_RECURSE MY_SOURCE CONFIGURE_DEPENDS "${SRC_DIR}/*")
file(GLOB_RECURSE MY_HEADERS CONFIGURE_DEPENDS "${INCLUDE_DIR}/*")

file(GLOB_RECURSE MY_SHADERS CONFIGURE_DEPENDS "${SHADER_DIR}/*")
file(GLOB_RECURSE MY_PNGS CONFIGURE_DEPENDS "${PNG_DIR}/*")


list(REMOVE_ITEM MY_SOURCE ${PCH_HEADERS})
list(REMOVE_ITEM MY_HEADERS ${PCH_HEADERS})

# Create Executable

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/app)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/app)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/app)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${PROJECT_SOURCE_DIR}/app)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${PROJECT_SOURCE_DIR}/app)

add_executable(${PROJECT_NAME} ${MY_SOURCE})

#set_property(GLOBAL PROPERTY USE_FOLDERS ON).

#source_group("Test\\Source" FILES ${MY_SOURCE})
#source_group("Test\\Include" FILES ${MY_HEADERS})
#source_group("Resources\\Shaders" FILES ${MY_SHADERS})
#source_group("Resources\\Pngs" FILES ${MY_PNGS})

if(WIN32)
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS>
        $<$<CONFIG:Debug>:/SUBSYSTEM:WINDOWS>)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/app
)

# Precompiled Header (falls verwendet)
target_precompile_headers(${PROJECT_NAME} PRIVATE ${PCH_HEADERS})

# Include-Direktiven
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIR} ${STB_INCLUDE_DIR} ${Eigen3_SOURCE_DIR})

# wxWidgets Linking
target_link_libraries(${PROJECT_NAME} PUBLIC wx::net wx::core wx::base wx::adv wx::html wx::gl wx::qa wx::xml wx::xrc)

# GLEW Linking
if (NOT GLEW_FOUND)
    target_link_libraries(${PROJECT_NAME} PRIVATE libglew_static)
else()
    target_link_libraries(${PROJECT_NAME} PUBLIC GLEW::glew_s)
endif()

# MyOpenGl Linking
target_link_libraries(${PROJECT_NAME} PUBLIC MyOpenGl)

# MyWxWidgets Linking
target_link_libraries(${PROJECT_NAME} PUBLIC MyWxWidgets)

# PRE BUILD
if(WIN32)
    set(ShadersToCharArray_EXECUTABLE "ShadersToCharArray.exe")
    set(PreparePngs_EXECUTABLE "PreparePngs.exe")
else()
    set(ShadersToCharArray_EXECUTABLE "ShadersToCharArray")
    set(PreparePngs_EXECUTABLE "PreparePngs")
endif()

add_dependencies(${PROJECT_NAME} ShadersToCharArray PreparePngs)

add_custom_command(
    TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${PROJECT_SOURCE_DIR}/resources/shaders/${ShadersToCharArray_EXECUTABLE}
    COMMENT "Calling ${ShadersToCharArray_EXECUTABLE}"
)

add_custom_command(
    TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${PROJECT_SOURCE_DIR}/resources/art/${PreparePngs_EXECUTABLE}
    COMMENT "Calling ${PreparePngs_EXECUTABLE}"
)
