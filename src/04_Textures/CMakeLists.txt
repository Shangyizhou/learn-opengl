add_executable(textures textures.cc)
target_include_directories(textures PRIVATE ${Stb_INCLUDE_DIR})
target_link_libraries(textures PRIVATE glfw glad::glad)

set_target_properties(textures PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/learn/
)