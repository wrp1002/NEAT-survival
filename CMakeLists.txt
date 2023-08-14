cmake_minimum_required(VERSION 3.5)

project(NEAT-survival LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

cmake_minimum_required(VERSION 3.12)

file(GLOB src CONFIGURE_DEPENDS "./NEAT\ Survival/*.h" "./NEAT\ Survival/*.cpp")

add_executable(
    NEAT-survival
    ${src}
)


target_link_libraries (NEAT-survival allegro allegro_font allegro_ttf allegro_primitives allegro_color allegro_dialog fmt)

install(TARGETS NEAT-survival
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
