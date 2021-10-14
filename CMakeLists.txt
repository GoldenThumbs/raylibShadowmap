#https://github.com/SasLuca/raylib-cmake-template/blob/master/src/main.c
cmake_minimum_required(VERSION 3.0)
project(shadowmap_test C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# --- raylib options ---
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
set(OPENGL_VERSION "3.3" CACHE STRING "" FORCE)
# ----------------------

add_subdirectory(libs/raylib)
add_subdirectory(src)

add_executable(${PROJECT_NAME} ${SOURCE_CODE})
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/")