project(mediaplayer)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Multimedia MultimediaWidgets Widgets)

add_executable(${PROJECT_NAME}
    main.cpp
	videoplayer.h
	videoplayer.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Multimedia
	Qt${QT_VERSION_MAJOR}::MultimediaWidgets
    Qt${QT_VERSION_MAJOR}::Widgets
)