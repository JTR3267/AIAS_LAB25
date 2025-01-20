# demo/CMakeLists.txt

# Define a library target `demo_components`:
# - Source files: TrafficGenerator.cc, Memory.cc
# - Include directories: Current source directory
# - Linked libraries: ACALSim::acalsim
add_library(demo_components TrafficGenerator.cc Memory.cc)
target_include_directories(demo_components PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(demo_components PUBLIC ACALSim::acalsim)

# Define an executable target `demo`:
# - Source file: main.cc
# - Linked library: demo_components
add_executable(demo main.cc)
target_link_libraries(demo PUBLIC demo_components)
