cmake_minimum_required(VERSION 3.16)
project(DisplayIntercept VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find packages
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Threads REQUIRED)
find_package(nlohmann_json 3.2.0 QUIET)

# Platform-specific sources
if(WIN32)
    set(PLATFORM_SOURCES
        src/capture/WindowsDxgiCapture.cpp
        src/interop/GLInteropHelper.cpp
    )
    set(PLATFORM_LIBS d3d11 dxgi)
    add_definitions(-DPLATFORM_WINDOWS)
    message(STATUS "Configuring for Windows...")
elseif(APPLE)
    set(PLATFORM_SOURCES
        src/capture/MacSCKCapture.mm
        src/interop/MacGLInterop.mm
    )
    set(PLATFORM_LIBS
        "-framework ScreenCaptureKit"
        "-framework Metal"
        "-framework CoreVideo"
        "-framework OpenGL"
    )
    add_definitions(-DPLATFORM_MACOS)
    message(STATUS "Configuring for macOS...")
else()
    message(FATAL_ERROR "Unsupported platform. Windows or macOS required.")
endif()

# ImGui sources
set(IMGUI_SOURCES
    external/imgui/imgui.cpp
    external/imgui/imgui_demo.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/backends/imgui_impl_glfw.cpp
    external/imgui/backends/imgui_impl_opengl3.cpp
)

# Main executable
add_executable(DisplayIntercept
    src/main.cpp
    src/ui/DebugUI.cpp
    src/passes/HDRNormalizePass.cpp
    src/passes/CalibrationPass.cpp
    src/stability/GraphicsRecoveryManager.cpp
    src/stability/FileWatcher.cpp
    src/plugin/PluginReloader.cpp
    ${PLATFORM_SOURCES}
    ${IMGUI_SOURCES}
)

target_include_directories(DisplayIntercept PRIVATE
    src
    external/imgui
    external/imgui/backends
)

target_link_libraries(DisplayIntercept
    OpenGL::GL
    glfw
    Threads::Threads
    ${PLATFORM_LIBS}
)

# Copy shaders and config to build directory
file(COPY shaders DESTINATION ${CMAKE_BINARY_DIR})
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

# Platform-specific linking
if(WIN32)
    target_link_libraries(DisplayIntercept ws2_32)
elseif(APPLE)
    set_source_files_properties(
        src/capture/MacSCKCapture.mm
        src/interop/MacGLInterop.mm
        PROPERTIES LANGUAGE OBJCXX
    )
endif()
