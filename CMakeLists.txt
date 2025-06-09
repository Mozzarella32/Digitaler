cmake_minimum_required(VERSION 3.13)

project(Digitaler)

include(FetchContent)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND NOT WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

add_subdirectory(${PROJECT_SOURCE_DIR}/submodules/eigen)

set(Eigen3_SOURCE_DIR ${PROJECT_SOURCE_DIR}/submodules/eigen)

add_subdirectory(${PROJECT_SOURCE_DIR}/submodules/MyWxWidgets)

add_subdirectory(${PROJECT_SOURCE_DIR}/submodules/PreparePngs)

add_subdirectory(${PROJECT_SOURCE_DIR}/submodules/ShadersToCharArray)

# Directories
set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(LIB_DIR "${PROJECT_SOURCE_DIR}/lib")

set(SHADER_DIR "${PROJECT_SOURCE_DIR}/resources/shaders/Source")
set(PNG_DIR "${PROJECT_SOURCE_DIR}/resources/art/Source")

set(PCH_HEADERS "${INCLUDE_DIR}/pch.hpp")

file(GLOB_RECURSE MY_SOURCE CONFIGURE_DEPENDS "${SRC_DIR}/*")
file(GLOB_RECURSE MY_HEADERS CONFIGURE_DEPENDS "${INCLUDE_DIR}/*")

#To trigger rebuild on changes//out of date comment
file(GLOB_RECURSE MY_SHADERS "${SHADER_DIR}/*")
file(GLOB_RECURSE MY_PNGS "${PNG_DIR}/*")


list(REMOVE_ITEM MY_SOURCE ${PCH_HEADERS})
list(REMOVE_ITEM MY_HEADERS ${PCH_HEADERS})

# Create Executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/app)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/app)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/app)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${PROJECT_SOURCE_DIR}/app)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${PROJECT_SOURCE_DIR}/app)

add_executable(${PROJECT_NAME} ${MY_SOURCE})

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
target_include_directories(${PROJECT_NAME} PUBLIC ${INCLUDE_DIR} ${Eigen3_SOURCE_DIR})

# wxWidgets Linking
target_link_libraries(${PROJECT_NAME} PUBLIC wx::base wx::core wx::gl)
# MyOpenGl Linking
target_link_libraries(${PROJECT_NAME} PUBLIC MyOpenGl)

# MyWxWidgets Linking
target_link_libraries(${PROJECT_NAME} PUBLIC MyWxWidgets)

# Invok the pre build executable to generate Shaders and Pngs as bytarrays as there is still no #embede suppoert on msvc
add_custom_target(
    Invoke_ShadersToCharArray
    COMMAND ${PROJECT_SOURCE_DIR}/resources/shaders/$<TARGET_FILE_NAME:ShadersToCharArray>
    COMMENT "Calling ShadersToCharArray"
)
add_dependencies(Invoke_ShadersToCharArray ShadersToCharArray)

add_custom_target(
    Invoke_PreparePngs
    COMMAND ${PROJECT_SOURCE_DIR}/resources/art/$<TARGET_FILE_NAME:PreparePngs>
    COMMENT "Calling PreparePngs"
)
add_dependencies(Invoke_PreparePngs PreparePngs)

add_dependencies(${PROJECT_NAME} Invoke_ShadersToCharArray Invoke_PreparePngs)

