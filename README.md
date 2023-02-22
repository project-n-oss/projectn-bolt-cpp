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

**Configure Bolt custom domain and httpClientFactory**

You must set Bolt custom domain through environment variable `BOLT_CUSTOM_DOMAIN` or through code (see [./example/main.cpp](./example/main.cpp)):

```c++
ProjectN::Bolt::BoltConfig::customDomain = "rvh.bolt.projectn.co";
ProjectN::Bolt::BoltConfig::Reset();
```

You must also set the `httpClientFactory_create_fn` option in the aws `SDKOptions` struct like so (see [./example/main.cpp](./example/main.cpp)): 

```c++
SDKOptions options;
options.httpOptions.httpClientFactory_create_fn = []() { return Aws::MakeShared<ProjectN::Bolt::BoltS3HttpClientFactory>(ALLOCATION_TAG); };
```

**Configure preferred region and availability zone:**

If running on an EC2 instance the SDK will use that instance's region and availability zone by default

If you want a specific region you can set with the environment variable `AWS_REGION`

If you want a specific availability zone you can set it with the a environment variable `AWS_ZONE_ID`.

You can also set those values in your code:

```c++
ProjectN::Bolt::BoltConfig::region = "us-east-2";
ProjectN::Bolt::BoltConfig::zoneId = "use1‑az2";
ProjectN::Bolt::BoltConfig::customDomain = "example.com";
ProjectN::Bolt::BoltConfig::Reset();
```

## Building from source

You can build this C++ library from source using the [./build.sh](./build.sh) bash script.
This will create a new folder `./install` with all of the headers and static library.

### build.sh

flags:

- r) set build type to Release
- d) set build type to Debug
- i) create an `install/` target folder

Example to create a release:

```bash
./build.sh -r -i
```

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
# cd ./build
cpack -G ZIP
```

## Git Submodule

```bash
git submodule update --init --recursive
```

## Creating New Release

We follow the same release versioning as aws-sdk-cpp.

### Steps

1. run `./update_version.sh` and give the aws-sdk-cpp version you want to update too. (This will checkout the correct version of the aws-sdk-cpp submodule and update the projectn cmake project to the same version)
2. git commit/push and run the `Release` Action on githu