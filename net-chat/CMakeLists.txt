project(net-chat)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Network Widgets)

add_executable(${PROJECT_NAME}
    main.cpp
	server.h
	server.cpp
	chatdialog.h
	chatdialog.cpp
	connection.h
	connection.cpp
	client.h
	client.cpp
	peermanager.h
	peermanager.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

target_link_libraries(${PROJECT_NAME} PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Network
    Qt::Widgets
)