cmake_minimum_required(VERSION 3.5)

project(BITBOT-Frontend-Z VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options(/MP)
endif()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools WebSockets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools WebSockets Network)
add_subdirectory(ThirdParty/ElaWidgetTools)
set(TS_FILES 
    Res/BITBOT-Frontend-Z_zh_CN.ts
    Res/UI.qrc
    )

set(UI_WIDGETS
    UI/widget/MetaRTDView.cpp
    UI/widget/MetaRTDView.h
    UI/widget/RTDKernelStates.cpp
    UI/widget/RTDKernelStates.h
    UI/widget/RTDKernelStates.ui
    UI/widget/RTDRobotStates.cpp
    UI/widget/RTDRobotStates.h
    UI/widget/RTDRobotStates.ui
    UI/widget/RTDKeyboardStatus.cpp
    UI/widget/RTDKeyboardStatus.h
    UI/widget/RTDKeyboardStatus.ui
    UI/widget/RTDGamepadStatus.cpp
    UI/widget/RTDGamepadStatus.h
    UI/widget/RTDGamepadStatus.ui
    UI/widget/RTDDeviceInfo.cpp
    UI/widget/RTDDeviceInfo.h
    UI/widget/RTDDeviceInfo.ui
    UI/widget/RTDConnection.cpp
    UI/widget/RTDConnection.h
    UI/widget/RTDConnection.ui
)

set(UI_FILES 
    UI/MainWindow.cpp
    UI/MainWindow.h
    UI/AboutPage.cpp
    UI/AboutPage.h
    UI/HomePage.cpp
    UI/HomePage.h
    UI/PilotPage.cpp
    UI/PilotPage.h
    UI/ViewDataPage.cpp
    UI/ViewDataPage.h
    UI/ViewDataPage.ui

    UI/AboutPage.cpp
    UI/AboutPage.h
    UI/SettingsPage.cpp
    UI/SettingsPage.h

    ${UI_WIDGETS}
)

set(COM_FILES
    Communication/BitbotTcpProtocalV1.h
    Communication/BitbotTcpProtocalV1.cpp
    Communication/MetaCommunication.hpp
)

set(UTIL_FILES
    Utils/GamepadDriver/ZQGamepad.cpp
    Utils/GamepadDriver/ZQGamepad.h
    Utils/GamepadDriver/ZGamepad_XInput.hpp
)


set(PROJECT_SOURCES
        main.cpp
        ${UI_FILES}
        ${TS_FILES}
        ${UTIL_FILES}
        ${COM_FILES}
)

include_directories(./ThirdParty/ElaWidgetTools/include)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(BITBOT-Frontend-Z
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET BITBOT-Frontend-Z APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(BITBOT-Frontend-Z SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(BITBOT-Frontend-Z
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(BITBOT-Frontend-Z PRIVATE
                     elawidgettools
                     Qt${QT_VERSION_MAJOR}::Widgets 
                     Qt${QT_VERSION_MAJOR}::WebSockets
                     Qt${QT_VERSION_MAJOR}::Network
                     XInput
                     )




# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.BITBOT-Frontend-Z)
endif()
set_target_properties(BITBOT-Frontend-Z PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS BITBOT-Frontend-Z
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(BITBOT-Frontend-Z)
endif()
