project(mediaplayer)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Multimedia MultimediaWidgets Widgets)

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
    Qt::Core
    Qt::Gui
    Qt::Multimedia
	Qt::MultimediaWidgets
    Qt::Widgets
)