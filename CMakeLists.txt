cmake_minimum_required(VERSION 3.5)

project(ttml-meta-db VERSION 0.3 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Network)

set(PROJECT_SOURCES
        main.cpp

        mainwindow.cpp
        mainwindow.h
        mainwindow.ui

        iddialog.cpp
        iddialog.h
        iddialog.ui

        artist.cpp
        artist.h
        album.cpp
        album.h
        track.cpp
        track.h
        dataentity.cpp
        dataentity.h
        database.cpp
        database.h
        artistmodel.cpp
        artistmodel.h
        metamodel.cpp
        metamodel.h
        albummodel.cpp
        albummodel.h
        trackmodel.cpp
        trackmodel.h
        albumartistmodel.cpp
        albumartistmodel.h
        trackalbummodel.cpp
        trackalbummodel.h
        trackfeatmodel.cpp
        trackfeatmodel.h
        idmodel.cpp
        idmodel.h
        iddelegate.cpp
        iddelegate.h

        wintoastlib.cpp
        wintoastlib.h

        wintoast.hpp

        app_win32.rc

        res.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ttml-meta-db
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ttml-meta-db APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(ttml-meta-db SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(ttml-meta-db
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(ttml-meta-db PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network)

set_target_properties(ttml-meta-db PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS ttml-meta-db
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(ttml-meta-db)
endif()
