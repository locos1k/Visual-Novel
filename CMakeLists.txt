cmake_minimum_required(VERSION 3.10)
project(LunarGarden)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Укажите путь к SFML если не находится автоматически
set(SFML_DIR "C:/SFML-2.5.1/lib/cmake/SFML")  # Пример для Windows

find_package(SFML 2.5 COMPONENTS graphics audio window system REQUIRED)

add_executable(LunarGarden
    main.cpp
    Entity.cpp
    Scene.cpp
    GameLogic.cpp
)

target_include_directories(LunarGarden PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${SFML_INCLUDE_DIR}
)

target_link_libraries(LunarGarden PRIVATE
    sfml-graphics
    sfml-audio
    sfml-window
    sfml-system
)

# Копирование ассетов в папку сборки
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/assets/
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets
)