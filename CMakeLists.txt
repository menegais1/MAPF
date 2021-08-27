cmake_minimum_required(VERSION 3.17)
project(MAPF)

set(CMAKE_CXX_STANDARD 20)

include_directories(Dependencies/glad/include)
include_directories(Dependencies/glm)
include_directories(Dependencies/tinyobjloader)
include_directories(Dependencies/imgui)
find_package(OpenGL REQUIRED)

add_executable(MAPF
        Dependencies/glad/src/glad.c
        Dependencies/imgui/imconfig.h
        Dependencies/imgui/imgui_demo.cpp
        Dependencies/imgui/imgui_draw.cpp
        Dependencies/imgui/imgui_impl_glfw.cpp
        Dependencies/imgui/imgui_impl_glfw.h
        Dependencies/imgui/imgui_impl_opengl3.cpp
        Dependencies/imgui/imgui_impl_opengl3.h
        Dependencies/imgui/imgui_internal.h
        Dependencies/imgui/imgui_widgets.cpp
        Dependencies/imgui/imgui.cpp
        Dependencies/imgui/imgui.h
        Dependencies/imgui/imstb_rectpack.h
        Dependencies/imgui/imstb_textedit.h
        Dependencies/imgui/imstb_truetype.h

        src/Backend/GraphicsLibrary.h
        src/Backend/GraphicsLibrary.cpp
        src/Backend/Shader.cpp
        src/Backend/Shader.h
        src/Backend/Texture2D.cpp
        src/Backend/Texture2D.h
        src/FileManagers/ModelLoader.cpp
        src/FileManagers/ModelLoader.h
        src/FileManagers/FileLoader.cpp
        src/FileManagers/FileLoader.h
        src/Callbacks.h
        src/PnuVertexInput.h
        src/PnuVertexInput.cpp
        src/main.cpp
        src/GameObject.h
        src/GlobalManager.h
        src/GlobalManager.cpp src/GameObject.cpp)


if (WIN32)
    include_directories(Dependencies/glfw-WIN_64/include)
    add_subdirectory(Dependencies/glfw-WIN_64)
    target_link_libraries(MAPF glfw gdi32 ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES})
endif ()

if (UNIX)
    include_directories(Dependencies/glfw/include)
    add_subdirectory(Dependencies/glfw)
    target_link_libraries(MAPF glfw ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES})
endif ()



