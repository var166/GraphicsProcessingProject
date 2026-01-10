cmake_minimum_required(VERSION 3.30)
project(proiectGP)

set(CMAKE_CXX_STANDARD 20)

add_executable(proiectGP Camera.cpp
        main.cpp
        Mesh.cpp
        Model3D.cpp
        Shader.cpp
        stb_image.cpp
        tiny_obj_loader.cpp
        Window.cpp
        SkyBox.cpp)

include_directories(proiectGP)
include_directories(
        /opt/homebrew/include
        /opt/homebrew/Cellar/glm/1.0.3/include
        /opt/homebrew/Cellar/glew/2.3.0_1/include
        /opt/homebrew/Cellar/glfw/3.4/include
        /Users/andreivarga/Documents/an3/GPLAB/ProiectGP


        ${CMAKE_SOURCE_DIR}
)
link_directories(
        /opt/homebrew/lib
        /opt/homebrew/Cellar/glew/2.3.0_1/lib
        /opt/homebrew/Cellar/glfw/3.4/lib
)
target_link_libraries(proiectGP
        /opt/homebrew/Cellar/glfw/3.4/lib/libglfw.dylib
        /opt/homebrew/Cellar/glew/2.3.0_1/lib/libGLEW.dylib
        "-framework OpenGL"
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
)
file(COPY models shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})