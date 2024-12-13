cmake_minimum_required(VERSION 3.5)

project(ZipViewer LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check https://doc.qt.io/qt/deployment-android.html for more information.
# They need to be set before the find_package( ...) calls below.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

# Указываем пути для различных платформ
if(WIN32)
    message(STATUS "Configuring for Windows...")
    set(CMAKE_PREFIX_PATH "C:/Qt/6.2.4/mingw_64") # Укажите путь к Qt для Windows
    set(MINIZIP_PATH "${CMAKE_SOURCE_DIR}/minizip-ng")
elseif(UNIX)
    message(STATUS "Configuring for Linux...")
    set(MINIZIP_PATH "${CMAKE_SOURCE_DIR}/minizip-ng")
endif()

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        MainWindow.cpp
        MainWindow.h
        MainWindow.ui
        ZipModel.cpp
        ZipModel.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ZipViewer
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(ZipViewer SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(ZipViewer
            ${PROJECT_SOURCES}
        )
    endif()
endif()
# Указываем путь к директории minizip-ng
include_directories(${CMAKE_SOURCE_DIR}/minizip-ng)

# Добавляем minizip-ng как подкаталог
add_subdirectory(minizip-ng)
#target_link_libraries(ZipViewer PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(ZipViewer PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets)
target_link_libraries(ZipViewer PRIVATE minizip)

# Условная настройка для Windows
if(WIN32)
    # Добавьте зависимости Windows, если необходимо
    target_compile_definitions(ZipViewer PRIVATE PLATFORM_WINDOWS)
elseif(UNIX)
    target_compile_definitions(ZipViewer PRIVATE PLATFORM_LINUX)
endif()
