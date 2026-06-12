add_library(colony_core STATIC
    core/EventSystem.cpp
    core/PolicyEngine.cpp
    core/ResourceAllocator.cpp
    core/SimulationEngine.cpp
)
target_include_directories(colony_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(colony_core PRIVATE -Wall -Wextra -Wpedantic)

add_executable(martian main.cpp ui/Renderer.cpp)
target_link_libraries(martian PRIVATE colony_core raylib)
target_compile_options(martian PRIVATE -Wall -Wextra)
