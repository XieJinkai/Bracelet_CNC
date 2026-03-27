cmake_minimum_required(VERSION 3.20)

project(BraceletCNC LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(_occt_found_dir "")
if(EXISTS "E:/Git/OCCT/occt_vc14-64/cmake/OpenCASCADEConfig.cmake")
    set(_occt_found_dir "E:/Git/OCCT/occt_vc14-64/cmake")
endif()

set(_occt_paths "$ENV{Path}")
foreach(_p IN LISTS _occt_paths)
    if(_occt_found_dir)
        break()
    endif()
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

find_package(VTK QUIET COMPONENTS CommonCore IOImage)
if(NOT TARGET VTK::CommonCore)
    add_library(VTK::CommonCore INTERFACE IMPORTED)
endif()
if(NOT TARGET VTK::IOImage)
    add_library(VTK::IOImage INTERFACE IMPORTED)
endif()

find_package(OpenCASCADE REQUIRED)

find_package(Qt6 QUIET COMPONENTS Widgets OpenGL OpenGLWidgets Test)
if(Qt6_FOUND)
    set(QT_LIBS Qt6::Widgets Qt6::OpenGL Qt6::OpenGLWidgets)
    set(QT_TEST_LIBS Qt6::Test)
else()
    find_package(Qt5 REQUIRED COMPONENTS Widgets OpenGL Test)
    set(QT_LIBS Qt5::Widgets Qt5::OpenGL)
    set(QT_TEST_LIBS Qt5::Test)
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
set(_occt_libs
    ${OpenCASCADE_FoundationClasses_LIBRARIES}
    ${OpenCASCADE_ModelingData_LIBRARIES}
    ${OpenCASCADE_ModelingAlgorithms_LIBRARIES}
    ${OpenCASCADE_Visualization_LIBRARIES}
)
list(REMOVE_ITEM _occt_libs TKIVtk TKIVtkDraw TKD3DHost TKOpenGles TKOpenGlesTest TKD3DHostTest)
target_link_libraries(BraceletCNC PRIVATE ${_occt_libs} ${QT_LIBS})

if(DEFINED OpenCASCADE_USE_FILE)
    include(${OpenCASCADE_USE_FILE})
endif()

enable_testing()

add_executable(BraceletCNC_Tests
    tests/OccViewTests.cpp
    src/OccView.cpp
    src/OccView.h
)
target_include_directories(BraceletCNC_Tests PRIVATE ${OpenCASCADE_INCLUDE_DIR})
target_include_directories(BraceletCNC_Tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(BraceletCNC_Tests PRIVATE ${_occt_libs} ${QT_LIBS} ${QT_TEST_LIBS})
add_test(NAME BraceletCNC_Tests COMMAND BraceletCNC_Tests)
