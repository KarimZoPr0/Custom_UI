cmake_minimum_required(VERSION 3.27)
project(Custom_UI)

set(CMAKE_CXX_STANDARD 17)

file(GLOB CLIENT_SOURCES game/*cpp include/*.h)
add_executable(Custom_UI ${CLIENT_SOURCES})

find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(Custom_UI
        PRIVATE
        $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
        $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)

find_package(glad CONFIG REQUIRED)
target_link_libraries(Custom_UI PRIVATE glad::glad)
