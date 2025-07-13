cmake_minimum_required(VERSION 3.31)
project(jpdy)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(jpdy
        src/main.cpp
        
        # Model files
        src/model/board.cpp
        src/model/cell.cpp
        src/model/game_state.cpp
        include/model/board.h
        include/model/cell.h
        include/model/team.h
        
        # Controller files
        src/controller/game_controller.cpp
        include/controller/game_controller.h
        
        # View files
        src/view/main_window.cpp
        src/view/config_widget.cpp
        src/view/game_board_widget.cpp
        src/view/question_dialog.cpp
        src/view/team_widget.cpp
        include/view/main_window.h
        include/view/config_widget.h
        include/view/game_board_widget.h
        include/view/question_dialog.h
        include/view/team_widget.h
)

target_include_directories(jpdy PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(jpdy
        Qt::Core
        Qt::Gui
        Qt::Widgets
)

