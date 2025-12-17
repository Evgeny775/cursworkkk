cmake_minimum_required(VERSION 3.19)
project(cursworkkk LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Sql)

qt_standard_project_setup()

qt_add_executable(cursworkkk
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    databasemanager.h databasemanager.cpp
    authmanager.h authmanager.cpp
    user.h user.cpp
    ticketmanager.h ticketmanager.cpp
)

target_link_libraries(cursworkkk
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Sql

)

include(GNUInstallDirs)

install(TARGETS cursworkkk
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET cursworkkk
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
