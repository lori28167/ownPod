cmake_minimum_required(VERSION 3.20)
project(ownpod-player VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# LVGL configuration
set(LV_CONF_BUILD_DISABLE_EXAMPLES ON)
set(LV_CONF_BUILD_DISABLE_DEMOS ON)
set(LV_CONF_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include/lv_conf.h")

# Find system libraries
find_package(PkgConfig REQUIRED)
pkg_check_modules(MPV REQUIRED mpv)
pkg_check_modules(GPIOD REQUIRED libgpiod)
pkg_check_modules(DBUS REQUIRED dbus-1)

# Sources
file(GLOB_RECURSE SOURCES
    src/*.cpp
    src/*.c
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    include
    lib/lvgl
    ${MPV_INCLUDE_DIRS}
    ${GPIOD_INCLUDE_DIRS}
    ${DBUS_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    lvgl
    ${MPV_LIBRARIES}
    ${GPIOD_LIBRARIES}
    ${DBUS_LIBRARIES}
    pthread
)

# LVGL as subdirectory (clone into lib/lvgl)
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/lib/lvgl/CMakeLists.txt")
    add_subdirectory(lib/lvgl)
endif()
