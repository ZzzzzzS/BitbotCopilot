cmake_minimum_required(VERSION 3.5)

project(BitbotCopilot VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options(/MP)
endif()

find_package(QT NAMES Qt5 REQUIRED COMPONENTS Widgets LinguistTools WebSockets Network Svg PrintSupport OpenGL)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools  WebSockets Network Svg PrintSupport OpenGL)



add_subdirectory(ThirdParty/ElaWidgetTools/src)
set(TS_FILES 
    Res/Translation/BitbotCopilot_zh_CN.ts
    )

add_definitions(-DQCUSTOMPLOT_USE_OPENGL)
set(QCP_FILES
    ./ThirdParty/qcustomplot/qcustomplot.cpp
    ./ThirdParty/qcustomplot/qcustomplot.h
    ./ThirdParty/qcustomplot/SmoothGenerator.hpp
)

set(THIRDPARTY_FILES
    ${QCP_FILES}
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
    UI/widget/RTDDeviceInfoTableModel.h
    UI/widget/RTDDeviceInfoTableModel.cpp
    UI/widget/RTDConnection.cpp
    UI/widget/RTDConnection.h
    UI/widget/RTDConnection.ui
    UI/widget/BackendManager.cpp
    UI/widget/BackendManager.h
    UI/widget/BackendManager.ui
    UI/widget/CustomSplashScreen.cpp
    UI/widget/CustomSplashScreen.h
    UI/widget/CustomImageLabel.hpp
    UI/widget/DataViewerFlowIndicator.cpp
    UI/widget/DataViewerFlowIndicator.h
)

set(DATA_VIEWER_UI
    UI/DataViewer/DataViewerPage.cpp
    UI/DataViewer/DataViewerPage.h
    UI/DataViewer/DataViewerPage.ui
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

    UI/AboutPage.cpp
    UI/AboutPage.h
    UI/AboutPage.ui
    UI/SettingsPage.cpp
    UI/SettingsPage.h

    ${UI_WIDGETS}
    ${DATA_VIEWER_UI}
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
    Utils/Settings/SettingsHandler.cpp
    Utils/Settings/SettingsHandler.h
    Utils/DataReadWriter/csv_io.hpp
)


set(PROJECT_SOURCES
        main.cpp
        ${UI_FILES}
        ${TS_FILES}
        ${UTIL_FILES}
        ${COM_FILES}
        ${THIRDPARTY_FILES}
        Res/UI.qrc
)

include_directories(
    ./
    ./ThirdParty/ElaWidgetTools/src/include
    ./ThirdParty/qcustomplot
    ./UI/widget
)

set(app_icon_resource_windows "${CMAKE_CURRENT_SOURCE_DIR}/Res/icon.rc")

execute_process(
    COMMAND git log -1 --format=%h
    WORKING_DIRECTORY
    ${CMAKE_CURRENT_LIST_DIR}
    OUTPUT_VARIABLE GIT_HASH
    
OUTPUT_STRIP_TRAILING_WHITESPACE
)
message("Build version ${GIT_HASH}")
add_definitions(-DBUILD_VERSION_COMMIT_HASH="${GIT_HASH}")



if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(BitbotCopilot
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${app_icon_resource_windows}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET BitbotCopilot APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(BitbotCopilot SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(BitbotCopilot
            ${PROJECT_SOURCES}
            ${app_icon_resource_windows}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

if(WIN32)

target_link_libraries(BitbotCopilot PRIVATE
                     elawidgettools
                     Qt${QT_VERSION_MAJOR}::Widgets 
                     Qt${QT_VERSION_MAJOR}::WebSockets
                     Qt${QT_VERSION_MAJOR}::Network
                     Qt${QT_VERSION_MAJOR}::Svg
                     Qt${QT_VERSION_MAJOR}::PrintSupport
                     Qt${QT_VERSION_MAJOR}::OpenGL
                    XInput
                    Opengl32
                    glu32
                     )

elseif(LINUX)

target_link_libraries(BitbotCopilot PRIVATE
                     elawidgettools
                     Qt${QT_VERSION_MAJOR}::Widgets 
                     Qt${QT_VERSION_MAJOR}::WebSockets
                     Qt${QT_VERSION_MAJOR}::Network
                     Qt${QT_VERSION_MAJOR}::Svg
                     Qt${QT_VERSION_MAJOR}::PrintSupport
                     Qt${QT_VERSION_MAJOR}::OpenGL
                     OpenGL
                     )
endif()





# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.BitbotCopilot)
endif()
set_target_properties(BitbotCopilot PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS BitbotCopilot
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(BitbotCopilot)
endif()
