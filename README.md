# Vulkan Game Engine

A simple 3D game engine built with Vulkan and GLFW in C++. This project serves as a learning implementation of modern graphics programming using the Vulkan API.

## Features

- **3D Rendering**: Basic 3D object rendering using Vulkan
- **Window Management**: GLFW integration for cross-platform window creation
- **Modern Graphics API**: Utilizes Vulkan for high-performance graphics
- **C++ Implementation**: Written in modern C++

## Prerequisites

Before building, ensure you have the following installed:

- **C++ Compiler** with C++17 support (GCC, Clang, or MSVC)
- **Vulkan SDK** (version 1.2 or higher)
- **GLFW** (version 3.3 or higher)
- **CMake** (version 3.10 or higher)

## Building the Project

```bash
# Clone the repository
git clone <repository-url>
cd VulkanGameEngine

# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build the project
make
```

## Usage

```bash
# Run the engine from the build directory
./VulkanGameEngine
```

## Project Structure

```
VulkanGameEngine/
├── src/                 # Source files
├── include/             # Header files
├── shaders/            # Vulkan shader files
└── CMakeLists.txt      # Build configuration TODO
```

## Dependencies

- **Vulkan**: Low-overhead graphics and compute API
- **GLFW**: Multi-platform library for window creation and input
- **GLM**: Mathematics library for graphics programming

## Acknowledgments

This project was developed following tutorials and resources from:
- Brendan Galea's Vulkan tutorial series
- [Vulkan Tutorial](https://vulkan-tutorial.com)


## Future Improvements

- Texture mapping support
- Lighting and shading implementation
- Camera controls
- Model loading from files
- Scene management system

*Note: This is an educational project focused on learning Vulkan graphics programming.*
