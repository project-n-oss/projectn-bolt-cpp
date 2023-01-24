![projectn.png](./projectn.png)
# projectn-bolt-cpp

This repository provides bolt client sdk libraries for projectn.

## Prerequisites

- C++11 or higher
- [cmake 3.21](https://cmake.org/) or higher
- [aws-sdk-cpp](https://docs.aws.amazon.com/sdk-for-cpp/v1/developer-guide/getting-started.html)

## Getting Started

You can download one of the prebuilt static library .zip in the [release page](TODO).
You can also build the projectn library from [source](#building-from-source).

### Using the library

You can look at the [./example](./example) folder to see how to use our library with your app.

## Building from source

You can build this C++ library from source using the [./build.sh](./build.sh) bash script.
This will create a new folder `./install` with all of the headers and static library.

### CMake

You can also use cmake directly to build this library and pass in cmake arguments like `-DBUILD_SHARED_LIBS=1`:

```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --target install --config Release
```

### CPack

The prebuilt packages are built with [cpack](https://cmake.org/cmake/help/latest/module/CPack.html). After building the library with cmake, you can use `cpack` to build your own package like so:

```bash
# inside ./build
cpack -G ZIP
```


## Git Submodule

```bash
git submodule update --init --recursive
```