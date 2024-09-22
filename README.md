# C++ By Example

## Requirements

- **C++20 Compiler** (e.g., GCC 10+, Clang 10+, or MSVC 2019+)
- **CMake** (version 3.10+)
- **vcpkg**

To install dependencies, you can use **vcpkg** or your system's package manager.

### Installing vcpkg ###
```bash
git clone https://github.com/microsoft/vcpkg.git
```
```bash 
cd vcpkg
```
```bash
./bootstrap-vcpkg.sh
```
  
### Installing Dependencies 
```bash 
vcpkg install
```
## Building the Project

To build the project, you can either use the provided `Makefile` or manually invoke CMake.

### Using Makefile

To build the project, simply run:

```bash
make all
```

This will invoke CMake, configure the project, and build the executables in the `build/` directory.

### Clean the Build

To remove the `build/` directory and clean the project:

```bash
make clean
```

## Running Examples

### Hello World
```bash
make run-hello-world
```
### Asynchronous Boost.Asio Hello World
```bash
make run-asio-hello-world
```

This will start an HTTP server that listens on port `8080`. You can test the server by opening a browser or using `curl`:

```bash
curl http://localhost:8080
```

The server will respond with a "Hello, World!" message.
