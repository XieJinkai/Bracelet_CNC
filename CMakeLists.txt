cmake_minimum_required(VERSION 3.20)

project(BraceletCNC LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(_occt_paths "$ENV{Path}")
set(_occt_found_dir "")
foreach(_p IN LISTS _occt_paths)
    if(EXISTS "${_p}/OpenCASCADEConfig.cmake")
        set(_occt_found_dir "${_p}")
        break()
    endif()
    if(EXISTS "${_p}/cmake/OpenCASCADE/OpenCASCADEConfig.cmake")
        set(_occt_found_dir "${_p}/cmake/OpenCASCADE")
        break()
    endif()
    if(EXISTS "${_p}/cmake/opencascade/OpenCASCADEConfig.cmake")
        set(_occt_found_dir "${_p}/cmake/opencascade")
        break()
    endif()
    get_filename_component(_p_parent "${_p}" DIRECTORY)
    if(EXISTS "${_p_parent}/cmake/OpenCASCADE/OpenCASCADEConfig.cmake")
        set(_occt_found_dir "${_p_parent}/cmake/OpenCASCADE")
        break()
    endif()
    if(EXISTS "${_p_parent}/lib/cmake/opencascade/OpenCASCADEConfig.cmake")
        set(_occt_found_dir "${_p_parent}/lib/cmake/opencascade")
        break()
    endif()
endforeach()

if(_occt_found_dir)
    set(OpenCASCADE_DIR "${_occt_found_dir}" CACHE PATH "" FORCE)
endif()

find_package(OpenCASCADE REQUIRED)

find_package(Qt6 QUIET COMPONENTS Widgets OpenGL OpenGLWidgets)
if(Qt6_FOUND)
    set(QT_LIBS Qt6::Widgets Qt6::OpenGL Qt6::OpenGLWidgets)
else()
    find_package(Qt5 REQUIRED COMPONENTS Widgets OpenGL)
    set(QT_LIBS Qt5::Widgets Qt5::OpenGL)
endif()

set(SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/OccView.cpp
    src/OccView.h
)

add_executable(BraceletCNC ${SOURCES})
target_include_directories(BraceletCNC PRIVATE ${OpenCASCADE_INCLUDE_DIR})
target_link_libraries(BraceletCNC PRIVATE ${OpenCASCADE_LIBRARIES} ${QT_LIBS})

if(DEFINED OpenCASCADE_USE_FILE)
    include(${OpenCASCADE_USE_FILE})
endif()
