cmake_minimum_required(VERSION 3.24)

project(BitbotCopilot VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/MP)
    add_definitions(-D_WINSOCKAPI_)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin) # .exe / .dll
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin) # .dll（共享库）
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin) # .lib / .a

add_definitions(-DPRERELEASE_BUILD)
# add update url from a file
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/update_url.txt")
    file(READ "${CMAKE_CURRENT_SOURCE_DIR}/update_url.txt" UPDATE_URL)
    string(STRIP "${UPDATE_URL}" UPDATE_URL)
    if(UPDATE_URL)
        message(STATUS "Found update URL: ${UPDATE_URL}")
        add_definitions(-DUPDATE_CHANNEL="${UPDATE_URL}")
    else()
        message(WARNING "No update URL found.")
        add_definitions(-DUPDATE_CHANNEL="")
    endif()
else()
    message(WARNING "No update file is given.")
endif()

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets LinguistTools WebSockets Network Svg PrintSupport OpenGL)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools WebSockets Network Svg PrintSupport OpenGL)
find_package(libssh CONFIG REQUIRED)
message("Qt version: ${QT_VERSION_MAJOR}, ${QT_VERSION_MINOR}, ${QT_VERSION_PATCH}")
message("libssh version: ${libssh_VERSION}")

include(FetchContent)
FetchContent_Declare(
    qtgamepadlegacy
    URL file://${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/qtgamepadlegacy.zip
    OVERRIDE_FIND_PACKAGE # 让 find_package(Qt6Gamepad) 指向刚构建好的库
)

FetchContent_MakeAvailable(qtgamepadlegacy)


add_subdirectory(ThirdParty/ElaWidgetTools/src)
add_subdirectory(ThirdParty/QSimpleUpdater)

set(TS_FILES
    Res/Translation/BitbotCopilot_zh_CN.ts
)

add_definitions(-DQCUSTOMPLOT_USE_OPENGL)
set(QCP_FILES
    ./ThirdParty/qcustomplot/qcustomplot.cpp
    ./ThirdParty/qcustomplot/qcustomplot.h
    ./ThirdParty/qcustomplot/SmoothGenerator.hpp
)

set(QSU_FILES
    ./ThirdParty/QSimpleUpdater/etc/resources/qsimpleupdater.qrc
    ./ThirdParty/QSimpleUpdater/include/QSimpleUpdater.h
    ./ThirdParty/QSimpleUpdater/src/AuthenticateDialog.cpp
    ./ThirdParty/QSimpleUpdater/src/AuthenticateDialog.h
    ./ThirdParty/QSimpleUpdater/src/AuthenticateDialog.ui
    ./ThirdParty/QSimpleUpdater/src/Downloader.cpp
    ./ThirdParty/QSimpleUpdater/src/Downloader.h
    ./ThirdParty/QSimpleUpdater/src/Downloader.ui
    ./ThirdParty/QSimpleUpdater/src/QSimpleUpdater.cpp
    ./ThirdParty/QSimpleUpdater/src/Updater.cpp
    ./ThirdParty/QSimpleUpdater/src/Updater.h
)

set(THIRDPARTY_FILES
    ${QCP_FILES}
    ${QSU_FILES}
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
    UI/widget/FluentLoadingWidget.cpp
    UI/widget/FluentLoadingWidget.h
    UI/widget/FluentMessageBox.hpp
)

set(DATA_VIEWER_UI
    UI/DataViewer/DataViewerPage.cpp
    UI/DataViewer/DataViewerPage.h
    UI/DataViewer/DataViewerPage.ui
    UI/DataViewer/DataViewerFlowIndicator.cpp
    UI/DataViewer/DataViewerFlowIndicator.h
    UI/DataViewer/RemoteFileSelector.cpp
    UI/DataViewer/RemoteFileSelector.h
)

set(VIRTUAL_TRACK_PAD_UI
    UI/VirtualTrackpad/VirtualTrackpad.cpp
    UI/VirtualTrackpad/VirtualTrackpad.h
    UI/VirtualTrackpad/VirtualTrackpad.ui
    UI/VirtualTrackpad/SimpleTrackpad.cpp
    UI/VirtualTrackpad/SimpleTrackpad.h
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
    ${VIRTUAL_TRACK_PAD_UI}
)

set(COM_FILES
    Communication/BitbotTcpProtocalV1.h
    Communication/BitbotTcpProtocalV1.cpp
    Communication/MetaCommunication.hpp
    Communication/MetaBackendCom.hpp
    Communication/LocalCommand.hpp
    Communication/RCM/SessionManager.cpp
    Communication/RCM/SessionManager.h
    Communication/RCM/RemoteCommand.hpp
    Communication/RCM/SftpFileSystemModel.cpp
    Communication/RCM/SftpFileSystemModel.h
    Communication/RCM/SftpFileTransport.cpp
    Communication/RCM/SftpFileTransport.h
)

set(UTIL_FILES
    Utils/GamepadDriver/ZQGamepad.cpp
    Utils/GamepadDriver/ZQGamepad.h
    Utils/GamepadDriver/ZGamepad_XInput.hpp
    Utils/Settings/SettingsHandler.cpp
    Utils/Settings/SettingsHandler.h
    Utils/Settings/parser.hpp
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
    ./ThirdParty/QSimpleUpdater/include
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

    #qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} )
    qt6_add_translations(BitbotCopilot TS_FILES ${TS_FILES} QM_FILES_OUTPUT_VARIABLE qm_files)
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
        Qt${QT_VERSION_MAJOR}::GamepadLegacy
        XInput
        Opengl32
        glu32
        ssh
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
        Qt${QT_VERSION_MAJOR}::GamepadLegacy
        OpenGL
        ssh
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

message("
Bitbot Copilot is a bitbot GUI tool for robot control and data visualization.")
message("Visit https://github.com/ZzzzzzS/BitbotCopilot for detailed documentation.")
message("+------------------------------------------------------------------------------------------+
| BBBB   III  TTTTT  BBBB    OOO   TTTTT       CCCC   OOO   PPPP   III  L      OOO   TTTTT |
| B   B   I     T    B   B  O   O    T        C      O   O  P   P   I   L     O   O    T   |
| BBBB    I     T    BBBB   O   O    T        C      O   O  PPPP    I   L     O   O    T   |
| B   B   I     T    B   B  O   O    T        C      O   O  P       I   L     O   O    T   |
| BBBB   III    T    BBBB    OOO     T         CCCC   OOO   P      III  LLLLL  OOO     T   |
+------------------------------------------------------------------------------------------+
")
