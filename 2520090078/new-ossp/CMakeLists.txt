cmake_minimum_required(VERSION 3.16)
project(ParallelFileSearch VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Threads REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# Core independent search engine library
add_library(pfs_core STATIC
    include/search_result.h
    include/thread_pool.h
    include/search_worker.h
    include/search_engine.h
    include/export_manager.h
    include/logger.h
    src/thread_pool.cpp
    src/search_worker.cpp
    src/search_engine.cpp
    src/export_manager.cpp
    src/logger.cpp
)

target_include_directories(pfs_core PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(pfs_core PUBLIC 
    Threads::Threads
)

# Desktop GUI Application
add_executable(ParallelFileSearch
    include/search_controller.h
    src/search_controller.cpp
    ui/main_window.h
    src/main_window.cpp
    src/main.cpp
)

target_include_directories(ParallelFileSearch PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/ui
)

target_link_libraries(ParallelFileSearch PRIVATE
    pfs_core
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# Enable testing
enable_testing()
add_subdirectory(tests)
