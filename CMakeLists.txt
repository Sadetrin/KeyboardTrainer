cmake_minimum_required(VERSION 3.16)

project(KeyboardTrainer VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Multimedia MultimediaWidgets)

set(PROJECT_SOURCES
    main.cpp

    KeyboardTrainer.h
    KeyboardTrainer.cpp
    KeyboardTrainer.ui

    DataManager.h
    DataManager.cpp

    Level.h
    Level.cpp

    Trainer.h
    Trainer.cpp

    User.h
    User.cpp

    MainWindow.h
    MainWindow.cpp

    MyLineEdit.h
    MyLineEdit.cpp

    Records.h
    Records.cpp

    RecordsWindow.h
    RecordsWindow.cpp

    resources.qrc
)

qt_add_executable(KeyboardTrainer
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    UserDialog.h UserDialog.cpp
    UserDialog.ui

)

# Установка иконки (только для Windows)
if(WIN32)
    set(APP_ICON_RESOURCE "${CMAKE_CURRENT_SOURCE_DIR}/app_icon.rc")
    target_sources(KeyboardTrainer PRIVATE ${APP_ICON_RESOURCE})
    set_target_properties(KeyboardTrainer PROPERTIES WIN32_EXECUTABLE TRUE)
endif()


# Связывание с библиотеками Qt
target_link_libraries(KeyboardTrainer PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Multimedia
    Qt6::MultimediaWidgets
)

# Установка свойств для macOS (если нужно)
set_target_properties(KeyboardTrainer PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.KeyboardTrainer
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
)

# Установка
include(GNUInstallDirs)
install(TARGETS KeyboardTrainer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Финализация Qt6
qt_finalize_executable(KeyboardTrainer)
