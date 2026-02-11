cmake_minimum_required(VERSION 3.20)
project(mc-shaders-helpers)

find_package(fmt CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(ZLIB REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

function(add_app_with_base name sources)
    add_executable(${name} src/base_app.cpp ${sources})
    target_include_directories(${name} PRIVATE include ${Stb_INCLUDE_DIR})
    target_link_libraries(${name} PRIVATE fmt::fmt glad::glad ZLIB::ZLIB SDL2::SDL2main SDL2::SDL2)
    install(TARGETS ${name} RUNTIME DESTINATION bin)
endfunction(add_app_with_base name sources)

add_executable(atlas_collapser src/atlas_collapser.cpp)
target_include_directories(atlas_collapser PRIVATE ${Stb_INCLUDE_DIR})
target_link_libraries(atlas_collapser PRIVATE fmt::fmt)
install(TARGETS atlas_collapser RUNTIME DESTINATION bin)

add_app_with_base(test src/test.cpp)

install(IMPORTED_RUNTIME_ARTIFACTS SDL2::SDL2 fmt::fmt DESTINATION bin)
install(DIRECTORY scripts DESTINATION .)

set(CPACK_GENERATOR "ZIP")
include(CPack)
